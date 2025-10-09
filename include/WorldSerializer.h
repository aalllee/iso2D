#ifndef WORLD_SERIALIZER_H
#define WORLD_SERIALIZER_H

#include <string>
#include <../../nlohmann/json.hpp>
#include "LayerManager.h"
#include "World.h"

enum class EntityType;

std::string entityTypeToString(EntityType type);
EntityType entityTypeFromString(const std::string& str);

class WorldSerializer {
public:
    static void saveToFile(const World& world, const std::string& filename);
    static void loadFromFile(World& world, const std::string& filename,AssetManager* assetManager);

private:
    static nlohmann::json save(const World& world);
    static void load(World& world, const nlohmann::json& j, AssetManager* assetManager);
};

#endif // WORLD_SERIALIZER_H
