//
// Created by User on 16/05/2025.
//

#include "AssetManager.h"

#include <stdexcept>
#include <string>
#include <iostream>

void AssetManager::initTileRegistry() {
    std::string spriteSheet0 = "prototype";
    tileTextureLookup["flat_grass"] = { spriteSheet0, sf::IntRect(32, 32, 32, 32) };
    tileTextureLookup["block_grass"] = { spriteSheet0, sf::IntRect(0, 96, 32, 32) };
    tileTextureLookup["steps_left"] = { spriteSheet0, sf::IntRect(128, 0, 32, 32)};
    tileTextureLookup["steps_right"] = { spriteSheet0, sf::IntRect(160, 0, 32, 32)};

    std::string characterSpriteSheet = "character";
    tileTextureLookup["character"] = { characterSpriteSheet, sf::IntRect(23,0,24,38) };

    std::string tree01 = "tree01";
    tileTextureLookup[tree01] = { tree01, sf::IntRect(0,0,105,128) };


    std::string spiritHomeTerrain = "spiritHomeTerrain";
    tileTextureLookup["block_sand_grass"] = {spiritHomeTerrain, sf::IntRect(0, 0, 32, 37) };
    tileTextureLookup["block_sand_grass"] = {spiritHomeTerrain, sf::IntRect(0, 0, 32, 37) };


}

void AssetManager::updateDefaultWorldOffset(const std::string texID, sf::Vector2f newWorldOffset) {
    auto it = tileTextureLookup.find(texID);
    if (it != tileTextureLookup.end()) {
       //found
        it->second.worldOffset =  newWorldOffset;
    } else {
       //not found
    }
}

sf::IntRect AssetManager::getTileRect(const std::string &id) const {
    auto it = tileTextureLookup.find(id);
    if (it != tileTextureLookup.end()) {
        return it->second.texRect;
    }
    // Return empty rect if not found
    std::cerr << "Warning: Tile rect not found for ID: " << id << std::endl;
    return sf::IntRect(0, 0, 0, 0);
}


bool AssetManager::loadTexture(const std::string& id, const std::string& path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path))
        return false;
    textures[id] = std::move(texture);
    return true;
}

const sf::Texture& AssetManager::getTexture(const std::string& id) const {
    auto it = textures.find(id);
    if (it == textures.end())
        throw std::runtime_error("Texture ID not found: " + id);
    return it->second;
}

bool AssetManager::hasTexture(const std::string& id) const {
    return textures.find(id) != textures.end();
}
