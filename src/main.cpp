#include <iostream>
#include <ostream>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <vector>
#include <filesystem>
#include "World.h"
#include "LayerManager.h"
#include "editor/SpriteWindow.h"

#ifdef __APPLE__
    #include <mach-o/dyld.h>
#elif defined(_WIN32)
    #include <windows.h>
#else
    #include <unistd.h>
#endif

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

// Get the directory where the executable is located
std::string getExecutableDir() {
    #ifdef __APPLE__
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
            return fs::path(path).parent_path().string() + "/";
        }
    #elif defined(_WIN32)
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return fs::path(path).parent_path().string() + "\\";
    #else
        char path[1024];
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path)-1);
        if (len != -1) {
            path[len] = '\0';
            return fs::path(path).parent_path().string() + "/";
        }
    #endif
    return "./"; // fallback to current directory
}

int main() {
    std::string exeDir = getExecutableDir();
    std::cout << "Executable directory: " << exeDir << std::endl;

    if (!debugFont.loadFromFile(exeDir + "assets/fonts/Arial.ttf")) {
        std::cout << "Failed to load font" << std::endl;
    }

    World world;
    world.init();

    LayerManager& layerManager = world.getLayerManager();

    AssetManager textures;
    textures.loadTexture("prototype", exeDir + "assets/prototype.png");
    textures.loadTexture("spiritHomeTerrain", exeDir + "assets/spiritHomeTerrain.png");
    textures.setDebugFont(debugFont);

    textures.loadTexture("tree01", exeDir + "assets/single/tree01.png");
    textures.loadTexture("tree2", exeDir + "assets/single/tree2.png");
    textures.loadTexture("moon", exeDir + "assets/moonset.png");
    textures.loadTexture("character", exeDir + "assets/character/SpriteSheet.png");

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
