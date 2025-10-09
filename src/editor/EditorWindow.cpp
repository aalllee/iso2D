//
// Created by User on 30/05/2025.
//

#include "EditorWindow.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "math.h"
#include "util/math.h"
#include <algorithm>
#include <sstream>

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
    drawTileDebugInfo(context,mouseTileCoord);
    window.draw(isoGrid);
    if (context.drawDebug) drawDebug(context);
    //drawDebug(context);

    //  ImGui::SFML::Render(window);
    window.display();
}

void EditorWindow::handleEvent(EditorContext &context) {
    sf::Event event;
     while (window.pollEvent(event)) {

         if (event.type == sf::Event::KeyPressed) {
             if (event.key.code == sf::Keyboard::P) {
                 fillSelectionsWithTile(context);
                 deselectTiles(context);
             }
             if (event.key.code == sf::Keyboard::BackSpace) {
                 deleteSelectedTiles(context);
                 selectedTiles.clear();
                 deselectTiles(context);
             }
         }

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


            if (context.currentMode == Mode::Universal) {

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {

                    selectedTiles.clear();
                }



                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
                    float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

                    sf::Vector2i tileCoord = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};
                    bool moveKeyHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
                    if (moveKeyHeld && !selectedTiles.empty()) {
                        // Check if clicking inside selection
                        if (selectedTiles.count({tileCoord.x, tileCoord.y})) {
                            isMovingTiles = true;
                            moveStartTileCoord = tileCoord;
                            moveOffset = {0, 0};
                            context.isDragging = true;
                            std::cout << "MOVE MODE: Started moving tiles" << std::endl;
                            movingTiles.clear();
                            for (auto& tile : selectedTiles) {
                                sf::Vector2i tileCoord = {tile.first,tile.second};
                                TileInstance* selectedTile =  context.currentLayer->getTile(tileCoord);

                                if (selectedTile) {
                                    movingTiles.insert({tileCoord.x, tileCoord.y});
                                }
                            }
                            selectedTiles.clear();
                        }
                    }
                    else {

                        if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)||sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
                            // Check if the Shift key is not pressed
                            deselectTiles(context);
                            std::cout << "SHIFT NOT PRESSED" << std::endl;
                        }

                        selectedTiles.insert({static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y))});
                        context.isDragging = true;
                        context.placePosStart = tileCoord;
                    }





                }

                if (event.type == sf::Event::MouseMoved && context.isDragging) {




                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, context.camera->getView());
                    float x = (worldPos.y / (16 / 2) + worldPos.x / (32 / 2)) / 2.f;
                    float y = (worldPos.y / (16 / 2) - worldPos.x / (32 / 2)) / 2.f;

                    sf::Vector2i tileCoord = {static_cast<int>(std::floor(x)),static_cast<int>(std::floor(y))};

                    if (isMovingTiles) {
                        // Calculate offset from start position
                        moveOffset.x = tileCoord.x - moveStartTileCoord.x;
                        moveOffset.y = tileCoord.y - moveStartTileCoord.y;
                        moveTilesPreview(context);
                    }
                    else {
                        selectedTiles_temp.clear();
                        // Determine the rectangle boundaries
                        int x1 = context.placePosStart.x;
                        int y1 = context.placePosStart.y;
                        int x2 = tileCoord.x;
                        int y2 = tileCoord.y;
                        int xmin = std::min(x1, x2);
                        int xmax = std::max(x1, x2);
                        int ymin = std::min(y1, y2);
                        int ymax = std::max(y1, y2);



                        // Nested loops to iterate through the rectangle
                        for (int i = xmin; i <= xmax; ++i) {
                            for (int j = ymin; j <= ymax; ++j) {
                                sf::Vector2i tile = {i,j};
                                selectedTiles_temp.insert({i,j});
                            }
                        }

                    }



                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    context.isDragging = false;

                    if (isMovingTiles) {
                        // COMMIT THE MOVE
                        if (context.currentLayer && (moveOffset.x != 0 || moveOffset.y != 0)) {
                            moveTiles(context);
                            movingTiles.clear();
                        }
                        isMovingTiles = false;
                        }
                    else{

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                            for (auto x : selectedTiles_temp) {
                                selectedTiles.erase(x);
                            }
                        }
                        else {
                            selectedTiles.insert( selectedTiles_temp.begin(), selectedTiles_temp.end());
                        }


                        selectedTiles_temp.clear();
                    }
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


    if (selectedTiles.size() > 0) {

        for (const auto& s : selectedTiles) {
            sf::Vector2f temp = Math::isoToCart(s.first, s.second);
            float x= temp.x;
            float y= temp.y;
            std::vector<sf::Vector2f> vertices = {
                {x, y},  // top-left
                {x+16.f, y+8.f},  // top-right
                {x, y+16.f},  // bottom-right
                {x-16.f, y+8.f}   // bottom-left
            };

            // Create a vertex array with 4 points (Quad)
            sf::VertexArray quad(sf::Quads, 4);


            TileInstance* tile = context.currentLayer->getTile(sf::Vector2i(s.first,s.second));
            if (tile) {
                tile->color.r = 255;
                tile->color.g = 0;
                tile->color.b = 100;
            }

            // Set positions and color
            for (size_t i = 0; i < 4; ++i) {
                quad[i].position = vertices[i];
                quad[i].color = sf::Color(255, 200, 0, 128);
            }

            window.draw(quad);

        }

        for (const auto& s : selectedTiles_temp) {
            sf::Vector2f temp = Math::isoToCart(s.first, s.second);
            float x= temp.x;
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
                quad[i].color = sf::Color(255, 200, 0, 128);
            }

            window.draw(quad);

        }


    }
}

