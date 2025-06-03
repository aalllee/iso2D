//
// Created by User on 30/05/2025.
//

#ifndef MATH_H
#define MATH_H

namespace Math{

    inline int TILE_WIDTH = 32;
    inline int TILE_HEIGHT = 16;

   inline  sf::Vector2f isoToCart(float x, float y) {
        return sf::Vector2f(
            (x - y) * TILE_WIDTH / 2,
            (x + y) * TILE_HEIGHT / 2
        );
    }

   inline  void drawCartesianGrid(sf::RenderWindow& window, int width=10, int height=10, float tileSize = 32.f) {
        sf::VertexArray lines(sf::Lines);

        sf::Color color = sf::Color::Blue;

        for (int x = 0; x <= width; ++x) {
            float xpos = x * tileSize;
            lines.append(sf::Vertex(sf::Vector2f(xpos, 0), color));
            lines.append(sf::Vertex(sf::Vector2f(xpos, height * tileSize), color));
        }

        for (int y = 0; y <= height; ++y) {
            float ypos = y * tileSize;
            lines.append(sf::Vertex(sf::Vector2f(0, ypos), color));
            lines.append(sf::Vertex(sf::Vector2f(width * tileSize, ypos), color));
        }

        window.draw(lines);
    }



inline sf::Vector2i cartToIso(float isoX, float isoY) {
        int x = static_cast<int>(std::floor((isoY / TILE_HEIGHT + isoX / TILE_WIDTH)));
        int y = static_cast<int>(std::floor((isoY / TILE_HEIGHT - isoX / TILE_WIDTH)));
        return sf::Vector2i(x, y);
    }

    inline float distance2(const sf::Vector2f& a, const sf::Vector2f& b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return (dx * dx + dy * dy);
    }

    inline sf::VertexArray buildInfiniteIsoGrid(int gridWidth=50, int gridHeight=50, int tileWidth=32, int tileHeight=16) {
        sf::VertexArray grid(sf::Lines);

        // Draw ↘ diagonals (vertical cartesian grid lines)
        for (int x = -gridWidth/2; x <= gridWidth; ++x) {
            sf::Vector2f start = isoToCart(x, -gridHeight);
            sf::Vector2f end   = isoToCart(x, gridHeight);

            grid.append(sf::Vertex(start, sf::Color::White));
            grid.append(sf::Vertex(end, sf::Color::Blue));

            start = isoToCart(-gridWidth, x);
            end   = isoToCart(gridWidth, x);

            grid.append(sf::Vertex(start, sf::Color::Blue));
            grid.append(sf::Vertex(end, sf::Color::White));

        }


        return grid;
    }

}

#endif //MATH_H
