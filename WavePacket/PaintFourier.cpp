#include "stdafx.h"
#include "PaintFourier.h"
#include "Gdiplus.h"

using namespace Gdiplus;

/** Инициализация GDI+.*/
PaintFourier::PaintFourier()
{
	GdiplusStartupInput input;
	Status s;
	s = GdiplusStartup(&token, &input, NULL);
	if (s != Ok) MessageBox(L"Don't Open", L"Warning", MB_ICONERROR);
}

/** Деинициализация GDI+.*/
PaintFourier::~PaintFourier()
{
	GdiplusShutdown(token);
}

/** Функция обмена информацией между классами.*/
void PaintFourier::Exchange(double left, double right, 
	double low, double up,
	double step_setka_x, double step_setka_y, 
	double tau)
{
	_xmin = left;
	_xmax = right;
	_ymin = low;
	_ymax = up;
	_step_x = step_setka_x;
	_step_y = step_setka_y;
	_tau = tau;
	Invalidate();
}

/** Преобразование х к пиксельному формату.*/
double PaintFourier::Trans_X(LPDRAWITEMSTRUCT lpDrawItemStruct, double x)
{
	return (double)(lpDrawItemStruct->rcItem.right) / (double)(_xmax - _xmin)*((x)-_xmin);
}

/** Преобразование у к пиксельному формату.*/
double PaintFourier::Trans_Y(LPDRAWITEMSTRUCT lpDrawItemStruct, double y)
{
	return -(double)(lpDrawItemStruct->rcItem.bottom) / (double)(_ymax - _ymin)*((y)-_ymax);
}

/** Функция рисования.*/
void PaintFourier::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics paint(lpDrawItemStruct->hDC);
	Bitmap bmp(lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &paint);
	Graphics paint_virtual(&bmp);

	FontFamily font_Osi(L"Times New Roman");
	Gdiplus::Font font(&font_Osi, 10, FontStyleRegular, UnitPixel);

	SolidBrush brush(Color::White);
	SolidBrush brush_dot(Color::Brown);
	Pen Pen_Gr(Color::Yellow, (Gdiplus::REAL)2);
	Pen Pen_Gr2(Color::Red, (Gdiplus::REAL)3);
	Pen Pen_Osi(Color::White, (Gdiplus::REAL)0.1);
	Pen Pen_Setka(Color::WhiteSmoke, (Gdiplus::REAL)0.1);
	Pen_Setka.SetDashStyle(DashStyle::DashStyleDash);

	paint_virtual.SetSmoothingMode(SmoothingModeAntiAlias);
	paint_virtual.Clear(Color::DarkBlue);

	// Главные оси
	paint_virtual.DrawLine(&Pen_Osi,
		(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmin), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, 0),
		(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmax), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, 0));
	paint_virtual.DrawLine(&Pen_Osi,
		(Gdiplus::REAL)Trans_X(lpDrawItemStruct, 0), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin),
		(Gdiplus::REAL)Trans_X(lpDrawItemStruct, 0), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymax));

	// Сетка
	// Вертикальные линии
	for (double x = 0; x <= _xmax; x += _xmax / 4)
	{
		paint_virtual.DrawLine(&Pen_Setka,
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin),
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymax));
	}

	for (double x = 0; x >= _xmin; x -= _xmax / 4)
	{
		paint_virtual.DrawLine(&Pen_Setka,
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin),
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymax));
	}

	// Горизонтальные линии
	for (double y = 0; y <= _ymax; y += _ymax / 4)
	{
		paint_virtual.DrawLine(&Pen_Setka,
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmin), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y),
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmax), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y));
	}
	for (double y = 0; y >= _ymin; y -= _ymax / 4)
	{
		paint_virtual.DrawLine(&Pen_Setka,
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmin), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y),
			(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _xmax), (Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y));
	}

	CString str;

	// Подпись точек по горизонтали
	for (double x = 0; x <= _xmax; x += _xmax / 4)
	{
		if (x != 0)
		{
			str.Format(_T("%.1f"), x);
			paint_virtual.DrawString(str, -1, &font, PointF((Gdiplus::REAL)Trans_X(lpDrawItemStruct, x),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin / 40)), NULL, &brush);
		}
	}
	for (double x = 0; x >= _xmin; x -= _xmax / 4)
	{
		if (x != 0)
		{
			str.Format(_T("%.1f"), x);
			paint_virtual.DrawString(str, -1, &font, PointF((Gdiplus::REAL)Trans_X(lpDrawItemStruct, x),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin / 40)), NULL, &brush);
		}
	}

	// Подпись точек по вертикали
	for (double y = 0; y <= _ymax; y += _ymax / 4)
	{
		if (y != 0)
		{
			str.Format(_T("%.1f"), y);
			paint_virtual.DrawString(str, -1, &font, PointF((Gdiplus::REAL)Trans_X(lpDrawItemStruct, (_xmax / 100)),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y - y / 100)), NULL, &brush);
		}
		else
		{
			str.Format(_T("%.0f"), y);
			paint_virtual.DrawString(str, -1, &font, PointF((Gdiplus::REAL)Trans_X(lpDrawItemStruct, (_xmax / 100)),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y - y / 100)), NULL, &brush);
		}
	}
	for (double y = 0; y >= _ymin; y -= _ymax / 4)
	{
		if (y != 0)
		{
			str.Format(_T("%.1f"), y);
			paint_virtual.DrawString(str, -1, &font, PointF((Gdiplus::REAL)Trans_X(lpDrawItemStruct, (_xmax / 100)),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, y - y / 100)), NULL, &brush);
		}
	}

	if (!vec_fft_abs.empty())
	{
		double step = 1 / _tau / vec_fft_abs.size();
		double x1 = 0;
		double x2 = x1 + step;
		for (size_t i = 0; i < vec_fft_abs.size() - 1; i++)
		{
			paint_virtual.DrawLine(&Pen_Gr,
				(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x1),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, vec_fft_abs[i]),
				(Gdiplus::REAL)Trans_X(lpDrawItemStruct, x2),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, vec_fft_abs[i + 1]));
			x1 = x2;
			x2 += step;
		}
		if (_enable)
		{
			paint_virtual.DrawLine(&Pen_Gr2,
				(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _x_vert_line),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymin),
				(Gdiplus::REAL)Trans_X(lpDrawItemStruct, _x_vert_line),
				(Gdiplus::REAL)Trans_Y(lpDrawItemStruct, _ymax));
		}
	}

	paint.DrawImage(&bmp, 0, 0);
}
