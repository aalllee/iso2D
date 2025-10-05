//
// Created by User on 30/05/2025.
//

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <SFML/Graphics.hpp>
#include "EditorContext.h"

class EditorWindow {

public:
    EditorWindow(sf::Vector2u size, const std::string& title);

    void update(const sf::Time& dt, EditorContext& context);
    void render(EditorContext& context, sf::View& view);
    void handleEvent(EditorContext& context);
    bool isOpen() const;

    sf::RenderWindow& getWindow() {return window;};

private:
    void processClick(const sf::Vector2f& worldPos, EditorContext& context);

    void drawDebug(EditorContext& context);
    void drawColliders(EditorContext& context);
    void drawEventRegions(EditorContext& context);
    void drawPlayerDebug(EditorContext& context);
    void highlightTileOnHover(sf::Vector2i pos);
    bool isColliderVertClicked(sf::Vector2f mouse);
    void drawSelectionHighlight(EditorContext& context);


    void updateCamera(EditorContext& context,const sf::Time& dt);

    sf::RenderWindow window;
    sf::View view;

    sf::RectangleShape selectionHighlight;

    sf::VertexArray isoGrid;

    sf::Vector2i newPos = {0,0};
    sf::Vector2i mouseTileCoord = {0, 0};

    EventRegion* selectedArea = nullptr;
    Collider* selectedCollider = nullptr;
    int selectedVertexIndex = 0;

    bool bDrawDebug = true;

};


inline void EditorWindow::drawPlayerDebug(EditorContext &context) {
    Player* p = context.entityManager->getPlayer();

    if(!p) return;



    // Create a circle shape with radius 50
    sf::CircleShape circle(2.0f);  // radius
    circle.setFillColor(sf::Color::Green);
    sf::Vector2f worldOffset = p->getTileInstance()->worldOffset;
    // Set position (x, y)
    circle.setPosition(worldOffset.x - 2, worldOffset.y - 2);

    // Set fill color
    circle.setFillColor(sf::Color::Green);
    window.draw(circle);// Draw the circle

    sf::Vector2f anchor = p->getTileInstance()->anchorOffset + worldOffset;
    circle.setPosition(anchor.x - 2, anchor.y - 2);
    circle.setFillColor(sf::Color::Blue);

    window.draw(circle);// Draw the circle



    float x = p->getTextureRect().left;
    float y = p->getTextureRect().top;
    float w = p->getTextureRect().width;
    float h = p->getTextureRect().height;

    x = worldOffset.x - w/2;
    y = worldOffset.y - h/2;

    // Create a rectangle shape with size 120x60
    sf::RectangleShape rectangle(sf::Vector2f(w, h));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineThickness(1.0f);
    rectangle.setOutlineColor(sf::Color::Red);

    window.draw(rectangle);


}
#endif //EDITORWINDOW_H
