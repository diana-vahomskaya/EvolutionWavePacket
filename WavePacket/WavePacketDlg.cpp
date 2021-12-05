
// WavePacketDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "WavePacket.h"
#include "WavePacketDlg.h"
#include "afxdialogex.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _USE_MATH_DEFINES
#include "math.h"


// диалоговое окно CWavePacketDlg


CWavePacketDlg::CWavePacketDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WAVEPACKET_DIALOG, pParent)
	, _Uo(30)
	, _R(3)
	, _a(2.1)
	, _b(2.5)
	, _mu(0)
	, _sigma(0.15)
	, _tau(0.05)
	, TimerID(0)
	, _N_Time(5000)
	, _n_FFT(256)
	, FFT_enable(false)
	, cstrStatus(_T("Status"))
	, K(0.5)
	, Beckon(50)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWavePacketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_1, PntDnWvPckt);
	DDX_Text(pDX, IDC_EDIT1, _R);
	DDX_Text(pDX, IDC_EDIT2, _a);
	DDX_Text(pDX, IDC_EDIT3, _b);
	DDX_Control(pDX, IDC_STATIC_2, PntFFT);
	DDX_Control(pDX, IDC_STATIC_3, PntWvFunc);
	DDX_Text(pDX, IDC_EDIT6, _Uo);
	DDX_Control(pDX, IDC_PROGRESS1, ProgressValueFFT);
	DDX_Control(pDX, IDC_SLIDER1, sldrPacket);
	DDX_Control(pDX, IDC_SLIDER2, sldrFFT);
	DDX_Text(pDX, IDC_STATUS, cstrStatus);
	DDX_Text(pDX, IDC_EDIT4, _tau);
	DDX_Text(pDX, IDC_EDIT5, _n_FFT);
	DDX_Control(pDX, IDC_STATIC_wp, grpbxWavePacket);
	DDX_Control(pDX, IDC_STATIC_fft, grpbxFFT);
	DDX_Control(pDX, IDC_STATIC_wf, grpbxFunc);
	DDX_Control(pDX, IDC_STATIC_params, grbpxParams);
	DDX_Control(pDX, IDC_STATIC_Uo, grpbxUo);
	DDX_Control(pDX, IDC_STATIC_R, grpbxR);
	DDX_Control(pDX, IDC_STATIC_add_con, grpbxCond);
	DDX_Control(pDX, IDC_STATIC_samp, grpbxSamp);
	DDX_Text(pDX, IDC_EDIT7, K);
	DDX_Text(pDX, IDC_EDIT8, Beckon);
}

BEGIN_MESSAGE_MAP(CWavePacketDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CWavePacketDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &CWavePacketDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CWavePacketDlg::OnBnClickedButton4)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// обработчики сообщений CWavePacketDlg

