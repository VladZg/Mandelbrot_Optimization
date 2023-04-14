# Optimization of drawing Mandelbrot set

## Introduction
In this work I tried different ways of speeding up the Mandelbrot set calculation algorithm. For that purpose I used available on my computer SIMD-instructions: SSE, AVX2 and AVX512.

**Goals**: 
- to achieve maximum speedup of the program using SIMD-instructions and optimization flags
- to measure and compare perfomances of SSE, AVX2 and AVX512 instructions
- to evaluate influence of compilation flags on the speed of the programm

## Algorithm of drawing

![Example 1](Pictures/1.png)

The Mandelbrot set is a good example of the beauty of mathematics, and there is a simple algorithm to construct it.



![Algorithm](Pictures/algorithm.png)

As we see, perfomance of algorithm depends on amount of pixels in resulting picture

``t ~ width * height``

Number of operating pixels in algorithm is ``640*560 = 358400``. It actually takes time to process all of them alternately, so algorithm optimization required.

## Optimization principles
Ideas of optimization that use SSE, AVX2 and AVX512 instructions are the same, but they partly have some different functions that are actually lead to the same result.

We are able to process 4 pixels at the same time by using ``__m128``, ``__m128i`` variables from the SSE instruction set, 8 pixels at the same time by using ``__m256``, ``__m256i`` variables and AVX2 and finally process 16 pixels by using ``__m512``, ``__m512i`` variables and AVX512 instructions. Here is mechanism of optimization described step-by-step.

... this section is in progress ...
... pay us to keep up to date with updates ...

## Time measuring principles
To visualise the Mandelbrot set, I use SFML, a modern graphics library for C++. It allows you to create a graphical window of the required size and display a picture as an array of pixels. There are special data type for pixels in SFML, it called ``Color`` and it contains four ``_uint8_t``.

There is a problem of evaluating algorithm working time while it uses SFML functions. For example we don't need to include diplaying time in calculations of fps, because it is not a part of the algorithm we speeding up and it requires the use of slow SFML functions.

To evaluate the speed of working algorithms correctly, I have created 2 programms: first one for visualizing Mandelbrot set and second one only for speed measurments. To compare programms perfomances I calculate fps value of every Mandelbrot calculation in cycle. This is where the ``clock.restart()`` function from this library helped me, it updates the time counter and returns the time since its last call.

``fps = 1 / calculating_time``

An important factor that affects perfomance is also how the colour components are calculated from the resulting number of iterations for each pixel. So that this part of the drawing algorithm is not limiting, I use a simple way to convert the number of iterations into colour components for the programm that evaluates perfomance of algorithm:

``n -> RGB(n, 255, 255-n, 255)``

However, for the version of programm that draws the resulting set I use various ways of translating the number of iterations to color to get beautiful images. You can see the examples of pictures in the end of this work.

Programm that evaluates speed also calculates average value of fps for the better precision. My measurements are made with a precision about ``2%``. The experiment shows that even room temperature can be important when estimating time, so all measurements had to be taken quickly and in the same conditions to eliminate the influence of other random processes on the computer's performance.

``avg_fps = sum_fps / N_fps``

## Perfomance
Running programms with different optimization flags and evaluating their perfomances gave me data for the following table. Measurements are taken in 3 runs of the programm and all three anerage values are also averaged.

|optimization \ flag|None    |-O0 |-O1 |-O2  |-O3      |-Ofast   |
|:------------------|:------:|:--:|:--:|:--: |:-------:|:-------:|
|no AVX inctructions|**2.5** |2.5 |4.5 |4.5  |4.6      |4.8      |
|                   |        |    |    |     |         |         |
|SSE inctructions   |**4.1** |4.1 |16.9|162.7|**163.1**|163.0    |
|abs speed increase |**1.6** |1.6 |3.8 |36.2 |35.5     |34.0     |
|                   |        |    |    |     |         |         |
|AVX2 inctructions  |**8.3** |8.3 |29.5|157.1|157.1    |156.9    |
|abs speed increase |**3.3** |3.3 |6.6 |34.9 |34.2     |32.7     |
|rel speed increase |**2.1** |2.1 |1.7 |~1.0 |~1.0     |~1.0     |
|                   |        |    |    |     |         |         |
|AVX512 inctructions|**14.2**|14.2|44.2|155.1|156.2    |155.2    |
|abs speed increase |**5.7** |5.7 |9.8 |34.5 |34.0     |32.3     |
|rel speed increase |**1.7** |1.7 |1.5 |~1.0 |~1.0     |~1.0     |

In the table programm speed is given in ``fps = 1 / sec``

To compare SIMD-instructions with each other and evaluate an impact of optimization flags I calculated 2 differernt сoefficients of speed increase:
- ``abs_speed_increase = instruction_fps / no_instruction_fps``, where time periods are taken from the perfomances with the same flags set
- ``rel_speed_increase = instruction_fps / prev_instruction_fps``, where time periods are also taken from the perfomances with the same flags set

The absolute coefficients are used to compare a perfomence of program with and without an AVX or SSE instruction. The relative coefficients are used to compare SSE, AVX2 and AVX512 instructions with each other.

The impact of the optimization flags can be seen directly from the resulting table.

To evaluate maximum speed up of the algorithm achieved in this work I use the following coefficient
``max_speed_increase = (max_fps / no_optimizations_fps)``

## Conclusions
Thus, optimizing Mandelbrot set drawing algorithm by using AVX2 instructions, a speedup of **27.6/4.6 = 6.0** times was achieved

## Colourful examples

![Example 2](Pictures/2.png)

![Example 3](Pictures/3.png)

![Example 4](Pictures/4.png)

![Example 5](Pictures/5.png)

![Example 6](Pictures/6.png)

![Example 7](Pictures/7.png)

![Example 8](Pictures/8.png)

![Example 9](Pictures/9.png)

![Example 10](Pictures/10.png)

![Example 11](Pictures/11.png)

![Example 12](Pictures/12.png)

![Example 13](Pictures/13.png)

... pay us to keep up to date with updates ...

For unsubscribed users only English version of the description available.

... pay us to keep up to date with updates ...
