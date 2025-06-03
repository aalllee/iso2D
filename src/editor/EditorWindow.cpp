//
// Created by User on 30/05/2025.
//

#include "EditorWindow.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "math.h"
#include "../../include/util/math.h"

EditorWindow::EditorWindow(sf::Vector2u size, const std::string &title) :window(sf::VideoMode(size.x, size.y), title) {

    isoGrid =  Math::buildInfiniteIsoGrid();
  // ImGui::SFML::Init(window);
    view.setSize(static_cast<float>(size.x), static_cast<float>(size.y));
    view.setCenter(view.getSize() / 2.f);
    window.setView(view);

    selectionHighlight.setFillColor(sf::Color::Transparent);
    selectionHighlight.setOutlineColor(sf::Color::Red);
    selectionHighlight.setOutlineThickness(1.f);
}

void EditorWindow::update(const sf::Time &dt, EditorContext &context) {
    handleEvent(context);
    updateCamera(context,dt);
    render(context,context.camera->getView());
}

void EditorWindow::render(EditorContext &context, sf::View &view) {
    window.clear();
    window.setView(view);
   // ImGui::SFML::Render(window);


   context.layerManager->renderAll_interleavedBatching(window,*context.assetManager,*context.entityManager);
    drawDebug(context);
    highlightTileOnHover(mouseTileCoord);
    window.draw(isoGrid);

    //  ImGui::SFML::Render(window);
    window.display();
}

void EditorWindow::handleEvent(EditorContext &context) {
    sf::Event event;
     while (window.pollEvent(event)) {

         if (event.type == sf::Event::MouseMoved) {

             sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
             sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
             float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
             float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

             std::cout << "TILE COORDINATE" << x << ", " << y << ")\n";

             mouseTileCoord  = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};


         }

        if(event.type == sf::Event::Resized)
         {

           context.camera->resetView(event.size.width, event.size.height);
         }

            if (event.type == sf::Event::Closed)
                window.close();


            if (context.currentMode == Mode::Place) {

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {


                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                        float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
                        float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

                        std::cout << "TILE COORDINATE" << x << ", " << y << ")\n";

                        sf::Vector2i tileCoord = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};

                        std::string selectedSpriteID =  context.selectedSprite.textureID;

                        auto it = context.assetManager->getTileTextureLookup().find(selectedSpriteID);//currentSpriteSelection.textureID);
                        if (it != context.assetManager->getTileTextureLookup().end()) {
                            // Key exists
                            if (context.currentLayer)
                                context.currentLayer->addTile(tileCoord,selectedSpriteID, context.selectedSprite.textureRect);
                            else {
                                std::cout << "context current layer null" << std::endl;
                            }

                        } else {
                        }


                }


            }

            if (context.currentMode == Mode::Edit) {

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace) {

                    if (context.selectedTile) {
                        std::cout << "DELETING TILE" << context.selectedTile->textureID << context.selectedTile->gridPos.x << context.selectedTile->gridPos.y << std::endl;
                        context.currentLayer->removeTile(context.selectedTile->gridPos);
                        context.selectedTile = nullptr;
                     //   currentLayer->buildVertexBatches();

                    }

                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    std::cout << "EDIT MODE" << worldPos.x << ", " << worldPos.y << ")\n";





                    context.selectedTile = context.currentLayer->getTileAtPixelPrecise(worldPos);
                    //context.selectedTile = selectedTile;



                       if(context.selectedTile) {

                            context.isDragging = true;
                            //context.dragOffset = worldPos;

                            break;

                        }

                }
                // Mouse Release
                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    if (context.isDragging && context.selectedTile) {


                        //sf::Vector2i newGridPos = isoToCart(finalIso.x, finalIso.y);
                        sf::Vector2i oldGridPos = context.selectedTile->gridPos;

                        if (newPos != oldGridPos) {
                            context.currentLayer->moveTile(oldGridPos, newPos); // encapsulated movement logic
                          //  currentLayer->buildVertexBatches();
                        }

                        context.isDragging = false;
                        //selectedTile = nullptr;
                    }
                    }

                // Mouse Moved
                if (event.type == sf::Event::MouseMoved && context.isDragging && context.selectedTile) {
                    std::cout << "Dragging" << std::endl;

                    sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
                   newPos = Math::cartToIso(mousePos.x, mousePos.y);
                    }
                }

             if (context.currentMode == Mode::EditCollider) {

                if (event.type == sf::Event::MouseButtonPressed &&
                   event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    // std::cout << "EDIT MODE" << worldPos.x << ", " << worldPos.y << ")\n";

                    // === First: Try to select a vertex ===
                    const float grabRadius = 8.0f;
                    for (auto& region : context.world->getEventManager().getEventRegions()) {
                        auto& verts = region->getPolygon(); // You may need to make this non-const or add a setter
                        std::cout << "world pos" << worldPos.x << ", " << worldPos.y << ")\n";
                        for (int i = 0; i < verts.size(); ++i) {
                            sf::Vector2f vert = verts[i];
                            float dist = std::sqrt(Math::distance2(worldPos, vert));

                            if (dist < grabRadius) {
                                std::cout << dist << std::endl;

                                selectedArea = region.get();
                                selectedVertexIndex = i;
                                context.isDragging = true;

                            }
                        }
                    }


                    for (auto& collider : context.world->getCollisionManager().getColliders()) {
                        auto& verts = collider->getVertices(); // You may need to make this non-const or add a setter
                        std::cout << "world pos" << worldPos.x << ", " << worldPos.y << ")\n";
                        for (int i = 0; i < verts.size(); ++i) {
                            sf::Vector2f vert = verts[i];
                            float dist = std::sqrt(Math::distance2(worldPos, vert));

                            if (dist < grabRadius) {
                                std::cout << dist << std::endl;

                                selectedCollider = collider.get();
                                selectedVertexIndex = i;
                                context.isDragging = true;

                            }
                        }
                    }
                }

                if (event.type == sf::Event::MouseMoved && context.isDragging) {
                    sf::Vector2f worldPos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}, context.camera->getView());

                    if (selectedCollider && selectedVertexIndex >= 0) {
                        std::cout << "DRAGGING COLLIDER" << std::endl;

                        selectedCollider->getVertices()[selectedVertexIndex] = worldPos;
                    }

                   else if (selectedArea && selectedVertexIndex >= 0) {
                        std::cout << "DRAGGING COLLIDER" << std::endl;

                        selectedArea->getPolygon()[selectedVertexIndex] = worldPos;
                    }
                }


                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (context.isDragging) {
                        if (selectedCollider) {
                            selectedCollider = nullptr;
                            selectedVertexIndex = -1;
                        }
                        if (selectedArea) {
                            selectedArea = nullptr;
                            selectedVertexIndex = -1;
                        }
                        context.isDragging = false;
                    }
                }
            }
            }
}

