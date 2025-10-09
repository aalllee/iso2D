//
// Created by User on 27/05/2025.
//

#include "SpriteWindow.h"
#include "WorldSerializer.h"
#include <iostream>

SpriteWindow::SpriteWindow(sf::Vector2u size, const std::string &title)
  :window(sf::VideoMode(size.x, size.y), title) {
    ImGui::SFML::Init(window);
    view.setSize(static_cast<float>(size.x), static_cast<float>(size.y));
    view.setCenter(view.getSize() / 2.f);
    window.setView(view);

    selectionHighlight.setFillColor(sf::Color::Transparent);
    selectionHighlight.setOutlineColor(sf::Color::Red);
    selectionHighlight.setOutlineThickness(1.f);

}

void SpriteWindow::SaveLoadUI(EditorContext& context) {

    if (context.world == nullptr) {
        ImGui::Text("WORLD IS NULL");
        return;
    }

    static char filenameBuf[128] = "map.json";


    ImGui::InputText("Filename", filenameBuf, IM_ARRAYSIZE(filenameBuf));

    if (ImGui::Button("Save World")) {
        WorldSerializer::saveToFile(*context.world, filenameBuf);
    }
    ImGui::SameLine();

    if (ImGui::Button("Load World")) {
        WorldSerializer::loadFromFile(*context.world, filenameBuf, context.assetManager);
    }

}

void SpriteWindow::update(const sf::Time &dt, EditorContext &context) {

    ImGui::SFML::Update(window, dt);

    ImGui::Begin("Editor");

    int mode = static_cast<int>(context.currentMode);
    ImGui::Text("Editor mode:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Collider", mode == 2)) context.currentMode = Mode::EditCollider;
    ImGui::SameLine();
    if (ImGui::RadioButton("Universal", mode==3)) context.currentMode = Mode::Universal;



   if (ImGui::BeginTabBar("MyTabBar")) {
        if (ImGui::BeginTabItem("Layers")) {
            renderLayerUI(context);
            ImGui::EndTabItem();
        }

       if (context.selectedSprite) {

           if (ImGui::BeginTabItem("Sprite Properties")) {
               // Safe lookup - check if key exists
               const auto& lookup = context.assetManager->getTileTextureLookup();
               auto it = lookup.find(context.selectedSprite->textureID);

               if (it != lookup.end()) {
                   sf::Vector2f wo = it->second.worldOffset;

                   float wo_temp[2] = {wo.x, wo.y};
                   ImGui::SetNextItemWidth(100.0f);
                   if (ImGui::InputFloat2("World Offset", wo_temp)) {

                       sf::Vector2f newWorldOffset = {wo_temp[0], wo_temp[1]};

                       context.assetManager->updateDefaultWorldOffset(context.selectedSprite->textureID, newWorldOffset);

                       for (auto& layer : context.layerManager->getLayers()) {
                            for (auto& tile : layer.second.getTilePool()) {
                                if (tile->textureID == context.selectedSprite->textureID) {
                                    tile->worldOffset = newWorldOffset;
                                }
                            }
                       }
                   }
               } else {
                   ImGui::Text("Sprite not found in asset registry: %s", context.selectedSprite->textureID.c_str());
               }

               ImGui::EndTabItem();
           }
       }

           if (ImGui::BeginTabItem("Save / Load")) {
               SaveLoadUI(context);
               ImGui::EndTabItem();
           }


        ImGui::EndTabBar();
    }





    if (context.entityManager && context.entityManager->getPlayer()) {
        int playerRenderLayer = context.entityManager->getPlayer()->getRenderLayer();
        if (ImGui::InputInt("player Render layer", &playerRenderLayer)) {

           context.entityManager->getPlayer()->setRenderLayer(playerRenderLayer);
        }
    }



    ImGui::End();


    context.currentLayer = context.layerManager->getLayer(context.selectedLayerId);


}

bool SpriteWindow::isOpen() const {
    return window.isOpen();
}

