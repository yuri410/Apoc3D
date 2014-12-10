#pragma once

namespace dip
{
	struct Complex
	{
		float real = 0;
		float imaginary = 0;

		Complex() { }

		Complex(float r, float i)
			: real(r), imaginary(i) { }

		friend static Complex operator +(const Complex& a, const Complex& b)
		{
			return Complex(a.real + b.real, a.imaginary + b.imaginary);
		}
		friend static Complex operator -(const Complex& a, const Complex& b)
		{
			return Complex(a.real - b.real, a.imaginary - b.imaginary);
		}
		friend static Complex operator *(const Complex& a, const Complex& b)
		{
			return Complex(a.real*b.real - a.imaginary*b.imaginary,
				a.real*b.imaginary + a.imaginary*b.real);
		}

		float Mod() const
		{
			float x = fabs(real);
			float y = fabs(imaginary);
			if (x == 0)
				return y;

			if (y == 0)
				return x;
			if (x > y)
				return x * sqrtf(1 + (y / x)*(y / x));
			return y * sqrtf(1 + (x / y)*(x / y));
		}
		float Angle() const
		{
			if (real == 0 && imaginary == 0)
				return 0;

			if (real == 0)
			{
				if (imaginary > 0)
					return Math::Half_PI;

				return -Math::Half_PI;
			}

			if (real > 0)
				return atan2f(imaginary, real);

			if (imaginary >= 0)
				return atan2f(imaginary, real) + Math::PI;

			return atan2f(imaginary, real) - Math::PI;
		}

		Complex Conjugate() const
		{
			return Complex(real, -imaginary);
		}
	};


	void fft(const Complex* src, Complex* dest, int n);
	void ifft(const Complex* src, Complex* dest, int count);
	void fft2(const float* src, Complex* dest, int width, int height, bool inv);
	void ifft2(const Complex* src, float* dest, int width, int height, bool inv);


	class FreqDomainFilter
	{
	public:
		virtual float GetWeight(int u, int v) const = 0;
	protected:
		FreqDomainFilter(bool hp, int w, int h, float d0) : m_isHighPass(hp), m_width(w), m_height(h), m_d0(d0) { }

		int m_width;
		int m_height;
		bool m_isHighPass;
		float m_d0;
	};

	class IPF : public FreqDomainFilter
	{
	public:
		IPF(bool highpass, int w, int h, float d0)
			: FreqDomainFilter(highpass, w, h, d0) { }

		virtual float GetWeight(int u, int v) const
		{
			float du = (u - m_width / 2.0f);
			float dv = (v - m_height / 2.0f);
			float d2 = du*du + dv*dv;
			if (m_isHighPass)
			{
				return (d2 >= m_d0*m_d0) ? 1.0f : 0.0f;
			}
			return (d2 <= m_d0*m_d0) ? 1.0f : 0.0f;
		}
	};

	class FPF : public FreqDomainFilter
	{
	public:
		FPF(bool highpass, int w, int h, float d0)
			: FreqDomainFilter(highpass, w, h, d0) { }

		virtual float GetWeight(int u, int v) const
		{
			float du = (u - m_width / 2.0f);
			float dv = (v - m_height / 2.0f);
			float d2 = sqrtf(du*du + dv*dv);
			if (m_isHighPass)
			{
				return Math::Saturate(1 - expf(-0.5f* d2 / (m_d0*m_d0)));
			}
			return Math::Saturate(expf(-0.5f* d2 / (m_d0*m_d0)));
		}
	};

	struct SignatureVector
	{
		double* Values;
		int32 Size;


		SignatureVector(double* arr, int32 count)
			: Size(count), Values(arr)
		{ }

		SignatureVector(const double* arr, int32 count)
			: Size(count)
		{
			Values = new double[Size];
			memcpy(Values, arr, sizeof(double) * Size);
		}

		SignatureVector(const SignatureVector& another)
			: Size(another.Size)
		{
			Values = new double[Size];
			memcpy(Values, another.Values, sizeof(double) * Size);
		}