bool EditorWindow::isOpen() const {
    return window.isOpen();
}

void EditorWindow::processClick(const sf::Vector2f &worldPos, EditorContext &context) {
}

void EditorWindow::drawDebug(EditorContext &context) {
    drawColliders(context);
    drawEventRegions(context);
    drawPlayerDebug(context);
}

void EditorWindow::drawColliders(EditorContext &context) {
    context.world->getCollisionManager().drawColliders(window);
}

void EditorWindow::drawEventRegions(EditorContext &context) {
    context.world->getEventManager().drawDebug(window, sf::Color::White);
}

void EditorWindow::highlightTileOnHover(sf::Vector2i pos) {

    sf::Vector2f temp = Math::isoToCart(pos.x, pos.y);
    float x = temp.x;
    float y= temp.y;


    std::vector<sf::Vector2f> vertices = {
        {x, y},  // top-left
        {x+16.f, y+8.f},  // top-right
        {x, y+16.f},  // bottom-right
        {x-16.f, y+8.f}   // bottom-left
    };

    // Create a vertex array with 4 points (Quad)
    sf::VertexArray quad(sf::Quads, 4);

    // Set positions and color
    for (size_t i = 0; i < 4; ++i) {
        quad[i].position = vertices[i];
        quad[i].color = sf::Color(255, 0, 0, 128);
    }

    window.draw(quad);

}

void EditorWindow::updateCamera(EditorContext &context,const sf::Time& dt) {
    context.camera->handleInput(window, dt.asSeconds());
    context.camera->setCenter(context.world->getEntityManager().getPlayer()->getTileInstance()->worldOffset);
    context.camera->apply(window);
}