void EditorWindow::fillSelectionsWithTile(EditorContext &context) {
    if (!context.selectedSprite) return;
    std::string selectedSpriteID = context.selectedSprite->textureID;

    const auto& lookup = context.assetManager->getTileTextureLookup();
    auto it = lookup.find(selectedSpriteID);

    if (it != lookup.end()) {
        // Key exists
        sf::Vector2f worldOffset = it->second.worldOffset;

        for (auto& tile : selectedTiles) {
            if (context.currentLayer) {
                sf::Vector2i tileCoord = sf::Vector2i(tile.first, tile.second);
                context.currentLayer->addTile(tileCoord, selectedSpriteID, context.selectedSprite->textureRect, {0,0}, worldOffset);
            } else {
                std::cout << "context current layer null" << std::endl;
            }
        }
    } else {
        std::cout << "Sprite ID not found in asset lookup: " << selectedSpriteID << std::endl;
    }
}

void EditorWindow::deselectTiles(EditorContext &context) {
    for (const auto& s : selectedTiles) {
        TileInstance* tile = context.currentLayer->getTile(sf::Vector2i(s.first,s.second));
        if (tile) {
            tile->color.r = 255;
            tile->color.g = 255;
            tile->color.b = 255;
        }

    }

    selectedTiles.clear();

}



void EditorWindow::moveTiles(EditorContext &context) {

      if (!context.currentLayer) return;

      // Step 1: Check if move is valid (destinations occupied by non-movingtiles)
      for (const auto& [tileX, tileY] : movingTiles) {
          sf::Vector2i destination = {tileX + moveOffset.x, tileY +moveOffset.y};

          // Check if destination is occupied
          TileInstance* destinationTile = context.currentLayer->getTile(destination);

          if (destinationTile) {
              // Destination occupied - check if it's part of the moving set
              bool isSelfIntersection = movingTiles.count({destination.x,destination.y}) > 0;

              if (!isSelfIntersection) {
                  // Blocked by external tile
                  std::cout << "MOVE BLOCKED: External tile at destination" << std::endl;

                  // Reset preview
                  for (const auto& [x, y] : movingTiles) {
                      TileInstance* tile = context.currentLayer->getTile({x,y});
                      if (tile) {
                          sf::Color color(255, 255, 255, 255);
                            tile->color= color;
                          tile->worldOffset = {0, 0};
                      }
                  }
                  return;
              }
          }
      }

      // Step 2: Extract all tile data (snapshot before removal)
      struct TileData {
          sf::Vector2i oldPos;
          sf::Vector2i newPos;
          std::string textureID;
          sf::IntRect textureRect;
          sf::Vector2f anchorOffset;
          sf::Vector2f worldOffset;
          sf::Color color;
      };

      std::vector<TileData> tileDataList;

      for (const auto& [x, y] : movingTiles) {
          sf::Vector2i oldPos = {x, y};
          TileInstance* tile = context.currentLayer->getTile(oldPos);

          if (tile) {
              TileData data;
              data.oldPos = oldPos;
              data.newPos = {x + moveOffset.x, y + moveOffset.y};
              data.textureID = tile->textureID;
              data.textureRect = tile->textureRect;
              data.anchorOffset = tile->anchorOffset;
              data.worldOffset = {0, 0}; // Reset offset after move
              data.color = tile->color;

              tileDataList.push_back(data);
          }
      }

      // Step 3: Remove all tiles from old positions
      for (const auto& data : tileDataList) {
          context.currentLayer->removeTile(data.oldPos);
      }

      // Step 4: Re-add all tiles at new positions
      for (const auto& data : tileDataList) {
          context.currentLayer->addTile(
              data.newPos,
              data.textureID,
              data.textureRect,
              data.anchorOffset,
              data.worldOffset
          );

          // Restore color if needed
          TileInstance* newTile = context.currentLayer->getTile(data.newPos);
          if (newTile) {
              newTile->color = data.color;
          }
      }

      // Step 5: Update selection to new positions
      selectedTiles.clear();
      for (const auto& data : tileDataList) {
          selectedTiles.insert({data.newPos.x, data.newPos.y});
      }

      std::cout << "MOVED " << tileDataList.size() << " tiles by offset ("
                << moveOffset.x << ", " << moveOffset.y << ")" << std::endl;
  }

