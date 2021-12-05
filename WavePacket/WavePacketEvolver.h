#pragma once

#include <vector>
#include <complex>

using namespace std;


class WavePacketEvolver
{
public:
	// Размер области моделирования.
	double model_size;
	// Глубина ямы
	double _Uo;  
	// Ширина ямы
	double a;  
	double _K;
	double gamma;
	double _B;

	/* Ширина области моделирования без учета поглощающих слоев.*/
	double b;
	// Временной шаг
	double tau;  
	/** Пространственный шаг.*/
	double step_r;
	int _N_Time;
	int _t;
	bool stop_flag;

	/** Мнимая единица.*/
	complex<double> image_j;

	vector<complex<double>> vec_wave_packet;
	vector<complex<double>> vec_wave_packet_prev;

	vector<vector<complex<double>>> vec_fft;
	vector<vector<double>> vec_fft_abs;
	vector<vector<complex<double>>> vec_wave_func;
	int iter;
	int N_FFT;
	
	/** Потенциал задачи.*/
	vector<complex<double>> vec_U;
	/** Поглощающие слои.*/
	vector<complex<double>> vec_sigm;
	/** Поглощающие слои (производная).*/
	vector<complex<double>> vec_d_sigm;

	/** Вектор коэффициентов Ak.*/
	vector<complex<double>> vec_A;
	/** Вектор коэффициентов Bk.*/
	vector<complex<double>> vec_B;
	/** Вектор коэффициентов Ck.*/
	vector<complex<double>> vec_C;
	/** Вектор коэффициентов Dk.*/
	vector<complex<double>> vec_D;

	/** Вектор значений фукнции alpha.*/
	vector<complex<double>> vec_alpha;
	/** Вектор значений фукнции beta.*/
	vector<complex<double>> vec_beta;

	vector<double> vec_points_x_WavePacket;

	/** Устнаовить значения параметров.*/
	void SetParams(double size, int points, double t_step, double U_in, double a_in, double b_in, double gamma_in, double _K_in, double _B_in)
	{
		image_j = complex<double>(0, 1);
		model_size = size;
		tau = t_step;
		a = a_in; 
		b = b_in;
		_Uo = U_in;
		gamma = gamma_in;
		_K = _K_in;
		_B = _B_in;
		/** Выделение памяти для векторов.*/
		vec_points_x_WavePacket.resize(points);
		vec_wave_packet.resize(points);
		vec_wave_packet_prev.resize(points);
		vec_U.resize(points);
		vec_A.resize(points);
		vec_B.resize(points);
		vec_C.resize(points);
		vec_D.resize(points);
		vec_sigm.resize(points);
		vec_d_sigm.resize(points);
		vec_alpha.resize(points);
		vec_beta.resize(points);
		step_r = model_size / (double)(points - 1);
	}

	/** Вычислить значения потенциала задачи.*/
	void calculate_U()
	{
		for (size_t i = 0; i < vec_U.size(); i++)
		{
			double coord = (double)i / vec_wave_packet_prev.size() * model_size - model_size / 2.;
			vec_points_x_WavePacket[i] = coord;

			if (coord < a && coord > -a)
			{
				if (coord <= 0)
				{
					vec_U[i] = _B;
				}
				else
				{
					vec_U[i] = _K * coord;
				}
				
			}
			else
			{
				vec_U[i] = 0;
			}
		}
	}

	/** Вычислить функцию sigma.*/
	void calculate_SIGM()
	{
		for (size_t i = 0; i < vec_sigm.size(); i++)
		{
			double coord = (double)i / vec_wave_packet_prev.size() * model_size - model_size / 2.;

			if (coord < -b)
			{
				auto div = (1.0 + image_j * gamma * (coord + b) * (coord + b));
				vec_sigm[i] = 1.0 / div;
				vec_d_sigm[i] = -2.0 * image_j *gamma * (coord + b) / div / div;
			}
			else if (coord >= -b && coord <= b)
			{
				vec_sigm[i] = 1.0;
				vec_d_sigm[i] = 0;
			}
			else if (coord > b)
			{
				auto div = (1.0 + image_j * gamma * (coord - b) * (coord - b));
				vec_sigm[i] = 1.0 / div;
				vec_d_sigm[i] = -2.0 * image_j *gamma * (coord - b) / div / div;
			}
		}
	}

	/*complex<double> sigmf(int i)
	{
		double coord = (double)i / vec_wave_packet_prev.size() * model_size - model_size / 2.;
		if (coord < -b)
		{
			auto div = (1.0 + image_j * gamma * (coord + b) * (coord + b));
			return  1.0 / div;
		}
		else if (coord >= -b && coord <= b)
		{
			return 1.0;
		}
		else if (coord > b)
		{
			auto div = (1.0 + image_j * gamma * (coord - b) * (coord - b));
			return 1.0 / div;
		}
	}*/

