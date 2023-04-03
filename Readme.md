# Mandelbrot fractal

![Example 1](Pictures/1.png)

## Introduction

t ~ width * height = 560 * 500

## Perfomance

|optimisation \ flag|None   |-O0 |-O1 |-O2 |-O3     |-Ofast|
|:------------------|:-----:|:--:|:--:|:--:|:------:|:----:|
|no AVX inctructions|**3.1**|3.2 |5.6 |5.6 |5.7     |5.8   |
|AVX2 inctructions  |       |    |    |    |**31.2**|30.7  |

So coefficient of optimisation is ``k = t_max / t_min = 31.2 / 3.1 = 10.1``

## Colourful examples

![Example 2](Pictures/2.png)

![Example 3](Pictures/3.png)

![Example 4](Pictures/4.png)

![Example 5](Pictures/5.png)

![Example 6](Pictures/6.png)

![Example 7](Pictures/7.png)

![Example 8](Pictures/8.png)
