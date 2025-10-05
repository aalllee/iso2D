//
// Created by User on 27/05/2025.
//

#include "Collider.h"


Collider::Collider(const std::vector<sf::Vector2f>& vertices)
    : vertices(vertices) {}

bool Collider::contains(const sf::Vector2f& pt) const {
    bool inside = false;
    int n = vertices.size();
    for (int i = 0, j = n - 1; i < n; j = i++) {
        const auto& pi = vertices[i];
        const auto& pj = vertices[j];

        bool intersect = ((pi.y > pt.y) != (pj.y > pt.y)) &&
                         (pt.x < (pj.x - pi.x) * (pt.y - pi.y) / ((pj.y - pi.y) + 1e-10f) + pi.x);
        if (intersect)
            inside = !inside;
    }
    return inside;
}
float Collider::dot(const sf::Vector2f& a, const sf::Vector2f& b) const {
    return a.x * b.x + a.y * b.y;
}

bool Collider::crossesLine(const sf::Vector2f& oldPos, const sf::Vector2f& newPos) const {
    if (vertices.size() != 2) return false; // not a line

    sf::Vector2f p1 = vertices[0];
    sf::Vector2f p2 = vertices[1];
    sf::Vector2f lineVec = p2 - p1;
    sf::Vector2f normal(-lineVec.y, lineVec.x);

    float side1 = dot(normal, oldPos - p1);
    float side2 = dot(normal, newPos - p1);

    return (side1 * side2 < 0);
}

bool Collider::isCollision(const sf::Vector2f &newPos) const {
}

void Collider::removeVertex(int index) {


    vertices.erase(vertices.begin() + index);


}

bool Collider::crossesLineSegment(const sf::Vector2f& oldPos, const sf::Vector2f& newPos) const {
    // Check if the movement vector (oldPos → newPos) crosses segment (p1 → p2)
    sf::Vector2f p1 = vertices[0];
    sf::Vector2f p2 = vertices[1];
    auto cross = [](const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.y - a.y * b.x;
    };

    sf::Vector2f r = newPos - oldPos;
    sf::Vector2f s = p2 - p1;
    sf::Vector2f diff = p1 - oldPos;

    float rxs = cross(r, s);
    float diffxr = cross(diff, r);

    if (rxs == 0) {
        // Lines are parallel
        return false;
    }

    float t = cross(diff, s) / rxs;
    float u = diffxr / rxs;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}


void Collider::drawDebug(sf::RenderWindow& window, sf::Color color) const {

    sf::VertexArray lines(sf::LineStrip, vertices.size() + 1);
    for (size_t i = 0; i < vertices.size(); ++i) {
        lines[i].position = vertices[i];
        lines[i].color = color;
    }
    lines[vertices.size()].position = vertices[0];
    lines[vertices.size()].color = color;

    window.draw(lines);
if (isSelected) {
    // Draw circle handles on each vertex
    const float radius = 4.0f;
    sf::CircleShape handle(radius);
    handle.setOrigin(radius, radius); // center the circle
    handle.setFillColor(sf::Color::Yellow);
    handle.setOutlineColor(sf::Color::Black);
    handle.setOutlineThickness(1.0f);

    for (const auto& v : vertices) {
        handle.setPosition(v);
        window.draw(handle);


    }
}
}

const std::vector<sf::Vector2f>& Collider::getVertices() const {
    return vertices;
}
