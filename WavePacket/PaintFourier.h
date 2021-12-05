#pragma once
#include "afxwin.h"
#include <vector>
#include <complex>

class PaintFourier : public CStatic
{
private:
	// Объект для инициализации GDI+
	ULONG_PTR token;

	double _xmin, _xmax,   // Минимальное и максимальное значение по х
		_ymin, _ymax,   // Минимальное и максимальное значение по у
		_step_x, _step_y;   // Шаг по х и у
	double _tau;
public:
	bool _enable;
	double _x_vert_line;
	std::vector<double> vec_fft_abs;
	/** Конструктор.*/
	PaintFourier();
	/** Деструктор.*/
	~PaintFourier();

	/** Функция обмена информацией между классами.*/
	void Exchange(double left, double right,
		double low, double up, 
		double step_setka_x, double step_setka_y, 
		double tau);

	/** Функция преобразования х к пиксельному формату.*/
	double Trans_X(LPDRAWITEMSTRUCT lpDrawItemStruct, double x);
	/** Функция преобразования у к пиксельному формату.*/
	double Trans_Y(LPDRAWITEMSTRUCT lpDrawItemStruct, double y);
	/** Функция рисования.*/
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};