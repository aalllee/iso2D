//
// Created by User on 24/05/2025.
//

#include "../include/Player.h"
#include "collision/Collider.h"


Player::Player() : Entity(EntityType::Player) {
    renderTile.textureID = "character";
    renderTile.gridPos = {0,0};
    renderTile.anchorOffset = {0,18};
    renderTile.textureRect = sf::IntRect(23,0,24,38);
    renderTile.worldOffset = {0,0};
    renderLayer=1;


    //ANIMATION

    Animation idleAnim, runAnim;

    for (int i = 0; i < 8; ++i) {
        animations["walk_left"].addFrame({23*i, 6*36, 24, 38}, frameTime);
        animations["walk_right"].addFrame({23*i, 2*36, 24, 38}, frameTime);
        animations["walk_up"].addFrame({23*i, 4*36, 24, 38}, frameTime);
        animations["walk_down"].addFrame({23*i, 0*36, 24, 38}, frameTime);
        animations["walk_upLeft"].addFrame({23*i, 5*36, 24, 38}, frameTime);
        animations["walk_upRight"].addFrame({23*i, 3*36, 24, 38}, frameTime);
        animations["walk_downLeft"].addFrame({23*i, 7*36, 24, 38}, frameTime);
        animations["walk_downRight"].addFrame({23*i, 8*36, 24, 38}, frameTime);


    }




    animator.play(animations["walk_left"]);


}

void Player::handleInput(float deltaTime,  const std::vector<std::unique_ptr<Collider>>& colliders) {


   moveDir = {0.f,0.f};

    // Get input state
    bool up    = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool down  = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    // Diagonal input → isometric movement
    if (up && left) {
        moveDir = sf::Vector2f(-1, -0.5f*moveMod.y);  // up-left iso
        renderTile.textureRect.top = 5*36;
        animator.play(animations["walk_upLeft"]);
    }
    else if (up && right) {
        moveDir = sf::Vector2f(1, -0.5f);
        renderTile.textureRect.top = 3*36;// up-right iso
        animator.play(animations["walk_upRight"]);
    }
    else if (down && left) {
        moveDir = sf::Vector2f(-1, 0.5f);  // down-left iso
        renderTile.textureRect.top = 7*36;
        animator.play(animations["walk_downLeft"]);

    }
    else if (down && right) {
        moveDir = sf::Vector2f(1, 0.5f);
        renderTile.textureRect.top = 8*36;
        animator.play(animations["walk_downRight"]);

    } // down-right iso

    // Cardinal input → straight screen movement
    else if (up) {
        moveDir = sf::Vector2f(0.f, -1.f);
        animator.play(animations["walk_up"]);

    }
    else if (down) {
        moveDir = sf::Vector2f(0.f, 1.f);
        animator.play(animations["walk_down"]);

    }
    else if (left) {
        moveDir = sf::Vector2f(-1.f, 0.f);
        animator.play(animations["walk_left"]);

    }
    else if (right) {
        moveDir = sf::Vector2f(1.f, 0.f);
        animator.play(animations["walk_right"]);

    }

    if (moveDir.x != 0.f && moveDir.y != 0.f) {
        float len = std::sqrt(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
        moveDir /= len;
    }

    float speed = 50.f;
    sf::Vector2f nextPos = renderTile.worldOffset  + moveDir * speed * deltaTime;

    for (const auto& collider : colliders) {

        if (collider->getVertices().size() == 2 &&
            collider->crossesLineSegment(renderTile.worldOffset+ renderTile.anchorOffset,
           nextPos+ renderTile.anchorOffset )) {
            return; // Line collision

        }

        if (collider->getVertices().size() > 2 && collider->contains(nextPos+ renderTile.anchorOffset))
            return; // Collision. do not move
    }

    renderTile.worldOffset = nextPos;

}

void Player::animate() {
    /*
    if (animClock.getElapsedTime().asSeconds() > frameTime) {
        renderTile.textureRect.left += 23;

        if (renderTile.textureRect.left >= 8 * 23)
            renderTile.textureRect.left = 0;

        animClock.restart();
    }
    */

}


void Player::update(float deltaTime,  const std::vector<std::unique_ptr<Collider>>& colliders) {
    handleInput(deltaTime, colliders);
    //animate();

    animator.update(deltaTime);
    renderTile.textureRect = animator.getCurrentFrameRect();

}

