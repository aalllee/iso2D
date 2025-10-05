#include "../include/LayerManager.h"
#include <algorithm>
#include <iostream>

// Create and add a new layer
int LayerManager::createLayer(const std::string& name) {
    int id = getNextFreeLayerIndex();
    layers[id] = TileLayer();
    drawOrder.push_back(id);
    layerOpacity[id] = 0.f;
    return id;
}

// Remove a layer
void LayerManager::removeLayer(int id) {
    layers.erase(id);
    drawOrder.erase(std::remove(drawOrder.begin(), drawOrder.end(), id), drawOrder.end());
    layerOpacity.erase(id);
}

// Get layer by ID
TileLayer* LayerManager::getLayer(int id) {
    auto it = layers.find(id);
    return it != layers.end() ? &it->second : nullptr;
}

// Move layer up in draw order
void LayerManager::moveLayerUp(int id) {
    auto it = std::find(drawOrder.begin(), drawOrder.end(), id);
    if (it != drawOrder.end() && (it + 1) != drawOrder.end()) {
        std::iter_swap(it, it + 1);
    }
}

// Move layer down in draw order
void LayerManager::moveLayerDown(int id) {
    auto it = std::find(drawOrder.begin(), drawOrder.end(), id);
    if (it != drawOrder.begin() && it != drawOrder.end()) {
        std::iter_swap(it, it - 1);
    }
}

void LayerManager::includeDrawableEntity(int id, TileInstance* entityTile) {
    layers[id].insertDynamicEntity(entityTile);
}

// Standard rendering
void LayerManager::renderAll(sf::RenderWindow& window, const std::map<std::string, sf::Texture>& textures) {
    int drawCalls = 0;
    for (int id : drawOrder) {
        auto it = layers.find(id);
        if (it == layers.end() || !it->second.getRenderViewport()) continue;

        TileLayer& layer = it->second;
        layer.buildVertexBatches();
        const auto& batches = layer.getTextureBatches();

        for (const auto& [texID, vertexArray] : batches) {
            sf::RenderStates states;
            auto texIt = textures.find(texID);
            if (texIt != textures.end()) {
                states.texture = &texIt->second;
                window.draw(vertexArray, states);
                drawCalls++;
            }
        }
    }
    std::cout << "DRAW CALLS: " << drawCalls << std::endl;
}

// Render with interleaved batching
void LayerManager::renderAll_interleavedBatching(sf::RenderWindow& window, AssetManager& textures, const EntityManager& entityManager) {
    int drawCalls = 0;
    for (int id : drawOrder) {
        auto it = layers.find(id);
        if (it == layers.end() || !it->second.getRenderViewport()) continue;

        TileLayer& layer = it->second;
        sf::VertexArray currentBatch(sf::Quads);
        std::string currentTextureID = "";
        sf::RenderStates currentState;

        updateDrawOrderForLayer(id, entityManager);

        for (TileInstance* tile : perLayerDrawOrder) {
            std::string texID = textures.getTileTextureLookup().at(tile->textureID).textureID;

            if (texID != currentTextureID) {
                if (currentBatch.getVertexCount() > 0) {
                    currentState.texture = &textures.getTextures().at(currentTextureID);
                    window.draw(currentBatch, currentState);
                    currentBatch.clear();
                    drawCalls++;
                }
                currentTextureID = texID;
            }

            sf::Vector2f pos = Math::isoToCart(tile->gridPos.x, tile->gridPos.y) + tile->worldOffset;
            sf::IntRect rect = (tile->textureID == "character") ? tile->textureRect : textures.getTileTextureLookup().at(tile->textureID).texRect;

            float w = static_cast<float>(rect.width);
            float h = static_cast<float>(rect.height);
            float tx = static_cast<float>(rect.left);
            float ty = static_cast<float>(rect.top);
            int opacity = layerOpacity[id] * 255;

            sf::Color color(255, 255, 255, opacity);
            currentBatch.append(sf::Vertex({pos.x - w / 2.f, pos.y - h / 2.f}, color, {tx, ty}));
            currentBatch.append(sf::Vertex({pos.x + w / 2.f, pos.y - h / 2.f}, color, {tx + w, ty}));
            currentBatch.append(sf::Vertex({pos.x + w / 2.f, pos.y + h / 2.f}, color, {tx + w, ty + h}));
            currentBatch.append(sf::Vertex({pos.x - w / 2.f, pos.y + h / 2.f}, color, {tx, ty + h}));
        }

        if (currentBatch.getVertexCount() > 0) {
            currentState.texture = &textures.getTextures().at(currentTextureID);
            window.draw(currentBatch, currentState);
            drawCalls++;
        }
    }
}

// Update draw order for a specific layer
void LayerManager::updateDrawOrderForLayer(int layerId, const EntityManager& entityManager) {
    auto it = layers.find(layerId);
    if (it == layers.end()) return;

    TileLayer& layer = it->second;
    perLayerDrawOrder.clear();

    for (auto& tile : layer.getTilePool()) {
        perLayerDrawOrder.push_back(tile.get());
    }

    for (const auto& entity : entityManager.getEntities()) {
        if (entity && entity->getTileInstance() && entity->getRenderLayer() == layerId) {
            perLayerDrawOrder.push_back(entity->getTileInstance());
        }
    }

    std::sort(perLayerDrawOrder.begin(), perLayerDrawOrder.end(), [](TileInstance* a, TileInstance* b) {
        sf::Vector2i aWorld = Math::cartToIso(a->worldOffset.x + a->anchorOffset.x, a->worldOffset.y + a->anchorOffset.y);
        sf::Vector2i bWorld = Math::cartToIso(b->worldOffset.x + b->anchorOffset.x, b->worldOffset.y + b->anchorOffset.y);

        float za = a->gridPos.x + a->gridPos.y + aWorld.x + aWorld.y;
        float zb = b->gridPos.x + b->gridPos.y + bWorld.x + bWorld.y;



        if (za == zb)
            return std::tie(a->gridPos.y, a->gridPos.x) < std::tie(b->gridPos.y, b->gridPos.x);
        return za < zb;
    });
}

void LayerManager::drawDebugOverlay(TileInstance* tile) {
    // Placeholder for debugging
}

// Getters
const std::map<int, TileLayer>& LayerManager::getLayers() const { return layers; }
const std::vector<int>& LayerManager::getDrawOrder() const { return drawOrder; }
std::map<int, float>& LayerManager::getOpacityMap() { return layerOpacity; }

int LayerManager::getNextFreeLayerIndex() const {
    int index = 0;
    while (layers.count(index)) ++index;
    return index;
}