void SpriteWindow::renderSpriteList(EditorContext &context) {
    if (context.assetManager == nullptr) return;
    if ( cachedSpriteList) {
        for (int i=0; i<sprites.size(); i++) {
            window.draw(sprites[i]);
            sf::FloatRect bounds = sprites[i].getGlobalBounds();
            sf::RectangleShape rect;
            rect.setPosition(bounds.left, bounds.top);
            rect.setSize({bounds.width, bounds.height});
            rect.setFillColor(sf::Color::Transparent);



            if ( context.selectedSprite &&  spriteViewTiles[i].textureID == context.selectedSprite->textureID) {

                rect.setSize({(float)context.selectedSprite->textureRect.width, (float)context.selectedSprite->textureRect.height}) ;
                //  rect.setPosition(customGridPos.x + x, customGridPos.y + y);
                rect.setOutlineColor(sf::Color::Green);
            }
            else {
                rect.setOutlineColor(sf::Color::Blue);
            }

           // x+= sprite.getGlobalBounds().width + 10;

            rect.setOutlineThickness(1.0f);

            window.draw(rect);


        }



        return;
    }

    sprites.clear();
    spriteViewTiles.clear();
    int x=0,y =0;




    std::cout << "spriteViewTiles Size: " << spriteViewTiles.size() << std::endl;

    for (const auto& pair : context.assetManager->getTileTextureLookup()) {
        sf::Sprite sprite(context.assetManager->getTexture(pair.second.textureID));

        TileInstance spriteData;
        spriteData.gridPos = {x,y};
        spriteData.textureID = pair.first; //sprite texture ID (grass, stone, ..)
        spriteData.textureRect = pair.second.texRect;
        spriteData.worldOffset = pair.second.worldOffset;
        sprite.setTextureRect(pair.second.texRect);

        sprite.setPosition(x, y);
        sprites.push_back(sprite);
        spriteViewTiles.push_back(spriteData);

       // window.draw(sprite);




        sf::FloatRect bounds = sprite.getGlobalBounds();



        sf::RectangleShape rect;
        rect.setPosition(bounds.left, bounds.top);
        rect.setSize({bounds.width, bounds.height});
        rect.setFillColor(sf::Color::Transparent);



        if ( context.selectedSprite && pair.first == context.selectedSprite->textureID) {

            rect.setSize({(float)context.selectedSprite->textureRect.width, (float)context.selectedSprite->textureRect.height}) ;
          //  rect.setPosition(customGridPos.x + x, customGridPos.y + y);
            rect.setOutlineColor(sf::Color::Green);
        }
        else {
            rect.setOutlineColor(sf::Color::Blue);
        }

        x+= sprite.getGlobalBounds().width + 10;

        rect.setOutlineThickness(1.0f);

        window.draw(rect);  // Draw the bounding box
    }

    cachedSpriteList = true;
}

