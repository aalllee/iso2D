#include "EntityManager.h"

void EntityManager::addEntity(std::unique_ptr<Entity> entity) {
    if (!entity) return;

    if (entity->getType() == EntityType::Player) {
        player = dynamic_cast<Player*>(entity.get());
    }

    entities.push_back(std::move(entity));
}

const std::vector<std::unique_ptr<Entity>>& EntityManager::getEntities() const {
    return entities;
}

std::vector<std::unique_ptr<Entity>>& EntityManager::getEntities() {
    return entities;
}

Player* EntityManager::getPlayer() const {
    return player;
}

void EntityManager::updatePlayer(float deltaTime, const std::vector<std::unique_ptr<Collider>>& colliders) {
    if (!player) return;
    player->update(deltaTime, colliders);
}

void EntityManager::clear() {
    entities.clear();
    player = nullptr;
}