		~SignatureVector()
		{
			delete[] Values;
			Values = nullptr;
		}

		void SetZero()
		{
			memset(Values, 0, sizeof(double) * Size);
		}

		SignatureVector& operator +=(const SignatureVector& rhs)
		{
			assert(Size == rhs.Size);
			for (int32 i = 0; i < Size; i++)
				Values[i] += rhs.Values[i];
			return *this;
		}
		SignatureVector& operator +=(const double& rhs)
		{
			for (int32 i = 0; i < Size; i++) Values[i] += rhs;
			return *this;
		}
		SignatureVector& operator *=(const double& rhs)
		{
			for (int32 i = 0; i < Size; i++) Values[i] *= rhs;
			return *this;
		}
		SignatureVector& operator /=(const double& rhs)
		{
			double invRhs = 1.0 / rhs;
			for (int32 i = 0; i < Size; i++) Values[i] *= invRhs;
			return *this;
		}

	};

	class Cluster
	{
	public:
		List<int> CurrentMemberShip;
		SignatureVector Mean;

		Cluster(int dataindex, const SignatureVector* data, int vectorCount)
			: m_vectorCount(vectorCount), Mean(*data)//, m_centroidIndex(dataindex)
		{
			CurrentMemberShip.Add(dataindex);
		}

		void UpdateMean(SignatureVector** coordinates)
		{
			Mean.SetZero();

			int counter = 0;
			bool centroidIncluded = false;
			for (int i = 0; i < CurrentMemberShip.getCount(); i++)
			{
				int idx = CurrentMemberShip[i];
				//if (!centroidIncluded && idx == m_centroidIndex)
				//{
				//centroidIncluded = true;
				//}

				const SignatureVector& coord = *coordinates[idx];
				Mean += coord;

				counter++;
			}

			//if (!centroidIncluded)
			//{
			//Mean += *coordinates[m_centroidIndex];
			//counter++;
			//}

			if (counter > 0)
			{
				Mean /= (double)counter;
			}

		}

		//int getCentroidIndex() const { return m_centroidIndex; }
	private:
		int m_vectorCount;
		//int m_centroidIndex;
	};



	class KMeans
	{
	public:
		KMeans(SignatureVector** data, int vectorCount, const List<int>& initialCoords)
			: m_vectorCount(vectorCount), m_k(initialCoords.getCount())
		{
			m_coordinates = data;
			m_cluster = new Cluster*[m_k];
			m_clusterAssignments = new int[vectorCount];
			for (int32 i = 0; i < vectorCount; i++)
				m_clusterAssignments[i] = -1;
			//memset(m_clusterAssignments, 0, sizeof(int)*vectorCount);

			m_nearestCluster = new int[vectorCount];
			for (int32 i = 0; i < vectorCount; i++)
				m_nearestCluster[i] = -1;
			//memset(m_nearestCluster, 0, sizeof(int)*vectorCount);

			m_distanceCache = new double*[vectorCount];

			for (int i = 0; i < vectorCount; i++)
			{
				m_distanceCache[i] = new double[vectorCount];
				memset(m_distanceCache[i], 0, sizeof(double)*vectorCount);
			}


			for (int i = 0; i < m_k; i++)
			{
				int temp = initialCoords[i];

				m_clusterAssignments[temp] = i;
				m_cluster[i] = new Cluster(temp, m_coordinates[temp], vectorCount);
			}
		}
		KMeans(SignatureVector** data, int vectorCount, int k)
			: m_k(k), m_vectorCount(vectorCount)
		{
			m_coordinates = data;
			m_cluster = new Cluster*[k];
			m_clusterAssignments = new int[vectorCount];
			memset(m_clusterAssignments, 0, sizeof(int)*vectorCount);

			m_nearestCluster = new int[vectorCount];
			memset(m_nearestCluster, 0, sizeof(int)*vectorCount);

			m_distanceCache = new double*[vectorCount];

			for (int i = 0; i < vectorCount; i++)
			{
				m_distanceCache[i] = new double[vectorCount];
				memset(m_distanceCache[i], 0, sizeof(double)*vectorCount);
			}
			InitRandom();
		}

