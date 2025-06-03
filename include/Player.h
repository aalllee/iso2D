//
// Created by User on 24/05/2025.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "../include/Entity.h"
#include "../src/collision/Collider.h"
#include "../include/Animation.h"
#include "../include/Animator.h"


class Player : public Entity {

public:
    Player();

    void handleInput(float deltaTime, const std::vector<std::unique_ptr<Collider>>& colliders);
    void animate();
    void update(float deltaTime,  const std::vector<std::unique_ptr<Collider>>& colliders);



private:
    sf::Clock animClock;
    float frameTime = 0.1f;
    sf::Vector2f moveDir = {0,0};
    sf::Vector2f moveMod = {1,1};

    Animator animator;
    std::unordered_map<std::string, Animation> animations;

};



#endif //PLAYER_H
