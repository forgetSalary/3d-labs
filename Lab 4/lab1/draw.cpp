#include "draw.h"
#include "cmath"
#define M_PI 3.14159265358979323846

Figure::Figure() {
	this->atoms = NULL;
	this->atoms_count = 0;
	this->hdc = NULL;
}

Figure::Figure(HDC hdc, draw_atom_t* atoms, int atoms_count) {
	this->atoms = atoms;
	this->atoms_count = atoms_count;
	this->hdc = hdc;
}

void Figure::draw() {
	for (size_t i = 0; i < atoms_count; i++) {
		if (atoms[i].func == move_to) {
			MoveToEx(hdc, atoms[i].pos.x, atoms[i].pos.y, NULL);
		}
		else if (atoms[i].func == line_to) {
			LineTo(hdc, atoms[i].pos.x, atoms[i].pos.y);
		}

	}
}

void Figure::transform(Dot* matrix) {
#define to_arr(S) ((double*)(&(S)))
	double elem_c = 0;

	draw_atom_t* result = (draw_atom_t*)_malloca(sizeof(draw_atom_t) * atoms_count);

	int i, j, r;
	for (i = 0; i < atoms_count; i++) {
		for (j = 0; j < 3; j++) {
			elem_c = 0;
			for (r = 0; r < 3; r++) {
				elem_c = elem_c + to_arr(atoms[i].pos)[r] * to_arr(matrix[r])[j];
				to_arr(result[i].pos)[j] = elem_c;
			}
		}
	}

	for (size_t i = 0; i < atoms_count; i++) {
		atoms[i].pos = result[i].pos;
	}

	_freea(result);
}


double Figure::min_x() {
	double num = atoms[0].pos.x;
	for (size_t i = 1; i < atoms_count; i++) {
		if ((atoms[i].pos.x) < num) {
			num = atoms[i].pos.x;
		}
	}
	return num;
}
double Figure::min_y() {
	double num = atoms[0].pos.y;
	for (size_t i = 1; i < atoms_count; i++) {
		if ((atoms[i].pos.y) < num) {
			num = atoms[i].pos.y;
		}
	}
	return num;
}
double Figure::max_x() {
	double num = atoms[0].pos.x;
	for (size_t i = 1; i < atoms_count; i++) {
		if ((atoms[i].pos.x) > num) {
			num = atoms[i].pos.x;
		}
	}
	return num;
}
double Figure::max_y() {
	double num = atoms[0].pos.y;
	for (size_t i = 1; i < atoms_count; i++) {
		if ((atoms[i].pos.y) > num) {
			num = atoms[i].pos.y;
		}
	}
	return num;
}


void Figure::move(double delta_x, double delta_y) {
	Dot trans_matrix[3] = { {1,0,0},{0,1,0},{delta_x,delta_y,1} };
	transform(trans_matrix);
}

void Figure::miror_x() {
	double min = this->min_x();
	double max = this->max_x();

	int delta = 2 * min + (max - min);
	Dot trans_matrix[3] = { {-1,0,0},{0,1,0},{delta,0,1} };
	transform(trans_matrix);
}

void Figure::miror_y() {
	double min = this->min_y();
	double max = this->max_y();

	double delta = 2 * min + (max - min);
	Dot trans_matrix[3] = { {1,0,0},{0,-1,0},{0,delta,1} };
	transform(trans_matrix);
}

void Figure::rotate(int deg) {
	double rad = deg * (M_PI / 180);

	double x1, x2, y1, y2;
	x1 = cos(rad);
	y1 = sin(rad);
	x2 = -y1;
	y2 = x1;

	Dot trans_matrix[3] = { {x1,y1,0},{x2,y2,0},{0,0,1} };
	transform(trans_matrix);
}

void Figure::resize(double delta_x, double delta_y) {
	double _max_x = max_x();
	double _max_y = max_y();
	Dot trans_matrix[3] = { {delta_x,0,0},{0,delta_y,0},{0,0,1} };
	transform(trans_matrix);
}