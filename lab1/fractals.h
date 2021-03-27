#pragma once
#define FRACTALS_H

#ifndef DRAW_H
#include "draw.h"
#endif

#include <complex>

void newton_variation(Dot* Z1, Dot* Z2, double C);

draw_atom_t* generate_fractals_atoms(int n, Dot& start, double c, void fractal_func(Dot*, Dot*, double));