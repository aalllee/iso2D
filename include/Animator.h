//
// Created by User on 27/05/2025.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../include/Animation.h"
#include <SFML/Graphics.hpp>

class Animator {
private:
    const Animation* currentAnimation = nullptr;
    float elapsed = 0.0f;
    std::size_t currentFrame = 0;
    bool looping = true;

public:
    void play(const Animation& anim, bool loop = true);

    void update(float dt);

    sf::IntRect getCurrentFrameRect() const;
};



#endif //ANIMATOR_H
