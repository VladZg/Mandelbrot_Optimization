#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "./AppUtils.h"

// #define DRAW_MODE

using namespace sf;

void MandelbrotCalc(Uint8 * pixels);

#define CYCLE_MAX 1
#define N_MAX     255

const int width = 560;
const int height = 500;
const int num_pixels = width * height;
float x_max = 1.0;
float x_min = -2.0;
float y_max = 1.0;
float y_min = -1.0;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;
const float R0 = 10.0;

#ifdef DRAW_MODE

int main()
{
    RenderWindow window(VideoMode(width, height), "Mandelbrot");
    Uint8 pixels[4*num_pixels] = {};

    Image image;
    image.create(width, height, pixels);
    Texture texture;
    texture.loadFromImage(image);
    Sprite sprite(texture);

    float fps_num = 0.0;
    char fps_str[20] = {};
    Font* font = new Font;
    font->loadFromFile("./fonts/arial.ttf");
    Text* fps_obj  = CreateTextObject(font, fps_str, 10, Color::Magenta, Text::Style::Bold, 0.0, 0.0);

    Clock clock;
    float current_time = 0.0;

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
                            float delta_x = (x_max - x_min)/8;
                            float delta_y = (y_max - y_min)/8;
                            x_min -= delta_x;
                            x_max += delta_x;
                            y_min -= delta_y;
                            y_max += delta_y;
                            break;
                        }

                        case Keyboard::RShift:
                        {
                            float delta_x = (x_max - x_min)/8;
                            float delta_y = (y_max - y_min)/8;
                            x_min += delta_x;
                            x_max -= delta_x;
                            y_min += delta_y;
                            y_max -= delta_y;
                            break;
                        }
                    }

                    dx = (x_max-x_min)/width;
                    dy = (y_max-y_min)/height;

                    break;
                }
            }
        }

        MandelbrotCalc(pixels);

        texture.update(pixels, width, height, 0, 0);
        sprite.setTexture(texture);

        current_time = clock.restart().asSeconds();
        fps_num = 1.0 / (current_time);
        sprintf(fps_str, "fps: %f", fps_num);
        fps_obj->setString(fps_str);

        window.draw(sprite);
        window.draw(*fps_obj);
        window.display();
    }

    return 1;
}

#else

int main()
{
    Uint8 pixels[4*num_pixels] = {};

    Clock clock;
    float fps = 0.0;
    float sum_fps = 0.0;
    int n_fps = 0;
    float avg_fps = 0.0;

    while (true)
    {
        float current_time = clock.restart().asSeconds();

        MandelbrotCalc(pixels);

        float working_time = clock.restart().asSeconds();
        fps = 1.f / working_time;
        n_fps++;
        sum_fps += fps;
        avg_fps = sum_fps / n_fps;

        if (n_fps % 10 == 0) printf("FPS: %.2f => average FPS: %.2f\n", fps, avg_fps);
    }

    return 1;
}

#endif

void MandelbrotCalc(Uint8 * pixels)
{
    float Y0 = y_max;

    for (int yi = 0; yi < height; yi++)
    {
        float X0 = x_min;

        for (int xi = 0; xi < width; xi++)
        {
            int n = 0;
            float x = 0.0;
            float y = 0.0;
            float x2 = 0.0;
            float y2 = 0.0;
            float xy = 0.0;
            float R = 0.0;

            while (n++ < N_MAX && R <= R0)
            {
                x2 = x * x;
                y2 = y * y;
                xy = x * y;
                x  = x2 - y2 + X0;
                y  = xy + xy + Y0;
                R  = x*x + y*y;
            }

            int pixel_i = 4*yi*width+4*xi;

            pixels[pixel_i  ] = 255 ;
            pixels[pixel_i+1] = 255 ;
            pixels[pixel_i+2] = 255 ;
            pixels[pixel_i+3] = n   ;

            X0 += dx;
        }

        Y0 -= dy;
    }
}
