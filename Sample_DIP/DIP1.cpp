/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "DIP1.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/Label.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/UILib/PictureBox.h"
#include "apoc3d/UILib/ComboBox.h"
#include "apoc3d/UILib/CheckBox.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Platform/Thread.h"

#include <complex>
#include <cstddef>

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


namespace dip
{
	struct Complex
	{
		float real;
		float imaginary;

		Complex()
			: real(0), imaginary(0)
		{ }

		Complex(float r, float i)
			: real(r), imaginary(i)
		{ }

		friend static Complex operator +(const Complex& a, const Complex& b)
		{
			return Complex(a.real+b.real, a.imaginary+b.imaginary);
		}
		friend static Complex operator -(const Complex& a, const Complex& b)
		{
			return Complex(a.real-b.real, a.imaginary-b.imaginary);
		}
		friend static Complex operator *(const Complex& a, const Complex& b)
		{
			return Complex(a.real*b.real-a.imaginary*b.imaginary, 
				a.real*b.imaginary+a.imaginary*b.real);
		}
		float Mod() const
		{
			float x = fabs(real);
			float y = fabs(imaginary);
			if (x==0)
				return y;

			if (y == 0)
				return x;
			if (x>y)
				return x * sqrtf(1+(y/x)*(y/x));
			return y * sqrtf(1+(x/y)*(x/y));
		}
		float Angle() const
		{
			if (real == 0 && imaginary == 0)
				return 0;

			if (real == 0)
			{
				if (imaginary>0)
					return Math::Half_PI;

				return -Math::Half_PI;
			}

			if (real>0)
				return atan2f(imaginary,real);

			if (imaginary>=0)
				return atan2f(imaginary,real) + Math::PI;

			return atan2f(imaginary,real) - Math::PI;
		}

		Complex Conjugate() const
		{
			return Complex(real, -imaginary);
		}
	};


	void fft(const Complex* src, Complex* dest, int n)
	{
		float log2 = logf(2.0f);
		int r = (int)(logf((float)n)/log2);

		Complex* w = new Complex[n/2];
		Complex* interVal1 = new Complex[n];
		Complex* interVal2 = new Complex[n];
		Complex* temp = new Complex[n];

		memcpy(interVal1, src, sizeof(Complex)*n);

		for (int i=0;i<n/2;i++)
		{
			float angle = -i * Math::PI * 2.0f / n;
			w[i] = Complex(cosf(angle), sinf(angle));
		}
		
		for (int i=0;i<r;i++)
		{
			int interval = 1 << i;
			int halfN = 1 << (r-i);

			for (int j=0;j<interval;j++)
			{
				int gap = j*halfN;

				for (int k=0;k<halfN/2;k++)
				{
					
					assert(k+gap>=0);
					assert(k+gap<n);
					assert(k+gap+halfN/2>=0);
					assert(k+gap+halfN/2<n);
					assert(k*interval>=0);
					assert(k*interval<n/2);

					interVal2[k+gap] = interVal1[k+gap] + interVal1[k+gap+halfN/2];
					interVal2[k+halfN/2+gap] = (interVal1[k+gap] - interVal1[k+gap+halfN/2]) * w[k*interval];
				}
			}
			memcpy(interVal1, interVal2, sizeof(Complex)*n);
		}
		for(int j = 0; j < n; j++)
		{
			int p = 0;
			for(int i = 0; i < r; i++)
			{
				if (j&(1<<i))
				{
					p+=1<<(r-i-1);
				}
			}
			dest[j]=interVal1[p];
		}

		delete[] w;
		delete[] interVal2;
		delete[] interVal1;
		delete[] temp;
	};
	void ifft(const Complex* src, Complex* dest, int count)
	{
		Complex* src2 = new Complex[count];
		memcpy(src2,src,count*sizeof(Complex));
		for (int i=0;i<count;i++)
		{
			src2[i] = src[i].Conjugate();
		}

		Complex* interVal = dest;
		fft(src2, interVal, count);

		for (int i=0;i<count;i++)
		{
			interVal[i] = Complex(interVal[i].real/count, -interVal[i].imaginary/count);
		}
		delete[] src2;
	}
	void fft2(const float* src, Complex* dest, int width, int height, bool inv)
	{
		int pixels = width*height;
		Complex* tempCom1 = dest;//new Complex[pixels];

		for (int i=0;i<pixels;i++)
		{
			if (inv)
			{
				if ((i/width + i % width) % 2 ==0)
				{
					tempCom1[i] = Complex(src[i],0);
				}
				else
				{
					tempCom1[i] = Complex(-src[i],0);
				}
			}
			else
			{
				tempCom1[i] = Complex(src[i],0);
			}
		}

		Complex* tempCom2 = new Complex[width];
		Complex* tempCom3 = new Complex[width];

		for (int i=0;i<height;i++)
		{
			for (int j = 0; j < width; j++) tempCom2[j] = tempCom1[i * width + j];
			
			fft(tempCom2, tempCom3, width);

			for (int j = 0; j < width; j++) tempCom1[i * width + j] = tempCom3[j];
		}
		delete[] tempCom2;
		delete[] tempCom3;


		Complex* tempCom4 = new Complex[height];
		Complex* tempCom5 = new Complex[height];
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++) tempCom4[j] = tempCom1[j*width+i];

			fft(tempCom4, tempCom5, height);

			for (int j=0;j<height;j++) tempCom1[j*width+i] = tempCom5[j];
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
		for (int i=0;i<height;i++)
		{
			for (int j=0;j<width;j++) tempCom2[j] = tempCom1[i*width+j];
				 
			ifft(tempCom2, tempCom3, width);
			for (int j=0;j<width;j++) tempCom1[i*width+j] = tempCom3[j];
				 
		}
		delete[] tempCom2;
		delete[] tempCom3;