	/** Вычислить коэффициенты Ak, Bk, Ck, Dk.*/
	void calculate_coeffs()
	{
		for (size_t i = 1; i < vec_A.size() - 1; i++)
		{
			vec_A[i] = -image_j * tau / 2.0 / step_r / step_r;
		}
		for (size_t i = 1; i < vec_B.size() - 1; i++)
		{
			vec_B[i] = - image_j * tau / 2.0 / step_r / step_r;
		}
		for (size_t i = 1; i < vec_C.size() - 1; i++)
		{
			vec_C[i] = 1.0 + image_j * tau * vec_U[i] / 2.0 + image_j * tau / step_r / step_r;
		}
		for (size_t i = 1; i < vec_C.size() - 1; i++)
		{
			vec_D[i] = vec_wave_packet_prev[i] +
				image_j * tau / 2.0 * ((vec_wave_packet_prev[i + 1] - 2.0 * vec_wave_packet_prev[i]
					+ vec_wave_packet_prev[i - 1]) / step_r / step_r - vec_U[i] * vec_wave_packet_prev[i]);
		}
	}
	/*complex<double> psi_fz_ss(int i)
	{
		if (i == 0 || i == vec_wave_packet_prev.size() - 1)
		{
			return 0;
		}
		else
		{
			return (vec_sigm[i + 1] * vec_wave_packet_prev[i + 1] -
				(vec_sigm[i + 1] + vec_sigm[i - 1]) * vec_wave_packet_prev[i] +
				vec_sigm[i - 1] * vec_wave_packet_prev[i - 1]) / step_r / step_r;
		}
	}*/

	/** Вычислить коэффициенты Ak, Bk, Ck, Dk (для производной).*/
	void alt_calculate_coeffs()
	{
		for (size_t i = 1; i < vec_A.size() - 1; i++)
		{
			vec_A[i] = -(image_j * tau / 2.0) * vec_sigm[i] * ((vec_sigm[i] / step_r / step_r) - vec_d_sigm[i] / (2.0 * step_r));
		}
		for (size_t i = 1; i < vec_B.size() - 1; i++)
		{
			vec_B[i] = -(image_j * tau / 2.0) * vec_sigm[i] * ((vec_sigm[i] / step_r / step_r) + vec_d_sigm[i] / (2.0 * step_r));
		}
		for (size_t i = 1; i < vec_C.size() - 1; i++)
		{
			vec_C[i] = 1.0 + (image_j * tau / 2.0) * (vec_U[i] + 2.0 * vec_sigm[i] * vec_sigm[i] / step_r / step_r);
		}
		for (size_t i = 1; i < vec_D.size() - 1; i++)
		{
			vec_D[i] = vec_wave_packet_prev[i] +
				(image_j * tau / 2.0) * (- vec_U[i] * vec_wave_packet_prev[i]) +
				(image_j * tau / 2.0) * (vec_sigm[i] * vec_d_sigm[i] * (vec_wave_packet_prev[i + 1] - vec_wave_packet_prev[i - 1]) /
				(2.0 * step_r)) + (image_j * tau / 2.0) * (vec_sigm[i] * vec_sigm[i] * (vec_wave_packet_prev[i + 1] - 
					2.0 * vec_wave_packet_prev[i] + vec_wave_packet_prev[i - 1]) / step_r / step_r);
		}
	}

	/** Проинициализировать состояние волнового пакета.*/
	void set_init_state(double mean, double disp)
	{
		for (size_t i = 0; i < vec_wave_packet_prev.size(); i++)
		{
			double coord = (double)i / vec_wave_packet_prev.size() * model_size - model_size / 2.;
			vec_wave_packet_prev[i] = exp(-(coord - mean) * (coord - mean) / 4.0 / disp / disp);
		}
		calculate_U();
		calculate_SIGM();
	}

	void step()
	{ 
		alt_calculate_coeffs();
		forward();
		backward();
		vec_wave_packet_prev.assign(vec_wave_packet.begin(), vec_wave_packet.end());
	}

	/** Прогонка вперед для функций alpha и beta.*/
	void forward()
	{
		vec_alpha[1] = 0;
		vec_beta[1] = 0;
		for (size_t i = 2; i < vec_alpha.size(); i++)
		{
			complex<double> div = vec_C[i - 1] + vec_A[i - 1] * vec_alpha[i - 1];

			vec_alpha[i] = - vec_B[i - 1] / div;
			vec_beta[i] = (vec_D[i - 1] - vec_A[i - 1] * vec_beta[i - 1]) / div;
		}
	}

	/** Прогонка назад для волнового пакета.*/
	void backward()
	{
		vec_wave_packet[vec_wave_packet.size() - 1] = 0;

		for (size_t i = vec_wave_packet.size() - 1; i > 0; i--)
		{
			vec_wave_packet[i - 1] = vec_alpha[i] * vec_wave_packet[i] + vec_beta[i];
		}
	}

	void stop()
	{
		stop_flag = true;
	}

	void run()
	{
		stop_flag = false;

		while (!stop_flag)
		{
			step();
		}
	}

	/** Произвести БПФ.
	* @param *data - вектор над которым произвести БПФ.
	* @param n - количество отсчётов в векторе.
	* @param is - если is = -1 - прямое, если is = 1 - обратное.
	*/
	void fourier(vector<complex<double>> & data, int n, int is)
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
};