#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <iostream>

#include <string>
#include "math.h"
#include "util/math.h"

enum class TileType {
    None,
    Grass,
    Road,
    Steps,
    Water,
    Wall,
    // Add more as needed
};



struct TileInstance {
    //TileType type;
    sf::Vector2i gridPos;
    std::string textureID;
    sf::IntRect textureRect;
    sf::Vector2f anchorOffset = {0,0};
    sf::Vector2f worldOffset = {0,0};
    sf::Vector2f zOrder = {0,0};
    //int firstVertexIndex = 0;
};

namespace std {
    template <>
    struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& v) const noexcept {
            std::size_t hx = std::hash<int>{}(v.x);
            std::size_t hy = std::hash<int>{}(v.y);
            return hx ^ (hy << 1);
        }
    };
}

class TileLayer {
public:
    using TilePtr = std::unique_ptr<TileInstance>;

    void addTile(const sf::Vector2i& pos, const std::string& textureID, const sf::IntRect& texRect, const sf::Vector2f& anchorOffset={0.f,0.f}, const sf::Vector2f& worldOffset={0.f,0.f});

    void removeTile(const sf::Vector2i& pos);

    void insertDynamicEntity(TileInstance* tile);

    void buildVertexBatches();

    void drawTileDebug(TileInstance* tile);


    TileInstance* getTileAtPixelPrecise(sf::Vector2f worldPos);

    bool moveTile(const sf::Vector2i& from, const sf::Vector2i& to);

    const std::vector<std::unique_ptr<TileInstance>>& getTilePool() const;




    TileInstance* getTile(const sf::Vector2i& pos);


    const std::vector<TileInstance*>& getDrawOrder();

    void clear();

    std::size_t size() const { return tilePool.size(); }

    const  std::map<std::string, sf::VertexArray>& getTextureBatches(){return textureBatches;}

    inline const bool getRenderViewport() {return renderViewport;}
    inline void setRenderViewport(bool v) {renderViewport = v;}
    inline void setSortEveryFrame(bool v) {sortEveryFrame = v;}
private:
    std::vector<TilePtr> tilePool;                            // Owns memory
    std::unordered_map<sf::Vector2i, TileInstance*> tileIndex; // Fast lookup //coord -> layer i
    std::vector<TileInstance*> drawOrder;
    std::map<std::string, sf::VertexArray> textureBatches;

    bool renderViewport = true;
// Render order
    bool dirty = false;

    bool sortEveryFrame = false;
};
#endif
