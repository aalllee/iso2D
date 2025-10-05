#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Tile.h"

enum class EntityType {
    Base,
    Player,
    NPC,
    Item,
    Interactable,
    Static,
};


class Entity {
public:
    Entity(const EntityType type = EntityType::Base);
    virtual ~Entity() = default;

    const EntityType& getType() const;
    const std::string& getName() const;

    void setName(const std::string& newName);

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;

    void setTextureID(const std::string& id);
    const std::string& getTextureID() const;

    void setTextureRect(const sf::IntRect& rect);
    const sf::IntRect& getTextureRect() const;

    TileInstance* getTileInstance() {return &renderTile;}
    const TileInstance* getTileInstance() const {return &renderTile;} //for read only

    int getRenderLayer() {return renderLayer;}
    void setRenderLayer(int layer) {renderLayer = layer;}

protected:
    const EntityType type;
    std::string name = "Unnamed";
    sf::Vector2f position{0.f, 0.f};
    std::string textureID = "unknown";
    sf::IntRect textureRect{0, 0, 32, 32};
    TileInstance renderTile;
    int renderLayer = 0;

};

#endif // ENTITY_H
