#include <SFML/Graphics.hpp>
#include "Constants.h"
#ifndef WINDOW_H
#define WINDOW_H

namespace Graphics
{
    // Pass render window and have it display events
    void checkEvents(sf::RenderWindow& app);
    void pushRGBA(sf::RenderWindow& app, const unsigned char* pixels);
};

#endif // WINDOW_H
