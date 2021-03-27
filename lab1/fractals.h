#pragma once
#define FRACTALS_H

#ifndef DRAW_H
#include "draw.h"
#endif

#include "thread"
#include "atomic"
#include "mutex"

#include <complex>

void draw_fractal(
    HDC hdc,
    int src_x,
    int src_y,
    int iterations,
    double zoom
);