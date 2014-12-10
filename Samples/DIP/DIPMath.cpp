#include "DIPMath.h"

namespace dip
{

	void fft(const Complex* src, Complex* dest, int n)
	{
		float log2 = logf(2.0f);
		int r = (int)(logf((float)n) / log2);

		Complex* w = new Complex[n / 2];
		Complex* interVal1 = new Complex[n];
		Complex* interVal2 = new Complex[n];
		Complex* temp = new Complex[n];

		memcpy(interVal1, src, sizeof(Complex)*n);

		for (int i = 0; i < n / 2; i++)
		{
			float angle = -i * Math::PI * 2.0f / n;
			w[i] = Complex(cosf(angle), sinf(angle));
		}

		for (int i = 0; i < r; i++)
		{
			int interval = 1 << i;
			int halfN = 1 << (r - i);

			for (int j = 0; j < interval; j++)
			{
				int gap = j*halfN;

				for (int k = 0; k < halfN / 2; k++)
				{

					assert(k + gap >= 0);
					assert(k + gap < n);
					assert(k + gap + halfN / 2 >= 0);
					assert(k + gap + halfN / 2 < n);
					assert(k*interval >= 0);
					assert(k*interval < n / 2);

					interVal2[k + gap] = interVal1[k + gap] + interVal1[k + gap + halfN / 2];
					interVal2[k + halfN / 2 + gap] = (interVal1[k + gap] - interVal1[k + gap + halfN / 2]) * w[k*interval];
				}
			}
			memcpy(interVal1, interVal2, sizeof(Complex)*n);
		}
		for (int j = 0; j < n; j++)
		{
			int p = 0;
			for (int i = 0; i < r; i++)
			{
				if (j&(1 << i))
				{
					p += 1 << (r - i - 1);
				}
			}
			dest[j] = interVal1[p];
		}

		delete[] w;
		delete[] interVal2;
		delete[] interVal1;
		delete[] temp;
	};
	void ifft(const Complex* src, Complex* dest, int count)
	{
		Complex* src2 = new Complex[count];
		memcpy(src2, src, count*sizeof(Complex));
		for (int i = 0; i < count; i++)
		{
			src2[i] = src[i].Conjugate();
		}

		Complex* interVal = dest;
		fft(src2, interVal, count);

		for (int i = 0; i < count; i++)
		{
			interVal[i] = Complex(interVal[i].real / count, -interVal[i].imaginary / count);
		}
		delete[] src2;
	}
	void fft2(const float* src, Complex* dest, int width, int height, bool inv)
	{
		int pixels = width*height;
		Complex* tempCom1 = dest;//new Complex[pixels];

		for (int i = 0; i < pixels; i++)
		{
			if (inv)
			{
				if ((i / width + i % width) % 2 == 0)
				{
					tempCom1[i] = Complex(src[i], 0);
				}
				else
				{
					tempCom1[i] = Complex(-src[i], 0);
				}
			}
			else
			{
				tempCom1[i] = Complex(src[i], 0);
			}
		}

		Complex* tempCom2 = new Complex[width];
		Complex* tempCom3 = new Complex[width];

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++) tempCom2[j] = tempCom1[i * width + j];

			fft(tempCom2, tempCom3, width);

