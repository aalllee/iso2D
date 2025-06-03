//
// Created by User on 29/05/2025.
//

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "EventRegion.h"
#include <vector>


class EventManager {
    public:
    EventManager();

   void updateEventRegions(std::vector<std::unique_ptr<Entity>>&entities);

    void addEventRegion(const std::vector<sf::Vector2f>& vertices);

    void deleteEventRegion(int id);

    void deleteAllEventRegions();

    void removeEventRegion(std::unique_ptr<EventRegion> eventRegion) {}

   void drawDebug(sf::RenderWindow& window, sf::Color color) const;

    const std::vector<std::unique_ptr<EventRegion>>& getEventRegions() const {return eventRegions;}

    const std::map<int, EventRegion*>& getEventRegionMap() const {return eventRegionMap;}

    private:
    std::vector<std::unique_ptr<EventRegion>> eventRegions;
    std::map<int,EventRegion*> eventRegionMap;
    int nextId=0;
};



#endif //EVENTMANAGER_H
