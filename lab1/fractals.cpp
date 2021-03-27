#include "fractals.h"

#define complex_mul_a(a1,a2,b1,b2) (((a1)*(a2) - (b1)*(b2)))
#define complex_mul_b(a1,a2,b1,b2) ((a1)*(b2)+(a2)*(b1))

void newton_variation(Dot* Z1, Dot* Z2, double C) {
	//Z=Z^5 + C
	*Z2 = { 1,1,0 };
	for (size_t i = 0; i < 5; i++){
		Z2->x *= complex_mul_a(Z1->x, Z1->x, Z1->y, Z1->y);
		Z2->y *= complex_mul_b(Z1->x, Z1->x, Z1->y, Z1->y) + C;
	}
	Z2->x + C, Z2->y + C;
}

draw_atom_t* generate_fractals_atoms(int n, Dot& start,double c,void fractal_func(Dot*,Dot*,double)) {
	draw_atom_t* atoms = new draw_atom_t[n+1];
	atoms[0].func = move_to;
	atoms[0].pos = start;
	for (size_t i = 1; i < n; i++){
		atoms[i].func = line_to;
		fractal_func(&atoms[i - 1].pos, &atoms[i].pos, c);
	}
	return atoms;
}