			for (int j = 0; j < width; j++) tempCom1[i * width + j] = tempCom3[j];
		}
		delete[] tempCom2;
		delete[] tempCom3;


		Complex* tempCom4 = new Complex[height];
		Complex* tempCom5 = new Complex[height];
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++) tempCom4[j] = tempCom1[j*width + i];

			fft(tempCom4, tempCom5, height);

			for (int j = 0; j < height; j++) tempCom1[j*width + i] = tempCom5[j];
		}

		delete[] tempCom4;
		delete[] tempCom5;

	}

	void ifft2(const Complex* src, float* dest, int width, int height, bool inv)
	{
		int pixels = width*height;
		Complex* tempCom1 = new Complex[pixels];
		memcpy(tempCom1, src, pixels* sizeof(Complex));

		Complex* tempCom2 = new Complex[width];
		Complex* tempCom3 = new Complex[width];
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++) tempCom2[j] = tempCom1[i*width + j];

			ifft(tempCom2, tempCom3, width);
			for (int j = 0; j < width; j++) tempCom1[i*width + j] = tempCom3[j];

		}
		delete[] tempCom2;
		delete[] tempCom3;

		Complex* tempCom4 = new Complex[height];
		Complex* tempCom5 = new Complex[height];
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++) tempCom4[j] = tempCom1[j * width + i];

			ifft(tempCom4, tempCom5, height);
			for (int j = 0; j < height; j++) tempCom1[j*width + i] = tempCom5[j];

		}
		delete[] tempCom4;
		delete[] tempCom5;


		for (int i = 0; i < pixels; i++)
		{
			float temp;
			if (inv)
			{
				if ((i / width + i % width) % 2 == 0)
					temp = tempCom1[i].real;
				else
					temp = -tempCom1[i].real;
			}
			else
			{
				temp = tempCom1[i].real;
			}
			dest[i] = temp;
		}
		delete[] tempCom1;
	}



	void echbg(double a[], int n)
	{
		int i, j, k, u, v;
		double d, t;
		for (k = 1; k <= n - 2; k++)
		{
			d = 0.0;
			for (j = k; j <= n - 1; j++)
			{
				u = j*n + k - 1;
				t = a[u];
				if (fabs(t) > fabs(d))
				{
					d = t;
					i = j;
				}
			}
			if (fabs(d) + 1.0 != 1.0)
			{
				if (i != k)
				{
					for (j = k - 1; j <= n - 1; j++)
					{
						u = i*n + j;
						v = k*n + j;
						t = a[u];
						a[u] = a[v];
						a[v] = t;
					}
					for (j = 0; j <= n - 1; j++)
					{
						u = j*n + i;
						v = j*n + k;
						t = a[u];
						a[u] = a[v];
						a[v] = t;
					}
				}
				for (i = k + 1; i <= n - 1; i++)
				{
					u = i*n + k - 1;
					t = a[u] / d;
					a[u] = 0.0;
					for (j = k; j <= n - 1; j++)
					{
						v = i*n + j;
						a[v] = a[v] - t*a[k*n + j];
					}
					for (j = 0; j <= n - 1; j++)
					{
						v = j*n + k;
						a[v] = a[v] + t*a[j*n + i];
					}
				}
			}
		}
		return;
	}

	bool Matrix_EigenValue(double *K1, int n, int LoopNumber, double Error1, double *Ret)
	{
		int i, j, k, t, m, Loop1;
		double b, c, d, g, xy, p, q, r, x, s, e, f, z, y, temp, *A;
		A = new double[n*n];
		memcpy(A, K1, sizeof(double)*n*n);
		echbg(A, n);
		m = n;
		Loop1 = LoopNumber;
		while (m != 0)
		{
			t = m - 1;
			while (t > 0)
			{
				temp = abs(A[(t - 1)*n + t - 1]);
				temp += abs(A[t*n + t]);
				temp = temp*Error1;
				if (abs(A[t*n + t - 1]) > temp)
				{
					t--;
				}
				else
				{
					break;
				}
			}
			if (t == m - 1)
			{
				Ret[(m - 1) * 2] = A[(m - 1)*n + m - 1];
				Ret[(m - 1) * 2 + 1] = 0;
				m -= 1;
				Loop1 = LoopNumber;
			}
			else if (t == m - 2)
			{
				b = -A[(m - 1)*n + m - 1] - A[(m - 2)*n + m - 2];
				c = A[(m - 1)*n + m - 1] * A[(m - 2)*n + m - 2] - A[(m - 1)*n + m - 2] * A[(m - 2)*n + m - 1];
				d = b*b - 4 * c;
				y = sqrt(abs(d));
				if (d > 0)
				{
					xy = 1;
					if (b < 0)
					{
						xy = -1;
					}
					Ret[(m - 1) * 2] = -(b + xy*y) / 2;
					Ret[(m - 1) * 2 + 1] = 0;
					Ret[(m - 2) * 2] = c / Ret[(m - 1) * 2];
					Ret[(m - 2) * 2 + 1] = 0;
				}
				else
				{
					Ret[(m - 1) * 2] = -b / 2;
					Ret[(m - 2) * 2] = -b / 2;
					Ret[(m - 1) * 2 + 1] = y / 2;
					Ret[(m - 2) * 2 + 1] = -y / 2;
				}
				m -= 2;
				Loop1 = LoopNumber;
			}
			else
			{
				if (Loop1 < 1)
				{
					return false;
				}
				Loop1--;
				j = t + 2;
				while (j < m)
				{
					A[j*n + j - 2] = 0;
					j++;
				}
				j = t + 3;
				while (j < m)
				{
					A[j*n + j - 3] = 0;
					j++;
				}
				k = t;
				while (k < m - 1)
				{
					if (k != t)
					{
						p = A[k*n + k - 1];
						q = A[(k + 1)*n + k - 1];
						if (k != m - 2)
						{
							r = A[(k + 2)*n + k - 1];
						}
						else
						{
							r = 0;
						}
					}
					else
					{
						b = A[(m - 1)*n + m - 1];
						c = A[(m - 2)*n + m - 2];
						x = b + c;
						y = b*c - A[(m - 2)*n + m - 1] * A[(m - 1)*n + m - 2];
						p = A[t*n + t] * (A[t*n + t] - x) + A[t*n + t + 1] * A[(t + 1)*n + t] + y;
						q = A[(t + 1)*n + t] * (A[t*n + t] + A[(t + 1)*n + t + 1] - x);
						r = A[(t + 1)*n + t] * A[(t + 2)*n + t + 1];
					}
					if (p != 0 || q != 0 || r != 0)
					{
						if (p < 0)
						{
							xy = -1;
						}
						else
						{
							xy = 1;
						}
						s = xy*sqrt(p*p + q*q + r*r);
						if (k != t)
						{
							A[k*n + k - 1] = -s;
						}
						e = -q / s;
						f = -r / s;
						x = -p / s;
						y = -x - f*r / (p + s);
						g = e*r / (p + s);
						z = -x - e*q / (p + s);
						for (j = k; j<m; j++)
						{
							b = A[k*n + j];
							c = A[(k + 1)*n + j];
							p = x*b + e*c;
							q = e*b + y*c;
							r = f*b + g*c;
							if (k != m - 2)
							{
								b = A[(k + 2)*n + j];
								p += f*b;
								q += g*b;
								r += z*b;
								A[(k + 2)*n + j] = r;
							}
							A[(k + 1)*n + j] = q;
							A[k*n + j] = p;
						}
						j = k + 3;
						if (j>m - 2)
						{
							j = m - 1;
						}
						for (i = t; i < j + 1; i++)
						{
							b = A[i*n + k];
							c = A[i*n + k + 1];
							p = x*b + e*c;
							q = e*b + y*c;
							r = f*b + g*c;
							if (k != m - 2)
							{
								b = A[i*n + k + 2];
								p += f*b;
								q += g*b;
								r += z*b;
								A[i*n + k + 2] = r;
							}
							A[i*n + k + 1] = q;
							A[i*n + k] = p;
						}
					}
					k++;
				}
			}
		}
		delete[]A;
		return true;
	}
}