#include <iostream>
#include <ostream>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <vector>
#include <filesystem>
#include "World.h"
#include "LayerManager.h"
#include "editor/SpriteWindow.h"
namespace fs = std::filesystem;

#include "IsoCamera.h"
#include "AssetManager.h"
#include <string>

#include "imgui-SFML.h"
#include "util/math.h"
#include "editor/EditorWindow.h"

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 16;
const int MAP_WIDTH = 7;
const int MAP_HEIGHT = 5;

sf::Font debugFont;
sf::Clock deltaClock;

int main() {

    if (!debugFont.loadFromFile("../assets/fonts/Arial.ttf")) {
        std::cout << "Failed to load font" << std::endl;
    }

    World world;
    world.init();

    LayerManager& layerManager = world.getLayerManager();

    AssetManager textures;
    textures.loadTexture("prototype", "assets/prototype.png");

    textures.loadTexture("tree01", "assets/single/tree01.png");
    textures.loadTexture("tree2", "assets/single/tree2.png");
    textures.loadTexture("moon", "assets/moonset.png");
    textures.loadTexture("character", "assets/character/SpriteSheet.png");

    textures.initTileRegistry();

    EditorContext context;
    context.assetManager =&textures;
    context.entityManager = &world.getEntityManager();
    context.layerManager = &world.getLayerManager();
    context.world = &world;

    std::vector<sf::Vector2f> testCollider = {{20,20}, {50,20}, {50,50},{20,50}};

    world.getCollisionManager().createCollider(testCollider);

   // sf::RenderWindow window(sf::VideoMode(800, 600), "Isometric Test");
    SpriteWindow spWindow(sf::Vector2u(500,500),"sprite window");
    EditorWindow editorWindow(sf::Vector2u(800, 600),"editor window");

    sf::Clock clock;

    sf::Vector2f center = Math::isoToCart(MAP_WIDTH / 2, MAP_HEIGHT / 2);
    IsoCamera camera(center, {800.f, 600.f});
    context.camera = &camera;


    while (editorWindow.isOpen()) {
        sf::Time dt = deltaClock.restart();
        float deltaTime = dt.asSeconds();

        spWindow.update(dt,context);
        spWindow.handleEvent(context);
        spWindow.render(context);


        world.getEntityManager().updatePlayer(deltaTime, world.getCollisionManager().getColliders());
        world.getEventManager().updateEventRegions(world.getEntityManager().getEntities());
        editorWindow.update(dt, context);

    }

    return 0;
}
