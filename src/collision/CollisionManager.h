//
// Created by User on 30/05/2025.
//

#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "Collider.h"


class CollisionManager {
public:
    CollisionManager();

    void createCollider(const std::vector<sf::Vector2f>& vertices);

    void deleteCollider(int id);

    void deleteAllColliders();

    void drawColliders(sf::RenderWindow &window);

    const std::vector<std::unique_ptr<Collider>>& getColliders() const {return colliders;};
   // std::vector<std::unique_ptr<Collider>>& getColliders() {};


    const std::map<int,Collider*>& getColliderMap() {return colliderMap;};

private:

    std::vector<std::unique_ptr<Collider>> colliders;
    std::map<int,Collider*> colliderMap;
    int nextId = 1;
};



#endif //COLLISIONMANAGER_H
