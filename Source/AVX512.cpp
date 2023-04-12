#include "../Include/Config.h"
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "../Include/AppUtils.h"
#include "../Include/IntrinsicsPrintf.h"
#include <immintrin.h>

#define DRAW_MODE

inline void MandelbrotCalc(Color* pixels);
inline __m512i GetIteration(__m512 X0, __m512 Y0);

#define CYCLE_MAX 1
#define N_MAX     255
#define VECT_SIZE 16

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

const __m512 xShift_coeffs = _mm512_set_ps(15.f, 14.f, 13.f, 12.f, 11.f, 10.f, 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
const __m512 R_max = _mm512_set1_ps(r_max);

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

inline void MandelbrotCalc(Color* pixels)
{
    __m512 Y0 = _mm512_set1_ps(y_max);

    __m512 xShift = _mm512_mul_ps(_mm512_set1_ps(dx), xShift_coeffs);
    __m512 Dx =  _mm512_set1_ps(VECT_SIZE*dx);
    __m512 Dy = _mm512_set1_ps(dy);

    for (int yi = 0; yi < height; yi++)
    {
        __m512 X0 = _mm512_add_ps( _mm512_set1_ps(x_min), xShift);

        for (int xi = 0; xi < width; xi+=VECT_SIZE)
        {
            // printf_m512(X0);
            // printf_m512(Y0);
            // printf("\n");

            __m512i N = GetIteration(X0, Y0);

            // printf_m512i(N);

            int* N_int = (int*)(&N);
            int pixel_i = yi*width+xi;

            for (int i = 0; i < VECT_SIZE; i++, pixel_i++)
            {
                Uint8 n = (Uint8)(N_int[i]);
                pixels[pixel_i] = {(Uint8)(255%n), (Uint8)(255%n%n), (Uint8)(255%n%n%n), (Uint8)255};
                //255 % n;            // n;
                //255 % n % n;        // 64 + n%4*64;
                //255 % n % n % n;    // 255 - n;
                //255;                // n%255; //128 + n%2*128;
            }

            X0 = _mm512_add_ps(X0, Dx);
        }

        Y0 = _mm512_sub_ps(Y0, Dy);
    }
}

inline __m512i GetIteration(__m512 X0, __m512 Y0)
{
    __m512i N   = _mm512_setzero_si512();
    __m512  x   = _mm512_setzero_ps();
    __m512  y   = _mm512_setzero_ps();
    __m512  x2  = _mm512_setzero_ps();
    __m512  y2  = _mm512_setzero_ps();
    __m512  xy  = _mm512_setzero_ps();
    __m512  R   = _mm512_setzero_ps();

    for (int i = 0; i < N_MAX; i++)
    {
        __mmask16 cmp = _mm512_cmp_ps_mask(R_max, R, _CMP_GT_OQ);   // _mm512_xor_ps
        // int mask = _mm_movemask_ps(cmp);                    // ?
        if (!cmp) break;

        x2 = _mm512_mul_ps(x, x);
        y2 = _mm512_mul_ps(y, y);
        xy = _mm512_mul_ps(x, y);
        x  = _mm512_add_ps(_mm512_sub_ps(x2, y2), X0);
        y  = _mm512_add_ps(_mm512_add_ps(xy, xy), Y0);
        R  = _mm512_add_ps(x2, y2);

        // __m128i mask = _mm_set1_epi32(1);
        // mask = _mm_maskz_broadcastb_epi8(cmp, mask);
        // __m512i dN =_mm512_cvtepu8_epi32(mask);

        __m512i dN = _mm512_setzero_epi32();
        dN = _mm512_mask_blend_epi32(cmp, _mm512_setzero_epi32(), _mm512_set1_epi32(1));
        N = _mm512_add_epi16(N, dN);
    }

    return N;
}
