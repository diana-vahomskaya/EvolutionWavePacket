#pragma once
#include "afxwin.h"
#include <vector>
#include <complex>

class PaintDynWavePacket : public CStatic
{
private:
	// Объект для инициализации GDI+
	ULONG_PTR token;

	double _xmin, _xmax,   // Минимальное и максимальное значение по х
		_ymin, _ymax,   // Минимальное и максимальное значение по у
		_step_x, _step_y;   // Шаг по х и у
public:
	double _x_vert_line;
	bool _enable;
	std::vector<double> vec_psi_abs;   // Вектор функции psi(x)
	std::vector<std::complex<double>> vec_Uo;

	/** Конструктор.*/
	PaintDynWavePacket();
	/** Деструктор.*/
	~PaintDynWavePacket();

	/** Функция обмена информацией между классами.*/
	void Exchange(double left, double right, double low, double up, double step_setka_x, double step_setka_y);

	/** Функция преобразования х к пиксельному формату.*/
	double Trans_X(LPDRAWITEMSTRUCT lpDrawItemStruct, double x);
	/** Функция преобразования у к пиксельному формату.*/
	double Trans_Y(LPDRAWITEMSTRUCT lpDrawItemStruct, double y);
	/** Функция рисования.*/
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};