/*
    bool moveValid = true;
        for (auto& tile : movingTiles) {

                sf::Vector2i tileCoord = sf::Vector2i(tile.first+moveOffset.x,tile.second+moveOffset.y);
              TileInstance* destinationTile =  context.currentLayer->getTile(tileCoord);




            std::pair<int, int> target = {tile.first+moveOffset.x, tile.second+moveOffset.y};

            bool selfIntersection = false;
            if (movingTiles.find(target) != movingTiles.end()) {
                std::cout << "Exists!\n";
                selfIntersection = true;
            } else {
                std::cout << "Does not exist.\n";
            }

            if (destinationTile && !selfIntersection) {
                moveValid = false;

            }


        }

    for (auto& tile : movingTiles) {

        sf::Vector2i startTileCoord = sf::Vector2i(tile.first,tile.second);
        TileInstance* startTile =  context.currentLayer->getTile(startTileCoord );
        startTile->worldOffset = {0,0};
    }
//self intersection moves are being canceled by moveTile fn !!!!!!!!!!!!!!
    if (moveValid) {
        for (auto& tile : movingTiles) {
            sf::Vector2i start = sf::Vector2i(tile.first,tile.second);
            sf::Vector2i end = sf::Vector2i(tile.first+moveOffset.x,tile.second+moveOffset.y);

            TileInstance* tp = context.currentLayer->getTile(end);
            TileInstance temp = context.currentLayer->getTile(end);
            //context.currentLayer->moveTile(start,end);
        }
    }


}
*/
void EditorWindow::moveTilesPreview(EditorContext &context) {
    for (auto& tile : movingTiles) {
        sf::Vector2i tileCoord = sf::Vector2i(tile.first,tile.second);
        TileInstance* startTile =  context.currentLayer->getTile(tileCoord);

        startTile->worldOffset = Math::isoToCart(moveOffset.x, moveOffset.y);






    }

}

void EditorWindow::drawTileDebugInfo(EditorContext &context, sf::Vector2i debugInspectedCoord) {
    if (!context.showTileDebug || !context.currentLayer) return;

    TileInstance* tile = context.currentLayer->getTile(debugInspectedCoord);

    // Get view bounds for positioning in corner
    sf::Vector2f viewCenter = context.camera->getView().getCenter();
    sf::Vector2f viewSize = context.camera->getView().getSize();

    // Top-left corner of the view
    float left = viewCenter.x - viewSize.x / 2.f;
    float top = viewCenter.y - viewSize.y / 2.f;

    float pw = viewSize.x * 0.2f;
    float ph = viewSize.x * 0.2f;



    // Background panel
    sf::RectangleShape panel({pw, ph});
    panel.setPosition(left , top );
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    // Text setup with scaled font
    sf::Text text;
    text.setFont(context.assetManager->getDebugFont());
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);
    text.setPosition(left, top);

    // Counter camera zoom to keep text at constant size
    float cameraZoom = viewSize.x / 800.0f;  // 800 = default view width
    text.setScale(cameraZoom, cameraZoom);

    std::stringstream ss;
    ss << "=== TILE DEBUG ===\n";
    ss << "Pos: (" << debugInspectedCoord.x << "," << debugInspectedCoord.y << ")\n";
    ss << "Layer: " << context.selectedLayerId << "\n";

    if (tile) {
        // Truncate long texture IDs
        std::string texID = tile->textureID;
        if (texID.length() > 15) {
            texID = texID.substr(0, 12) + "...";
        }
        ss << "Tex: " << texID << "\n";
        ss << "Rect: " << tile->textureRect.width << "x" << tile->textureRect.height << "\n";
        ss << "WO: (" << (int)tile->worldOffset.x << "," << (int)tile->worldOffset.y << ")\n";
        ss << "Anchor: (" << (int)tile->anchorOffset.x << "," << (int)tile->anchorOffset.y << ")\n";
        ss << "RGBA: (" << (int)tile->color.r << "," << (int)tile->color.g
           << "," << (int)tile->color.b << "," << (int)tile->color.a << ")";
    } else {
        ss << "\n[EMPTY]";
    }

    text.setString(ss.str());
    window.draw(text);

}

void EditorWindow::updateCamera(EditorContext &context, const sf::Time& dt) {
    if (!context.camera) return;

    context.camera->handleInput(window, dt.asSeconds());

    // Safe null-checking chain
    if (context.world) {
        Player* player = context.world->getEntityManager().getPlayer();
        if (player) {
            TileInstance* tileInst = player->getTileInstance();
            if (tileInst) {
                context.camera->setCenter(tileInst->worldOffset);
            }
        }
    }

    context.camera->apply(window);
}
