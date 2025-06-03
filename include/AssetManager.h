#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

struct TileTextureData {
    std::string textureID;
    sf::IntRect texRect;
};


class AssetManager {
public:
    bool loadTexture(const std::string& id, const std::string& path);
    const sf::Texture& getTexture(const std::string& id) const;
    bool hasTexture(const std::string& id) const;
    const std::map<std::string, sf::Texture>& getTextures() {return textures;}
    void initTileRegistry();

    const std::unordered_map<std::string, TileTextureData>& getTileTextureLookup(){return tileTextureLookup;}
private:
    std::map<std::string, sf::Texture> textures;


// key: tileID (string), value:TextureData
  std::unordered_map<std::string, TileTextureData> tileTextureLookup;
};
