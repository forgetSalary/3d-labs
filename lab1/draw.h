#pragma once

#define DRAW_H

#ifndef FRAMEWORK_H
#include "framework.h"
#endif

typedef enum draw_func :uint8_t {
	move_to = 0,
	line_to
}draw_func_t;

struct Dot{
	double x;
	double y;
	double z;
};

typedef struct draw_atom_s{
	Dot pos;
	draw_func_t func;
}draw_atom_t;

class Figure {
private:
	draw_atom_t* atoms;
	HDC hdc;
	int atoms_count;
public:
	Figure();
	Figure(HDC,draw_atom_t*,int);
	~Figure() {};
private:
	void transform(Dot*);
	double min_x();
	double min_y();
	double max_x();
	double max_y();
public:
	void draw();
	void move(double delta_x, double delta_y);
	void miror_x();
	void miror_y();
	void rotate(int deg);
	void resize(double delta_x, double delta_y);
};