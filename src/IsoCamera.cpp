#include "IsoCamera.h"

#include <iostream>

IsoCamera::IsoCamera(const sf::Vector2f& startCenter, const sf::Vector2f& size) {
    view.setSize({800,450});
    view.setCenter(startCenter);
}

sf::FloatRect IsoCamera::getLetterboxView(int windowWidth, int windowHeight)
{
    float windowRatio = (float)windowWidth / (float)windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;
    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX = 0.0f;
    float posY = 0.0f;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else {
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    return sf::FloatRect(posX, posY, sizeX, sizeY);
}

void IsoCamera::handleInput(const sf::RenderWindow& window, float dt) {
    sf::Vector2f move(0.f, 0.f);



    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        move.x -= moveSpeed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        move.x += moveSpeed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        move.y -= moveSpeed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        move.y += moveSpeed * dt;

    view.move(move);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        zoomLevel *= 0.999f;
        view.zoom(0.999f);
       // std::cout << "zoom level: " << zoomLevel << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        zoomLevel *= 1.001f;
        view.zoom(1.001f);

    }
}

void IsoCamera::apply(sf::RenderWindow& window) const {
    window.setView(view);
}
