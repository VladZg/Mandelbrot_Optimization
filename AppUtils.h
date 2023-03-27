#ifndef APP_UTILS_H
#define APP_UTILS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <cassert>

using namespace sf;

Music* CreateMusicObject(const char* source);
Text* CreateTextObject(Font* font, const char* msg, int fontsize, Color color, Text::Style style, float x_position, float y_position);
Sprite* CreateSpriteObject(const char* source, float x_position, float y_position);
int StretchSprite(Sprite* sprite, float width, float height);
Color HSV2RGB(float H, float S, float V);

int StartApp(int file_hash, const char* filename);

#endif
