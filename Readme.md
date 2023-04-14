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


As we see, perfomance of algorithm depends on amount of pixels in resulting picture

``t ~ width * height``

Number of operating pixels in algorithm is ``640*560 = 358400``. It actually takes time to process all of them, so algorithm optimization required.

## Optimization principles
Ideas of optimization that use AVX2 and AVX512 instructions are the same.

We are able to process 8 pixels at the same time by using __m256i variables and AVX2 instructions or 16 pixels by using __m512i variables and AVX512 instructions. Here is mechanism of optimization described step-by-step.

... this section is in progress ...
... pay us to keep up to date with updates ...

## Time measuring principles
To evaluate and compare the speed of working algorithms I calculate fps value of 10 cycles of Alpha-Blending.

``fps = 1 / blending_time``

It's important to evaluate the time correctly, so when I do it, app don't draw anything in SFML, just operates with arrays of pixels

Evaluations made with precision ~ ``0.3 sec^(-1)``

## Perfomance
Running programms with different optimization flags and evaluating their perfomances gave me data for the following table:

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

In the table programm speed is given in ``fps = 1/sec``.

To compare SIMD-instructions with each other and evaluate an impact of optimization flags I calculated 2 differernt сoefficients of speed increase:
- ``abs_speed_increase = instruction_fps / no_instruction_fps``, where time periods are taken from the perfomances with the same flags set
- ``rel_speed_increase = instruction_fps / prev_instruction_fps``, where time periods are also taken from the perfomances with the same flags set

The absolute coefficient are used to compare a perfomence of program with and without an AVX or SSE instruction. The relative coefficient are used to compare SSE, AVX2 and AVX512 instructions with each other.

The impact of the optimisation flags can be seen directly from the resulting table.

To evaluate maximum speed up of the algorithm achieved in this work I use the following coefficient:
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
