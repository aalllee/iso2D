//
// Created by User on 27/05/2025.
//

#ifndef EDITORCONTEXT_H
#define EDITORCONTEXT_H

#include <SFML/Graphics.hpp>

#include "../../include/Tile.h"
#include "../../include/AssetManager.h"
#include "../../include/EntityManager.h"
#include "../../include/LayerManager.h"
#include "../../include/World.h"
#include "../../include/IsoCamera.h"

#include <string>

enum class Mode {
    Place, //Num1
    Edit ,// Num2
    EditCollider

};

struct EditorContext {

    Mode currentMode = Mode::Place;

    AssetManager* assetManager = nullptr;
    EntityManager* entityManager = nullptr;
    LayerManager* layerManager = nullptr;
    TileInstance selectedSprite;
    TileInstance* selectedTile = nullptr;
    int selectedLayerId = 0;

    TileLayer* currentLayer = nullptr;


    // Drag state
    bool isDragging = false;
    sf::Vector2i dragTileNewPos = {0, 0};

    // Grid customization
    bool spriteViewCustomGrid = false;
    sf::Vector2i customGridSize = {32, 32};
    sf::Vector2i customGridPos = {0, 0};



    int selectedCollider = 0;
    int selectedEventRegion = 0;
    World* world;
    IsoCamera* camera = nullptr;


};


#endif //EDITORCONTEXT_H
