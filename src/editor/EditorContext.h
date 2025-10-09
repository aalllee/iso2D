//
// Created by User on 27/05/2025.
//

#ifndef EDITORCONTEXT_H
#define EDITORCONTEXT_H

#include <SFML/Graphics.hpp>

#include "Tile.h"
#include "AssetManager.h"
#include "EntityManager.h"
#include "LayerManager.h"
#include "World.h"
#include "IsoCamera.h"

#include <string>

enum class Mode {
    Place, //Num1
    Edit ,// Num2
    EditCollider,
    Universal,

};

struct EditorContext {

    Mode currentMode = Mode::Universal;

    AssetManager* assetManager = nullptr;
    EntityManager* entityManager = nullptr;
    LayerManager* layerManager = nullptr;
    TileInstance* selectedSprite= nullptr;
    TileInstance* selectedTile = nullptr;
    Collider* selectedCollider_p = nullptr;
    EventRegion* selectedEventRegion_p = nullptr;
    int selectedLayerId = 0;

    TileLayer* currentLayer = nullptr;


    // Drag state
    bool isDragging = false;
    sf::Vector2i dragTileNewPos = {0, 0};
    sf::Vector2i placePosStart = {0,0};

    // Grid customization
    bool spriteViewCustomGrid = false;
    sf::Vector2i customGridSize = {32, 32};
    sf::Vector2i customGridPos = {0, 0};



    int selectedCollider = 0;
    int selectedEventRegion = 0;
    World* world;
    IsoCamera* camera = nullptr;

    bool enableGridSnapping = false;

    bool drawDebug = true;
    bool showTileDebug = true;

    float worldOffset_temp = 0.0f;





};


#endif //EDITORCONTEXT_H