void SpriteWindow::renderLayerUI(EditorContext &context) {

        if (context.layerManager == nullptr) return;

        const auto& layers = context.layerManager->getLayers();
        const auto& drawOrder = context.layerManager->getDrawOrder();

        std::map<int,bool> isExpanded;




        for (int index = 0; index < drawOrder.size(); ++index) {
            int layerId = drawOrder[index];
            isExpanded[layerId] = false;
            ImGui::PushID(layerId);

            bool selected = (context.selectedLayerId == layerId);
            std::string label = "Layer " + std::to_string(layerId);
            ImGui::SetNextItemWidth(150.0f); // Set width to 150 pixels
            if (ImGui::Selectable(label.c_str(), selected)) {
                context.selectedLayerId = layerId;
                context.currentLayer = context.layerManager->getLayer(context.selectedLayerId);

                isExpanded[layerId] = !isExpanded[layerId];
            }
            TileLayer* layer = context.layerManager->getLayer(layerId);
            bool visible = true;
            if (layer) visible = layer->getRenderViewport();

           // ImGui::SameLine();
            if (ImGui::Checkbox("visible", &visible)) {


                if (layer) layer->setRenderViewport(visible);
            }

            ImGui::SameLine();
            if (ImGui::ArrowButton("Up", ImGuiDir_Up)) {
                context.layerManager->moveLayerUp(layerId);
            }
            ImGui::SameLine();
            if (ImGui::ArrowButton("Down", ImGuiDir_Down)) {
                context.layerManager->moveLayerDown(layerId);
            }
            ImGui::SameLine();
            float fvalue = context.layerManager->getOpacityMap()[layerId];
            ImGui::SetNextItemWidth(100.0f); // Set width to 150 pixels
            ImGui::SliderFloat("My Float Slider", &fvalue, 0.0f, 1.0f);
            context.layerManager->getOpacityMap()[layerId] = fvalue;

            std::vector<std::string> elements = {"element1","element2"};
            if (selected) {

                    ImGui::Indent();

                for (auto& element : context.world->getCollisionManager().getColliderMap()) {
                    std::string label = "Collision " + std::to_string(element.first);
                    bool selectedCollider = context.selectedCollider == element.first;
                    element.second->setIsSelected(selectedCollider);

                    ImGui::PushID(element.first);
                    if (ImGui::Selectable(label.c_str(),selectedCollider)) {
                        context.selectedCollider = element.first;
                        context.selectedCollider_p = element.second;

                    }
                    ImGui::PopID();


                }

                for (auto& element : context.world->getEventManager().getEventRegionMap()) {
                    std::string label = "Event Region " + std::to_string(element.first);
                    bool selectedEventRegion = context.selectedEventRegion == element.first;
                    element.second->setIsSelected(selectedEventRegion);
                    ImGui::PushID(element.first);
                    if (ImGui::Selectable(label.c_str(),selectedEventRegion)) {
                        context.selectedEventRegion = element.first;
                        context.selectedEventRegion_p = element.second;
                    }
                    ImGui::PopID();


                }
                    ImGui::Unindent();

                if (ImGui::Button("Delete")) {
                    context.layerManager->removeLayer(layerId);
                    context.selectedLayerId = -1;
                    ImGui::PopID();
                    break; // Avoid iterator invalidation
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        if (ImGui::Button("Add Layer")) {
            context.selectedLayerId = context.layerManager->createLayer();
        }

    //ImGui::InputInt("vertex count", &playerRenderLayer);
     if (ImGui::Button("Add Collider")) {
        std::vector<sf::Vector2f> colliderVerts ={{20,20}, {50,20}, {50,50},{20,50}};
        context.world->getCollisionManager().createCollider(colliderVerts);
    }

    ImGui::SameLine();

    if (ImGui::Button("remove vert")) {
        std::vector<sf::Vector2f> colliderVerts ={{20,20}, {50,20}, {50,50},{20,50}};
        context.world->getCollisionManager().createCollider(colliderVerts);
    }

    if (ImGui::Button("Remove Collider") ) {
        context.world->getCollisionManager().deleteCollider(context.selectedCollider);
        context.selectedCollider_p = nullptr;
    }

    ImGui::Separator();

    if (ImGui::Button("Add Event Region")) {
        std::vector<sf::Vector2f> verts ={{-20,20}, {20,20}, {20,-20},{-20,-20}};
        context.world->getEventManager().addEventRegion(verts);
    }
    if (ImGui::Button("Remove Event Region") ) {
        context.world->getEventManager().deleteEventRegion(context.selectedEventRegion);
    }

    bool gridSnapping = context.enableGridSnapping;
    if (ImGui::Checkbox("Grid Snapping", &gridSnapping)) {
        context.enableGridSnapping = gridSnapping;


    }

    bool debug = context.drawDebug;
    if (ImGui::Checkbox("Debug", &debug)) {
        context.drawDebug = debug;
    }



}



void SpriteWindow::render(EditorContext& context) {
    window.clear();
    window.setView(view);
    ImGui::SFML::Render(window);

   renderSpriteList(context);


  //  ImGui::SFML::Render(window);
    window.display();
}

void SpriteWindow::handleEvent(EditorContext &context) {
    sf::Event event;
    while (window.pollEvent(event)) {
        // 1. Pass the event to ImGui
        ImGui::SFML::ProcessEvent(window, event);

        // 2. Handle SFML events specific to this sprite window
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePixel);
            std::cout << "mouse pressed" << std::endl;
            for (size_t i = 0; i < sprites.size(); ++i) {
                if (sprites[i].getGlobalBounds().contains(worldPos)) {
                    // Update context with selected sprite
                    context.selectedSprite = &spriteViewTiles[i];
                    //context.selectedSprite->textureID = spriteViewTiles[i].textureID;
                    //context.selectedSprite->textureRect = sprites[i].getTextureRect();

                    std::cout << "Sprite " << context.selectedSprite->textureID << " selected.\n";
                    break;
                }
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                // spriteView.move(0.f, -event.mouseWheelScroll.delta * 30.f);
                //  window.setView(spriteView);
            }
        }

        if (event.type == sf::Event::Resized) {
            view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
            view.setCenter(view.getSize() / 2.f);
            window.setView(view);
        }

    }
}

