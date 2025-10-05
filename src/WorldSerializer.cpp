#include "WorldSerializer.h"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

std::string entityTypeToString(EntityType type) {
    switch (type) {
        case EntityType::Player: return "Player";
        case EntityType::NPC: return "NPC";
        case EntityType::Item: return "Item";
        default: return "Unknown";
    }
}

EntityType entityTypeFromString(const std::string& str) {
    if (str == "Player") return EntityType::Player;
    if (str == "NPC") return EntityType::NPC;
    if (str == "Item") return EntityType::Item;
    throw std::runtime_error("Unknown EntityType string: " + str);
}

void WorldSerializer::saveToFile(const World& world, const std::string& filename) {
    json j = save(world);
    std::ofstream out(filename);
    if (out.is_open()) {
        out << j.dump(2);
    }
}

void WorldSerializer::loadFromFile(World& world, const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;

    json j;
    in >> j;
    load(world, j);
}

json WorldSerializer::save(const World& w) {
    json j;
    j["version"] = 1;
    j["drawOrder"] = w.getLayerManager().getDrawOrder();

    for (const auto& pair : w.getLayerManager().getLayers()) {
        int id = pair.first;
        const TileLayer& layer = pair.second;
        json layerJson;

        if (const_cast<TileLayer&>(layer).getDrawOrder().empty()) {
            layerJson["tiles"] = json::array();
        }

        for (TileInstance* tile : const_cast<TileLayer&>(layer).getDrawOrder()) {
            layerJson["tiles"].push_back({
                {"x", tile->gridPos.x},
                {"y", tile->gridPos.y},
                {"textureID", tile->textureID},
                {"anchorOffset", {tile->anchorOffset.x, tile->anchorOffset.y}},
                {"worldOffset", {tile->worldOffset.x, tile->worldOffset.y}},
                {"texRect", {tile->textureRect.left, tile->textureRect.top, tile->textureRect.width, tile->textureRect.height}}
            });
        }

        j["layers"][std::to_string(id)] = layerJson;
    }

    for (auto& entity : w.getEntityManager().getEntities()) {
        json entityJson;

        entityJson["tile"].push_back({
            {"x", entity->getTileInstance()->gridPos.x},
            {"y", entity->getTileInstance()->gridPos.y},
            {"textureID", entity->getTileInstance()->textureID},
            {"texRect", {
                entity->getTileInstance()->textureRect.left,
                entity->getTileInstance()->textureRect.top,
                entity->getTileInstance()->textureRect.width,
                entity->getTileInstance()->textureRect.height
            }}
        });

        entityJson["data"].push_back({
            {"renderLayer", entity->getRenderLayer()},
        });

        j["entities"][entityTypeToString(entity->getType())].push_back(entityJson);
    }

    //SAVE COLLIDER COORDINATES
    json colliderJson;
    for (auto& collider : w.getCollisionManager().getColliders()) {

        json vertexArray = json::array();
        for (int i=0; i<collider->getVertices().size(); i++) {
            //colliderJson["vertices"].push_back({collider->getVertices()[i].x,collider->getVertices()[i].y});
            vertexArray.push_back(collider->getVertices()[i].x);
            vertexArray.push_back(collider->getVertices()[i].y);
        }
       colliderJson["vertices"].push_back(vertexArray);
    }
    j["colliders"].push_back(colliderJson);


    //SAVE EVENT REGION COORDINATES
    json eventRegionJson;
    for (auto& eventRegion : w.getEventManager().getEventRegions()) {
        json vertexArray = json::array();

        for (int i=0; i<eventRegion->getPolygon().size(); i++) {
            vertexArray.push_back(eventRegion->getPolygon()[i].x);
            vertexArray.push_back(eventRegion->getPolygon()[i].y);
        }
        eventRegionJson["vertices"].push_back(vertexArray);
    }
    j["eventRegions"].push_back(eventRegionJson);


    return j;
}

void WorldSerializer::load(World& world, const json& j) {

    world.getCollisionManager().deleteAllColliders();
    world.getEventManager().deleteAllEventRegions();

    LayerManager& layerManager = world.getLayerManager();
    EntityManager& entityManager = world.getEntityManager();

    while (!layerManager.getDrawOrder().empty()) {
        layerManager.removeLayer(layerManager.getDrawOrder().back());
    }

    for (int id : j["drawOrder"]) {
        layerManager.createLayer();
    }

    entityManager.clear();

    for (auto& [typeStr, entityArray] : j["entities"].items()) {
        for (const auto& entityJson : entityArray) {
            if (typeStr == "Player") {
                auto player = std::make_unique<Player>();

                if (entityJson.contains("data")) {
                    for (const auto& dataEntry : entityJson["data"]) {
                        int renderLayer = dataEntry.value("renderLayer", 0);
                        player->setRenderLayer(renderLayer);
                    }
                }

                if (entityJson.contains("tile")) {
                    for (const auto& tile : entityJson["tile"]) {
                        TileInstance* playerTile = player->getTileInstance();
                        if (playerTile) {
                            playerTile->gridPos.x = tile.value("x", 0);
                            playerTile->gridPos.y = tile.value("y", 0);
                            playerTile->textureID = tile.value("textureID", "");
                            const auto& rect = tile["texRect"];
                            if (rect.size() == 4) {
                                player->setTextureRect({rect[0], rect[1], rect[2], rect[3]});
                            }
                        }
                    }
                }

                entityManager.addEntity(std::move(player));
            }

            // TODO: handle other entity types like NPC and Item
        }
    }

    for (auto it = j["layers"].begin(); it != j["layers"].end(); ++it) {
        const std::string& key = it.key();
        const json& layerJson = it.value();
        int id = std::stoi(key);
        TileLayer* layer = layerManager.getLayer(id);
        if (!layer) continue;

        for (const auto& tileJson : layerJson["tiles"]) {
            sf::Vector2i pos{tileJson["x"], tileJson["y"]};
            std::string texID = tileJson["textureID"];
            auto anchorOffset_ = tileJson["anchorOffset"];
            auto worldOffset_ = tileJson["worldOffset"];
            auto r = tileJson["texRect"];
            sf::IntRect rect{r[0], r[1], r[2], r[3]};

            sf::Vector2f anchorOffset{anchorOffset_[0], anchorOffset_[1]};
            sf::Vector2f worldOffset{worldOffset_[0], worldOffset_[1]};

            layer->addTile(pos, texID, rect, anchorOffset, worldOffset);
        }
    }

    for (const auto& colliderJson : j["colliders"]) {
        for (const auto& colliderArray : colliderJson["vertices"]) {
            // colliderArray is a JSON array like: [x1, y1, x2, y2, ...]
            std::vector<sf::Vector2f> vertices;

            for (size_t i = 0; i + 1 < colliderArray.size(); i += 2) {
                float x = colliderArray[i];
                float y = colliderArray[i + 1];
                vertices.emplace_back(x, y);
            }

            world.getCollisionManager().createCollider(vertices);
        }
    }


    for (const auto& eventRegionJson : j["eventRegions"]) {
        for (const auto& eventRegionArray : eventRegionJson["vertices"]) {
            // colliderArray is a JSON array like: [x1, y1, x2, y2, ...]
            std::vector<sf::Vector2f> vertices;

            for (size_t i = 0; i + 1 < eventRegionArray.size(); i += 2) {
                float x = eventRegionArray[i];
                float y = eventRegionArray[i + 1];
                vertices.emplace_back(x, y);
            }

            world.getEventManager().addEventRegion(vertices);
        }
    }


}
