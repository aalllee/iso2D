//
// Created by User on 27/05/2025.
//

#include "../include/Animator.h"


void Animator::play(const Animation& anim, bool loop) {
    if (currentAnimation != &anim) {
        currentAnimation = &anim;
        currentFrame = 0;
        elapsed = 0.0f;
        looping = loop;
    }

}

void Animator::update(float dt) {

    if (!currentAnimation || currentAnimation->getSize() == 0) return;

    elapsed += dt;
    while (elapsed >= currentAnimation->getFrame(currentFrame).duration) {
        elapsed -= currentAnimation->getFrame(currentFrame).duration;
        currentFrame++;
        if (currentFrame >= currentAnimation->getSize()) {
            if (looping)
                currentFrame = 0;
            else
                currentFrame = currentAnimation->getSize() - 1; // Stay on last frame
        }
    }


}

sf::IntRect Animator::getCurrentFrameRect() const {

        if (!currentAnimation || currentAnimation->getSize() == 0)
            return sf::IntRect();
        return currentAnimation->getFrame(currentFrame).rect;


}