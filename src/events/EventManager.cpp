//
// Created by User on 29/05/2025.
//

#include "EventManager.h"

EventManager::EventManager() {
    eventRegions.clear();
}

void EventManager::updateEventRegions(std::vector<std::unique_ptr<Entity>> &entities) {
    for (auto& entity : entities) {
        for (auto& eventRegion : eventRegions) {
            eventRegion->update(*entity);
        }
    }
}

void EventManager::addEventRegion(const std::vector<sf::Vector2f>& vertices)  {
    nextId++;

    auto eventRegion = std::make_unique<EventRegion>(vertices);
    EventRegion* raw = eventRegion.get();
    eventRegions.push_back(std::move(eventRegion));

    eventRegionMap[nextId] = raw;


}

void EventManager::deleteEventRegion(int id) {

    auto iter = eventRegionMap.find(id);
    if (iter == eventRegionMap.end()) return;

    EventRegion* target = iter->second;

    auto it = std::find_if(eventRegions.begin(), eventRegions.end(),
        [target](const std::unique_ptr<EventRegion>& region) {
            return region.get() == target;
        });

    if (it != eventRegions.end()) {
        eventRegions.erase(it);
    }

    eventRegionMap.erase(id);
}

void EventManager::deleteAllEventRegions() {
    eventRegionMap.clear();
    eventRegions.clear();
    nextId = 0;
}

void EventManager::drawDebug(sf::RenderWindow &window, sf::Color color) const{
    for (auto& eventRegion : eventRegions) {

        int size = eventRegion->getSize();
        if (size < 2) continue;



        sf::VertexArray lines(sf::LineStrip, size + 1);
        for (std::size_t i = 0; i < size; ++i) {
            lines[i].position = eventRegion->getPolygon()[i];
            lines[i].color = color;
        }
        // Close the loop by connecting the last point to the first
        lines[size].position = eventRegion->getPolygon()[0];
        lines[size].color = color;

        window.draw(lines);

        if (eventRegion->getIsSelected()) {
            // Draw circle handles on each vertex
            const float radius = 4.0f;
            sf::CircleShape handle(radius);
            handle.setOrigin(radius, radius); // center the circle
            handle.setFillColor(sf::Color::Yellow);
            handle.setOutlineColor(sf::Color::Black);
            handle.setOutlineThickness(1.0f);

            for (const auto& v : eventRegion->getPolygon()) {
                handle.setPosition(v);
                window.draw(handle);


            }
        }

    }



}
