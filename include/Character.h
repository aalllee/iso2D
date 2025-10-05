//
// Created by User on 14/05/2025.
//

#pragma once

#include <SFML/Graphics.hpp>

#include "Tile.h"



class Character {
public:
    Character(const sf::Texture& texture, sf::Vector2f startPos);

    void handleInput(float deltaTime);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    const sf::Vector2f& getPosition() const;

    TileInstance* getCharacterTile(){return &characterTile;}

private:
    sf::Sprite sprite;
    sf::IntRect rectSource;
    sf::Vector2f worldPos;
    sf::Clock animClock;


    float speed = 100.0f;
    float frameTime = 0.1f;
    int frameCount = 4;
    int frameHeight = 16;

    TileInstance characterTile;

    void animate();
    void printDebug();
   sf::Vector2f IsoToCart();
    sf::Vector2f cartToIso(float x, float y);
};




