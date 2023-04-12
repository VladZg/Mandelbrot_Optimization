#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "../Include/AppUtils.h"

// #define DRAW_MODE

using namespace sf;

void MandelbrotCalc(Color* pixels);
Uint8 GetIteration(float X0, float Y0);

#define N_CYCLE 1
#define N_MAX   255

const int width = 640;
const int height = 560;
const int num_pixels = width * height;

const float R_max = 10.0;

float x_max =  1.0;
float x_min = -2.0;
float y_max =  1.0;
float y_min = -1.0;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;

#ifdef DRAW_MODE

void MoveScreen(int keycode, float* x_max, float* x_min, float* y_max, float* y_min);

int main()
{
    RenderWindow window(VideoMode(width, height), "Mandelbrot");
    Color pixels[num_pixels] = {};

    Image image;
    image.create(width, height, (Uint8*)pixels);
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
                    MoveScreen(event.key.code, &x_max, &x_min, &y_max, &y_min);

                    dx = (x_max-x_min)/width;
                    dy = (y_max-y_min)/height;

                    break;
                }
            }
        }

        clock.restart();

        MandelbrotCalc(pixels);

        texture.update((Uint8*)pixels, width, height, 0, 0);
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

void MoveScreen(int keycode, float* x_max, float* x_min, float* y_max, float* y_min)
{
    switch(keycode)
    {
        case Keyboard::A:
        {
            *x_min -= 5*dx;
            *x_max -= 5*dx;
            break;
        }

        case Keyboard::D:
        {
            *x_min += 5*dx;
            *x_max += 5*dx;
            break;
        }

        case Keyboard::W:
        {
            *y_min += 5*dy;
            *y_max += 5*dy;
            break;
        }

        case Keyboard::S:
        {
            *y_min -= 5*dy;
            *y_max -= 5*dy;
            break;
        }

        case Keyboard::Key::LShift:
        {
            float delta_x = (*x_max - *x_min)/8;
            float delta_y = (*y_max - *y_min)/8;
            *x_min -= delta_x;
            *x_max += delta_x;
            *y_min -= delta_y;
            *y_max += delta_y;
            break;
        }

        case Keyboard::RShift:
        {
            float delta_x = (*x_max - *x_min)/8;
            float delta_y = (*y_max - *y_min)/8;
            *x_min += delta_x;
            *x_max -= delta_x;
            *y_min += delta_y;
            *y_max -= delta_y;
            break;
        }
    }
}

#else

int main()
{
    Color pixels[num_pixels] = {};

    Clock clock;
    float fps = 0.0;
    float sum_fps = 0.0;
    int   n_fps = 0;
    float avg_fps = 0.0;

    while (true)
    {
        clock.restart();

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

void MandelbrotCalc(Color* pixels)
{
    float Y0 = y_max;

    for (int yi = 0; yi < height; yi++)
    {
        float X0 = x_min;

        for (int xi = 0; xi < width; xi++)
        {
            // printf("%f\n", X0);
            // printf("%f\n", Y0);
            // printf("\n");

            Uint8 n = GetIteration(X0, Y0);
            // printf("%d ", n);

            pixels[yi*width+xi] = {n, 255, n, n};

            X0 += dx;
        }

        Y0 -= dy;
    }
}

Uint8 GetIteration(float X0, float Y0)
{
    Uint8 n_iter = 0;
    float x  = 0.0;
    float y  = 0.0;
    float x2 = 0.0;
    float y2 = 0.0;
    float xy = 0.0;
    float R  = 0.0;

    for (; R < R_max && n_iter < N_MAX; n_iter++)
    {
        x2 = x * x;
        y2 = y * y;
        xy = x * y;
        x  = x2 - y2 + X0;
        y  = xy + xy + Y0;
        R  = x*x + y*y;
    }

    return n_iter;
}
