//
// Created by User on 30/05/2025.
//

#include "CollisionManager.h"


CollisionManager::CollisionManager() {
    colliders.clear();
}

void CollisionManager::createCollider(const std::vector<sf::Vector2f>& vertices) {
    nextId++;
    auto collider_ptr = std::make_unique<Collider>(vertices);
    Collider* raw = collider_ptr.get();
    colliders.push_back(std::move(collider_ptr));

    colliderMap[nextId] = raw;
    //collider_ptr NULL : transfered ownership

}

void CollisionManager::deleteCollider(int id) {

    auto iter = colliderMap.find(id);
    if (iter == colliderMap.end())
        return; // id not found

    Collider* target = iter->second;

    auto it = std::find_if(colliders.begin(), colliders.end(),
         [target](const std::unique_ptr<Collider>& ptr) {
             return ptr.get() == target;
         });

    if (it != colliders.end()) {
        colliders.erase(it);
    }

    colliderMap.erase(id);
}

void CollisionManager::deleteAllColliders() {
    colliderMap.clear();
    colliders.clear();
    nextId = 0;
}

void CollisionManager::drawColliders(sf::RenderWindow &window) {
    for (auto& collider : colliders) {
        collider->drawDebug(window);
    }
}
