#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include <memory>

#include "Entity.h"
#include "Player.h"

class EntityManager {
public:
    void addEntity(std::unique_ptr<Entity> entity);

    const std::vector<std::unique_ptr<Entity>>& getEntities() const;
    std::vector<std::unique_ptr<Entity>>& getEntities();

    Player* getPlayer() const;

    void updatePlayer(float deltaTime, const std::vector<std::unique_ptr<Collider>>& colliders);

    void clear();

private:
    std::vector<std::unique_ptr<Entity>> entities;
    Player* player = nullptr;
};

#endif // ENTITYMANAGER_H
