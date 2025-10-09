//
// Created by User on 24/05/2025.
//

#include "World.h"
//include "../include/Tile.h"
#include "Tree.h"
#include "../src/events/EventRegion.h"

World::World() {
}

void World::init() {
    // Create tile layers
    int backgroundLayer = layerManager.createLayer("Base");


    //Create demo tree entity
    auto tree = std::make_unique<Tree>();
    entityManager.addEntity(std::move(tree));

    // Create and add player
    auto player = std::make_unique<Player>();
    player->setPosition({100, 200});
    entityManager.addEntity(std::move(player));

    if (player == nullptr) std::cout << "player transfered -> null" << std::endl;

    std::vector<sf::Vector2f> poly = {
        {0.f, 0.f},
        {100.f, 0.f},
        {100.f, 100.f},
        {0.f, 100.f}
    };

    eventManager.addEventRegion(poly);



}

void World::clearWorld() {
    //temp
    collisionManager.deleteAllColliders();
}

void World::update(float deltaTime) {

}

void World::render(sf::RenderWindow& window, const std::map<std::string, sf::Texture>& textures) {

//currently rendering through LayerManager->renderAll



}
