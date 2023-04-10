#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "./AppUtils.h"
#include <immintrin.h>

using namespace sf;

// #define DRAW_MODE

void MandelbrotCalc(Uint8 * pixels);
inline void printf_m256(__m256 a);
inline void SetPixels(Uint8* pixels, __m256 X0, __m256 Y0, __m256 R_max, int xi, int yi);

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
const __m256 Dx_coeffs = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

inline void GetFps(Clock clock, float* fps, int* n_fps, float* sum_fps, float* avg_fps);

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

    float fps_num = 0.f;
    char fps_str[20] = {};
    Font* font = new Font;
    font->loadFromFile("./fonts/arial.ttf");
    Text* fps_obj  = CreateTextObject(font, fps_str, 18, Color::Magenta, Text::Style::Bold, 0.0, 0.0);

    Clock clock;
    float current_time = 0.f;

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
        sprintf(fps_str, "fps: %.2f", fps_num);
        fps_obj->setString(fps_str);

        window.draw(sprite);
        // window.draw(*fps_obj);
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
        clock.restart();

        MandelbrotCalc(pixels);

        // GetFps(clock, &fps, &n_fps, &sum_fps, &avg_fps);

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

// inline void GetFps(Clock clock, float* fps, int* n_fps, float* sum_fps, float* avg_fps)
// {
//     float working_time = clock.restart().asSeconds();
//     *fps = 1.f / working_time;
//     (*n_fps)++;
//     (*sum_fps) += *fps;
//     (*avg_fps) = *sum_fps / *n_fps;
// }

inline void MandelbrotCalc(Uint8* pixels)
{
    for (int i = 0; i < 4 * num_pixels; i++)
        pixels[i] = 255;

    __m256 Y0 = _mm256_set1_ps(y_max);
    __m256 Dx = _mm256_mul_ps(_mm256_set1_ps(dx), Dx_coeffs);
    __m256 Dy = _mm256_set1_ps(dy);
    __m256 R_max = _mm256_set1_ps(r_max);

    for (int yi = 0; yi < height; yi++)
    {
        Y0 = _mm256_sub_ps(Y0, Dy);

        for (int xi = 0; xi < width; xi+=8)
        {
            __m256 X0 = _mm256_add_ps(_mm256_set1_ps(x_min + xi*dx), Dx);

            SetPixels(pixels, X0, Y0, R_max, xi, yi);
        }
    }
}

inline void SetPixels(Uint8* pixels, __m256 X0, __m256 Y0, __m256 R_max, int xi, int yi)
{
    __m256i N  = _mm256_setzero_si256();
    __m256 cmp = _mm256_setzero_ps();
    __m256 x   = _mm256_setzero_ps();
    __m256 y   = _mm256_setzero_ps();
    __m256 x2  = _mm256_setzero_ps();
    __m256 y2  = _mm256_setzero_ps();
    __m256 xy  = _mm256_setzero_ps();
    __m256 R   = _mm256_setzero_ps();

    for (int i = 0; i < N_MAX; i++)
    {
        x2 = _mm256_mul_ps(x, x);
        y2 = _mm256_mul_ps(y, y);
        xy = _mm256_mul_ps(x, y);
        x  = _mm256_add_ps(_mm256_sub_ps(x2, y2), X0);
        y  = _mm256_add_ps(_mm256_add_ps(xy, xy), Y0);
        R  = _mm256_add_ps(x2, y2);

        cmp = _mm256_cmp_ps(R_max, R, _CMP_GT_OQ);
        int mask = _mm256_movemask_ps(cmp);
        if (!mask) break;

        __m256i dN =_mm256_castps_si256(cmp);
        N = _mm256_add_epi16(N, dN);
    }

    int pixel_i = 4*yi*width+4*xi;

    for (int i = 0; i < 8; i++)
    {
        Uint8 n = (Uint8)(((int*)(&N))[i]);
        pixels[pixel_i  ] = (255 + n) * 23 % n    ; //255 % n;            // n;
        pixels[pixel_i+1] = (255 - n) * 23 % n    ; //255 % n % n;        // 64 + n%4*64;
        pixels[pixel_i+2] = (255 / n) * 23 % n    ; //255 % n % n % n;    // 255 - n;
        pixels[pixel_i+3] = 200                   ; //255;                // n%255; //128 + n%2*128;
        pixel_i += 4;
    }
}

inline void printf_m256(__m256 a)
{
    for (int i = 0; i < 8; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}
