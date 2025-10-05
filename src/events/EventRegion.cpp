//
// Created by User on 29/05/2025.
//

#include "EventRegion.h"


void EventRegion::drawDebug(sf::RenderWindow& window, sf::Color color) const {
    if (polygon.size() < 2) return;

    sf::VertexArray lines(sf::LineStrip, polygon.size() + 1);
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        lines[i].position = polygon[i];
        lines[i].color = color;
    }
    // Close the loop by connecting the last point to the first
    lines[polygon.size()].position = polygon[0];
    lines[polygon.size()].color = color;

    window.draw(lines);

    if (isSelected) {
        // Draw circle handles on each vertex
        const float radius = 4.0f;
        sf::CircleShape handle(radius);
        handle.setOrigin(radius, radius); // center the circle
        handle.setFillColor(sf::Color::Yellow);
        handle.setOutlineColor(sf::Color::Black);
        handle.setOutlineThickness(1.0f);

        for (const auto& v : polygon) {
            handle.setPosition(v);
            window.draw(handle);


        }
    }
}