		Complex* tempCom4 = new Complex[height];
		Complex* tempCom5 = new Complex[height];
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++) tempCom4[j] = tempCom1[j * width + i];
				 
			ifft(tempCom4, tempCom5, height);
			for (int j=0;j<height;j++) tempCom1[j*width+i] = tempCom5[j];
				
		}
		delete[] tempCom4;
		delete[] tempCom5;

		
		for (int i=0;i<pixels;i++)
		{
			float temp;
			if (inv)
			{
				if ((i/width + i % width) % 2 ==0)
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


	class FreqDomainFilter
	{
	public:
		virtual float GetWeight(int u, int v) const = 0;
	protected:
		FreqDomainFilter(bool hp, int w, int h, float d0) : m_isHighPass(hp), m_d0(d0), m_width(w), m_height(h) { }

		int m_width;
		int m_height;
		bool m_isHighPass;
		float m_d0;
	};
	class IPF : public FreqDomainFilter
	{
	public:
		IPF(bool highpass, int w, int h, float d0)
			: FreqDomainFilter(highpass,w,h, d0)
		{ }

		virtual float GetWeight(int u, int v) const
		{
			float du = (u-m_width/2.0f);
			float dv = (v-m_height/2.0f);
			float d2 = du*du+dv*dv;
			if (m_isHighPass)
			{
				return (d2>=m_d0*m_d0) ? 1.0f : 0.0f;
			}
			return (d2<=m_d0*m_d0) ? 1.0f : 0.0f;
		}
	};
	class FPF : public FreqDomainFilter
	{
	public:
		FPF(bool highpass, int w, int h, float d0)
			: FreqDomainFilter(highpass,w,h, d0)
		{ }

		virtual float GetWeight(int u, int v) const
		{
			float du = (u-m_width/2.0f);
			float dv = (v-m_height/2.0f);
			float d2 = sqrtf(du*du+dv*dv);
			if (m_isHighPass)
			{
				return Math::Saturate(1-expf(-0.5f* d2/(m_d0*m_d0)));
			}
			return Math::Saturate(expf(-0.5f* d2/(m_d0*m_d0)));
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
			for (int32 i=0;i<Size;i++)
				Values[i] += rhs.Values[i];
			return *this;
		}
		SignatureVector& operator +=(const double& rhs)
		{
			for (int32 i=0;i<Size;i++) Values[i] += rhs;
			return *this;
		}
		SignatureVector& operator *=(const double& rhs)
		{
			for (int32 i=0;i<Size;i++) Values[i] *= rhs;
			return *this;
		}
		SignatureVector& operator /=(const double& rhs)
		{
			double invRhs = 1.0/rhs;
			for (int32 i=0;i<Size;i++) Values[i] *= invRhs;
			return *this;
		}

	};

	class Cluster
	{
	public:
		FastList<int> CurrentMemberShip;
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
			for (int i=0;i<CurrentMemberShip.getCount();i++)
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

			if (counter>0)
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
		KMeans(SignatureVector** data, int vectorCount, const FastList<int>& initialCoords)
			: m_vectorCount(vectorCount),  m_k(initialCoords.getCount())
		{
			m_coordinates = data;
			m_cluster = new Cluster*[m_k];
			m_clusterAssignments = new int[vectorCount];
			for (int32 i=0;i<vectorCount;i++)
				m_clusterAssignments[i] = -1;
			//memset(m_clusterAssignments, 0, sizeof(int)*vectorCount);

			m_nearestCluster = new int[vectorCount];
			for (int32 i=0;i<vectorCount;i++)
				m_nearestCluster[i] = -1;
			//memset(m_nearestCluster, 0, sizeof(int)*vectorCount);

			m_distanceCache = new double*[vectorCount];

			for (int i=0;i<vectorCount;i++)
			{
				m_distanceCache[i] = new double[vectorCount];
				memset(m_distanceCache[i], 0, sizeof(double)*vectorCount);
			}


			for (int i=0;i<m_k;i++)
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
			
			for (int i=0;i<vectorCount;i++)
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
				for (int i=0;i<m_k;i++)
				{
					m_cluster[i]->UpdateMean(m_coordinates);
				}

				for (int i=0;i<m_vectorCount;i++)
				{
					for (int j=0;j<m_k;j++)
					{
						double dist = getDistance(*m_coordinates[i], m_cluster[j]->Mean);
						m_distanceCache[i][j] = dist;
					}
				}

				for (int i=0;i<m_vectorCount;i++)
				{
					m_nearestCluster[i] = nearestCluster(i);
				}

				int k=0;
				for (int i=0;i<m_vectorCount;i++)
				{
					if (m_nearestCluster[i] == m_clusterAssignments[i])
						k++;
				}
				if (k==m_vectorCount)
					break;

				for (int j=0;j<m_k;j++)
				{
					m_cluster[j]->CurrentMemberShip.Clear();
				}
				for (int i=0;i<m_vectorCount;i++)
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
			for (int i=0;i<m_k;i++)
			{
				int temp = (int)(m_vectorCount * (double)i/(double)(m_k-1));//Randomizer::Next(m_coordCount);
				if (temp<0) temp = 0;
				if (temp>=m_vectorCount) temp = m_vectorCount-1;

				m_clusterAssignments[temp] = i;
				m_cluster[i] = new Cluster(temp, m_coordinates[temp], m_vectorCount);
			}
		}
		int nearestCluster(int ndx)
		{
			int nearest = -1;
			double min = std::numeric_limits<double>::infinity();

			for (int c=0;c<m_k;c++)
			{
				double d = m_distanceCache[ndx][c];
				if (d<min)
				{
					min=d;
					nearest=c;
				}
			}
			if (nearest ==-1)
			{
				assert(0);
			}
			return nearest;
		}
		double getDistance(const SignatureVector& coord, const SignatureVector& center)
		{
			return 1-ComputeCosineSimilarity(coord, center);
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
	

	void echbg(double a[],int n);

	bool Matrix_EigenValue(double *K1,int n,int LoopNumber,double Error1,double *Ret)
	{
		int i,j,k,t,m,Loop1;
		double b,c,d,g,xy,p,q,r,x,s,e,f,z,y,temp,*A;
		A=new double[n*n];
		memcpy(A,K1, sizeof(double)*n*n);
		echbg(A,n);
		m=n;
		Loop1=LoopNumber;
		while(m!=0)
		{
			t=m-1;
			while(t>0)
			{
				temp=abs(A[(t-1)*n+t-1]);
				temp+=abs(A[t*n+t]);
				temp=temp*Error1;
				if (abs(A[t*n+t-1])>temp)
				{
					t--;
				}
				else
				{
					break;
				}
			}
			if (t==m-1)
			{
				Ret[(m-1)*2]=A[(m-1)*n+m-1];
				Ret[(m-1)*2+1]=0;
				m-=1;
				Loop1=LoopNumber;
			}
			else if(t==m-2)
			{
				b=-A[(m-1)*n+m-1]-A[(m-2)*n+m-2];
				c=A[(m-1)*n+m-1]*A[(m-2)*n+m-2]-A[(m-1)*n+m-2]*A[(m-2)*n+m-1];
				d=b*b-4*c;
				y=sqrt(abs(d));
				if (d>0)
				{
					xy=1;
					if (b<0)
					{
						xy=-1;
					}
					Ret[(m-1)*2]=-(b+xy*y)/2;
					Ret[(m-1)*2+1]=0;
					Ret[(m-2)*2]=c/Ret[(m-1)*2];
					Ret[(m-2)*2+1]=0;
				}
				else
				{
					Ret[(m-1)*2]=-b/2;
					Ret[(m-2)*2]=-b/2;
					Ret[(m-1)*2+1]=y/2;
					Ret[(m-2)*2+1]=-y/2;
				}
				m-=2;
				Loop1=LoopNumber;
			}
			else
			{
				if (Loop1<1)
				{
					return false;
				}
				Loop1--;
				j=t+2;
				while (j<m)
				{
					A[j*n+j-2]=0;
					j++;
				}
				j=t+3;
				while (j<m)
				{
					A[j*n+j-3]=0;
					j++;
				}
				k=t;
				while (k<m-1)
				{
					if (k!=t)
					{
						p=A[k*n+k-1];
						q=A[(k+1)*n+k-1];
						if (k!=m-2)
						{
							r=A[(k+2)*n+k-1];
						}
						else
						{
							r=0;
						}
					}
					else
					{
						b=A[(m-1)*n+m-1];
						c=A[(m-2)*n+m-2];
						x=b+c;
						y=b*c-A[(m-2)*n+m-1]*A[(m-1)*n+m-2];
						p=A[t*n+t]*(A[t*n+t]-x)+A[t*n+t+1]*A[(t+1)*n+t]+y;
						q=A[(t+1)*n+t]*(A[t*n+t]+A[(t+1)*n+t+1]-x);
						r=A[(t+1)*n+t]*A[(t+2)*n+t+1];
					}
					if (p!=0 || q!=0 || r!=0)
					{
						if (p<0)
						{
							xy=-1;
						}
						else
						{
							xy=1;
						}
						s=xy*sqrt(p*p+q*q+r*r);
						if (k!=t)
						{
							A[k*n+k-1]=-s;
						}
						e=-q/s;
						f=-r/s;
						x=-p/s;
						y=-x-f*r/(p+s);
						g=e*r/(p+s);
						z=-x-e*q/(p+s);
						for (j=k;j<m;j++)
						{
							b=A[k*n+j];
							c=A[(k+1)*n+j];
							p=x*b+e*c;
							q=e*b+y*c;
							r=f*b+g*c;
							if (k!=m-2)
							{
								b=A[(k+2)*n+j];
								p+=f*b;
								q+=g*b;
								r+=z*b;
								A[(k+2)*n+j]=r;
							}
							A[(k+1)*n+j]=q;
							A[k*n+j]=p;
						}
						j=k+3;
						if (j>m-2)
						{
							j=m-1;
						}
						for (i=t;i<j+1;i++)
						{
							b=A[i*n+k];
							c=A[i*n+k+1];
							p=x*b+e*c;
							q=e*b+y*c;
							r=f*b+g*c;
							if (k!=m-2)
							{
								b=A[i*n+k+2];
								p+=f*b;
								q+=g*b;
								r+=z*b;
								A[i*n+k+2]=r;
							}
							A[i*n+k+1]=q;
							A[i*n+k]=p;
						}
					}
					k++;
				}
			}
		}
		delete []A;
		return true;
	}

	//Change a matrix to Hessen berg matrix using elementary similarity transformation.
	//a-array n*n, input and output
	//n-dimension of 
	void echbg(double a[],int n) 
	{ int i,j,k,u,v; 
	double d,t; 
	for (k=1; k<=n-2; k++) 
	{ 
		d=0.0; 
		for (j=k; j<=n-1; j++) 
		{ 
			u=j*n+k-1; 
			t=a[u]; 
			if (fabs(t)>fabs(d)) 
			{ 
				d=t; 
				i=j; 
			} 
		} 
		if (fabs(d)+1.0!=1.0) 
		{ 
			if (i!=k) 
			{ 
				for (j=k-1; j<=n-1; j++) 
				{ 
					u=i*n+j; 
					v=k*n+j; 
					t=a[u]; 
					a[u]=a[v]; 
					a[v]=t; 
				} 
				for (j=0; j<=n-1; j++) 
				{ 
					u=j*n+i; 
					v=j*n+k; 
					t=a[u]; 
					a[u]=a[v]; 
					a[v]=t; 
				} 
			} 
			for (i=k+1; i<=n-1; i++) 
			{ 
				u=i*n+k-1; 
				t=a[u]/d; 
				a[u]=0.0; 
				for (j=k; j<=n-1; j++) 
				{ 
					v=i*n+j; 
					a[v]=a[v]-t*a[k*n+j]; 
				} 
				for (j=0; j<=n-1; j++) 
				{ 
					v=j*n+k; 
					a[v]=a[v]+t*a[j*n+i]; 
				} 
			} 
		} 
	} 
	return; 
	} 






    DIP1::DIP1(RenderWindow* wnd)
        : Game(wnd)
    {
		memset(&m_originalHgram, 0, sizeof(HistogramData));
		memset(&m_w11result1HG, 0, sizeof(HistogramData));
		memset(&m_w11result2HG, 0, sizeof(HistogramData));
    }

	DIP1::~DIP1()
	{		

	}

	void DIP1::DrawHistogramFor(Sprite* sprite, const Apoc3D::Math::Rectangle& rect, 
		const HistogramData& data)
	{
		sprite->Draw(m_UIskin->WhitePixelTexture, rect, CV_White);
		if (data.HighestPixelCount==0)
			return;

		int stepLen = rect.Width / 256;
		int cx = 0;
		for (int i=0;i<256;i++)
		{
			cx += stepLen;
			Apoc3D::Math::Rectangle spike;
			spike.X = (rect.Width *i) / 256 + rect.X;
			spike.Width = stepLen;
			if (spike.Width<1)
				spike.Width = 1;
			spike.Height = (data.PixelCount[i] * rect.Height) / data.HighestPixelCount;
			spike.Y = rect.Height - spike.Height + rect.Y;
			sprite->Draw(m_UIskin->WhitePixelTexture, spike, CV_Black);
		}
	}
	void DIP1::GenerateHistogramInformation(Texture* texture, HistogramData& result)
	{
		DataRectangle dataR = texture->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				result.PixelCount[srcData[j]]++;
				if (result.PixelCount[srcData[j]]>result.HighestPixelCount)
					result.HighestPixelCount = result.PixelCount[srcData[j]];
			}
			srcData += dataR.getPitch();
		}
		texture->Unlock(0);
		result.TotalPixelCount = dataR.getWidth() * dataR.getHeight();
	}

	void DIP1::Work11(MenuItem* ctrl)
	{
		m_frmW11->Show();
	}
	void DIP1::Work11_Process(Control* ctrl)
	{
		DataRectangle dataR1 = m_w11result1->Lock(0, LOCK_None);
		DataRectangle dataR2 = m_w11result2->Lock(0, LOCK_None);

		DataRectangle dataR = m_original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				float val = (srcData[j]) * 1.5f + 1.2f;
				val = Math::Clamp(val,0.0f,255.0f);

				dstR1[j] = (byte)(val);

				val = (srcData[j]) * 0.7f + 1.2f;
				val = Math::Clamp(val,0.0f,255.0f);

				dstR2[j] = (byte)(val);
			}
			srcData += dataR.getPitch();
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		m_original->Unlock(0);

		m_w11result1->Unlock(0);
		m_w11result2->Unlock(0);

		GenerateHistogramInformation(m_w11result1,m_w11result1HG);
		GenerateHistogramInformation(m_w11result2,m_w11result2HG);
	}
	void DIP1::Work11_HG1(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		DrawHistogramFor(sprite, *dstRect, m_originalHgram);
	}
	void DIP1::Work11_HG2(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		DrawHistogramFor(sprite, *dstRect, m_w11result1HG);
	}
	void DIP1::Work11_HG3(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		DrawHistogramFor(sprite, *dstRect, m_w11result2HG);
	}
	void DIP1::Work12(MenuItem* ctrl)
	{
		m_frmW12->Show();
	}
	void DIP1::Work12_Process(Control* ctrl)
	{
		float percentage = 0.2f;
		float percentage2 = 0.2f;

		DataRectangle dataR1 = m_w12noise->Lock(0, LOCK_None);
		DataRectangle dataR2 = m_w12noise2->Lock(0, LOCK_None);
		DataRectangle dataR = m_original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				float rnd = Randomizer::NextFloat() * 2 - 1;

				if (rnd > 1.0f-percentage)
				{
					dstR1[j] = 0xff;
				}
				else if (rnd < -1+percentage)
				{
					dstR1[j] = 0;
				}
				else
				{
					dstR1[j] = srcData[j];
				}

				rnd = Randomizer::NextFloat() * 2 - 1;

				if (rnd > 1.0f-percentage2)
				{
					dstR2[j] = 0xff;
				}
				else if (rnd < -1+percentage2)
				{
					dstR2[j] = 0;
				}
				else
				{
					dstR2[j] = srcData[j];
				}
			}
			srcData += dataR.getPitch();
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		m_original->Unlock(0);

		// ======================================================

		DataRectangle res11 = m_w12result1_1->Lock(0, LOCK_None);
		DataRectangle res21 = m_w12result2_1->Lock(0, LOCK_None);
		DataRectangle res12 = m_w12result1_2->Lock(0, LOCK_None);
		DataRectangle res22 = m_w12result2_2->Lock(0, LOCK_None);

		byte* res11ptr = (byte*)res11.getDataPointer();
		byte* res21ptr = (byte*)res21.getDataPointer();
		byte* res12ptr = (byte*)res12.getDataPointer();
		byte* res22ptr = (byte*)res22.getDataPointer();

		dstR1 = (byte*)dataR1.getDataPointer();
		dstR2 = (byte*)dataR2.getDataPointer();

		for (int i=0;i<dataR1.getHeight();i++)
		{
			for (int j=0;j<dataR1.getWidth();j++)
			{
				int total = 0;
				
				byte filter[9];
				
				for (int ii=-1;ii<=1;ii++)
					for (int jj=-1;jj<=1;jj++)
					{
						int boxX = jj + j;
						int boxY = ii + i;

						if (boxX<0) boxX = 0;
						if (boxY<0) boxY = 0;
						if (boxX>=dataR1.getWidth()) boxX = dataR1.getWidth();
						if (boxY>=dataR1.getHeight()) boxY = dataR1.getHeight();

						total += dstR1[boxY*dataR1.getPitch()+boxX];
						filter[(ii+1)*3+jj+1] = dstR1[boxY*dataR1.getPitch()+boxX];
					}
				total /= 9;

				int total2 = 0;
				byte filter2[25];
				for (int ii=-2;ii<=2;ii++)
					for (int jj=-2;jj<=2;jj++)
					{
						int boxX = jj + j;
						int boxY = ii + i;

						if (boxX<0) boxX = 0;
						if (boxY<0) boxY = 0;
						if (boxX>=dataR1.getWidth()) boxX = dataR1.getWidth();
						if (boxY>=dataR1.getHeight()) boxY = dataR1.getHeight();

						total2 += dstR2[boxY*dataR2.getPitch()+boxX];
						filter2[(ii+2)*5+jj+2] = dstR2[boxY*dataR2.getPitch()+boxX];
					}
				total2 /= 25;

				if (total>255)total = 255;
				if (total2>255)total2 = 255;
				
				res11ptr[j] = (byte)total;
				res21ptr[j] = (byte)total2;

				for (int ii=0;ii<9;ii++)
					for (int jj=ii;jj<9;jj++)
						if (filter[ii] < filter[jj])
						{
							byte temp = filter[jj];
							filter[jj] = filter[ii];
							filter[ii] = temp;
						}
				res12ptr[j] = filter[4];
				for (int ii=0;ii<25;ii++)
					for (int jj=ii;jj<25;jj++)
						if (filter2[ii] < filter2[jj])
						{
							byte temp = filter2[jj];
							filter2[jj] = filter2[ii];
							filter2[ii] = temp;
						}
				res22ptr[j] = filter2[12];
			}
			res11ptr += res11.getPitch();
			res21ptr += res21.getPitch();
			res12ptr += res12.getPitch();
			res22ptr += res22.getPitch();
		}

		m_w12result1_1->Unlock(0);
		m_w12result2_1->Unlock(0);
		m_w12result1_2->Unlock(0);
		m_w12result2_2->Unlock(0);


		m_w12noise->Unlock(0);
		m_w12noise2->Unlock(0);
	}
	void DIP1::Work13(MenuItem* ctrl)
	{
		m_frmW13->Show();
	}
	void DIP1::Work13_Process(Control* ctrl)
	{
		Matrix preTransl;
		Matrix rotation;

		Matrix postTransl;

		Matrix::CreateTranslation(preTransl, -127,-127,0);
		Matrix::CreateRotationZ(rotation, Math::PI/6.0f);

		Matrix::CreateTranslation(postTransl, 127,127,0);

		Matrix temp, temp2;
		Matrix::Multiply(temp, preTransl, rotation);
		Matrix::Multiply(temp2, temp, postTransl);
		//Matrix::Inverse(temp, temp2);

		DataRectangle dataR1 = m_w13result->Lock(0, LOCK_None);

		DataRectangle dataR = m_original256->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		for (int i=0;i<dataR1.getHeight();i++)
		{
			for (int j=0;j<dataR1.getWidth();j++)
			{
				Vector3 p = Vector3((float)j,(float)i,0);

				p = Vector3::TransformCoordinate(p, temp2);

				int sx = (int)p.X;
				int sy = (int)p.Y;


				if (sx <0 || sy <0 || sx >= dataR.getWidth() || sy >= dataR.getHeight())
				{
					dstR1[j] = 0;
				}
				else
				{
					dstR1[j] = (byte)(srcData[sy * dataR.getPitch() + sx]);
				}
			}
			dstR1 += dataR1.getPitch();
		}
		m_original256->Unlock(0);

		m_w13result->Unlock(0);
	}
	void DIP1::Work21(MenuItem* ctrl)
	{
		m_frmW21->Show();
	}
	void DIP1::Work21_Process(Control* ctrl)
	{
		Work21_SubProcess(m_original512, m_w21result1_1, m_w21result1_2, m_w21result1_3,
			StringUtils::ParseInt32(m_w21Thres1_1->Text), StringUtils::ParseInt32(m_w21Thres1_2->Text), StringUtils::ParseInt32(m_w21Thres1_3->Text));
		Work21_SubProcess(m_originalAlt, m_w21result2_1, m_w21result2_2, m_w21result2_3,
			StringUtils::ParseInt32(m_w21Thres2_1->Text), StringUtils::ParseInt32(m_w21Thres2_2->Text), StringUtils::ParseInt32(m_w21Thres2_3->Text));

	}
	void DIP1::Work21_SubProcess(Texture* original, Texture* resultTex1, Texture* resultTex2, Texture* resultTex3,
		int thres1, int thres2, int thres3)
	{
		int threshold1 = thres1;
		int threshold2 = thres2;
		int threshold3 = thres3;

		DataRectangle dataR1 = resultTex1->Lock(0, LOCK_None);
		DataRectangle dataR2 = resultTex2->Lock(0, LOCK_None);
		DataRectangle dataR = original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		int* lapDataBuffer = new int[dataR.getHeight() * dataR.getWidth()];
		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				// ==== roberts =====
				int sy = abs(i-1) % dataR.getHeight();
				int sx = (j+1) % dataR.getWidth();
				int gradX = srcData[sy * dataR.getPitch() + sx] - srcData[i*dataR.getPitch()+j];
				int gradY = srcData[sy * dataR.getPitch() + j] - srcData[i*dataR.getPitch()+sx];

				float grad = sqrtf((float)gradX*gradX + gradY*gradY);
				dstR1[j] = grad > threshold1 ? 0xff : 0;

				// ==== sobel ====
				byte buffer[9];
				for (int ii=-1;ii<=1;ii++)
					for (int jj=-1;jj<=1;jj++)
					{
						int srcX = abs(j+jj) % dataR.getWidth();
						int srcY = abs(i+ii) % dataR.getHeight();

						buffer[(ii+1)*3+jj+1] = srcData[srcY*dataR.getPitch()+srcX];
					}

				int sobelX = buffer[0] + 2*buffer[3] + buffer[6] - buffer[2] - 2*buffer[3+2]-buffer[6+2];
				int sobelY = buffer[0] + 2*buffer[1] + buffer[2] - buffer[6] - 2*buffer[6+1]-buffer[6+2];
				grad = sqrtf((float)sobelX*sobelX + sobelY*sobelY);

				dstR2[j] = grad > threshold2 ? 0xff : 0;

				// ==== Laplacian =====
				int g = 
					buffer[0] +
					buffer[1] +
					buffer[2] +

					buffer[3] +
					buffer[5] +

					buffer[6] +
					buffer[7] +
					buffer[8] -

					8 * buffer[4];

				lapDataBuffer[i * dataR.getWidth() + j] = g;
			}
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		resultTex1->Unlock(0);
		resultTex2->Unlock(0);
		original->Unlock(0);

		// 找到零交叉点
		DataRectangle dataR3 = resultTex3->Lock(0, LOCK_None);
		byte* dstR3 = (byte*)dataR3.getDataPointer();
		for (int i=0;i<dataR3.getHeight();i++)
		{
			for (int j=0;j<dataR3.getWidth();j++)
			{
				int center = lapDataBuffer[i * dataR3.getWidth()+j];
				int bottom = lapDataBuffer[((i+1)%dataR3.getHeight())*dataR3.getWidth()+j];
				int top = lapDataBuffer[(abs(i-1)%dataR3.getHeight())*dataR3.getWidth()+j];
				int right = lapDataBuffer[i*dataR3.getWidth()+(j+1)%dataR3.getWidth()];
				int left = lapDataBuffer[i*dataR3.getWidth()+abs(j-1)%dataR3.getWidth()];

				if (center < 0 && bottom > 0 &&
					abs(center-bottom) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && top > 0 &&
					abs(center-top) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && right > 0 && 
					abs(center-right) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && left > 0 &&
					abs(center-left) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center == 0)
				{
					if (bottom>0 && top<0 && 
						abs(top-bottom)>2*threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (bottom < 0 && top > 0 &&
						abs(top-bottom)>2*threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right>0 && left<0 &&
						abs(right-left) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right<0 && left>0 &&
						abs(right-left) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
				}
				else
				{
					dstR3[j] = 0;
				}
			}
			dstR3 += dataR3.getPitch();
		}

		resultTex3->Unlock(0);
	}

	void DIP1::Work22(MenuItem* ctrl)
	{
		m_frmW22->Show();
	}
	void DIP1::Work22_Process(Control* ctrl)
	{
		Texture* inputTex = m_w22OriginalImage->getImage();
		float percentageL = StringUtils::ParseSingle(m_w22Thres1_1->Text)*0.01f * inputTex->getWidth() * 0.5f;
		float percentageH = StringUtils::ParseSingle(m_w22Thres2_1->Text)*0.01f * inputTex->getWidth() * 0.5f;

		IPF ilpf(false, inputTex->getWidth(), inputTex->getHeight(), 
			percentageL);
		IPF ihpf(true, inputTex->getWidth(), inputTex->getHeight(), 
			percentageH);
		FPF flpf(false, inputTex->getWidth(), inputTex->getHeight(), 
			percentageL);
		FPF fhpf(true, inputTex->getWidth(), inputTex->getHeight(), 
			percentageH);

		int pixels = inputTex->getWidth() * inputTex->getHeight();
		Complex* freqDom = new Complex[pixels];
		float* grayValues = new float[pixels];


		DataRectangle dataR = inputTex->Lock(0,LOCK_None);

		const byte* srcPtr = (const byte*)dataR.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				grayValues[i*inputTex->getWidth()+j] = (float)srcPtr[j];
			}
			srcPtr+= dataR.getPitch();
		}

		inputTex->Unlock(0);

		fft2(grayValues, freqDom, inputTex->getWidth(), inputTex->getHeight(), true);

		delete[] grayValues;


		DataRectangle dataR1 = m_w22resultAmp->Lock(0,LOCK_None);
		DataRectangle dataR2 = m_w22resultPhase->Lock(0,LOCK_None);

		byte* destPtr = (byte*)dataR1.getDataPointer();
		byte* destPtr2 = (byte*)dataR2.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Mod()*0.01f,0.0f,255.0f);
				destPtr2[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Angle()/(Math::PI*2)*255,0.0f,255.0f);
			}
			destPtr += dataR1.getPitch();
			destPtr2 += dataR2.getPitch();
		}

		m_w22resultAmp->Unlock(0);
		m_w22resultPhase->Unlock(0);




		Work22_Subprocess(freqDom, ilpf, m_w22result1_1);

		Work22_Subprocess(freqDom, ihpf, m_w22result1_2);

		Work22_Subprocess(freqDom, flpf, m_w22result2_1);

		Work22_Subprocess(freqDom, fhpf, m_w22result2_2);



		delete[] freqDom;

	}
	void DIP1::Work22_Subprocess(const Complex* source, const FreqDomainFilter& filter, Texture* resultTex)
	{
		Texture* inputTex = m_w22OriginalImage->getImage();

		int pixels = inputTex->getWidth() * inputTex->getHeight();
		Complex* temp = new Complex[pixels];
		float* result = new float[pixels];
		memcpy(temp, source, sizeof(Complex) * pixels);


		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				float weight = filter.GetWeight(j,i);

				Complex cwei(weight,weight);

				Complex& c = temp[i*inputTex->getHeight()+j];
				
				c=cwei*c;
				//c = cwei;
			}
		}

		ifft2(temp, result, inputTex->getWidth(), inputTex->getHeight(), true);
		

		DataRectangle dataR = resultTex->Lock(0,LOCK_None);

		byte* destPtr = (byte*)dataR.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				destPtr[j] = (byte)Math::Clamp(result[i*inputTex->getWidth()+j],0.0f,255.0f);
			}
			destPtr += dataR.getPitch();
		}

		resultTex->Unlock(0);
		delete[] temp;
		delete[] result;
	}
	void DIP1::Work22_SelImage(Control* ctrl)
	{
		switch (m_w22ImageSel->getSelectedIndex())
		{
		case 0:
			m_w22OriginalImage->setImage(m_original256);
			break;
		case 1:
			m_w22OriginalImage->setImage(m_originalAlt256);
			break;
		case 2:
			m_w22OriginalImage->setImage(m_originalAperture256);
			break;
		}
	}
	void DIP1::Work23(MenuItem* ctrl)
	{
		m_frmW23->Show();
	}
	void DIP1::Work23_Process(Control* ctrl)
	{
		Texture* source = m_w23OriginalImage->getImage();

		DataRectangle dataR = source->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();

		
		String* strCode = new String[256];
		float* hist = new float[256];
		float* feq = new float[256];
		int* hMap = new int[256];
		float entr = 0;
		float codeLeng = 0;

		for (int i=0;i<256;i++)
		{
			hist[i] = 0;
			hMap[i] = i;
		}

		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				byte grayValue = srcData[i*dataR.getPitch()+j];
				hist[grayValue]++;
			}
		}

		for (int i=0;i<256;i++)
		{
			hist[i] /= (dataR.getWidth()*dataR.getHeight());
			feq[i] = hist[i];
		}

		for (int i=0;i<255;i++)
		{
			for (int j=0;j<255-i;j++)
			{
				if (hist[j] > hist[j+1])
				{
					float temp = hist[j];
					hist[j] = hist[j+1];
					hist[j+1] = temp;

					for (int k=0;k<256;k++)
					{
						if (hMap[k] == j)
							hMap[k] = j+1;
						else if (hMap[k]==j+1)
							hMap[k] = j;
					}
				}
			}
		}
		

		for (int i=0;i<255;i++)
		{
			if (hist[i] == 0)
				continue;

			for (int j=1;j<255;j++)
			{
				for (int k=0;k<256;k++)
				{
					if (hMap[k] == j)
					{
						strCode[k] = L"1" + strCode[k];
					}
					else if (hMap[k] == j+1)
					{
						strCode[k] = L"0" + strCode[k];
					}
				}

				hist[j+1] += hist[j];

				for (int k=0;k<256;k++)
				{
					if (hMap[k] == j)
						hMap[k] = j+1;
				}

				for (int m=j+1;m<255;m++)
				{
					if (hist[m] > hist[m+1])
					{
						float temp = hist[m];
						hist[m] = hist[m+1];
						hist[m+1] = temp;

						for (int k=0;k<256;k++)
						{
							if (hMap[k] == m)
								hMap[k] = m+1;
							else if (hMap[k] == m+1)
								hMap[k] = m;
						}
					}
					else break;
				}
			}

			break;
		}

		m_w23result->getItems().Clear();
		for (int i=0;i<256;i++)
		{
			String line[4];
			line[0] = StringUtils::IntToString(i);
			line[1] = StringUtils::SingleToString(feq[i], 8);
			line[2] = strCode[i];
			if (line[2].empty())
			{
				line[3] = L"0";
			}
			else
			{
				line[3] = StringUtils::UIntToString(strCode[i].length());

				codeLeng += feq[i] * strCode[i].length();

				if (feq[i]>0)
				{
					entr -= feq[i] * logf(feq[i]) / logf(2.0f);
				}
				
			}
			m_w23result->getItems().AddRow(line);
		}
		source->Unlock(0);

		m_w23comRate->Text = L"Image Entropy: " + StringUtils::SingleToString(entr, 3) +
			L"\nCompression Ratio: " + StringUtils::SingleToString(codeLeng/8.0f, 4);

		delete[] hist;
		delete[] feq;
		delete[] hMap;
	}
	void DIP1::Work23_SelImage(Control* ctrl)
	{
		switch (m_w23ImageSel->getSelectedIndex())
		{
		case 0:
			m_w23OriginalImage->setImage(m_original);
			break;
		case 1:
			m_w23OriginalImage->setImage(m_originalGrad);
			break;
		}
	}

	void DIP1::WorkDM(MenuItem* ctrl)
	{
		m_frmDM->Show();
	}
	void DIP1::WorkDM_Process(Control* ctrl)
	{

		for (int i=0;i<m_resultDisplays.getCount();i++)
		{
			m_frmDM->getControls().Remove(m_resultDisplays[i]);
			delete m_resultDisplays[i];
		}
		m_resultDisplays.Clear();

		double* dmSources[DMCount];
		for (int k=0;k<DMCount;k++)
		{
			Texture* inputTex = m_dmOriginals[k];
			Texture* outputTex = m_dmSpecturm[k];

			int pixels = inputTex->getWidth() * inputTex->getHeight();
			Complex* freqDom = new Complex[pixels];
			float* grayValues = new float[pixels];


			DataRectangle dataR = inputTex->Lock(0,LOCK_None);

			const byte* srcPtr = (const byte*)dataR.getDataPointer();

			for (int i=0;i<inputTex->getHeight();i++)
			{
				for (int j=0;j<inputTex->getWidth();j++)
				{
					grayValues[i*inputTex->getWidth()+j] = (float)srcPtr[j];
				}
				srcPtr+= dataR.getPitch();
			}

			inputTex->Unlock(0);

			fft2(grayValues, freqDom, inputTex->getWidth(), inputTex->getHeight(), true);

			delete[] grayValues;

			dmSources[k] = new double[inputTex->getHeight()*inputTex->getWidth()];// freqDom;


			dataR = outputTex->Lock(0,LOCK_None);

			byte* destPtr = (byte*)dataR.getDataPointer();

			for (int i=0;i<outputTex->getHeight();i++)
			{
				for (int j=0;j<outputTex->getWidth();j++)
				{
					destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Mod()*0.01f,0.0f,255.0f);
					dmSources[k][i*inputTex->getWidth()+j] = freqDom[i*inputTex->getWidth()+j].Mod();
				}
				destPtr += dataR.getPitch();
			}

			outputTex->Unlock(0);
		}

		//double** data = new double*[DMCount];
		SignatureVector** vectors = new SignatureVector*[DMCount];
		for (int k=0;k<DMCount;k++)
		{
			//data[k] = new double[512];
			double* sigData = new double[512];
			
			//double* temp = new double[512];
			bool ret = Matrix_EigenValue(dmSources[k],256,2000,0.1,sigData);

			assert(ret);

			vectors[k] = new SignatureVector(sigData, 512);

			//delete[] temp;
		}


		FastList<int> icoords(DMCount);
		for (int i=0;i<DMCount;i++)
		{
			if (m_dmInitCluster[i]->getValue())
			{
				icoords.Add(i);
			}
		}

		if (icoords.getCount()>0)
		{
			KMeans km(vectors, DMCount, icoords);
			km.Start();

			const Cluster* const* cluster = km.getClusters();
			int sx = 5;
			for (int i=0;i<icoords.getCount();i++)
			{
				const Cluster* clus = cluster[i];
				for (int j=0;j<clus->CurrentMemberShip.getCount();j++)
				{
					int imgIdx = clus->CurrentMemberShip[j];
					PictureBox* pb = new PictureBox(Point(5 + sx,60+256+70), 1);
					pb->SetSkin(m_UIskin);
					pb->Size = Point(128,128);
					pb->setImage(m_dmOriginals[imgIdx]);
					pb->Initialize(m_device);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx+=133;
				}
				if (i != icoords.getCount()-1)
				{
					PictureBox* pb = new PictureBox(Point(5 + sx,60+256+70), 1);
					pb->SetSkin(m_UIskin);
					pb->Size = Point(32,128);
					pb->Initialize(m_device);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx+=37;
				}
			}
		}
		
		for (int k=0;k<DMCount;k++)
		{
			delete[] dmSources[k];
			delete vectors[k];
		}
		delete[] vectors;
	}

	void DIP1::OnFrameStart() { }
	void DIP1::OnFrameEnd() { }
	void DIP1::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();
		Game::Draw(time);

		m_device->EndFrame();

	}
	void DIP1::Initialize()
	{
		m_window->setTitle(L"DIP Demos");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
		//m_audioEngine = new Sounds(this);
	}
	void DIP1::Finalize()
	{
		Game::Finalize();
	}
	void DIP1::Load()
	{
		Game::Load();

		{
			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"dipworks.pak");
			rule.AddCheckPoint(cp);

			FileLocation fl = FileSystem::getSingleton().Locate(L"image1.tex", rule);
			m_original = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image256.tex", rule);
			m_colorPhoto = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image256_gray.tex", rule);
			m_original256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image512_gray.tex", rule);
			m_original512 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image2.tex", rule);
			m_originalAlt = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image2_256l8.tex", rule);
			m_originalAlt256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image3_256l8.tex", rule);
			m_originalAperture256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"grad.tex", rule);
			m_originalGrad = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);


			ObjectFactory* fac = m_device->getObjectFactory();
			m_w11result1 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w11result2 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			
			m_w12noise = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w12noise2 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w12result1_1 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w12result1_2 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w12result2_1 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w12result2_2 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_w13result = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			
			m_w21result1_1 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result1_2 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result1_3 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_1 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_2 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_3 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_w22result1_1 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result2_1 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result1_2 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result2_2 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_w22resultAmp = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22resultPhase = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
		}

		{
			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"dmining.pak");
			rule.AddCheckPoint(cp);

			ObjectFactory* fac = m_device->getObjectFactory();
			{
				
				FileLocation fl = FileSystem::getSingleton().Locate(L"a4.tex", rule);
				m_dmOriginals[0] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a3.tex", rule);
				m_dmOriginals[1] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a2.tex", rule);
				m_dmOriginals[2] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a1.tex", rule);
				m_dmOriginals[3] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b1.tex", rule);
				m_dmOriginals[4] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b2.tex", rule);
				m_dmOriginals[5] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"ba_1.tex", FileLocateRule::Default);// rule);
				m_dmOriginals[6] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b4.tex", rule);
				m_dmOriginals[7] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			}
			//{
			//	FileLocation* fl = FileSystem::getSingleton().Locate(L"a4.tex", rule);
			//	m_dmOriginals[0] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);


			//	fl = FileSystem::getSingleton().Locate(L"b4.tex", rule);
			//	m_dmOriginals[7] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);


			//	{
			//		DataRectangle dataR1 = m_dmOriginals[7]->Lock(0,LOCK_None);
			//		DataRectangle dataR2 = m_dmOriginals[0]->Lock(0,LOCK_None);

			//		//int rndShift=0;

			//		for (int k=1;k<7;k++)
			//		{
			//			const byte* srcPtr1 = (const byte*)dataR1.getDataPointer();
			//			const byte* srcPtr2 = (const byte*)dataR2.getDataPointer();
			//			m_dmOriginals[k] = fac->CreateTexture(m_dmOriginals[0]->getWidth(), m_dmOriginals[0]->getHeight(),1, TU_Static, FMT_Luminance8); 

			//			DataRectangle dataR = m_dmOriginals[k]->Lock(0,LOCK_None);
			//			byte* dstPtr = (byte*)dataR.getDataPointer();

			//			for (int i=0;i<m_dmOriginals[0]->getHeight();i++)
			//			{
			//				for (int j=0;j<m_dmOriginals[0]->getWidth();j++)
			//				{
			//					dstPtr[j] = (byte)Math::Lerp(srcPtr1[j], srcPtr2[j], 1-(k/8.0f));
			//				}
			//				srcPtr1+= dataR1.getPitch();
			//				srcPtr2+= dataR2.getPitch();
			//				dstPtr += dataR.getPitch();
			//			}

			//			m_dmOriginals[k]->Unlock(0);
			//		}

			//		m_dmOriginals[0]->Unlock(0);
			//		m_dmOriginals[7]->Unlock(0);

			//	}
			//if (Randomizer::Next()%2)
			//{
			//	swap(m_dmOriginals[2], m_dmOriginals[6]);
			//}
			//if (Randomizer::Next()%2)
			//{
			//	swap(m_dmOriginals[3], m_dmOriginals[5]);
			//}
			//if (Randomizer::Next()%2)
			//{
			//	swap(m_dmOriginals[1], m_dmOriginals[5]);
			//}
			//}
			

			for (int i=0;i<DMCount;i++)
			{
				m_dmSpecturm[i] = fac->CreateTexture(m_dmOriginals[i]->getWidth(), m_dmOriginals[i]->getHeight(),1, TU_Static, FMT_Luminance8); 
			}

		}

		m_console->Minimize();


		m_mainMenu = new Menu();
		m_mainMenu->SetSkin(m_UIskin);


		{
			MenuItem* pojMenu = new MenuItem(L"Demos");
			SubMenu* pojSubMenu = new SubMenu(0);
			pojSubMenu->SetSkin(m_UIskin);

			MenuItem* mi=new MenuItem(L"Histogram Processing");
			mi->event().Bind(this, &DIP1::Work11);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Noise Generation and Reduction");
			mi->event().Bind(this, &DIP1::Work12);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Transformation");
			mi->event().Bind(this, &DIP1::Work13);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"-");
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Edge Filters");
			mi->event().Bind(this, &DIP1::Work21);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Freq Domain Filters");
			mi->event().Bind(this, &DIP1::Work22);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Huffman");
			mi->event().Bind(this, &DIP1::Work23);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Clustering");
			mi->event().Bind(this, &DIP1::WorkDM);
			pojSubMenu->Add(mi,0);


			m_mainMenu->Add(pojMenu,pojSubMenu);
		}

		{
			MenuItem* aboutMenu = new MenuItem(L"Help");
			
			SubMenu* aboutSubMenu = new SubMenu(0);
			aboutSubMenu->SetSkin(m_UIskin);

			MenuItem* mi=new MenuItem(L"About...");
			mi->event().Bind(this, &DIP1::ShowAbout);
			aboutSubMenu->Add(mi,0);

			m_mainMenu->Add(aboutMenu,aboutSubMenu);
		}


		m_mainMenu->Initialize(m_device);
		UIRoot::setMainMenu(m_mainMenu);

		// Histogram Processing
		{
			m_frmW11 = new Form(FBS_Fixed, L"Histogram Processing");
			m_frmW11->SetSkin(m_UIskin);
			m_frmW11->Size.X = 256*3+5*4;
			m_frmW11->Size.Y = 500;
			m_frmW11->Position.Y = 100;


			int sy = 5 + m_UIskin->FormTitle->Height;
			PictureBox* m_pbW11Original = new PictureBox(Point(5,sy), 1);
			m_pbW11Original->SetSkin(m_UIskin);
			m_pbW11Original->Size = Point(256,256);
			m_pbW11Original->setImage(m_original);
			m_frmW11->getControls().Add(m_pbW11Original);

			PictureBox* m_pbW11Result1 = new PictureBox(Point(5+256+5,sy), 1);
			m_pbW11Result1->SetSkin(m_UIskin);
			m_pbW11Result1->Size = Point(256,256);
			m_pbW11Result1->setImage(m_w11result1);
			m_frmW11->getControls().Add(m_pbW11Result1);

			PictureBox* m_pbW11Result2 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			m_pbW11Result2->SetSkin(m_UIskin);
			m_pbW11Result2->Size = Point(256,256);
			m_pbW11Result2->setImage(m_w11result2);
			m_frmW11->getControls().Add(m_pbW11Result2);


			Button* m_btnProcess = new Button(Point(12,460),100, L"Process");
			m_btnProcess->SetSkin(m_UIskin);
			m_btnProcess->eventRelease().Bind(this, &DIP1::Work11_Process);
			m_frmW11->getControls().Add(m_btnProcess);

			sy = 283;
			Label* lbl = new Label(Point(5, sy), L"Original Image", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"(1.5, 1.2)", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"(0.7, 1.2)", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);

			sy = 310;

			PictureBox* m_pbW11OriginalHG = new PictureBox(Point(5,sy), 0);
			m_pbW11OriginalHG->SetSkin(m_UIskin);
			m_pbW11OriginalHG->Size = Point(256,128);
			m_pbW11OriginalHG->eventPictureDraw().Bind(this, &DIP1::Work11_HG1);
			m_frmW11->getControls().Add(m_pbW11OriginalHG);

			PictureBox* m_pbW11Result1HG = new PictureBox(Point(5+256+5,sy), 0);
			m_pbW11Result1HG->SetSkin(m_UIskin);
			m_pbW11Result1HG->Size = Point(256,128);
			m_pbW11Result1HG->eventPictureDraw().Bind(this, &DIP1::Work11_HG2);
			m_frmW11->getControls().Add(m_pbW11Result1HG);

			PictureBox* m_pbW11Result2HG = new PictureBox(Point(5+(256+5)*2,sy), 0);
			m_pbW11Result2HG->SetSkin(m_UIskin);
			m_pbW11Result2HG->Size = Point(256,128);
			m_pbW11Result2HG->eventPictureDraw().Bind(this, &DIP1::Work11_HG3);
			m_frmW11->getControls().Add(m_pbW11Result2HG);

			sy = 440;
			lbl = new Label(Point(5, sy), L"Histogram", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"Histogram", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Histogram", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW11->getControls().Add(lbl);


			m_frmW11->Initialize(m_device);
			UIRoot::Add(m_frmW11);
		}
		// Noise Generation and Reduction
		{
			m_frmW12 = new Form(FBS_Fixed, L"Noise Generation and Reduction");
			m_frmW12->SetSkin(m_UIskin);
			m_frmW12->Size.X = 256*4+5*5;
			m_frmW12->Size.Y = 720;
			m_frmW12->Position.Y = 100;


			Button* m_btnProcess = new Button(Point(12,460-128),100, L"Process");
			m_btnProcess->SetSkin(m_UIskin);
			m_btnProcess->eventRelease().Bind(this, &DIP1::Work12_Process);
			m_frmW12->getControls().Add(m_btnProcess);


			int sy = 5 + m_UIskin->FormTitle->Height;
			PictureBox* pbOriginal = new PictureBox(Point(5,sy), 1);
			pbOriginal->SetSkin(m_UIskin);
			pbOriginal->Size = Point(256,256);
			pbOriginal->setImage(m_original);
			m_frmW12->getControls().Add(pbOriginal);

			PictureBox* pbNoise = new PictureBox(Point(5+256+5,sy), 1);
			pbNoise->SetSkin(m_UIskin);
			pbNoise->Size = Point(256,256);
			pbNoise->setImage(m_w12noise);
			m_frmW12->getControls().Add(pbNoise);

			PictureBox* pbAF1 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbAF1->SetSkin(m_UIskin);
			pbAF1->Size = Point(256,256);
			pbAF1->setImage(m_w12result1_1);
			m_frmW12->getControls().Add(pbAF1);

			PictureBox* pbMF1 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbMF1->SetSkin(m_UIskin);
			pbMF1->Size = Point(256,256);
			pbMF1->setImage(m_w12result1_2);
			m_frmW12->getControls().Add(pbMF1);


			sy += 260;
			Label* lbl = new Label(Point(5, sy), L"Original Image", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"Salt & Pepper Noise[20%]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);


			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Averaging Filter[3x3]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Median Filter[3x3]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);


			sy += 25;
			PictureBox* pbNoise2 = new PictureBox(Point(5+256+5,sy), 1);
			pbNoise2->SetSkin(m_UIskin);
			pbNoise2->Size = Point(256,256);
			pbNoise2->setImage(m_w12noise2);
			m_frmW12->getControls().Add(pbNoise2);

			PictureBox* pbAF2 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbAF2->SetSkin(m_UIskin);
			pbAF2->Size = Point(256,256);
			pbAF2->setImage(m_w12result2_1);
			m_frmW12->getControls().Add(pbAF2);

			PictureBox* pbMF2 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbMF2->SetSkin(m_UIskin);
			pbMF2->Size = Point(256,256);
			pbMF2->setImage(m_w12result2_2);
			m_frmW12->getControls().Add(pbMF2);


			sy+=256;
			lbl = new Label(Point(5 + 256 + 5, sy), L"Salt & Pepper Noise[20%]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Averaging Filter[5x5]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Median Filter[5x5]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW12->getControls().Add(lbl);




			m_frmW12->Initialize(m_device);
			UIRoot::Add(m_frmW11);
		}
		// Transformation
		{
			m_frmW13 = new Form(FBS_Fixed, L"Transformation");
			m_frmW13->SetSkin(m_UIskin);
			m_frmW13->Size.X = 256*2+5*3;
			m_frmW13->Size.Y = 500-128;
			m_frmW13->Position.Y = 100;

			int sy = 5 + m_UIskin->FormTitle->Height;
			PictureBox* pbOriginal = new PictureBox(Point(5,sy), 1);
			pbOriginal->SetSkin(m_UIskin);
			pbOriginal->Size = Point(256,256);
			pbOriginal->setImage(m_original);
			m_frmW13->getControls().Add(pbOriginal);

			PictureBox* pbResult = new PictureBox(Point(5+256+5,sy), 1);
			pbResult->SetSkin(m_UIskin);
			pbResult->Size = Point(256,256);
			pbResult->setImage(m_w13result);
			m_frmW13->getControls().Add(pbResult);

			Button* btnProcess = new Button(Point(12,460-128),100, L"Process");
			btnProcess->SetSkin(m_UIskin);
			btnProcess->eventRelease().Bind(this, &DIP1::Work13_Process);
			m_frmW13->getControls().Add(btnProcess);

			sy += 260;
			Label* lbl = new Label(Point(5, sy), L"Original Image[256x256]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW13->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"Rotated", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW13->getControls().Add(lbl);


			m_frmW13->Initialize(m_device);
			UIRoot::Add(m_frmW13);
		}

		// Edge Filter
		{
			m_frmW21 = new Form(FBS_Fixed, L"Edge Filter");
			m_frmW21->SetSkin(m_UIskin);
			m_frmW21->Size.X = 256*4+5*5;
			m_frmW21->Size.Y = 700;
			m_frmW21->Position.Y = 100;


			int sy = 5 + m_UIskin->FormTitle->Height;

			PictureBox* pbOriginal = new PictureBox(Point(5,sy), 1);
			pbOriginal->SetSkin(m_UIskin);
			pbOriginal->Size = Point(256,256);
			pbOriginal->setImage(m_original512);
			m_frmW21->getControls().Add(pbOriginal);

			PictureBox* pbResult = new PictureBox(Point(5+256+5,sy), 1);
			pbResult->SetSkin(m_UIskin);
			pbResult->Size = Point(256,256);
			pbResult->setImage(m_w21result1_1);
			m_frmW21->getControls().Add(pbResult);

			PictureBox* pbResult2 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbResult2->SetSkin(m_UIskin);
			pbResult2->Size = Point(256,256);
			pbResult2->setImage(m_w21result1_2);
			m_frmW21->getControls().Add(pbResult2);

			PictureBox* pbResult3 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbResult3->SetSkin(m_UIskin);
			pbResult3->Size = Point(256,256);
			pbResult3->setImage(m_w21result1_3);
			m_frmW21->getControls().Add(pbResult3);


			Button* btnProcess = new Button(Point(12,620),100, L"Process");
			btnProcess->SetSkin(m_UIskin);
			btnProcess->eventRelease().Bind(this, &DIP1::Work21_Process);
			m_frmW21->getControls().Add(btnProcess);

			sy += 260;
			Label* lbl = new Label(Point(5, sy), L"Original Image[512x512]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"Roberts", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Sobel", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Laplacian", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			sy += 25;

			lbl = new Label(Point(5 + 256 + 5, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			m_w21Thres1_1 = new TextBox(Point(5 + 256 + 5 + 156, sy), 100, L"15");
			m_w21Thres1_1->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres1_1);

			m_w21Thres1_2 = new TextBox(Point(5 + (256 + 5)*2 + 156, sy), 100, L"15");
			m_w21Thres1_2->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres1_2);

			m_w21Thres1_3 = new TextBox(Point(5 + (256 + 5)*3 + 156, sy), 100, L"40");
			m_w21Thres1_3->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres1_3);


			sy += 25;

			PictureBox* pbOriginal2 = new PictureBox(Point(5,sy), 1);
			pbOriginal2->SetSkin(m_UIskin);
			pbOriginal2->Size = Point(256,256);
			pbOriginal2->setImage(m_originalAlt);
			m_frmW21->getControls().Add(pbOriginal2);

			PictureBox* pbResult2_1 = new PictureBox(Point(5+256+5,sy), 1);
			pbResult2_1->SetSkin(m_UIskin);
			pbResult2_1->Size = Point(256,256);
			pbResult2_1->setImage(m_w21result2_1);
			m_frmW21->getControls().Add(pbResult2_1);

			PictureBox* pbResult2_2 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbResult2_2->SetSkin(m_UIskin);
			pbResult2_2->Size = Point(256,256);
			pbResult2_2->setImage(m_w21result2_2);
			m_frmW21->getControls().Add(pbResult2_2);

			PictureBox* pbResult2_3 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbResult2_3->SetSkin(m_UIskin);
			pbResult2_3->Size = Point(256,256);
			pbResult2_3->setImage(m_w21result2_3);
			m_frmW21->getControls().Add(pbResult2_3);


			sy+=256;
			lbl = new Label(Point(5, sy), L"Original Image[512x512]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + 256 + 5, sy), L"Roberts", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Sobel", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Laplacian", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			sy += 25;

			lbl = new Label(Point(5 + 256 + 5, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Threshold:", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(lbl);

			m_w21Thres2_1 = new TextBox(Point(5 + 256 + 5 + 156, sy), 100, L"40");
			m_w21Thres2_1->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres2_1);

			m_w21Thres2_2 = new TextBox(Point(5 + (256 + 5)*2 + 156, sy), 100, L"40");
			m_w21Thres2_2->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres2_2);

			m_w21Thres2_3 = new TextBox(Point(5 + (256 + 5)*3 + 156, sy), 100, L"120");
			m_w21Thres2_3->SetSkin(m_UIskin);
			m_frmW21->getControls().Add(m_w21Thres2_3);


			m_frmW21->Initialize(m_device);
			UIRoot::Add(m_frmW21);
		}
		// Freq Domain Filters
		{
			m_frmW22 = new Form(FBS_Fixed, L"Freq Domain Filters");
			m_frmW22->SetSkin(m_UIskin);
			m_frmW22->Size.X = 256*4+5*5;
			m_frmW22->Size.Y = 700;
			m_frmW22->Position.Y = 100;

			Button* btnProcess = new Button(Point(12,450),100, L"Process");
			btnProcess->SetSkin(m_UIskin);
			btnProcess->eventRelease().Bind(this, &DIP1::Work22_Process);
			m_frmW22->getControls().Add(btnProcess);

			List<String> items; items.Add(L"Image1"); items.Add(L"Image2"); items.Add(L"Image3");
			m_w22ImageSel = new ComboBox(Point(12, 480), 100, items);
			m_w22ImageSel->SetSkin(m_UIskin);
			m_w22ImageSel->eventSelectionChanged().Bind(this, &DIP1::Work22_SelImage);
			m_frmW22->getControls().Add(m_w22ImageSel);

			int sy = 5 + m_UIskin->FormTitle->Height;

			PictureBox* pbOriginal = new PictureBox(Point(5,sy), 1);
			pbOriginal->SetSkin(m_UIskin);
			pbOriginal->Size = Point(256,256);
			pbOriginal->setImage(m_original256);
			m_frmW22->getControls().Add(pbOriginal);
			m_w22OriginalImage = pbOriginal;

			{
				PictureBox* pbAmp = new PictureBox(Point(5+(256+5)*1,sy), 1);
				pbAmp->SetSkin(m_UIskin);
				pbAmp->Size = Point(256,256);
				pbAmp->setImage(m_w22resultAmp);
				m_frmW22->getControls().Add(pbAmp);

				PictureBox* pbPhase = new PictureBox(Point(5+(256+5)*1,sy+260+25), 1);
				pbPhase->SetSkin(m_UIskin);
				pbPhase->Size = Point(256,256);
				pbPhase->setImage(m_w22resultPhase);
				m_frmW22->getControls().Add(pbPhase);
			}

			PictureBox* pbResult = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbResult->SetSkin(m_UIskin);
			pbResult->Size = Point(256,256);
			pbResult->setImage(m_w22result1_1);
			m_frmW22->getControls().Add(pbResult);

			PictureBox* pbResult2 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbResult2->SetSkin(m_UIskin);
			pbResult2->Size = Point(256,256);
			pbResult2->setImage(m_w22result1_2);
			m_frmW22->getControls().Add(pbResult2);



			sy += 260;
			Label* lbl = new Label(Point(5, sy), L"Original Image[256x256]", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"ILPF", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"IHPF", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);


			sy += 25;
			PictureBox* pbResult1_2 = new PictureBox(Point(5+(256+5)*2,sy), 1);
			pbResult1_2->SetSkin(m_UIskin);
			pbResult1_2->Size = Point(256,256);
			pbResult1_2->setImage(m_w22result2_1);
			m_frmW22->getControls().Add(pbResult1_2);

			PictureBox* pbResult2_2 = new PictureBox(Point(5+(256+5)*3,sy), 1);
			pbResult2_2->SetSkin(m_UIskin);
			pbResult2_2->Size = Point(256,256);
			pbResult2_2->setImage(m_w22result2_2);
			m_frmW22->getControls().Add(pbResult2_2);

			sy+=256;


			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"FLPF", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"FHPF", 256, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);



			sy += 25;

			lbl = new Label(Point(5 + (256 + 5)*2, sy), L"Radius(%):", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(Point(5 + (256 + 5)*3, sy), L"Radius(%):", 156, Label::ALIGN_Center);
			lbl->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(lbl);

			m_w22Thres1_1 = new TextBox(Point(5 + (256 + 5)*2 + 156, sy), 100, L"15");
			m_w22Thres1_1->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(m_w22Thres1_1);

			m_w22Thres2_1 = new TextBox(Point(5 + (256 + 5)*3 + 156, sy), 100, L"1");
			m_w22Thres2_1->SetSkin(m_UIskin);
			m_frmW22->getControls().Add(m_w22Thres2_1);


			m_frmW22->Initialize(m_device);
			UIRoot::Add(m_frmW22);
		}
		// Huffman
		{
			m_frmW23 = new Form(FBS_Fixed, L"Huffman");
			m_frmW23->SetSkin(m_UIskin);
			m_frmW23->Size.X = 256*3+5*4;
			m_frmW23->Size.Y = 500;
			m_frmW23->Position.Y = 100;

			m_w23OriginalImage = new PictureBox(Point(5,5 + m_UIskin->FormTitle->Height), 1);
			m_w23OriginalImage->SetSkin(m_UIskin);
			m_w23OriginalImage->Size = Point(256,256);
			m_w23OriginalImage->setImage(m_original);
			m_frmW23->getControls().Add(m_w23OriginalImage);


			List2D<String> items(4,1);
			m_w23result = new ListView(Point(5+256+5, 5 + m_UIskin->FormTitle->Height), Point(400, 400),items);
			m_w23result->getColumnHeader().Add(ListView::Header(L"GrayValue", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"Chance", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"HaffmanCode", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"CodeLength", 100));
			m_w23result->SetSkin(m_UIskin);
			m_frmW23->getControls().Add(m_w23result);

			Button* m_btnProcess = new Button(Point(12,370),100, L"Process");
			m_btnProcess->SetSkin(m_UIskin);
			m_btnProcess->eventRelease().Bind(this, &DIP1::Work23_Process);
			m_frmW23->getControls().Add(m_btnProcess);

			List<String> items2; items2.Add(L"Image1"); items2.Add(L"Image2");
			m_w23ImageSel = new ComboBox(Point(12, 340), 100, items2);
			m_w23ImageSel->SetSkin(m_UIskin);
			m_w23ImageSel->eventSelectionChanged().Bind(this, &DIP1::Work23_SelImage);
			m_frmW23->getControls().Add(m_w23ImageSel);

			m_w23comRate = new Label(Point(12,410), L"", 256);
			m_w23comRate->SetSkin(m_UIskin);
			m_frmW23->getControls().Add(m_w23comRate);


			m_frmW23->Initialize(m_device);
			UIRoot::Add(m_frmW23);
		}

		// Clustering
		{
			m_frmDM = new Form(FBS_Fixed, L"Clustering");
			m_frmDM->SetSkin(m_UIskin);
			m_frmDM->Size.X = 133*DMCount+5*2+128;
			m_frmDM->Size.Y = 720;
			m_frmDM->Position.Y = 100;

			Button* m_btnProcess = new Button(Point(12,27),100, L"Process");
			m_btnProcess->SetSkin(m_UIskin);
			m_btnProcess->eventRelease().Bind(this, &DIP1::WorkDM_Process);
			m_frmDM->getControls().Add(m_btnProcess);

			
			for (int i=0;i<DMCount;i++)
			{
				PictureBox* pbOriginal = new PictureBox(Point(5 + i * 133,60), 1);
				pbOriginal->SetSkin(m_UIskin);
				pbOriginal->Size = Point(128,128);
				pbOriginal->setImage(m_dmOriginals[i]);
				m_frmDM->getControls().Add(pbOriginal);
			}


			for (int i=0;i<DMCount;i++)
			{
				m_dmPicBoxSpecturm[i] = new PictureBox(Point(5 + i * 133,60+128+15), 1);
				m_dmPicBoxSpecturm[i]->SetSkin(m_UIskin);
				m_dmPicBoxSpecturm[i]->Size = Point(128,128);
				m_dmPicBoxSpecturm[i]->setImage(m_dmSpecturm[i]);
				m_frmDM->getControls().Add(m_dmPicBoxSpecturm[i]);

				m_dmInitCluster[i] = new CheckBox(Point(5 + i * 133,60+128+20+128),L"Initial Cluster", false);
				m_dmInitCluster[i]->SetSkin(m_UIskin);
				m_dmInitCluster[i]->Size = Point(128,30);
				m_frmDM->getControls().Add(m_dmInitCluster[i]);

			}


			m_frmDM->Initialize(m_device);
			UIRoot::Add(m_frmDM);
		}

		{
			m_aboutDlg = new Form(FBS_Fixed, L"About");
			m_aboutDlg->SetSkin(m_UIskin);
			m_aboutDlg->Size.X = 460;
			m_aboutDlg->Size.Y = 175;

			Label* lbl = new Label(Point(30+128,30),
				L"DIP Demos by Tao Xin. \n"
				L"Powered by Apoc3D.", 
				m_aboutDlg->Size.X-40-128, Label::ALIGN_Left);
			lbl->SetSkin(m_UIskin);

			PictureBox* photo = new PictureBox(Point(15,30),1);
			photo->Size = Point(128, 128);
			photo->SetSkin(m_UIskin);
			photo->setImage(m_colorPhoto);

			m_aboutDlg->getControls().Add(photo);
			m_aboutDlg->getControls().Add(lbl);

			m_aboutDlg->Initialize(m_device);
			UIRoot::Add(m_aboutDlg);
		}
		GenerateHistogramInformation(m_original, m_originalHgram);
	
	}

	void DIP1::Unload()
	{
		Game::Unload();

		delete m_original;
		delete m_original512;
		delete m_colorPhoto;
		delete m_original256;
		delete m_originalAlt;
		delete m_originalAlt256;
		delete m_originalAperture256;
		delete m_originalGrad;

		delete m_w11result1;		
		delete m_w11result2;

		delete m_w12noise;
		delete m_w12noise2;

		delete m_w12result1_1;
		delete m_w12result1_2;
		delete m_w12result2_1;
		delete m_w12result2_2;

		delete m_w13result;

		delete m_w21result1_1;
		delete m_w21result1_2;
		delete m_w21result1_3;
		delete m_w21result2_1;
		delete m_w21result2_2;
		delete m_w21result2_3;

		delete m_w22result1_1;
		delete m_w22result2_1;
		delete m_w22result1_2;
		delete m_w22result2_2;
		delete m_w22resultAmp;
		delete m_w22resultPhase;

		for (int i=0;i<DMCount;i++)
		{
			delete m_dmOriginals[i];
			delete m_dmSpecturm[i];
		}

		delete m_sprite;
	}

	void DIP1::Update(const GameTime* const time)
	{
		Game::Update(time);
	}

	void DIP1::ShowAbout(MenuItem* ctrl)
	{
		m_aboutDlg->ShowModal();
	}
}
