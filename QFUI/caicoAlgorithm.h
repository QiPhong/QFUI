#pragma once
#include <cairo.h>
#define M_PI 3.14159265358979323846 

#include <cmath>
#include <windows.h>
void draw_gradient_ring(cairo_t* cr, double centerX, double centerY, double radius, double thickness, unsigned int color1, unsigned int color2,bool* endFlag);
void draw_circle(cairo_t* cr, double centerX, double centerY, double radius);
void DrawCairoContentToHDC(HDC hdc, int dx, int dy, int dw, int dh, cairo_surface_t* surface, int sx, int sy);
