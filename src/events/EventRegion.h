//
// Created by User on 29/05/2025.
//

#ifndef EVENTREGION_H
#define EVENTREGION_H

#include <vector>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "../../../include/Entity.h"

class EventRegion {

private:
    std::vector<sf::Vector2f> polygon;
    std::unordered_set<Entity*> charactersInside;
    std::vector<std::function<void(const Entity&)>> enterCallbacks;
    std::vector<std::function<void(const Entity&)>> exitCallbacks;

public:

    std::vector<sf::Vector2f>& getPolygon() {return polygon; };

    EventRegion(const std::vector<sf::Vector2f>& poly) : polygon(poly) {}

    void drawDebug(sf::RenderWindow& window, sf::Color color) const ;

    void onEnter() {
        std::cout << "Entering event region" << std::endl;
    }

    void onExit() {
        std::cout << "Exiting event region" << std::endl;
    }

    EventRegion& onEnter(std::function<void(const Entity&)> callback) {
        enterCallbacks.push_back(callback);
        return *this;
    }

    EventRegion& onExit(std::function<void(const Entity&)> callback) {
        exitCallbacks.push_back(callback);
        return *this;
    }

    void update(Entity& entity) {

        //if (!entity) return;

        bool isInside = pointInPolygon(entity.getTileInstance()->worldOffset);

        bool wasInside = charactersInside.find(&entity) != charactersInside.end();

        if (isInside && !wasInside) {
            /*
            for (auto& callback : enterCallbacks) {
                callback(entity);
            }
            */
            onEnter(); //no callbacks for now
            charactersInside.insert(&entity);
        } else if (!isInside && wasInside) {
            onExit();
            /*for (auto& callback : exitCallbacks) {
                callback(entity);
            }*/
            charactersInside.erase(&entity);
        }
    }

    // Get all characters currently inside
    const std::unordered_set<Entity*>& getCharactersInside() const {
        return charactersInside;
    }

    // Check if specific character is inside
    bool isCharacterInside(int characterId) const {
        return false;//charactersInside.find(characterId) != charactersInside.end();
    }

    int getSize() const { return polygon.size(); }

private:
    bool pointInPolygon(sf::Vector2f pos) const {
        bool inside = false;
        int j = polygon.size() - 1;

        for (int i = 0; i < polygon.size(); j = i++) {
            if (((polygon[i].y > pos.y) != (polygon[j].y > pos.y)) &&
                (pos.x < (polygon[j].x - polygon[i].x) * (pos.y - polygon[i].y) /
                 (polygon[j].y - polygon[i].y) + polygon[i].x)) {
                inside = !inside;
            }
        }
        return inside;
    }
};



#endif //EVENTREGION_H
