#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "./AppUtils.h"

using namespace sf;

const int width = 540;
const int height = 480;
const int num_pixels = width * height;
float x_max = 2.0;
float x_min = -2.0;
float y_max = 2.0;
float y_min = -2.0;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;
const float R0 = 10.0;
const int N_max = 255;

int main()
{
    RenderWindow window(VideoMode(width, height), "Mandelbrot");
    Uint8 pixels[4*num_pixels] = {};

    Image image;
    image.create(width, height, pixels);
    Texture texture;
    texture.loadFromImage(image);
    Sprite sprite(texture);

    while (window.isOpen())
    {
        window.clear();

        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed:
                {
                    window.close();
                    break;
                }

                case Event::KeyPressed:
                {
                    switch(event.key.code)
                    {
                        case Keyboard::A:
                        {
                            x_min -= 5*dx;
                            x_max -= 5*dx;
                            break;
                        }

                        case Keyboard::D:
                        {
                            x_min += 5*dx;
                            x_max += 5*dx;
                            break;
                        }

                        case Keyboard::W:
                        {
                            y_min += 5*dy;
                            y_max += 5*dy;
                            break;
                        }

                        case Keyboard::S:
                        {
                            y_min -= 5*dy;
                            y_max -= 5*dy;
                            break;
                        }

                        case Keyboard::Key::LShift:
                        {
                            x_min += x_min/2;
                            x_max += x_max/2;
                            y_min += y_min/2;
                            y_max += y_max/2;
                            break;
                        }

                        case Keyboard::RShift:
                        {
                            x_min -= x_min/2;
                            x_max -= x_max/2;
                            y_min -= y_min/2;
                            y_max -= y_max/2;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        dx = (x_max-x_min)/width;
        dy = (y_max-y_min)/height;

        for (int yi = 0; yi < height; yi++)
        {
            for (int xi = 0; xi < width; xi++)
            {
                float X0 = x_min + xi*dx;
                float Y0 = y_max - yi*dy;

                int n = 0;
                float x = 0.0;
                float y = 0.0;
                float x2 = 0.0;
                float y2 = 0.0;
                float xy = 0.0;
                float R = 0.0;

                while (n++ < N_max && R <= R0)
                {
                    x2 = x * x;
                    y2 = y * y;
                    xy = x * y;
                    x = x2 - y2 + Y0;
                    y = xy + xy + X0;
                    R = x*x + y*y;
                }

                int hue = (int)(255*n/256);
                int sat = 100;
                int val = n == N_max ? 0 : 100;
                Color hsv2rgb = HSV2RGB(hue, sat, val);

                pixels[4*yi*width+4*xi  ] = (Uint8) hsv2rgb.r;
                pixels[4*yi*width+4*xi+1] = (Uint8) hsv2rgb.g;
                pixels[4*yi*width+4*xi+2] = (Uint8) hsv2rgb.b;
                pixels[4*yi*width+4*xi+3] = (Uint8) 255;
            }
        }

        texture.update(pixels, width, height, 0, 0);
        sprite.setTexture(texture);

        window.draw(sprite);
        window.display();
    }

    return 1;
}
