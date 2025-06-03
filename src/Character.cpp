//
// Created by User on 14/05/2025.
//

#include "../include/Character.h"

#include <iostream>

Character::Character(const sf::Texture& texture, sf::Vector2f startPos)
    : worldPos(startPos)
{
   characterTile.textureID = "character";
    characterTile.gridPos = {0,0};
    characterTile.anchorOffset = {0,0};
    characterTile.textureRect = sf::IntRect(23,0,24,38);
    characterTile.worldOffset = {startPos.x,startPos.y};
   // sprite.setTexture(texture);
   // rectSource = sf::IntRect(23,0,24,38);
    //sprite.setTextureRect(rectSource);
   //sprite.setOrigin(12,32);
}



void Character::handleInput(float deltaTime) {

    sf::Vector2f moveDir(0.f, 0.f);

    // Get input state
    bool up    = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool down  = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    // Diagonal input → isometric movement
    if (up && left) {
        moveDir = sf::Vector2f(-1, -0.5f);  // up-left iso
        characterTile.textureRect.top = 5*36;
    }
    else if (up && right) {
        moveDir = sf::Vector2f(1, -0.5f);
         characterTile.textureRect.top = 3*36;// up-right iso
    }
    else if (down && left) {
        moveDir = sf::Vector2f(-1, 0.5f);  // down-left iso
        characterTile.textureRect.top = 7*36;

    }
    else if (down && right) {
        moveDir = sf::Vector2f(1, 0.5f);
        characterTile.textureRect.top = 8*36;

    } // down-right iso

    // Cardinal input → straight screen movement
    else if (up) {
        moveDir = sf::Vector2f(0.f, -1.f);
        characterTile.textureRect.top = 4*36;

    }
    else if (down) {
        moveDir = sf::Vector2f(0.f, 1.f);
        characterTile.textureRect.top = 0*36;

    }
    else if (left) {
        moveDir = sf::Vector2f(-1.f, 0.f);
        characterTile.textureRect.top = 6*36;

    }
    else if (right) {
        moveDir = sf::Vector2f(1.f, 0.f);
        characterTile.textureRect.top = 2*36;

    }

    // Normalize diagonal movement (optional for speed consistency)
    if (moveDir.x != 0.f && moveDir.y != 0.f) {
        float len = std::sqrt(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
        moveDir /= len;
    }

    // Apply movement
    float speed = 50.f;
    characterTile.worldOffset += moveDir * speed * deltaTime;
/*
    sf::Vector2f isoRightVector = { 16.f, 8.f };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
       // characterTile.worldOffset.x -= speed * deltaTime;
         characterTile.worldOffset.y -= speed * deltaTime;
            characterTile.textureRect.top = 4*36;


      //  return;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        //characterTile.worldOffset.x -= speed * deltaTime;
        characterTile.worldOffset.y += speed * deltaTime;

        characterTile.textureRect.top = 0*36;

    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        characterTile.worldOffset.x -= speed * deltaTime;
      //  characterTile.worldOffset.y -= speed * deltaTime;

        characterTile.textureRect.top= 6*36;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        characterTile.worldOffset.x += speed * deltaTime;
        //characterTile.worldOffset.y -= speed * deltaTime;

        characterTile.textureRect.top = 2*36;
    }


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&  sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        characterTile.textureRect.top = 5*36;

    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&  sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        characterTile.textureRect.top = 3*36;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&  sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        characterTile.textureRect.top = 8*36;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&  sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        characterTile.textureRect.top = 7*36;
    }

    */
}

void Character::animate() {
    if (animClock.getElapsedTime().asSeconds() > frameTime) {
        characterTile.textureRect.left += 23;

        if (characterTile.textureRect.left >= 8 * 23)
            characterTile.textureRect.left = 0;

        //sprite.setTextureRect(rectSource);
        animClock.restart();

        //std::cout << rectSource.left << std::endl;
    }


}

void Character::printDebug() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        sf::Vector2f pos = IsoToCart();
       //std::cout << pos.x <<  " " <<pos.y << std::endl;

       // std::cout << "SPRITE POS" << sprite.getPosition().x <<  " " <<sprite.getPosition().y << std::endl;
    }

}

sf::Vector2f Character::cartToIso(float x, float y) {
    float tileWidth = 32;
    float tileHeight = 16;
    return {
        (x - y) * tileWidth / 2,
        (x + y) * tileHeight / 2
    };
}

sf::Vector2f Character::IsoToCart() {

    float screenX = sprite.getPosition().x;
    float screenY = sprite.getPosition().y;


    float x = (screenY / (16 / 2) + screenX / (32 / 2)) / 2.f;
    float y = (screenY / (16 / 2) - screenX / (32 / 2)) / 2.f;
    return sf::Vector2f(x, y);
}

void Character::update(float deltaTime) {
    handleInput(deltaTime);
    animate();

    //sf::Vector2f isoPos = cartToIso(worldPos.x, worldPos.y);
   // isoPos = worldPos;
  //  sprite.setPosition(isoPos.x , isoPos.y ); // offset for camera/view


printDebug();

}

void Character::draw(sf::RenderWindow& window) {
    window.draw(sprite);

    sf::CircleShape dot(2);
    dot.setFillColor(sf::Color::Red);
    dot.setPosition(sprite.getPosition()-sf::Vector2f(2,2));
    window.draw(dot);

}

const sf::Vector2f & Character::getPosition() const {
    return worldPos;
}
