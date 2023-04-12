#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "../Include/AppUtils.h"
#include <immintrin.h>

using namespace sf;

// #define DRAW_MODE

inline void MandelbrotCalc(Color* pixels);
inline __m256i GetIteration(__m256 X0, __m256 Y0);

inline void printf_m256(__m256 a);
inline void printf_m256i(__m256i a);

#define CYCLE_MAX 1
#define N_MAX     255

const int width  = 640;
const int height = 560;
const int num_pixels = width * height;

const float r_max = 10.0;

float x_max =  1.0;
float x_min = -2.0;
float y_max =  1.0;
float y_min = -1.0;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;

const __m256 xShift_coeffs = _mm256_set_ps(7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.0);
const __m256 R_max = _mm256_set1_ps(r_max);

inline void GetFps(Clock clock, float* fps, int* n_fps, float* sum_fps, float* avg_fps);

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

// inline void GetFps(Clock clock, float* fps, int* n_fps, float* sum_fps, float* avg_fps)
// {
//     float working_time = clock.restart().asSeconds();
//     *fps = 1.f / working_time;
//     (*n_fps)++;
//     (*sum_fps) += *fps;
//     (*avg_fps) = *sum_fps / *n_fps;
// }

inline void MandelbrotCalc(Color* pixels)
{
    __m256 Y0 = _mm256_set1_ps(y_max);

    __m256 xShift = _mm256_mul_ps(_mm256_set1_ps(dx), xShift_coeffs);
    __m256 Dx =  _mm256_set1_ps(8*dx);
    __m256 Dy = _mm256_set1_ps(dy);

    for (int yi = 0; yi < height; yi++)
    {
        __m256 X0 = _mm256_add_ps( _mm256_set1_ps(x_min), xShift);

        for (int xi = 0; xi < width; xi+=8)
        {
            // printf_m256(X0);
            // printf_m256(Y0);
            // printf("\n");

            __m256i N = GetIteration(X0, Y0);

            // printf_m256i(N);

            int* N_int = (int*)(&N);
            int pixel_i = yi*width+xi;

            for (int i = 0; i < 8; i++, pixel_i++)
            {
                Uint8 n = (Uint8)(N_int[i]);
                pixels[pixel_i] = {n, 255, n, n};   //255 % n;            // n;
                                                    //255 % n % n;        // 64 + n%4*64;
                                                    //255 % n % n % n;    // 255 - n;
                                                    //255;                // n%255; //128 + n%2*128;
            }

            X0 = _mm256_add_ps(X0, Dx);
        }

        Y0 = _mm256_sub_ps(Y0, Dy);
    }
}

inline __m256i GetIteration(__m256 X0, __m256 Y0)
{
    __m256i N  = _mm256_setzero_si256();
    __m256 x   = _mm256_setzero_ps();
    __m256 y   = _mm256_setzero_ps();
    __m256 x2  = _mm256_setzero_ps();
    __m256 y2  = _mm256_setzero_ps();
    __m256 xy  = _mm256_setzero_ps();
    __m256 R   = _mm256_setzero_ps();

    for (int i = 0; i < N_MAX; i++)
    {
        __m256 cmp = _mm256_cmp_ps(R_max, R, _CMP_GT_OQ);
        int mask = _mm256_movemask_ps(cmp);
        if (!mask) break;

        x2 = _mm256_mul_ps(x, x);
        y2 = _mm256_mul_ps(y, y);
        xy = _mm256_mul_ps(x, y);
        x  = _mm256_add_ps(_mm256_sub_ps(x2, y2), X0);
        y  = _mm256_add_ps(_mm256_add_ps(xy, xy), Y0);
        R  = _mm256_add_ps(x2, y2);

        __m256i dN =_mm256_castps_si256(cmp);
        N = _mm256_add_epi16(N, dN);
    }

    return N;
}

inline void printf_m256(__m256 a)
{
    for (int i = 0; i < 8; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}

inline void printf_m256i(__m256i a)
{
    for (int i = 0; i < 8; i++) printf("%d ", (Uint8)(((int*)(&a))[i]));
    printf("\n");
}
