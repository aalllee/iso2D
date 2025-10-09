//
// Created by User on 27/05/2025.
//

#ifndef SPRITEWINDOW_H
#define SPRITEWINDOW_H

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "EditorContext.h"
#include "World.h"
#include "AssetManager.h"

class SpriteWindow {

public:
    SpriteWindow(sf::Vector2u size, const std::string& title);

    void update(const sf::Time& dt, EditorContext& context);
    void render(EditorContext& context);
    void handleEvent(EditorContext& context);
    bool isOpen() const;



private:
    void processSpriteClick(const sf::Vector2f& worldPos, EditorContext& context);

    void renderSpriteList( EditorContext& context);

    void renderLayerUI(EditorContext& context);

    void SaveLoadUI(EditorContext& context);



    sf::RenderWindow window;
    sf::RenderWindow spritePropertiesWindow;
    sf::View view;
    std::vector<sf::Sprite> sprites;
    std::vector<TileInstance> spriteViewTiles;

    // You might track selected state here or via context
    sf::RectangleShape selectionHighlight;

    bool cachedSpriteList = false;
};



#endif //SPRITEWINDOW_H
