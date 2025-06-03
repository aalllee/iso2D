#pragma once
#include <SFML/Graphics.hpp>

class IsoCamera {
public:
    IsoCamera(const sf::Vector2f& startCenter, const sf::Vector2f& size);

    void handleInput(const sf::RenderWindow& window, float dt);
    void apply(sf::RenderWindow& window) const;

    float getZoom() const { return zoomLevel; }
    void setZoom(float zoom) { this->zoomLevel = zoom; }

    void setView(const sf::View& view) { this->view = view; }

    void setCenter(const sf::Vector2f& center) { this->view.setCenter(center); }
    void setSize(const sf::Vector2f& size) { this->view.setSize(size); view.zoom(zoomLevel); }

    void resetView(float w, float h) {view.setViewport(getLetterboxView(w,h));}
    sf::FloatRect getLetterboxView(int windowWidth, int windowHeight);
    sf::View& getView()  { return view; }

private:
    sf::View view;
    float moveSpeed = 300.f;       // pixels per second
    float zoomSpeed = 0.999999f;        // factor
    float zoomLevel = 1.f;

    int virualWidth = 800;
    int virualHeight = 450;
};
