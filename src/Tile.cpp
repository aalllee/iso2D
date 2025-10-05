#include "Tile.h"

void TileLayer::addTile(const sf::Vector2i &pos, const std::string &textureID, const sf::IntRect &texRect,
const sf::Vector2f &anchorOffset, const sf::Vector2f &worldOffset) {

    if (tileIndex.count(pos))
        return; //key already exists, cant place there

    auto tile = std::make_unique<TileInstance>();
    tile->gridPos = pos;
    tile->textureID = textureID;
    tile->textureRect = texRect;
    tile->anchorOffset = anchorOffset;
    tile->worldOffset = worldOffset;
   // tile->zOrder = pos + worldOffset;

    TileInstance* raw = tile.get();
    tilePool.push_back(std::move(tile));
    drawOrder.push_back(raw);
    tileIndex[pos] = raw;
    dirty = true;

    std::cout << "TILE ADDED - Pos.x:" << pos.x << "POS.y:" << pos.y << std::endl;
    std::cout << "world offset" << worldOffset.x << " " << worldOffset.y << std::endl;
}

void TileLayer::removeTile(const sf::Vector2i &pos) {
    TileInstance* tileToRemove = tileIndex[pos];

    if (!tileToRemove) return; //nullptr guard


    tileIndex.erase(tileToRemove->gridPos);

    drawOrder.erase(std::remove(drawOrder.begin(), drawOrder.end(),tileToRemove), drawOrder.end());

    tilePool.erase(
        std::remove_if(tilePool.begin(), tilePool.end(),
            [&](const std::unique_ptr<TileInstance>& ptr) {
                return ptr.get() == tileToRemove;
            }),
        tilePool.end()
    );


}

void TileLayer::insertDynamicEntity(TileInstance *tile) {
    drawOrder.push_back(tile);
}

void TileLayer::buildVertexBatches()  {


    if (!dirty) return;
    // Clear previous batches
    for (auto& [id, array] : textureBatches) {
        array.clear();
    }

    for (TileInstance* tile : getDrawOrder()) {
        sf::VertexArray& array = textureBatches[tile->textureID];
        if (array.getPrimitiveType() != sf::Quads)
            array.setPrimitiveType(sf::Quads);

        // Convert to isometric position
        sf::Vector2f pos = Math::isoToCart(tile->gridPos.x, tile->gridPos.y);

        float w = static_cast<float>(tile->textureRect.width);
        float h = static_cast<float>(tile->textureRect.height);

        float tx = static_cast<float>(tile->textureRect.left);
        float ty = static_cast<float>(tile->textureRect.top);

        // Add quad vertices
        array.append(sf::Vertex({pos.x - w / 2.f, pos.y},           sf::Color::White, {tx,     ty}));
        array.append(sf::Vertex({pos.x + w / 2.f, pos.y},           sf::Color::White, {tx + w, ty}));
        array.append(sf::Vertex({pos.x + w / 2.f, pos.y + h},       sf::Color::White, {tx + w, ty + h}));
        array.append(sf::Vertex({pos.x - w / 2.f, pos.y + h},       sf::Color::White, {tx,     ty + h}));
    }
}



TileInstance * TileLayer::getTileAtPixelPrecise(sf::Vector2f worldPos)  {
    const auto& drawList = getDrawOrder(); // already Z-sorted back to front

    for (auto it = drawList.rbegin(); it != drawList.rend(); ++it) {
        TileInstance* tile = *it;
        sf::Vector2f isoPos = Math::isoToCart(tile->gridPos.x, tile->gridPos.y);
        float w = static_cast<float>(tile->textureRect.width);
        float h = static_cast<float>(tile->textureRect.height);


        std::cout << "GRID POS" <<tile->gridPos.x << " " << tile->gridPos.y << " " << std::endl;

        sf::FloatRect bounds(isoPos.x - w / 2.f, isoPos.y - h/2.f, w, h);
        std::cout << "BOUNDS:" <<bounds.left << " " << bounds.top << " " << bounds.width << " " << bounds.height << std::endl;
        std::cout << "MOUSE WORLD POS" <<worldPos.x << " " << worldPos.y << " " << std::endl;
        std::cout << "ISO POS" <<isoPos.x << " " << isoPos.y << " " << std::endl;
        // bounds.contains(worldPos.x, worldPos.y);
        if (bounds.contains(worldPos)) {
            std::cout << "TILE AT PIXEL FOUND" << tile->textureID << std::endl;

            return tile; // hit!
        }
    }

    return nullptr;
}

bool TileLayer::moveTile(const sf::Vector2i &from, const sf::Vector2i &to)  {
    auto it = tileIndex.find(from);
    if (it == tileIndex.end()) return false;

    if (tileIndex.count(to)) {
        std::cout << "Position already occupied. Move cancelled.\n";
        return false;
    }

    TileInstance* tile = it->second;
    tileIndex.erase(from);
    tile->gridPos = to;
    tileIndex[to] = tile;
    dirty = true;
    return true;

}

const std::vector<std::unique_ptr<TileInstance>> & TileLayer::getTilePool() const  {
    return tilePool;
}

TileInstance * TileLayer::getTile(const sf::Vector2i &pos)  {
    auto it = tileIndex.find(pos);
    return it != tileIndex.end() ? it->second : nullptr;
}

const std::vector<TileInstance *> & TileLayer::getDrawOrder() {
    //sort every frame for now, later sort only the dynamic layers...
    if (dirty || sortEveryFrame) {
        std::sort(drawOrder.begin(), drawOrder.end(), [](TileInstance* a, TileInstance* b) {
            sf::Vector2i aWorld = Math::cartToIso(a->worldOffset.x,a->worldOffset.y);
            sf::Vector2i bWorld = Math::cartToIso(b->worldOffset.x,b->worldOffset.y);


            int za = a->gridPos.x + a->gridPos.y + a->anchorOffset.x + a->anchorOffset.y + aWorld.x + aWorld.y;
            int zb = b->gridPos.x + b->gridPos.y + b->anchorOffset.x + b->anchorOffset.y + bWorld.x + bWorld.y;




            if (za == zb)
                return std::tie(a->gridPos.y, a->gridPos.x) < std::tie(b->gridPos.y, b->gridPos.x);
            return za < zb;
        });
        dirty = false;
    }
    return drawOrder;
}

void TileLayer::clear(){
    tilePool.clear();
    tileIndex.clear();
    drawOrder.clear();
    dirty = false;
}
