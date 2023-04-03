#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "./AppUtils.h"
#include <emmintrin.h>

using namespace sf;

// #define DRAW_MODE

void MandelbrotCalc(Uint8 * pixels);

#define CYCLE_MAX 1
#define N_MAX     255

const int width = 560;
const int height = 500;
const int num_pixels = width * height;
float x_max = 1.0f;
float x_min = -2.0f;
float y_max = 1.0f;
float y_min = -1.0f;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;
const float r_max = 10.0f;
const __m128 _3210 = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);

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

    float fps = 0.0;
    Clock clock;
    float current_time = 0.0;
    float sum_fps = 0.0;
    int n_fps = 0;
    float avg_fps = 0.0;

    while (true)
    {
        MandelbrotCalc(pixels);

        current_time = clock.restart().asSeconds();
        fps = 1.0 / (current_time);
        // printf("FPS: %f\n", fps_num);
        n_fps++;
        sum_fps += fps;
        avg_fps = sum_fps / n_fps;
        printf("FPS: %f => average FPS: %f\n", fps, avg_fps);
    }

    return 1;
}

#endif

void MandelbrotCalc(Uint8 * pixels)
{
    for (int i = 0; i < 4 * num_pixels; i++)
        pixels[i] = 255;

    __m128 Y0 = _mm_set1_ps(y_max);
    __m128 Dx = _mm_mul_ps(_mm_set1_ps(dx), _3210);
    __m128 Dy = _mm_set1_ps(dy);
    __m128 R_max = _mm_set1_ps(r_max);

    for (int yi = 0; yi < height; yi++)
    {
        Y0 = _mm_sub_ps(Y0, Dy);

        for (int xi = 0; xi < width; xi+=4)
        {
            __m128 X0 = _mm_add_ps(_mm_set1_ps(x_min + xi*dx), Dx);

            // for (int i = 0; i < 4; i++) {printf("(%.3f %.3f) ", *(float*)(&X0 + i*sizeof(float)), *(float*)(&Y0 + i*sizeof(float)));}
            // printf("\n");

            __m128i N  = _mm_setzero_si128();
            __m128 cmp = _mm_set1_ps(1.0f);
            __m128 x   = _mm_set1_ps(0.0f);
            __m128 y   = _mm_set1_ps(0.0f);
            __m128 x2  = _mm_set1_ps(0.0f);
            __m128 y2  = _mm_set1_ps(0.0f);
            __m128 xy  = _mm_set1_ps(0.0f);
            __m128 R   = _mm_set1_ps(0.0f);

            for (int i = 0; i < N_MAX; i++)
            {
                x2 = _mm_mul_ps(x, x);
                y2 = _mm_mul_ps(y, y);
                xy = _mm_mul_ps(x, y);
                x = _mm_add_ps(_mm_sub_ps(x2, y2), X0);
                y = _mm_add_ps(_mm_add_ps(xy, xy), Y0);
                R = _mm_add_ps(x2, y2);
                cmp = _mm_cmple_ps(R, R_max);

                int mask = _mm_movemask_ps(cmp);
                if (!mask) break;

                __m128i dN =_mm_castps_si128(cmp);
                N = _mm_add_epi16(N, dN);
            }

            // printf("(%d,%d): ", xi, yi);
            // for (int i = 0; i < 4; i++) {printf("%u ", *((Uint8*)(&N + i)));}
            // printf("\n");

            // for (int i = 0; i < 4; i++) printf("%u ", (Uint8)(((int*)(&N))[i]));
            // printf("\n");

            int pixel_i = 4*yi*width+4*xi;
            for (int i = 0; i < 4; i++) pixels[pixel_i+i*4+3] = (Uint8)(((int*)(&N))[3-i]);
        }
    }
}
