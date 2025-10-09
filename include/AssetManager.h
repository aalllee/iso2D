#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

struct TileTextureData {
    std::string textureID;
    sf::IntRect texRect;
    sf::Vector2f worldOffset;
};


class AssetManager {
public:
    bool loadTexture(const std::string& id, const std::string& path);
    const sf::Texture& getTexture(const std::string& id) const;
    bool hasTexture(const std::string& id) const;
    const std::map<std::string, sf::Texture>& getTextures() {return textures;}
    void initTileRegistry();
    void updateDefaultWorldOffset(const std::string texID, sf::Vector2f newWorldOffset);

    sf::IntRect getTileRect(const std::string& id) const;
    inline void setDebugFont(sf::Font& font){debugFont=font;};
    inline sf::Font& getDebugFont(){return debugFont;};
    const std::unordered_map<std::string, TileTextureData>& getTileTextureLookup(){return tileTextureLookup;}
private:
    std::map<std::string, sf::Texture> textures;
    sf::Font debugFont;


// key: tileID (string), value:TextureData
  std::unordered_map<std::string, TileTextureData> tileTextureLookup;
};
