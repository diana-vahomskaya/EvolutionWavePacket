
// WavePacketDlg.h : файл заголовка
//

#pragma once
#include "PaintDynWavePacket.h"
#include <vector>
#include <complex>
#include "afxwin.h"
#include "WavePacketEvolver.h"
#include "PaintFourier.h"
#include "PaintWaveFunc.h"
#include "afxcmn.h"

using namespace std;

// диалоговое окно CWavePacketDlg
class CWavePacketDlg : public CDialogEx
{
// Создание
public:
	CWavePacketDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAVEPACKET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	double _xmin_wv_pckt, _xmax_wv_pckt,   // Минимальное и максимальное значение по х (волновой пакет)
		_ymin_wv_pckt, _ymax_wv_pckt,   // Минимальное и максимальное значение по у (волновой пакет)
		_step_x_wv_pckt, _step_y_wv_pckt;   // Шаг по х и у (волновой пакет)
	double _xmin_four, _xmax_four,   // Минимальное и максимальное значение по х (БПФ)
		_ymin_four, _ymax_four,   // Минимальное и максимальное значение по у (БПФ)
		_step_x_four, _step_y_four;   // Шаг по х и у
	double _xmin_wv_func, _xmax_wv_func,   // Минимальное и максимальное значение по х (волновая функция)
		_ymin_wv_func, _ymax_wv_func,   // Минимальное и максимальное значение по у (волновая функция)
		_step_x_wv_func, _step_y_wv_func;   // Шаг по х и у
	double _Uo;   // Глубина ямы
	double _a;   // Ширина ямы
	double _b;   // Доп. ширина ямы
	double _mu;   // Среднее значение
	double _sigma;   // Дисперсия
	double _R;   // Граница потенциала и области определения волновой функции
	double _step_r;   // Пространственный шаг в промежутке [-R;R]
	double _tau;   // Временной шаг
	int _N_Time;   // Количество временных отсчетов
	int _n_FFT;
	bool FFT_enable;
	int iter_fft;
	double K;
	double Beckon;
public:
	double stepFFT;
	int _N;   // Количество шагов по пространству
	/** Объект класса отрисовки PaintDynWavePacket.*/
	PaintDynWavePacket PntDnWvPckt;
	PaintFourier PntFFT;
	PaintWaveFunc PntWvFunc;
	WavePacketEvolver evolver;

	vector<vector<complex<double>>> vec_fft;
	vector<vector<double>> vec_fft_abs;
	vector<vector<complex<double>>> vec_wave_func;
	vector<double> vec_points_x_FFT;

	/** Привести значения вектора к масштабу отросовки.*/
	void Scale(double &min, double &max, std::vector<double> &vec);
	void ScaleCmplx(double &min, double &max, std::vector<std::complex<double>> vec);

	/** Произвести БПФ.
	* @param *data - вектор над которым произвести БПФ.
	* @param n - количество отсчётов в векторе.
	* @param is - если is = -1 - прямое, если is = 1 - обратное.
	*/
	void fourier(vector<complex<double>> & data, int n, int is);
	
	/** Обработчик нажатия кнопки для запуска эволюции волнового пакета.*/
	afx_msg void OnBnClickedButton1();
	/** Обработчик кнопки для остановки эволюции волнового пакета.*/
	afx_msg void OnBnClickedButton2();
	/** Обработчик кнопки для вычисления FFT.*/
	afx_msg void OnBnClickedButton4();

	/** Обработчик таймера.*/
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	/** ИД таймера.*/
	ULONG_PTR TimerID;
	
	/** Объект ProgressBar для FFT.*/
	CProgressCtrl ProgressValueFFT;
	/** Объект Slider для волнового пакета.*/
	CSliderCtrl sldrPacket;
	/** Объект Slider для FFT..*/
	CSliderCtrl sldrFFT;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	/** Определение статуса подсчета FFT.*/
	CString cstrStatus;
	CStatic grpbxWavePacket;   // GroupBox for Wave packet
	CStatic grpbxFFT;   // GroupBox for FFT
	CStatic grpbxFunc;   // GroupBox for Wave function
	CStatic grbpxParams;   // GroupBox for Pit parameters
	CStatic grpbxUo;   // GroupBox for Uo
	CStatic grpbxR;   // GroupBox for R
	CStatic grpbxCond;   // GroupBox for add. cond.
	CStatic grpbxSamp;   // GroupBox for sampling
};
