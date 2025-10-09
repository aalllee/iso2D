//
// Created by User on 06/10/2025.
//
#include "Tree.h"

Tree::Tree() {

    renderTile.textureID = "tree01";
    renderTile.gridPos = {0,0};
    renderTile.anchorOffset = {0,18};
    renderTile.textureRect = sf::IntRect(0,0,105,128);
    renderTile.worldOffset = {0,0};
    renderLayer=1;


}
