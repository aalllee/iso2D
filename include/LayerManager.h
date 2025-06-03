#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include <map>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "EntityManager.h"
#include "Tile.h"
#include "AssetManager.h"

class LayerManager {
public:
    int createLayer(const std::string& name = "");
    void removeLayer(int id);

    TileLayer* getLayer(int id);
    void moveLayerUp(int id);
    void moveLayerDown(int id);
    void includeDrawableEntity(int id, TileInstance* entityTile);

    void renderAll(sf::RenderWindow& window, const std::map<std::string, sf::Texture>& textures);
    void renderAll_interleavedBatching(sf::RenderWindow& window, AssetManager& textures, const EntityManager& entityManager);
    void drawDebugOverlay(TileInstance* tile);

    const std::map<int, TileLayer>& getLayers() const;
    const std::vector<int>& getDrawOrder() const;
    std::map<int, float>& getOpacityMap();

private:
    int getNextFreeLayerIndex() const;
    void updateDrawOrderForLayer(int layerId, const EntityManager& entityManager);

    std::map<int, TileLayer> layers;
    std::vector<int> drawOrder;
    std::vector<TileInstance*> perLayerDrawOrder;
    std::map<int, float> layerOpacity;
};

#endif // LAYER_MANAGER_H