		void Start()
		{
			int iter = 0;
			while (1)
			{
				LogManager::getSingleton().Write(LOG_System, L"Iteration " + StringUtils::IntToString(iter) + L"...", LOGLVL_Infomation);
				for (int i = 0; i < m_k; i++)
				{
					m_cluster[i]->UpdateMean(m_coordinates);
				}

				for (int i = 0; i < m_vectorCount; i++)
				{
					for (int j = 0; j < m_k; j++)
					{
						double dist = getDistance(*m_coordinates[i], m_cluster[j]->Mean);
						m_distanceCache[i][j] = dist;
					}
				}

				for (int i = 0; i < m_vectorCount; i++)
				{
					m_nearestCluster[i] = nearestCluster(i);
				}

				int k = 0;
				for (int i = 0; i < m_vectorCount; i++)
				{
					if (m_nearestCluster[i] == m_clusterAssignments[i])
						k++;
				}
				if (k == m_vectorCount)
					break;

				for (int j = 0; j < m_k; j++)
				{
					m_cluster[j]->CurrentMemberShip.Clear();
				}
				for (int i = 0; i < m_vectorCount; i++)
				{
					m_cluster[m_nearestCluster[i]]->CurrentMemberShip.Add(i);
					m_clusterAssignments[i] = m_nearestCluster[i];
				}

				iter++;
			}
		}

		const Cluster* const* getClusters() const { return m_cluster; }
	private:

		void InitRandom()
		{
			for (int i = 0; i < m_k; i++)
			{
				int temp = (int)(m_vectorCount * (double)i / (double)(m_k - 1));//Randomizer::Next(m_coordCount);
				if (temp < 0) temp = 0;
				if (temp >= m_vectorCount) temp = m_vectorCount - 1;

				m_clusterAssignments[temp] = i;
				m_cluster[i] = new Cluster(temp, m_coordinates[temp], m_vectorCount);
			}
		}
		int nearestCluster(int ndx)
		{
			int nearest = -1;
			double min = std::numeric_limits<double>::infinity();

			for (int c = 0; c < m_k; c++)
			{
				double d = m_distanceCache[ndx][c];
				if (d < min)
				{
					min = d;
					nearest = c;
				}
			}
			if (nearest == -1)
			{
				assert(0);
			}
			return nearest;
		}
		double getDistance(const SignatureVector& coord, const SignatureVector& center)
		{
			return 1 - ComputeCosineSimilarity(coord, center);
		}
		double ComputeCosineSimilarity(const SignatureVector& vector1, const SignatureVector& vector2)
		{
			double denom = (VectorLength(vector1) * VectorLength(vector2));
			if (denom == 0)
				return 0;
			else
				return (InnerProduct(vector1, vector2) / denom);

		}

		double InnerProduct(const SignatureVector& vector1, const SignatureVector& vector2)
		{
			assert(vector1.Size == vector2.Size);

			double result = 0;
			for (int i = 0; i < vector1.Size; i++)
				result += vector1.Values[i] * vector2.Values[i];

			return result;
		}

		double VectorLength(const SignatureVector& vector)
		{
			double sum = 0;
			for (int i = 0; i < vector.Size; i++)
				sum = sum + (vector.Values[i] * vector.Values[i]);

			return sqrt(sum);
		}

		int m_vectorCount;
		SignatureVector** m_coordinates;
		int m_k;
		Cluster** m_cluster;

		int* m_clusterAssignments;
		int* m_nearestCluster;
		double** m_distanceCache;


	};


	void echbg(double a[], int n);

	bool Matrix_EigenValue(double *K1, int n, int LoopNumber, double Error1, double *Ret);

	//Change a matrix to Hessen berg matrix using elementary similarity transformation.
	//a-array n*n, input and output
	//n-dimension of 
	void echbg(double a[], int n);
}