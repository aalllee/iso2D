//
// Created by User on 27/05/2025.
//

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H



#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>



class Collider {
public:
    Collider(const std::vector<sf::Vector2f>& vertices);

    bool contains(const sf::Vector2f& point) const;

    bool crossesLine(const sf::Vector2f& oldPos, const sf::Vector2f& newPos) const ;

    bool isCollision(const sf::Vector2f& newPos) const;

    bool crossesLineSegment(const sf::Vector2f& oldPos, const sf::Vector2f& newPos) const;


    const std::vector<sf::Vector2f>& getVertices() const;

    void drawDebug(sf::RenderWindow& window, sf::Color color = sf::Color::Red) const;

    float dot(const sf::Vector2f& a, const sf::Vector2f& b) const ;

    std::vector<sf::Vector2f>& getVertices () {return vertices;}

private:
    std::vector<sf::Vector2f> vertices;
   // sf::Color color;
};




#endif //COLLISIONOBJECT_H
