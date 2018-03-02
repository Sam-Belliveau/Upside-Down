#include <SFML/Graphics.hpp>
#include "Constants.h"
#ifndef WINDOW_H
#define WINDOW_H

namespace Graphics
{
    void checkEvents(sf::RenderWindow& app);
    void pushRGBA(sf::RenderWindow& app, const unsigned char* pixels);
};

#endif // WINDOW_H
