//
// Created by User on 27/05/2025.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <SFML/Graphics.hpp>


struct AnimationFrame {
    sf::IntRect rect;  // Texture rectangle
    float duration;    // Duration in seconds
};


class Animation {
public:
    std::string name;
    std::vector<AnimationFrame> frames;

    void addFrame(const sf::IntRect& rect, float duration) {
        frames.push_back({rect, duration});
    }

    const AnimationFrame& getFrame(std::size_t index) const {
        return frames[index % frames.size()];
    }

    std::size_t getSize() const {
        return frames.size();
    }
};



#endif //ANIMATION_H
