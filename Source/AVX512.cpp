#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "../Include/AppUtils.h"
#include <immintrin.h>

using namespace sf;

// #define DRAW_MODE

void MandelbrotCalc(Uint8 * pixels);
inline void printf_m512(__m512 a);

#define CYCLE_MAX 1
#define N_MAX     255

const int width = 640;
const int height = 560;
const int num_pixels = width * height;
float x_max = 1.f;
float x_min = -2.f;
float y_max = 1.f;
float y_min = -1.f;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;
const float r_max = 10.f;
const __m512 Dx_coeffs = _mm512_set_ps(15.f, 14.f, 13.f, 12.f, 11.f, 10.f, 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

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

//     float fps = 0.0;
//     Clock clock;
//     float current_time = 0.0;
//     float sum_fps = 0.0;
//     int n_fps = 0;
//     float avg_fps = 0.0;
//
//     while (true)
//     {
        MandelbrotCalc(pixels);
//
//         current_time = clock.restart().asSeconds();
//         fps = 1.0 / (current_time);
//         // printf("FPS: %f\n", fps_num);
//         n_fps++;
//         sum_fps += fps;
//         avg_fps = sum_fps / n_fps;
//         printf("FPS: %f => average FPS: %f\n", fps, avg_fps);
//     }


    // printf_m512(_mm512_mul_ps(_mm512_set1_ps(dx), Dx_coeffs));

    return 1;
}

#endif

inline void MandelbrotCalc(Uint8* pixels)
{
    for (int i = 0; i < 4 * num_pixels; i++)
        pixels[i] = 255;

    __m512 Y0 = _mm512_set1_ps(y_max);
    __m512 Dx = _mm512_mul_ps(_mm512_set1_ps(dx), Dx_coeffs);
    __m512 Dy = _mm512_set1_ps(dy);
    __m512 R_max = _mm512_set1_ps(r_max);

    for (int yi = 0; yi < height; yi++)
    {
        Y0 = _mm512_sub_ps(Y0, Dy);

        for (int xi = 0; xi < width; xi+=16)
        {
            __m512 X0 = _mm512_add_ps(_mm512_set1_ps(x_min + xi*dx), Dx);

            // for (int i = 0; i < 16; i++) {printf("(%.3f %.3f) ", ((float*)(&X0))[i], ((float*)(&Y0))[i]);}
            // printf("\n");

            __m512i N  = _mm512_setzero_si512();
            __m512 cmp = _mm512_setzero_ps();
            __m512 x   = _mm512_setzero_ps();
            __m512 y   = _mm512_setzero_ps();
            __m512 x2  = _mm512_setzero_ps();
            __m512 y2  = _mm512_setzero_ps();
            __m512 xy  = _mm512_setzero_ps();
            __m512 R   = _mm512_setzero_ps();

            for (int i = 0; i < N_MAX; i++)
            {
                x2 = _mm512_mul_ps(x, x);
                y2 = _mm512_mul_ps(y, y);
                xy = _mm512_mul_ps(x, y);
                x = _mm512_add_ps(_mm512_sub_ps(x2, y2), X0);
                y = _mm512_add_ps(_mm512_add_ps(xy, xy), Y0);
                R = _mm512_add_ps(x2, y2);

                __m256 cmp1 = _mm256_cmp_ps(R, R_max, 1);
                __m256 cmp2 = _mm256_cmp_ps(R, R_max, 1);

                int mask1 = _mm256_movemask_ps(cmp1);
                int mask2 = _mm256_movemask_ps(cmp2);

                if (!(mask1 && mask2)) break;

                __m512 cmp =
                __m512i dN =_mm512_castps_si512(cmp);
                N = _mm512_add_epi16(N, dN);
            }
//             // for (int i = 0; i < 4; i++) printf("%u ", (Uint8)(((int*)(&N))[i]));
//             // printf("\n");
//
//             int pixel_i = 4*yi*width+4*xi;
//             for (int i = 0; i < 8; i++) pixels[pixel_i+i*4+3] = (Uint8)(((int*)(&N))[i]);
        }
    }
}

inline void printf_m512(__m512 a)
{
    for (int i = 0; i < 16; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}
