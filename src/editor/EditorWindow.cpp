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

    highlightTileOnHover(mouseTileCoord);
    window.draw(isoGrid);
    if (context.drawDebug) drawDebug(context);
    //drawDebug(context);

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

            // std::cout << "TILE COORDINATE" << x << ", " << y << ")\n";

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

                        context.isDragging = true;
                        std::cout << "PLACE MODE, LMB PRESSED EVENT" << std::endl;
                       sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
                    float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

                    std::cout << "TILE COORDINATE" << x << ", " << y << ")\n";

                   context.placePosStart = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};

                }



                if (context.isDragging) {

                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
                    float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

                    std::cout << "TILE COORDINATE" << x << ", " << y << ")\n";

                    sf::Vector2i tileCoord = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};

                    std::string selectedSpriteID =  context.selectedSprite.textureID;

                    int startX = context.placePosStart.x;
                    int startY = context.placePosStart.y;
                    int endX = tileCoord.x;
                    int endY = tileCoord.y;
                    int dx,dy = 1;
                    if (startX >= endX) {
                        startX = endX;
                        endX = context.placePosStart.x;

                    }
                    if (startY >= endY) {
                        startY = endY;
                        endY = context.placePosStart.y;
                    }

                    for (int i=startX; i<endX; i++) {
                        for (int j=startY; j<endY; j++) {
                            auto it = context.assetManager->getTileTextureLookup().find(selectedSpriteID);//currentSpriteSelection.textureID);
                            if (it != context.assetManager->getTileTextureLookup().end()) {
                                // Key exists
                                if (context.currentLayer) {
                                    tileCoord = sf::Vector2i(i,j);
                                    context.currentLayer->addTile(tileCoord,selectedSpriteID, context.selectedSprite.textureRect);
                                } else {
                                    std::cout << "context current layer null" << std::endl;
                                }

                            }

                        }
                    }



                }

                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    context.isDragging = false;
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

                 if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace) {
                     if (selectedVertexIndex > -1 && selectedCollider) {

                         selectedCollider->removeVertex(selectedVertexIndex);
                     }

                 }


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
                                context.selectedEventRegion_p = region.get();
                                selectedVertexIndex = i;

                                context.isDragging = true;
                                break;

                            }

                            selectedVertexIndex = -1;

                        }
                    }

                   auto it = context.world->getCollisionManager().getColliderMap().find(context.selectedCollider);
                   if ( it != context.world->getCollisionManager().getColliderMap().end()) {
                       auto& verts = it->second->getVertices();
                       for (int i = 0; i < verts.size(); ++i) {
                           sf::Vector2f vert = verts[i];
                           float dist = std::sqrt(Math::distance2(worldPos, vert));

                           if (dist < grabRadius) {
                               std::cout << dist << std::endl;

                               selectedCollider = it->second;
                               selectedVertexIndex = i;
                               context.isDragging = true;
                                break;
                               //it->second->

                           }
                           selectedVertexIndex = -1;

                       }


                   }




                }

                if (event.type == sf::Event::MouseMoved && context.isDragging) {
                    sf::Vector2f worldPos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}, context.camera->getView());

                    if (selectedCollider && selectedVertexIndex >= 0) {
                        std::cout << "DRAGGING COLLIDER" << std::endl;


                        if (context.enableGridSnapping) {
                            sf::Vector2i snapped = Math::cartToIso(worldPos.x,worldPos.y);
                            sf::Vector2f snappedWorld = Math::isoToCart((float)snapped.x,(float)snapped.y);
                           selectedCollider->getVertices()[selectedVertexIndex] = {snappedWorld.x,snappedWorld.y};
                        }
                        else {
                            selectedCollider->getVertices()[selectedVertexIndex] = worldPos;

                        }

                    }

                   else if (context.selectedEventRegion_p && selectedVertexIndex >= 0) {
                        std::cout << "DRAGGING COLLIDER" << std::endl;

                        context.selectedEventRegion_p->getPolygon()[selectedVertexIndex] = worldPos;
                    }
                }


                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (context.isDragging) {
                        if (selectedCollider) {
                            //selectedCollider = nullptr;
                          //  selectedVertexIndex = -1;
                        }
                        if (selectedArea) {
                          //  selectedArea = nullptr;
                          //  selectedVertexIndex = -1;
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
    drawSelectionHighlight(context);
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

bool EditorWindow::isColliderVertClicked(sf::Vector2f mouse) {

}

void EditorWindow::drawSelectionHighlight(EditorContext &context) {

    if (context.selectedCollider_p) {
        if (selectedVertexIndex >= 0 ) {
            sf::Vector2f selectedVert = context.selectedCollider_p->getVertices()[selectedVertexIndex];

            // Draw circle handles on each vertex
            const float radius = 4.0f;
            sf::CircleShape handle(radius);
            handle.setOrigin(radius, radius); // center the circle
            handle.setFillColor(sf::Color::Red);
            handle.setOutlineColor(sf::Color::Black);
            handle.setOutlineThickness(1.0f);
            handle.setPosition(selectedVert.x, selectedVert.y);
            window.draw(handle);
        }
    }

    if(context.selectedTile){


            if (context.selectedTile) {

                TileInstance* tile = context.selectedTile;
                sf::Vector2f isoPos = Math::isoToCart(tile->gridPos.x, tile->gridPos.y);
                float w = static_cast<float>(tile->textureRect.width);
                float h = static_cast<float>(tile->textureRect.height);


               // std::cout << "GRID POS" <<tile->gridPos.x << " " << tile->gridPos.y << " " << std::endl;

                sf::FloatRect bounds(isoPos.x - w / 2.f, isoPos.y, w, h);
               // std::cout << "BOUNDS:" <<bounds.left << " " << bounds.top << " " << bounds.width << " " << bounds.height << std::endl;
                //std::cout << "MOUSE WORLD POS" <<worldPos.x << " " << worldPos.y << " " << std::endl;
              //  std::cout << "ISO POS" <<isoPos.x << " " << isoPos.y << " " << std::endl;





                // Create a rectangle shape with size 120x60
                sf::RectangleShape rectangle(sf::Vector2f(w, h));
                rectangle.setPosition(isoPos.x - w / 2.f, isoPos.y - h/2.f);
                rectangle.setFillColor(sf::Color::Transparent);
                rectangle.setOutlineThickness(1.0f);
                rectangle.setOutlineColor(sf::Color::Red);

                window.draw(rectangle);


                sf::Vector2i aWorld = Math::cartToIso(tile->worldOffset.x,tile->worldOffset.y);
                sf::Vector2f cartPos = Math::isoToCart(tile->gridPos.x, tile->gridPos.y);


                int za = cartPos.x + tile->anchorOffset.x  + aWorld.x;
                int zy =  cartPos.y +  tile->anchorOffset.y  + aWorld.y;





                // Create a circle shape with radius 50
                sf::CircleShape circle(2.0f);  // radius
                circle.setFillColor(sf::Color::Green);
                sf::Vector2f worldOffset = tile->worldOffset;
                // Set position (x, y)
                circle.setPosition(za, zy);
                std::cout << tile->zOrder.x<< std::endl;
                std::cout << tile->zOrder.y<< std::endl;
                // Set fill color
                circle.setFillColor(sf::Color::Green);
                window.draw(circle);// Draw the circle





        }
    }
}

void EditorWindow::updateCamera(EditorContext &context,const sf::Time& dt) {
    context.camera->handleInput(window, dt.asSeconds());
    context.camera->setCenter(context.world->getEntityManager().getPlayer()->getTileInstance()->worldOffset);
    context.camera->apply(window);
}
