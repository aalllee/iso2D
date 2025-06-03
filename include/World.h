#ifndef WORLD_H
#define WORLD_H

#include "EntityManager.h"
#include "LayerManager.h"
#include "Player.h"
#include "../src/events/EventManager.h"
//#include "NPC.h"
#include "../src/collision/CollisionManager.h"

class World {
public:
    World();

    void init(); // Manual init
    void clearWorld();
    void update(float deltaTime);
    void render(sf::RenderWindow& window, const std::map<std::string, sf::Texture>& textures);
    inline LayerManager& getLayerManager() { return layerManager; }
    inline EntityManager& getEntityManager() { return entityManager; }
    EventManager& getEventManager() {return eventManager;};
    CollisionManager& getCollisionManager() {return collisionManager;};

    // Overload for const access
    const EntityManager& getEntityManager() const {return entityManager;};
    const LayerManager& getLayerManager() const {return layerManager;};
    const CollisionManager& getCollisionManager() const {return collisionManager;};
    const EventManager& getEventManager() const {return eventManager ;};
   // const CollisionManager& getCollisionManager() const {return collisionManager;};
private:
    LayerManager layerManager;
    EntityManager entityManager;
    EventManager eventManager;
    CollisionManager collisionManager;



    int playerID = -1;
};

#endif // WORLD_H
