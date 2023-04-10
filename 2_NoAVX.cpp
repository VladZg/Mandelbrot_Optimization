#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <cassert>
#include "./AppUtils.h"

using namespace sf;

typedef float data_16f[4];
typedef int   data_16i[4];

inline void set1_ps(data_16f arr, float num) {for (int i = 0; i < 4; i++) arr[i] = num;}
inline void set_ps(data_16f arr, float num0, float num1, float num2, float num3) {arr[0] = num0; arr[1] = num1; arr[2] = num2; arr[3] = num3;}
inline void cmp_ps(data_16f a, const data_16f b, const data_16f c) {}
inline void add_ps(data_16f a, const data_16f b, const data_16f c) {for (int i = 0; i < 4; i++) a[i] = b[i] + c[i];}
inline void sub_ps(data_16f a, const data_16f b, const data_16f c) {for (int i = 0; i < 4; i++) a[i] = b[i] - c[i];}
inline void mul_ps(data_16f a, const data_16f b, const data_16f c) {for (int i = 0; i < 4; i++) a[i] = b[i] * c[i];}

// #define DRAW_MODE

inline void MandelbrotCalc(Uint8 * pixels);

#define CYCLE_MAX 1
#define N_MAX     255

const int width = 640;
const int height = 560;
const int num_pixels = width * height;
float x_max = 1.0;
float x_min = -2.0;
float y_max = 1.0;
float y_min = -1.0;
float dx = (x_max-x_min)/width;
float dy = (y_max-y_min)/height;
const float r_max = 10.0;
const data_16f _0123 = {0.0, 1.0, 2.0, 3.0};

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

inline void MandelbrotCalc(Uint8 * pixels)
{
    for (int i = 0; i < 4 * num_pixels; i++)
        pixels[i] = 255;

    data_16f Y0 = {}; set1_ps(Y0, y_max);
    data_16f Dx = {}; set1_ps(Dx, dx); mul_ps(Dx, Dx, _0123);
    data_16f Dy = {}; set1_ps(Dy, dy);
    data_16f R_max = {}; set1_ps(R_max, r_max);
    int N_max[4] = {}; for (int i = 0; i < 4; i++) {N_max[i] = N_MAX;}

    for (int yi = 0; yi < height; yi++)
    {
        sub_ps(Y0, Y0, Dy);

        for (int xi = 0; xi < width; xi+=4)
        {
            data_16f X0 = {};
            set1_ps(X0, x_min + xi*dx);
            add_ps(X0, X0, Dx);

            // for (int i = 0; i < 4; i++) {printf("(%.3f %.3f): ", X0[i], Y0[i]);}
            // printf("\n");

            int N[4] = {0};
            int cmp[4] = {1};
            int mask = 1;

            data_16f x  = {};
            data_16f y  = {};
            data_16f x2 = {};
            data_16f y2 = {};
            data_16f xy = {};
            data_16f R  = {};

            // for (int i = 0; i < 4; i++) { mask |= cmp[i] << i;}

            while (mask)
            {
                mask = 0;
                mul_ps(x2, x, x);
                mul_ps(y2, y, y);
                mul_ps(xy, x, y);
                sub_ps(x, x2, y2); add_ps(x, x, X0);
                add_ps(y, xy, xy); add_ps(y, y, Y0);
                for (int i = 0; i < 4; i++) {cmp[i] = (x[i] * x[i] + y[i] * y[i] <= R_max[i]) && (N[i] <= N_MAX) ? 1 : 0;}
                for (int i = 0; i < 4; i++) {mask += cmp[i];}
                for (int i = 0; i < 4; i++) {N[i] += cmp[i];}
            }

            // printf("(%d,%d): ", xi, yi);
            // for (int i = 0; i < 4; i++) {printf("%d ", N[i]);}
            // printf("\n");

            int pixel_i = 4*yi*width+4*xi;
            for (int i = 0; i < 4; i++) pixels[pixel_i+i*4+3] = (Uint8) N[i];
        }
    }
}
