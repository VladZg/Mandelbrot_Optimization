#ifndef APP_UTILS_CPP
#define APP_UTILS_CPP

#include <stdlib.h>
#include <math.h>
#include "../Include/AppUtils.h"

Music* CreateMusicObject(const char* source)
{
    Music* music = new Music;
    music->openFromFile(source);
    assert(music != nullptr);

    return music;
}

Text* CreateTextObject(Font* font, const char* msg, int fontsize, Color color, Text::Style style, float x_position, float y_position)
{
    assert(msg != nullptr);
    Text* text = new Text;
    assert(text != nullptr);
    text->setFont(*font);
    text->setString(msg);
    text->setCharacterSize(fontsize);
    text->setFillColor(color);
    text->setStyle(style);
    text->setPosition(x_position, y_position);

    return text;
}

Sprite* CreateSpriteObject(const char* source, float x_position, float y_position)
{
    assert(source != nullptr);

    Image* image = new Image;
    assert(image != nullptr);
    image->loadFromFile(source);

    Texture* texture = new Texture;
    assert(texture != nullptr);
    texture->loadFromImage(*image);

    Sprite* sprite = new Sprite;
    assert(sprite != nullptr);
    sprite->setTexture(*texture);
    sprite->setPosition(x_position, y_position);

    return sprite;
}

int StretchSprite(Sprite* sprite, float width, float height)
{
    Vector2f targetSize(width, height);

    sprite->setScale(targetSize.x / sprite->getLocalBounds().width,
                     targetSize.y / sprite->getLocalBounds().height);

    return 1;
}

Color HSV2RGB(float H, float S, float V)
{
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
		return sf::Color::Black;
	}

	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;

	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}

	int R = (r + m) * 255;
	int G = (g + m) * 255;
	int B = (b + m) * 255;

	return sf::Color(R, G, B);
}

#endif