BOOL CWavePacketDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	/** Задание мнимой единицы.*/
	//I = std::complex<double>(0, 1);

	//CFont font;
	//font.CreateFontW(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));

	CFont font;
	font.CreateFontW(13, 0, 0, 0, FW_NORMAL, 1, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	grpbxWavePacket.SetFont(&font);
	grpbxFFT.SetFont(&font);
	grpbxFunc.SetFont(&font);
	grbpxParams.SetFont(&font);

	CFont font2;
	font2.CreateFontW(10, 0, 0, 0, FW_NORMAL, 1, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	grpbxCond.SetFont(&font2);
	grpbxR.SetFont(&font2);
	grpbxSamp.SetFont(&font2);
	grpbxUo.SetFont(&font2);

	stepFFT = 1 / _tau / _n_FFT;
	_N = 500;
	_step_r = (2 * _R) / _N;
	ProgressValueFFT.SetRange(0, 100);
	ProgressValueFFT.SetPos(0);

	sldrPacket.SetRange(1, _N - 2);
	sldrPacket.SetPos(_N / 2);
	sldrPacket.SetTicFreq(1);

	_xmin_wv_pckt = -_R; _xmax_wv_pckt = _R;
	_ymin_wv_pckt = -0.5; _ymax_wv_pckt = 2;
	PntDnWvPckt.Exchange(_xmin_wv_pckt, _xmax_wv_pckt, _ymin_wv_pckt, _ymax_wv_pckt, _xmax_wv_pckt / 5, _ymax_wv_pckt / 4);

	sldrFFT.SetRange(0, _n_FFT - 1);
	sldrFFT.SetPos(1);
	sldrFFT.SetTicFreq(1);

	_xmin_four = -0.1; _xmax_four = 1;
	_ymin_four = -1; _ymax_four = 10;
	PntFFT.Exchange(_xmin_four, _xmax_four, _ymin_four, _ymax_four, _xmax_four / 5, _ymax_four / 4, _tau);

	_xmin_wv_func = -_R; _xmax_wv_func = _R;
	_ymin_wv_func = -10; _ymax_wv_func = 10;
	PntWvFunc.Exchange(_xmin_wv_func, _xmax_wv_func, _ymin_wv_func, _ymax_wv_func, _xmax_wv_func / 5, _ymax_wv_func / 4);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CWavePacketDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CWavePacketDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/** Привести значения вектора к масштабу отросовки.*/
void CWavePacketDlg::Scale(double &min, double &max, std::vector<double> &vec)
{
	min = 0;
	max = 0;
	size_t size = vec.size();
	for (size_t i = 0; i < size; i++)
	{
		if (max < vec[i])
			max = vec[i];
		if (min > vec[i])
			min = vec[i];
	}
}

/** Привести значения вектора к масштабу отросовки.*/
void CWavePacketDlg::ScaleCmplx(double &min, double &max, std::vector<std::complex<double>> vec)
{
	min = 0;
	max = 0;
	size_t size = vec.size();
	for (size_t i = 0; i < size; i++)
	{
		if (max < vec[i].real())
			max = vec[i].real();
		if (min > vec[i].real())
			min = vec[i].real();
	}
}

/** Поток, вычиляющий эволюцию волнового пакета.*/
//int thread_proc(WavePacketEvolver & ev)
//{
//	ev.run();
//
//	return 0;
//}

/** Обработчик нажатия кнопки для запуска эволюции волнового пакета.*/
void CWavePacketDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	
	sldrFFT.SetRange(0, _n_FFT - 1);
	sldrFFT.SetPos(1);
	sldrFFT.SetTicFreq(1);
	
	ProgressValueFFT.SetPos(0);
	
	vec_fft.clear();
	vec_fft.resize(_N);

	vec_fft_abs.clear();
	vec_fft_abs.resize(_N);

	PntFFT.vec_fft_abs.clear();
	_xmin_four = -0.1; _xmax_four = 1;
	_ymin_four = -1; _ymax_four = 10;
	PntFFT._enable = false;
	PntFFT.Exchange(_xmin_four, _xmax_four, _ymin_four, _ymax_four, _xmax_four / 5, _ymax_four / 4, _tau);

	PntWvFunc.vec_wave_func.clear();
	_xmin_wv_func = -_R; _xmax_wv_func = _R;
	_ymin_wv_func = -10; _ymax_wv_func = 10;
	PntWvFunc.Exchange(_xmin_wv_func, _xmax_wv_func, _ymin_wv_func, _ymax_wv_func, _xmax_wv_func / 5, _ymax_wv_func / 4);

	PntDnWvPckt.vec_psi_abs.resize(_N);
	PntDnWvPckt.vec_Uo.resize(_N);

	evolver.SetParams(2.0 * _R, _N, _tau, _Uo, _a, _b, 1, K, Beckon);
	evolver.set_init_state(_mu, _sigma);

	for (size_t i = 0; i < evolver.vec_wave_packet_prev.size(); i++)
	{
		PntDnWvPckt.vec_Uo[i] = evolver.vec_U[i];
		PntDnWvPckt.vec_psi_abs[i] = abs(evolver.vec_wave_packet_prev[i]);
	}

	stepFFT = 1 / _tau / _n_FFT;
	vec_points_x_FFT.clear();
	for (int i = 0; i < _n_FFT; i++)
	{
		vec_points_x_FFT.push_back(i * stepFFT);
	}

	double min, max;
	Scale(min, max, PntDnWvPckt.vec_psi_abs);
	_xmin_wv_pckt = -_R; _xmax_wv_pckt = _R;
	_ymin_wv_pckt = -max / 5; _ymax_wv_pckt = 1.2 * max;
	PntDnWvPckt._enable = false;
	PntDnWvPckt.Exchange(_xmin_wv_pckt, _xmax_wv_pckt, _ymin_wv_pckt, _ymax_wv_pckt, _xmax_wv_pckt / 4, _ymax_wv_pckt / 4);
	UpdateData(FALSE);
	//thread * thr = new thread(thread_proc, ref(evolver));
	//Sleep(100);
	TimerID = SetTimer(1, 10, NULL);
}

/** Обработчик таймера.*/
void CWavePacketDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (FFT_enable)
	{
		evolver.step();
		for (int i = 0; i < _N; i++)
		{
			PntDnWvPckt.vec_psi_abs[i] = abs(evolver.vec_wave_packet[i]);
			vec_fft[i].push_back(evolver.vec_wave_packet[i]);
		}

		PntDnWvPckt._enable = true;
		int pos = sldrPacket.GetPos();
		PntDnWvPckt._x_vert_line = evolver.vec_points_x_WavePacket[pos];
		PntDnWvPckt._enable = true;
		double min, max;
		Scale(min, max, PntDnWvPckt.vec_psi_abs);
		_ymin_wv_pckt = -max / 5; _ymax_wv_pckt = max;
		PntDnWvPckt.Exchange(_xmin_wv_pckt, _xmax_wv_pckt, _ymin_wv_pckt, _ymax_wv_pckt, _xmax_wv_pckt / 4, _ymax_wv_pckt / 4);

		ProgressValueFFT.SetPos((double)iter_fft / (_n_FFT - 1) * 100);
		UpdateData(FALSE);
		
		if (iter_fft == (_n_FFT - 1))
		{
			vec_fft_abs.clear();
			vec_fft_abs.resize(_N);
			for (int i = 0; i < _N; i++)
			{
				fourier(vec_fft[i], _n_FFT, -1);
				for (int j = 0; j < _n_FFT; j++)
				{
					vec_fft_abs[i].push_back(abs(vec_fft[i][j]));
				}
			}

			int posfft = sldrFFT.GetPos();
			PntFFT._x_vert_line = vec_points_x_FFT[posfft];
			PntFFT._enable = true;

			int it = sldrPacket.GetPos();
			PntFFT.vec_fft_abs.resize(_n_FFT);
			PntFFT.vec_fft_abs.assign(vec_fft_abs[it].begin(), vec_fft_abs[it].end());
			Scale(min, max, PntFFT.vec_fft_abs);
			_xmin_four = -0.1; _xmax_four = 1 / _tau;
			_ymin_four = -max / 5; _ymax_four = max;
			PntFFT.Exchange(_xmin_four, _xmax_four, _ymin_four, _ymax_four, _xmax_four / 4, _ymax_four / 4, _tau);

			int itrt = sldrFFT.GetPos();
			PntWvFunc.vec_wave_func.resize(_N);
			for (int it1 = 0; it1 < _N; it1++)
			{
				for (int it2 = 0; it2 < _n_FFT; it2++)
				{
					if (it2 == itrt)
					{
						PntWvFunc.vec_wave_func[it1] = vec_fft[it1][itrt].real();
						break;
					}
				}
			}

			Scale(min, max, PntWvFunc.vec_wave_func);
			_xmin_wv_func = -_R; _xmax_wv_func = _R;
			_ymin_wv_func = min; _ymax_wv_func = max;
			PntWvFunc.Exchange(_xmin_wv_func, _xmax_wv_func, _ymin_wv_func, _ymax_wv_func, _xmax_wv_func / 4, _ymax_wv_func / 4);

			FFT_enable = false;
			cstrStatus.Format(_T("%s"), L"Completed!");
			//KillTimer(TimerID);
			//OnBnClickedButton2();
		}
		//evolver.iter = evolver.iter + 1;
		iter_fft++;
	}
	else
	{
		evolver.step();
		for (size_t i = 0; i < evolver.vec_wave_packet.size(); i++)
		{
			PntDnWvPckt.vec_psi_abs[i] = abs(evolver.vec_wave_packet[i]);
		}

		int pos_x2 = sldrFFT.GetPos();
		PntFFT._x_vert_line = vec_points_x_FFT[pos_x2];
		double min, max;
		Scale(min, max, PntDnWvPckt.vec_psi_abs);
		_ymin_wv_pckt = -max / 5; _ymax_wv_pckt = max;
		int pos_x = sldrPacket.GetPos();
		PntDnWvPckt._x_vert_line = evolver.vec_points_x_WavePacket[pos_x];
		PntDnWvPckt.Exchange(_xmin_wv_pckt, _xmax_wv_pckt, _ymin_wv_pckt, _ymax_wv_pckt, _xmax_wv_pckt / 4, _ymax_wv_pckt / 4);

		UpdateData(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}

/** Обработчик кнопки для остановки эволюции волнового пакета.*/
void CWavePacketDlg::OnBnClickedButton2()
{
	//evolver.stop();
	KillTimer(TimerID);
}

/** Обработчик кнопки для вычисления FFT.*/
void CWavePacketDlg::OnBnClickedButton4()
{
	FFT_enable = true;
	cstrStatus.Format(_T("%s"), L"Calculation of FFT...");
	vec_fft.clear();
	vec_fft.resize(_N);

	iter_fft = 0;
	//evolver.iter = 0;
	//evolver.N_FFT = _n_FFT - 1;
}

void CWavePacketDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == (CScrollBar*)&sldrPacket)
	{
		int it = sldrPacket.GetPos();
		PntDnWvPckt._x_vert_line = evolver.vec_points_x_WavePacket[it];
		PntDnWvPckt.Exchange(_xmin_wv_pckt, _xmax_wv_pckt, _ymin_wv_pckt, _ymax_wv_pckt, _xmax_wv_pckt / 4, _ymax_wv_pckt / 4);
		PntFFT.vec_fft_abs.assign(vec_fft_abs[it].begin(), vec_fft_abs[it].end());
		double min, max;
		Scale(min, max, PntFFT.vec_fft_abs);
		_ymin_four = -max / 5; _ymax_four = max;
		PntFFT.Exchange(_xmin_four, _xmax_four, _ymin_four, _ymax_four, _xmax_four / 4, _ymax_four / 4, _tau);
	}

	if (pScrollBar == (CScrollBar*)&sldrFFT)
	{
		int it = sldrFFT.GetPos();
		PntFFT._x_vert_line = vec_points_x_FFT[it];
		PntFFT.Exchange(_xmin_four, _xmax_four, _ymin_four, _ymax_four, _xmax_four / 4, _ymax_four / 4, _tau);
		for (int it1 = 0; it1 < _N; it1++)
		{
			for (int it2 = 0; it2 < _n_FFT; it2++)
			{
				if (it2 == it)
				{
					PntWvFunc.vec_wave_func[it1] = vec_fft[it1][it].real();
					break;
				}
			}
		}
		double min, max;
		Scale(min, max, PntWvFunc.vec_wave_func);
		_xmin_wv_func = -_R; _xmax_wv_func = _R;
		_ymin_wv_func = min; _ymax_wv_func = max;
		PntWvFunc.Exchange(_xmin_wv_func, _xmax_wv_func, _ymin_wv_func, _ymax_wv_func, _xmax_wv_func / 4, _ymax_wv_func / 4);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

/** Произвести БПФ.
* @param *data - вектор над которым произвести БПФ.
* @param n - количество отсчётов в векторе.
* @param is - если is = -1 - прямое, если is = 1 - обратное.
*/
void CWavePacketDlg::fourier(vector<complex<double>> & data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	double r,
		r1,
		theta,
		w_r,
		w_i,
		temp_r,
		temp_i;
	double pi = 3.1415926f;

	r = pi*is;   // прямое или отбратное
	j = 0;
	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = data[j].real();
			temp_i = data[j].imag();
			data[j].real(data[i].real());
			data[j].imag(data[i].imag());
			data[i].real(temp_r);
			data[i].imag(temp_i);
		}
		m = n >> 1;
		while (j >= m)
		{
			j -= m; m = (m + 1) / 2;
		}
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (double)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1*m;
			w_r = (double)cos((double)theta);
			w_i = (double)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r*data[j].real() - w_i*data[j].imag();
				temp_i = w_r*data[j].imag() + w_i*data[j].real();
				data[j].real(data[i].real() - temp_r);
				data[j].imag(data[i].imag() - temp_i);
				data[i].real(data[i].real() + temp_r);
				data[i].imag(data[i].imag() + temp_i);
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; i++)
		{
			data[i].real(data[i].real() / (double)n);
			data[i].imag(data[i].imag() / (double)n);
		}

}