#include "Entity.h"

Entity::Entity(const EntityType type) : type(type) {


}

const EntityType& Entity::getType() const {
    return type;
}



const std::string& Entity::getName() const {
    return name;
}

void Entity::setName(const std::string& newName) {
    name = newName;
}

void Entity::setPosition(const sf::Vector2f& pos) {
    position = pos;
}

sf::Vector2f Entity::getPosition() const {
    return position;
}

void Entity::setTextureID(const std::string& id) {
    textureID = id;
}

const std::string& Entity::getTextureID() const {
    return textureID;
}

void Entity::setTextureRect(const sf::IntRect& rect) {
    textureRect = rect;
}

const sf::IntRect& Entity::getTextureRect() const {
    return textureRect;
}
