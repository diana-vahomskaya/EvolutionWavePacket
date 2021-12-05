#pragma once
#include "afxwin.h"
#include <vector>
#include <complex>

class PaintFourier : public CStatic
{
private:
	// ������ ��� ������������� GDI+
	ULONG_PTR token;

	double _xmin, _xmax,   // ����������� � ������������ �������� �� �
		_ymin, _ymax,   // ����������� � ������������ �������� �� �
		_step_x, _step_y;   // ��� �� � � �
	double _tau;
public:
	bool _enable;
	double _x_vert_line;
	std::vector<double> vec_fft_abs;
	/** �����������.*/
	PaintFourier();
	/** ����������.*/
	~PaintFourier();

	/** ������� ������ ����������� ����� ��������.*/
	void Exchange(double left, double right,
		double low, double up, 
		double step_setka_x, double step_setka_y, 
		double tau);

	/** ������� �������������� � � ����������� �������.*/
	double Trans_X(LPDRAWITEMSTRUCT lpDrawItemStruct, double x);
	/** ������� �������������� � � ����������� �������.*/
	double Trans_Y(LPDRAWITEMSTRUCT lpDrawItemStruct, double y);
	/** ������� ���������.*/
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};