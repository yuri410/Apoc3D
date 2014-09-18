/* -----------------------------------------------------------------------------
	squish 1.11, manual amalgamation version
	original license:

	Copyright (c) 2006 Simon Brown                          si@sjbrown.co.uk

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the 
	"Software"), to	deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to 
	permit persons to whom the Software is furnished to do so, subject to 
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	-------------------------------------------------------------------------- */


#define SQUISH_USE_SSE 2

#include "squish.h"

#include <cmath>
#include <algorithm>
#include <cfloat>

// config.h
// Set to 1 when building squish to use Altivec instructions.
#ifndef SQUISH_USE_ALTIVEC
#define SQUISH_USE_ALTIVEC 0
#endif

// Set to 1 or 2 when building squish to use SSE or SSE2 instructions.
#ifndef SQUISH_USE_SSE
#define SQUISH_USE_SSE 0
#endif

// Internally et SQUISH_USE_SIMD when either Altivec or SSE is available.
#if SQUISH_USE_ALTIVEC && SQUISH_USE_SSE
#error "Cannot enable both Altivec and SSE!"
#endif
#if SQUISH_USE_ALTIVEC || SQUISH_USE_SSE
#define SQUISH_USE_SIMD 1
#else
#define SQUISH_USE_SIMD 0
#endif


#if SQUISH_USE_ALTIVEC
#include <altivec.h>
#elif SQUISH_USE_SSE
#include <xmmintrin.h>
#if ( SQUISH_USE_SSE > 1 )
#include <emmintrin.h>
#endif
#else
#endif

namespace squish 
{
	// maths.h/cpp
	class Vec3
	{
	public:
		typedef Vec3 const& Arg;

		Vec3() { }

		explicit Vec3(float s)
		{
			m_x = s;
			m_y = s;
			m_z = s;
		}

		Vec3(float x, float y, float z)
		{
			m_x = x;
			m_y = y;
			m_z = z;
		}

		float X() const { return m_x; }
		float Y() const { return m_y; }
		float Z() const { return m_z; }

		Vec3 operator-() const
		{
			return Vec3(-m_x, -m_y, -m_z);
		}

		Vec3& operator+=(Arg v)
		{
			m_x += v.m_x;
			m_y += v.m_y;
			m_z += v.m_z;
			return *this;
		}

		Vec3& operator-=(Arg v)
		{
			m_x -= v.m_x;
			m_y -= v.m_y;
			m_z -= v.m_z;
			return *this;
		}

		Vec3& operator*=(Arg v)
		{
			m_x *= v.m_x;
			m_y *= v.m_y;
			m_z *= v.m_z;
			return *this;
		}

		Vec3& operator*=(float s)
		{
			m_x *= s;
			m_y *= s;
			m_z *= s;
			return *this;
		}

		Vec3& operator/=(Arg v)
		{
			m_x /= v.m_x;
			m_y /= v.m_y;
			m_z /= v.m_z;
			return *this;
		}

		Vec3& operator/=(float s)
		{
			float t = 1.0f / s;
			m_x *= t;
			m_y *= t;
			m_z *= t;
			return *this;
		}

		friend Vec3 operator+(Arg left, Arg right)
		{
			Vec3 copy(left);
			return copy += right;
		}

		friend Vec3 operator-(Arg left, Arg right)
		{
			Vec3 copy(left);
			return copy -= right;
		}

		friend Vec3 operator*(Arg left, Arg right)
		{
			Vec3 copy(left);
			return copy *= right;
		}

		friend Vec3 operator*(Arg left, float right)
		{
			Vec3 copy(left);
			return copy *= right;
		}

		friend Vec3 operator*(float left, Arg right)
		{
			Vec3 copy(right);
			return copy *= left;
		}

		friend Vec3 operator/(Arg left, Arg right)
		{
			Vec3 copy(left);
			return copy /= right;
		}

		friend Vec3 operator/(Arg left, float right)
		{
			Vec3 copy(left);
			return copy /= right;
		}

		friend float Dot(Arg left, Arg right)
		{
			return left.m_x*right.m_x + left.m_y*right.m_y + left.m_z*right.m_z;
		}

		friend Vec3 Min(Arg left, Arg right)
		{
			return Vec3(
				std::min(left.m_x, right.m_x),
				std::min(left.m_y, right.m_y),
				std::min(left.m_z, right.m_z)
				);
		}

		friend Vec3 Max(Arg left, Arg right)
		{
			return Vec3(
				std::max(left.m_x, right.m_x),
				std::max(left.m_y, right.m_y),
				std::max(left.m_z, right.m_z)
				);
		}

		friend Vec3 Truncate(Arg v)
		{
			return Vec3(
				v.m_x > 0.0f ? std::floor(v.m_x) : std::ceil(v.m_x),
				v.m_y > 0.0f ? std::floor(v.m_y) : std::ceil(v.m_y),
				v.m_z > 0.0f ? std::floor(v.m_z) : std::ceil(v.m_z)
				);
		}

	private:
		float m_x;
		float m_y;
		float m_z;
	};

	static inline float LengthSquared(Vec3::Arg v) { return Dot(v, v); }

	class Sym3x3
	{
	public:
		Sym3x3() { }

		Sym3x3(float s)
		{
			for (int i = 0; i < 6; ++i)
				m_x[i] = s;
		}

		float operator[](int index) const { return m_x[index]; }
		float& operator[](int index) { return m_x[index]; }
	private:
		float m_x[6];
	};

	static Sym3x3 ComputeWeightedCovariance(int n, Vec3 const* points, float const* weights)
	{
		// compute the centroid
		float total = 0.0f;
		Vec3 centroid(0.0f);
		for (int i = 0; i < n; ++i)
		{
			total += weights[i];
			centroid += weights[i] * points[i];
		}
		centroid /= total;

		// accumulate the covariance matrix
		Sym3x3 covariance(0.0f);
		for (int i = 0; i < n; ++i)
		{
			Vec3 a = points[i] - centroid;
			Vec3 b = weights[i] * a;

			covariance[0] += a.X()*b.X();
			covariance[1] += a.X()*b.Y();
			covariance[2] += a.X()*b.Z();
			covariance[3] += a.Y()*b.Y();
			covariance[4] += a.Y()*b.Z();
			covariance[5] += a.Z()*b.Z();
		}

		// return it
		return covariance;
	}

	static Vec3 GetMultiplicity1Evector(Sym3x3 const& matrix, float evalue)
	{
		// compute M
		Sym3x3 m;
		m[0] = matrix[0] - evalue;
		m[1] = matrix[1];
		m[2] = matrix[2];
		m[3] = matrix[3] - evalue;
		m[4] = matrix[4];
		m[5] = matrix[5] - evalue;

		// compute U
		Sym3x3 u;
		u[0] = m[3] * m[5] - m[4] * m[4];
		u[1] = m[2] * m[4] - m[1] * m[5];
		u[2] = m[1] * m[4] - m[2] * m[3];
		u[3] = m[0] * m[5] - m[2] * m[2];
		u[4] = m[1] * m[2] - m[4] * m[0];
		u[5] = m[0] * m[3] - m[1] * m[1];

		// find the largest component
		float mc = std::fabs(u[0]);
		int mi = 0;
		for (int i = 1; i < 6; ++i)
		{
			float c = std::fabs(u[i]);
			if (c > mc)
			{
				mc = c;
				mi = i;
			}
		}

		// pick the column with this component
		switch (mi)
		{
			case 0:
				return Vec3(u[0], u[1], u[2]);

			case 1:
			case 3:
				return Vec3(u[1], u[3], u[4]);

			default:
				return Vec3(u[2], u[4], u[5]);
		}
	}

	static Vec3 GetMultiplicity2Evector(Sym3x3 const& matrix, float evalue)
	{
		// compute M
		Sym3x3 m;
		m[0] = matrix[0] - evalue;
		m[1] = matrix[1];
		m[2] = matrix[2];
		m[3] = matrix[3] - evalue;
		m[4] = matrix[4];
		m[5] = matrix[5] - evalue;

		// find the largest component
		float mc = std::fabs(m[0]);
		int mi = 0;
		for (int i = 1; i < 6; ++i)
		{
			float c = std::fabs(m[i]);
			if (c > mc)
			{
				mc = c;
				mi = i;
			}
		}

		// pick the first eigenvector based on this index
		switch (mi)
		{
			case 0:
			case 1:
				return Vec3(-m[1], m[0], 0.0f);

			case 2:
				return Vec3(m[2], 0.0f, -m[0]);

			case 3:
			case 4:
				return Vec3(0.0f, -m[4], m[3]);

			default:
				return Vec3(0.0f, -m[5], m[4]);
		}
	}

	static Vec3 ComputePrincipleComponent(Sym3x3 const& matrix)
	{
		// compute the cubic coefficients
		float c0 = matrix[0] * matrix[3] * matrix[5]
			+ 2.0f*matrix[1] * matrix[2] * matrix[4]
			- matrix[0] * matrix[4] * matrix[4]
			- matrix[3] * matrix[2] * matrix[2]
			- matrix[5] * matrix[1] * matrix[1];
		float c1 = matrix[0] * matrix[3] + matrix[0] * matrix[5] + matrix[3] * matrix[5]
			- matrix[1] * matrix[1] - matrix[2] * matrix[2] - matrix[4] * matrix[4];
		float c2 = matrix[0] + matrix[3] + matrix[5];

		// compute the quadratic coefficients
		float a = c1 - (1.0f / 3.0f)*c2*c2;
		float b = (-2.0f / 27.0f)*c2*c2*c2 + (1.0f / 3.0f)*c1*c2 - c0;

		// compute the root count check
		float Q = 0.25f*b*b + (1.0f / 27.0f)*a*a*a;

		// test the multiplicity
		if (FLT_EPSILON < Q)
		{
			// only one root, which implies we have a multiple of the identity
			return Vec3(1.0f);
		}
		else if (Q < -FLT_EPSILON)
		{
			// three distinct roots
			float theta = std::atan2(std::sqrt(-Q), -0.5f*b);
			float rho = std::sqrt(0.25f*b*b - Q);

			float rt = std::pow(rho, 1.0f / 3.0f);
			float ct = std::cos(theta / 3.0f);
			float st = std::sin(theta / 3.0f);

			float l1 = (1.0f / 3.0f)*c2 + 2.0f*rt*ct;
			float l2 = (1.0f / 3.0f)*c2 - rt*(ct + (float)sqrt(3.0f)*st);
			float l3 = (1.0f / 3.0f)*c2 - rt*(ct - (float)sqrt(3.0f)*st);

			// pick the larger
			if (std::fabs(l2) > std::fabs(l1))
				l1 = l2;
			if (std::fabs(l3) > std::fabs(l1))
				l1 = l3;

			// get the eigenvector
			return GetMultiplicity1Evector(matrix, l1);
		}
		else // if( -FLT_EPSILON <= Q && Q <= FLT_EPSILON )
		{
			// two roots
			float rt;
			if (b < 0.0f)
				rt = -std::pow(-0.5f*b, 1.0f / 3.0f);
			else
				rt = std::pow(0.5f*b, 1.0f / 3.0f);

			float l1 = (1.0f / 3.0f)*c2 + rt;		// repeated
			float l2 = (1.0f / 3.0f)*c2 - 2.0f*rt;

			// get the eigenvector
			if (std::fabs(l1) > std::fabs(l2))
				return GetMultiplicity2Evector(matrix, l1);
			else
				return GetMultiplicity1Evector(matrix, l2);
		}
	}


	// colourblock.h/cpp
	static int FloatToInt(float a, int limit)
	{
		// use ANSI round-to-zero behaviour to get round-to-nearest
		int i = (int)(a + 0.5f);

		// clamp to the limit
		if (i < 0)
			i = 0;
		else if (i > limit)
			i = limit;

		// done
		return i;
	}

	static int FloatTo565(Vec3::Arg colour)
	{
		// get the components in the correct range
		int r = FloatToInt(31.0f*colour.X(), 31);
		int g = FloatToInt(63.0f*colour.Y(), 63);
		int b = FloatToInt(31.0f*colour.Z(), 31);

		// pack into a single value
		return (r << 11) | (g << 5) | b;
	}

	static void WriteColourBlock(int a, int b, u8* indices, void* block)
	{
		// get the block as bytes
		u8* bytes = (u8*)block;

		// write the endpoints
		bytes[0] = (u8)(a & 0xff);
		bytes[1] = (u8)(a >> 8);
		bytes[2] = (u8)(b & 0xff);
		bytes[3] = (u8)(b >> 8);

		// write the indices
		for (int i = 0; i < 4; ++i)
		{
			u8 const* ind = indices + 4 * i;
			bytes[4 + i] = ind[0] | (ind[1] << 2) | (ind[2] << 4) | (ind[3] << 6);
		}
	}

	static void WriteColourBlock3(Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block)
	{
		// get the packed values
		int a = FloatTo565(start);
		int b = FloatTo565(end);

		// remap the indices
		u8 remapped[16];
		if (a <= b)
		{
			// use the indices directly
			for (int i = 0; i < 16; ++i)
				remapped[i] = indices[i];
		}
		else
		{
			// swap a and b
			std::swap(a, b);
			for (int i = 0; i < 16; ++i)
			{
				if (indices[i] == 0)
					remapped[i] = 1;
				else if (indices[i] == 1)
					remapped[i] = 0;
				else
					remapped[i] = indices[i];
			}
		}

		// write the block
		WriteColourBlock(a, b, remapped, block);
	}

	static void WriteColourBlock4(Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block)
	{
		// get the packed values
		int a = FloatTo565(start);
		int b = FloatTo565(end);

		// remap the indices
		u8 remapped[16];
		if (a < b)
		{
			// swap a and b
			std::swap(a, b);
			for (int i = 0; i < 16; ++i)
				remapped[i] = (indices[i] ^ 0x1) & 0x3;
		}
		else if (a == b)
		{
			// use index 0
			for (int i = 0; i < 16; ++i)
				remapped[i] = 0;
		}
		else
		{
			// use the indices directly
			for (int i = 0; i < 16; ++i)
				remapped[i] = indices[i];
		}

		// write the block
		WriteColourBlock(a, b, remapped, block);
	}

	static int Unpack565(u8 const* packed, u8* colour)
	{
		// build the packed value
		int value = (int)packed[0] | ((int)packed[1] << 8);

		// get the components in the stored range
		u8 red = (u8)((value >> 11) & 0x1f);
		u8 green = (u8)((value >> 5) & 0x3f);
		u8 blue = (u8)(value & 0x1f);

		// scale up to 8 bits
		colour[0] = (red << 3) | (red >> 2);
		colour[1] = (green << 2) | (green >> 4);
		colour[2] = (blue << 3) | (blue >> 2);
		colour[3] = 255;

		// return the value
		return value;
	}

	static void DecompressColour(u8* rgba, void const* block, bool isDxt1)
	{
		// get the block bytes
		u8 const* bytes = reinterpret_cast<u8 const*>(block);

		// unpack the endpoints
		u8 codes[16];
		int a = Unpack565(bytes, codes);
		int b = Unpack565(bytes + 2, codes + 4);

		// generate the midpoints
		for (int i = 0; i < 3; ++i)
		{
			int c = codes[i];
			int d = codes[4 + i];

			if (isDxt1 && a <= b)
			{
				codes[8 + i] = (u8)((c + d) / 2);
				codes[12 + i] = 0;
			}
			else
			{
				codes[8 + i] = (u8)((2 * c + d) / 3);
				codes[12 + i] = (u8)((c + 2 * d) / 3);
			}
		}

		// fill in alpha for the intermediate values
		codes[8 + 3] = 255;
		codes[12 + 3] = (isDxt1 && a <= b) ? 0 : 255;

		// unpack the indices
		u8 indices[16];
		for (int i = 0; i < 4; ++i)
		{
			u8* ind = indices + 4 * i;
			u8 packed = bytes[4 + i];

			ind[0] = packed & 0x3;
			ind[1] = (packed >> 2) & 0x3;
			ind[2] = (packed >> 4) & 0x3;
			ind[3] = (packed >> 6) & 0x3;
		}

		// store out the colours
		for (int i = 0; i < 16; ++i)
		{
			u8 offset = 4 * indices[i];
			for (int j = 0; j < 4; ++j)
				rgba[4 * i + j] = codes[offset + j];
		}
	}


	// colourset.h/cpp
	/*! @brief Represents a set of block colours
	*/
	class ColourSet
	{
	public:
		ColourSet(u8 const* rgba, int mask, int flags)
			: m_count(0),
			m_transparent(false)
		{
			// check the compression mode for dxt1
			bool isDxt1 = ((flags & kDxt1) != 0);
			bool weightByAlpha = ((flags & kWeightColourByAlpha) != 0);

			// create the minimal set
			for (int i = 0; i < 16; ++i)
			{
				// check this pixel is enabled
				int bit = 1 << i;
				if ((mask & bit) == 0)
				{
					m_remap[i] = -1;
					continue;
				}

				// check for transparent pixels when using dxt1
				if (isDxt1 && rgba[4 * i + 3] < 128)
				{
					m_remap[i] = -1;
					m_transparent = true;
					continue;
				}

				// loop over previous points for a match
				for (int j = 0;; ++j)
				{
					// allocate a new point
					if (j == i)
					{
						// normalise coordinates to [0,1]
						float x = (float)rgba[4 * i] / 255.0f;
						float y = (float)rgba[4 * i + 1] / 255.0f;
						float z = (float)rgba[4 * i + 2] / 255.0f;

						// ensure there is always non-zero weight even for zero alpha
						float w = (float)(rgba[4 * i + 3] + 1) / 256.0f;

						// add the point
						m_points[m_count] = Vec3(x, y, z);
						m_weights[m_count] = (weightByAlpha ? w : 1.0f);
						m_remap[i] = m_count;

						// advance
						++m_count;
						break;
					}

					// check for a match
					int oldbit = 1 << j;
					bool match = ((mask & oldbit) != 0)
						&& (rgba[4 * i] == rgba[4 * j])
						&& (rgba[4 * i + 1] == rgba[4 * j + 1])
						&& (rgba[4 * i + 2] == rgba[4 * j + 2])
						&& (rgba[4 * j + 3] >= 128 || !isDxt1);
					if (match)
					{
						// get the index of the match
						int index = m_remap[j];

						// ensure there is always non-zero weight even for zero alpha
						float w = (float)(rgba[4 * i + 3] + 1) / 256.0f;

						// map to this point and increase the weight
						m_weights[index] += (weightByAlpha ? w : 1.0f);
						m_remap[i] = index;
						break;
					}
				}
			}

			// square root the weights
			for (int i = 0; i < m_count; ++i)
				m_weights[i] = std::sqrt(m_weights[i]);
		}

		int GetCount() const { return m_count; }
		Vec3 const* GetPoints() const { return m_points; }
		float const* GetWeights() const { return m_weights; }
		bool IsTransparent() const { return m_transparent; }

		void RemapIndices(u8 const* source, u8* target) const
		{
			for (int i = 0; i < 16; ++i)
			{
				int j = m_remap[i];
				if (j == -1)
					target[i] = 3;
				else
					target[i] = source[j];
			}
		}

	private:
		int m_count;
		Vec3 m_points[16];
		float m_weights[16];
		int m_remap[16];
		bool m_transparent;
	};

	// colourfit.h/cpp
	class ColourFit
	{
	public:
		ColourFit(ColourSet const* colours, int flags)
			: m_colours(colours), m_flags(flags) { }

		void Compress(void* block)
		{
			bool isDxt1 = ((m_flags & kDxt1) != 0);
			if (isDxt1)
			{
				Compress3(block);
				if (!m_colours->IsTransparent())
					Compress4(block);
			}
			else
				Compress4(block);
		}

	protected:
		virtual void Compress3(void* block) = 0;
		virtual void Compress4(void* block) = 0;

		ColourSet const* m_colours;
		int m_flags;
	};

	// rangefit.h/cpp
	class RangeFit : public ColourFit
	{
	public:
		RangeFit(ColourSet const* colours, int flags)
			: ColourFit(colours, flags)
		{
			// initialise the metric
			bool perceptual = ((m_flags & kColourMetricPerceptual) != 0);
			if (perceptual)
				m_metric = Vec3(0.2126f, 0.7152f, 0.0722f);
			else
				m_metric = Vec3(1.0f);

			// initialise the best error
			m_besterror = FLT_MAX;

			// cache some values
			int const count = m_colours->GetCount();
			Vec3 const* values = m_colours->GetPoints();
			float const* weights = m_colours->GetWeights();

			// get the covariance matrix
			Sym3x3 covariance = ComputeWeightedCovariance(count, values, weights);

			// compute the principle component
			Vec3 principle = ComputePrincipleComponent(covariance);

			// get the min and max range as the codebook endpoints
			Vec3 start(0.0f);
			Vec3 end(0.0f);
			if (count > 0)
			{
				float min, max;

				// compute the range
				start = end = values[0];
				min = max = Dot(values[0], principle);
				for (int i = 1; i < count; ++i)
				{
					float val = Dot(values[i], principle);
					if (val < min)
					{
						start = values[i];
						min = val;
					}
					else if (val > max)
					{
						end = values[i];
						max = val;
					}
				}
			}

			// clamp the output to [0, 1]
			Vec3 const one(1.0f);
			Vec3 const zero(0.0f);
			start = Min(one, Max(zero, start));
			end = Min(one, Max(zero, end));

			// clamp to the grid and save
			Vec3 const grid(31.0f, 63.0f, 31.0f);
			Vec3 const gridrcp(1.0f / 31.0f, 1.0f / 63.0f, 1.0f / 31.0f);
			Vec3 const half(0.5f);
			m_start = Truncate(grid*start + half)*gridrcp;
			m_end = Truncate(grid*end + half)*gridrcp;
		}

	private:
		virtual void Compress3(void* block)
		{
			// cache some values
			int const count = m_colours->GetCount();
			Vec3 const* values = m_colours->GetPoints();

			// create a codebook
			Vec3 codes[3];
			codes[0] = m_start;
			codes[1] = m_end;
			codes[2] = 0.5f*m_start + 0.5f*m_end;

			// match each point to the closest code
			u8 closest[16];
			float error = 0.0f;
			for (int i = 0; i < count; ++i)
			{
				// find the closest code
				float dist = FLT_MAX;
				int idx = 0;
				for (int j = 0; j < 3; ++j)
				{
					float d = LengthSquared(m_metric*(values[i] - codes[j]));
					if (d < dist)
					{
						dist = d;
						idx = j;
					}
				}

				// save the index
				closest[i] = (u8)idx;

				// accumulate the error
				error += dist;
			}

			// save this scheme if it wins
			if (error < m_besterror)
			{
				// remap the indices
				u8 indices[16];
				m_colours->RemapIndices(closest, indices);

				// save the block
				WriteColourBlock3(m_start, m_end, indices, block);

				// save the error
				m_besterror = error;
			}
		}
		virtual void Compress4(void* block)
		{
			// cache some values
			int const count = m_colours->GetCount();
			Vec3 const* values = m_colours->GetPoints();

			// create a codebook
			Vec3 codes[4];
			codes[0] = m_start;
			codes[1] = m_end;
			codes[2] = (2.0f / 3.0f)*m_start + (1.0f / 3.0f)*m_end;
			codes[3] = (1.0f / 3.0f)*m_start + (2.0f / 3.0f)*m_end;

			// match each point to the closest code
			u8 closest[16];
			float error = 0.0f;
			for (int i = 0; i < count; ++i)
			{
				// find the closest code
				float dist = FLT_MAX;
				int idx = 0;
				for (int j = 0; j < 4; ++j)
				{
					float d = LengthSquared(m_metric*(values[i] - codes[j]));
					if (d < dist)
					{
						dist = d;
						idx = j;
					}
				}

				// save the index
				closest[i] = (u8)idx;

				// accumulate the error
				error += dist;
			}

			// save this scheme if it wins
			if (error < m_besterror)
			{
				// remap the indices
				u8 indices[16];
				m_colours->RemapIndices(closest, indices);

				// save the block
				WriteColourBlock4(m_start, m_end, indices, block);

				// save the error
				m_besterror = error;
			}
		}

		Vec3 m_metric;
		Vec3 m_start;
		Vec3 m_end;
		float m_besterror;
	};

	// simd.h
#if SQUISH_USE_ALTIVEC

#define VEC4_CONST( X ) Vec4( ( vector float )( X ) )

	class Vec4
	{
	public:
		typedef Vec4 Arg;

		Vec4() {}

		explicit Vec4(vector float v) : m_v(v) {}

		Vec4(Vec4 const& arg) : m_v(arg.m_v) {}

		Vec4& operator=(Vec4 const& arg)
		{
			m_v = arg.m_v;
			return *this;
		}

		explicit Vec4(float s)
		{
			union { vector float v; float c[4]; } u;
			u.c[0] = s;
			u.c[1] = s;
			u.c[2] = s;
			u.c[3] = s;
			m_v = u.v;
		}

		Vec4(float x, float y, float z, float w)
		{
			union { vector float v; float c[4]; } u;
			u.c[0] = x;
			u.c[1] = y;
			u.c[2] = z;
			u.c[3] = w;
			m_v = u.v;
		}

		Vec3 GetVec3() const
		{
			union { vector float v; float c[4]; } u;
			u.v = m_v;
			return Vec3(u.c[0], u.c[1], u.c[2]);
		}

		Vec4 SplatX() const { return Vec4(vec_splat(m_v, 0)); }
		Vec4 SplatY() const { return Vec4(vec_splat(m_v, 1)); }
		Vec4 SplatZ() const { return Vec4(vec_splat(m_v, 2)); }
		Vec4 SplatW() const { return Vec4(vec_splat(m_v, 3)); }

		Vec4& operator+=(Arg v)
		{
			m_v = vec_add(m_v, v.m_v);
			return *this;
		}

		Vec4& operator-=(Arg v)
		{
			m_v = vec_sub(m_v, v.m_v);
			return *this;
		}

		Vec4& operator*=(Arg v)
		{
			m_v = vec_madd(m_v, v.m_v, (vector float)(-0.0f));
			return *this;
		}

		friend Vec4 operator+(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(vec_add(left.m_v, right.m_v));
		}

		friend Vec4 operator-(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(vec_sub(left.m_v, right.m_v));
		}

		friend Vec4 operator*(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(vec_madd(left.m_v, right.m_v, (vector float)(-0.0f)));
		}

		//! Returns a*b + c
		friend Vec4 MultiplyAdd(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return Vec4(vec_madd(a.m_v, b.m_v, c.m_v));
		}

		//! Returns -( a*b - c )
		friend Vec4 NegativeMultiplySubtract(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return Vec4(vec_nmsub(a.m_v, b.m_v, c.m_v));
		}

		friend Vec4 Reciprocal(Vec4::Arg v)
		{
			// get the reciprocal estimate
			vector float estimate = vec_re(v.m_v);

			// one round of Newton-Rhaphson refinement
			vector float diff = vec_nmsub(estimate, v.m_v, (vector float)(1.0f));
			return Vec4(vec_madd(diff, estimate, estimate));
		}

		friend Vec4 Min(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(vec_min(left.m_v, right.m_v));
		}

		friend Vec4 Max(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(vec_max(left.m_v, right.m_v));
		}

		friend Vec4 Truncate(Vec4::Arg v)
		{
			return Vec4(vec_trunc(v.m_v));
		}

		friend bool CompareAnyLessThan(Vec4::Arg left, Vec4::Arg right)
		{
			return vec_any_lt(left.m_v, right.m_v) != 0;
		}

	private:
		vector float m_v;
	};
#elif SQUISH_USE_SSE

	#define SQUISH_SSE_SPLAT( a ) ( ( a ) | ( ( a ) << 2 ) | ( ( a ) << 4 ) | ( ( a ) << 6 ) )
	#define SQUISH_SSE_SHUF( x, y, z, w ) ( ( x ) | ( ( y ) << 2 ) | ( ( z ) << 4 ) | ( ( w ) << 6 ) )
	#define VEC4_CONST( X ) Vec4( X )

	class Vec4
	{
	public:
		typedef Vec4 const& Arg;

		Vec4() {}

		explicit Vec4(__m128 v) : m_v(v) {}

		Vec4(Vec4 const& arg) : m_v(arg.m_v) {}

		Vec4& operator=(Vec4 const& arg)
		{
			m_v = arg.m_v;
			return *this;
		}

		explicit Vec4(float s) : m_v(_mm_set1_ps(s)) {}

		Vec4(float x, float y, float z, float w) : m_v(_mm_setr_ps(x, y, z, w)) {}

		Vec3 GetVec3() const
		{
	#ifdef __GNUC__
			__attribute__((__aligned__(16))) float c[4];
	#else
			__declspec(align(16)) float c[4];
	#endif
			_mm_store_ps(c, m_v);
			return Vec3(c[0], c[1], c[2]);
		}

		Vec4 SplatX() const { return Vec4(_mm_shuffle_ps(m_v, m_v, SQUISH_SSE_SPLAT(0))); }
		Vec4 SplatY() const { return Vec4(_mm_shuffle_ps(m_v, m_v, SQUISH_SSE_SPLAT(1))); }
		Vec4 SplatZ() const { return Vec4(_mm_shuffle_ps(m_v, m_v, SQUISH_SSE_SPLAT(2))); }
		Vec4 SplatW() const { return Vec4(_mm_shuffle_ps(m_v, m_v, SQUISH_SSE_SPLAT(3))); }

		Vec4& operator+=(Arg v)
		{
			m_v = _mm_add_ps(m_v, v.m_v);
			return *this;
		}

		Vec4& operator-=(Arg v)
		{
			m_v = _mm_sub_ps(m_v, v.m_v);
			return *this;
		}

		Vec4& operator*=(Arg v)
		{
			m_v = _mm_mul_ps(m_v, v.m_v);
			return *this;
		}

		friend Vec4 operator+(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(_mm_add_ps(left.m_v, right.m_v));
		}

		friend Vec4 operator-(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(_mm_sub_ps(left.m_v, right.m_v));
		}

		friend Vec4 operator*(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(_mm_mul_ps(left.m_v, right.m_v));
		}

		//! Returns a*b + c
		friend Vec4 MultiplyAdd(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return Vec4(_mm_add_ps(_mm_mul_ps(a.m_v, b.m_v), c.m_v));
		}

		//! Returns -( a*b - c )
		friend Vec4 NegativeMultiplySubtract(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return Vec4(_mm_sub_ps(c.m_v, _mm_mul_ps(a.m_v, b.m_v)));
		}

		friend Vec4 Reciprocal(Vec4::Arg v)
		{
			// get the reciprocal estimate
			__m128 estimate = _mm_rcp_ps(v.m_v);

			// one round of Newton-Rhaphson refinement
			__m128 diff = _mm_sub_ps(_mm_set1_ps(1.0f), _mm_mul_ps(estimate, v.m_v));
			return Vec4(_mm_add_ps(_mm_mul_ps(diff, estimate), estimate));
		}

		friend Vec4 Min(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(_mm_min_ps(left.m_v, right.m_v));
		}

		friend Vec4 Max(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(_mm_max_ps(left.m_v, right.m_v));
		}

		friend Vec4 Truncate(Vec4::Arg v)
		{
	#if ( SQUISH_USE_SSE == 1 )
			// convert to ints
			__m128 input = v.m_v;
			__m64 lo = _mm_cvttps_pi32(input);
			__m64 hi = _mm_cvttps_pi32(_mm_movehl_ps(input, input));

			// convert to floats
			__m128 part = _mm_movelh_ps(input, _mm_cvtpi32_ps(input, hi));
			__m128 truncated = _mm_cvtpi32_ps(part, lo);

			// clear out the MMX multimedia state to allow FP calls later
			_mm_empty();
			return Vec4(truncated);
	#else
			// use SSE2 instructions
			return Vec4(_mm_cvtepi32_ps(_mm_cvttps_epi32(v.m_v)));
	#endif
		}

		friend bool CompareAnyLessThan(Vec4::Arg left, Vec4::Arg right)
		{
			__m128 bits = _mm_cmplt_ps(left.m_v, right.m_v);
			int value = _mm_movemask_ps(bits);
			return value != 0;
		}

	private:
		__m128 m_v;
	};

#else
	#define VEC4_CONST( X ) Vec4( X )

	class Vec4
	{
	public:
		typedef Vec4 const& Arg;

		Vec4() { }

		explicit Vec4(float s)
			: m_x(s), m_y(s), m_z(s), m_w(s) { }

		Vec4(float x, float y, float z, float w)
			: m_x(x), m_y(y), m_z(z), m_w(w) { }

		Vec3 GetVec3() const { return Vec3(m_x, m_y, m_z); }

		Vec4 SplatX() const { return Vec4(m_x); }
		Vec4 SplatY() const { return Vec4(m_y); }
		Vec4 SplatZ() const { return Vec4(m_z); }
		Vec4 SplatW() const { return Vec4(m_w); }

		Vec4& operator+=(Arg v)
		{
			m_x += v.m_x;
			m_y += v.m_y;
			m_z += v.m_z;
			m_w += v.m_w;
			return *this;
		}

		Vec4& operator-=(Arg v)
		{
			m_x -= v.m_x;
			m_y -= v.m_y;
			m_z -= v.m_z;
			m_w -= v.m_w;
			return *this;
		}

		Vec4& operator*=(Arg v)
		{
			m_x *= v.m_x;
			m_y *= v.m_y;
			m_z *= v.m_z;
			m_w *= v.m_w;
			return *this;
		}

		friend Vec4 operator+(Vec4::Arg left, Vec4::Arg right)
		{
			Vec4 copy(left);
			return copy += right;
		}

		friend Vec4 operator-(Vec4::Arg left, Vec4::Arg right)
		{
			Vec4 copy(left);
			return copy -= right;
		}

		friend Vec4 operator*(Vec4::Arg left, Vec4::Arg right)
		{
			Vec4 copy(left);
			return copy *= right;
		}

		//! Returns a*b + c
		friend Vec4 MultiplyAdd(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return a*b + c;
		}

		//! Returns -( a*b - c )
		friend Vec4 NegativeMultiplySubtract(Vec4::Arg a, Vec4::Arg b, Vec4::Arg c)
		{
			return c - a*b;
		}

		friend Vec4 Reciprocal(Vec4::Arg v)
		{
			return Vec4(
				1.0f / v.m_x,
				1.0f / v.m_y,
				1.0f / v.m_z,
				1.0f / v.m_w
				);
		}

		friend Vec4 Min(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(
				std::min(left.m_x, right.m_x),
				std::min(left.m_y, right.m_y),
				std::min(left.m_z, right.m_z),
				std::min(left.m_w, right.m_w)
				);
		}

		friend Vec4 Max(Vec4::Arg left, Vec4::Arg right)
		{
			return Vec4(
				std::max(left.m_x, right.m_x),
				std::max(left.m_y, right.m_y),
				std::max(left.m_z, right.m_z),
				std::max(left.m_w, right.m_w)
				);
		}

		friend Vec4 Truncate(Vec4::Arg v)
		{
			return Vec4(
				v.m_x > 0.0f ? std::floor(v.m_x) : std::ceil(v.m_x),
				v.m_y > 0.0f ? std::floor(v.m_y) : std::ceil(v.m_y),
				v.m_z > 0.0f ? std::floor(v.m_z) : std::ceil(v.m_z),
				v.m_w > 0.0f ? std::floor(v.m_w) : std::ceil(v.m_w)
				);
		}

		friend bool CompareAnyLessThan(Vec4::Arg left, Vec4::Arg right)
		{
			return left.m_x < right.m_x
				|| left.m_y < right.m_y
				|| left.m_z < right.m_z
				|| left.m_w < right.m_w;
		}

	private:
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};

#endif

	// clusterfit.h/cpp
	class ClusterFit : public ColourFit
	{
	public:
		ClusterFit(ColourSet const* colours, int flags)
			: ColourFit(colours, flags)
		{
			// set the iteration count
			m_iterationCount = (m_flags & kColourIterativeClusterFit) ? kMaxIterations : 1;

			// initialise the best error
			m_besterror = VEC4_CONST(FLT_MAX);

			// initialise the metric
			bool perceptual = ((m_flags & kColourMetricPerceptual) != 0);
			if (perceptual)
				m_metric = Vec4(0.2126f, 0.7152f, 0.0722f, 0.0f);
			else
				m_metric = VEC4_CONST(1.0f);

			// cache some values
			int const count = m_colours->GetCount();
			Vec3 const* values = m_colours->GetPoints();

			// get the covariance matrix
			Sym3x3 covariance = ComputeWeightedCovariance(count, values, m_colours->GetWeights());

			// compute the principle component
			m_principle = ComputePrincipleComponent(covariance);
		}

	private:
		bool ConstructOrdering(Vec3 const& axis, int iteration)
		{
			// cache some values
			int const count = m_colours->GetCount();
			Vec3 const* values = m_colours->GetPoints();

			// build the list of dot products
			float dps[16];
			u8* order = (u8*)m_order + 16 * iteration;
			for (int i = 0; i < count; ++i)
			{
				dps[i] = Dot(values[i], axis);
				order[i] = (u8)i;
			}

			// stable sort using them
			for (int i = 0; i < count; ++i)
			{
				for (int j = i; j > 0 && dps[j] < dps[j - 1]; --j)
				{
					std::swap(dps[j], dps[j - 1]);
					std::swap(order[j], order[j - 1]);
				}
			}

			// check this ordering is unique
			for (int it = 0; it < iteration; ++it)
			{
				u8 const* prev = (u8*)m_order + 16 * it;
				bool same = true;
				for (int i = 0; i < count; ++i)
				{
					if (order[i] != prev[i])
					{
						same = false;
						break;
					}
				}
				if (same)
					return false;
			}

			// copy the ordering and weight all the points
			Vec3 const* unweighted = m_colours->GetPoints();
			float const* weights = m_colours->GetWeights();
			m_xsum_wsum = VEC4_CONST(0.0f);
			for (int i = 0; i < count; ++i)
			{
				int j = order[i];
				Vec4 p(unweighted[j].X(), unweighted[j].Y(), unweighted[j].Z(), 1.0f);
				Vec4 w(weights[j]);
				Vec4 x = p*w;
				m_points_weights[i] = x;
				m_xsum_wsum += x;
			}
			return true;
		}

		virtual void Compress3(void* block)
		{
			// declare variables
			int const count = m_colours->GetCount();
			Vec4 const two = VEC4_CONST(2.0);
			Vec4 const one = VEC4_CONST(1.0f);
			Vec4 const half_half2(0.5f, 0.5f, 0.5f, 0.25f);
			Vec4 const zero = VEC4_CONST(0.0f);
			Vec4 const half = VEC4_CONST(0.5f);
			Vec4 const grid(31.0f, 63.0f, 31.0f, 0.0f);
			Vec4 const gridrcp(1.0f / 31.0f, 1.0f / 63.0f, 1.0f / 31.0f, 0.0f);

			// prepare an ordering using the principle axis
			ConstructOrdering(m_principle, 0);

			// check all possible clusters and iterate on the total order
			Vec4 beststart = VEC4_CONST(0.0f);
			Vec4 bestend = VEC4_CONST(0.0f);
			Vec4 besterror = m_besterror;
			u8 bestindices[16];
			int bestiteration = 0;
			int besti = 0, bestj = 0;

			// loop over iterations (we avoid the case that all points in first or last cluster)
			for (int iterationIndex = 0;;)
			{
				// first cluster [0,i) is at the start
				Vec4 part0 = VEC4_CONST(0.0f);
				for (int i = 0; i < count; ++i)
				{
					// second cluster [i,j) is half along
					Vec4 part1 = (i == 0) ? m_points_weights[0] : VEC4_CONST(0.0f);
					int jmin = (i == 0) ? 1 : i;
					for (int j = jmin;;)
					{
						// last cluster [j,count) is at the end
						Vec4 part2 = m_xsum_wsum - part1 - part0;

						// compute least squares terms directly
						Vec4 alphax_sum = MultiplyAdd(part1, half_half2, part0);
						Vec4 alpha2_sum = alphax_sum.SplatW();

						Vec4 betax_sum = MultiplyAdd(part1, half_half2, part2);
						Vec4 beta2_sum = betax_sum.SplatW();

						Vec4 alphabeta_sum = (part1*half_half2).SplatW();

						// compute the least-squares optimal points
						Vec4 factor = Reciprocal(NegativeMultiplySubtract(alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum));
						Vec4 a = NegativeMultiplySubtract(betax_sum, alphabeta_sum, alphax_sum*beta2_sum)*factor;
						Vec4 b = NegativeMultiplySubtract(alphax_sum, alphabeta_sum, betax_sum*alpha2_sum)*factor;

						// clamp to the grid
						a = Min(one, Max(zero, a));
						b = Min(one, Max(zero, b));
						a = Truncate(MultiplyAdd(grid, a, half))*gridrcp;
						b = Truncate(MultiplyAdd(grid, b, half))*gridrcp;

						// compute the error (we skip the constant xxsum)
						Vec4 e1 = MultiplyAdd(a*a, alpha2_sum, b*b*beta2_sum);
						Vec4 e2 = NegativeMultiplySubtract(a, alphax_sum, a*b*alphabeta_sum);
						Vec4 e3 = NegativeMultiplySubtract(b, betax_sum, e2);
						Vec4 e4 = MultiplyAdd(two, e3, e1);

						// apply the metric to the error term
						Vec4 e5 = e4*m_metric;
						Vec4 error = e5.SplatX() + e5.SplatY() + e5.SplatZ();

						// keep the solution if it wins
						if (CompareAnyLessThan(error, besterror))
						{
							beststart = a;
							bestend = b;
							besti = i;
							bestj = j;
							besterror = error;
							bestiteration = iterationIndex;
						}

						// advance
						if (j == count)
							break;
						part1 += m_points_weights[j];
						++j;
					}

					// advance
					part0 += m_points_weights[i];
				}

				// stop if we didn't improve in this iteration
				if (bestiteration != iterationIndex)
					break;

				// advance if possible
				++iterationIndex;
				if (iterationIndex == m_iterationCount)
					break;

				// stop if a new iteration is an ordering that has already been tried
				Vec3 axis = (bestend - beststart).GetVec3();
				if (!ConstructOrdering(axis, iterationIndex))
					break;
			}

			// save the block if necessary
			if (CompareAnyLessThan(besterror, m_besterror))
			{
				// remap the indices
				u8 const* order = (u8*)m_order + 16 * bestiteration;

				u8 unordered[16];
				for (int m = 0; m < besti; ++m)
					unordered[order[m]] = 0;
				for (int m = besti; m < bestj; ++m)
					unordered[order[m]] = 2;
				for (int m = bestj; m < count; ++m)
					unordered[order[m]] = 1;

				m_colours->RemapIndices(unordered, bestindices);

				// save the block
				WriteColourBlock3(beststart.GetVec3(), bestend.GetVec3(), bestindices, block);

				// save the error
				m_besterror = besterror;
			}
		}
		virtual void Compress4(void* block)
		{
			// declare variables
			int const count = m_colours->GetCount();
			Vec4 const two = VEC4_CONST(2.0f);
			Vec4 const one = VEC4_CONST(1.0f);
			Vec4 const onethird_onethird2(1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 9.0f);
			Vec4 const twothirds_twothirds2(2.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f, 4.0f / 9.0f);
			Vec4 const twonineths = VEC4_CONST(2.0f / 9.0f);
			Vec4 const zero = VEC4_CONST(0.0f);
			Vec4 const half = VEC4_CONST(0.5f);
			Vec4 const grid(31.0f, 63.0f, 31.0f, 0.0f);
			Vec4 const gridrcp(1.0f / 31.0f, 1.0f / 63.0f, 1.0f / 31.0f, 0.0f);

			// prepare an ordering using the principle axis
			ConstructOrdering(m_principle, 0);

			// check all possible clusters and iterate on the total order
			Vec4 beststart = VEC4_CONST(0.0f);
			Vec4 bestend = VEC4_CONST(0.0f);
			Vec4 besterror = m_besterror;
			u8 bestindices[16];
			int bestiteration = 0;
			int besti = 0, bestj = 0, bestk = 0;

			// loop over iterations (we avoid the case that all points in first or last cluster)
			for (int iterationIndex = 0;;)
			{
				// first cluster [0,i) is at the start
				Vec4 part0 = VEC4_CONST(0.0f);
				for (int i = 0; i < count; ++i)
				{
					// second cluster [i,j) is one third along
					Vec4 part1 = VEC4_CONST(0.0f);
					for (int j = i;;)
					{
						// third cluster [j,k) is two thirds along
						Vec4 part2 = (j == 0) ? m_points_weights[0] : VEC4_CONST(0.0f);
						int kmin = (j == 0) ? 1 : j;
						for (int k = kmin;;)
						{
							// last cluster [k,count) is at the end
							Vec4 part3 = m_xsum_wsum - part2 - part1 - part0;

							// compute least squares terms directly
							Vec4 const alphax_sum = MultiplyAdd(part2, onethird_onethird2, MultiplyAdd(part1, twothirds_twothirds2, part0));
							Vec4 const alpha2_sum = alphax_sum.SplatW();

							Vec4 const betax_sum = MultiplyAdd(part1, onethird_onethird2, MultiplyAdd(part2, twothirds_twothirds2, part3));
							Vec4 const beta2_sum = betax_sum.SplatW();

							Vec4 const alphabeta_sum = twonineths*(part1 + part2).SplatW();

							// compute the least-squares optimal points
							Vec4 factor = Reciprocal(NegativeMultiplySubtract(alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum));
							Vec4 a = NegativeMultiplySubtract(betax_sum, alphabeta_sum, alphax_sum*beta2_sum)*factor;
							Vec4 b = NegativeMultiplySubtract(alphax_sum, alphabeta_sum, betax_sum*alpha2_sum)*factor;

							// clamp to the grid
							a = Min(one, Max(zero, a));
							b = Min(one, Max(zero, b));
							a = Truncate(MultiplyAdd(grid, a, half))*gridrcp;
							b = Truncate(MultiplyAdd(grid, b, half))*gridrcp;

							// compute the error (we skip the constant xxsum)
							Vec4 e1 = MultiplyAdd(a*a, alpha2_sum, b*b*beta2_sum);
							Vec4 e2 = NegativeMultiplySubtract(a, alphax_sum, a*b*alphabeta_sum);
							Vec4 e3 = NegativeMultiplySubtract(b, betax_sum, e2);
							Vec4 e4 = MultiplyAdd(two, e3, e1);

							// apply the metric to the error term
							Vec4 e5 = e4*m_metric;
							Vec4 error = e5.SplatX() + e5.SplatY() + e5.SplatZ();

							// keep the solution if it wins
							if (CompareAnyLessThan(error, besterror))
							{
								beststart = a;
								bestend = b;
								besterror = error;
								besti = i;
								bestj = j;
								bestk = k;
								bestiteration = iterationIndex;
							}

							// advance
							if (k == count)
								break;
							part2 += m_points_weights[k];
							++k;
						}

						// advance
						if (j == count)
							break;
						part1 += m_points_weights[j];
						++j;
					}

					// advance
					part0 += m_points_weights[i];
				}

				// stop if we didn't improve in this iteration
				if (bestiteration != iterationIndex)
					break;

				// advance if possible
				++iterationIndex;
				if (iterationIndex == m_iterationCount)
					break;

				// stop if a new iteration is an ordering that has already been tried
				Vec3 axis = (bestend - beststart).GetVec3();
				if (!ConstructOrdering(axis, iterationIndex))
					break;
			}

			// save the block if necessary
			if (CompareAnyLessThan(besterror, m_besterror))
			{
				// remap the indices
				u8 const* order = (u8*)m_order + 16 * bestiteration;

				u8 unordered[16];
				for (int m = 0; m < besti; ++m)
					unordered[order[m]] = 0;
				for (int m = besti; m < bestj; ++m)
					unordered[order[m]] = 2;
				for (int m = bestj; m < bestk; ++m)
					unordered[order[m]] = 3;
				for (int m = bestk; m < count; ++m)
					unordered[order[m]] = 1;

				m_colours->RemapIndices(unordered, bestindices);

				// save the block
				WriteColourBlock4(beststart.GetVec3(), bestend.GetVec3(), bestindices, block);

				// save the error
				m_besterror = besterror;
			}
		}

		enum { kMaxIterations = 8 };

		int m_iterationCount;
		Vec3 m_principle;
		u8 m_order[16 * kMaxIterations];
		Vec4 m_points_weights[16];
		Vec4 m_xsum_wsum;
		Vec4 m_metric;
		Vec4 m_besterror;
	};

	// singlecolourfit.h/cpp
	struct SourceBlock
	{
		u8 start;
		u8 end;
		u8 error;
	};

	struct SingleColourLookup
	{
		SourceBlock sources[2];
	};

	const SingleColourLookup lookup_5_3[] =
	{
		{ { { 0, 0, 0 }, { 0, 0, 0 } } },
		{ { { 0, 0, 1 }, { 0, 0, 1 } } },
		{ { { 0, 0, 2 }, { 0, 0, 2 } } },
		{ { { 0, 0, 3 }, { 0, 1, 1 } } },
		{ { { 0, 0, 4 }, { 0, 1, 0 } } },
		{ { { 1, 0, 3 }, { 0, 1, 1 } } },
		{ { { 1, 0, 2 }, { 0, 1, 2 } } },
		{ { { 1, 0, 1 }, { 0, 2, 1 } } },
		{ { { 1, 0, 0 }, { 0, 2, 0 } } },
		{ { { 1, 0, 1 }, { 0, 2, 1 } } },
		{ { { 1, 0, 2 }, { 0, 2, 2 } } },
		{ { { 1, 0, 3 }, { 0, 3, 1 } } },
		{ { { 1, 0, 4 }, { 0, 3, 0 } } },
		{ { { 2, 0, 3 }, { 0, 3, 1 } } },
		{ { { 2, 0, 2 }, { 0, 3, 2 } } },
		{ { { 2, 0, 1 }, { 0, 4, 1 } } },
		{ { { 2, 0, 0 }, { 0, 4, 0 } } },
		{ { { 2, 0, 1 }, { 0, 4, 1 } } },
		{ { { 2, 0, 2 }, { 0, 4, 2 } } },
		{ { { 2, 0, 3 }, { 0, 5, 1 } } },
		{ { { 2, 0, 4 }, { 0, 5, 0 } } },
		{ { { 3, 0, 3 }, { 0, 5, 1 } } },
		{ { { 3, 0, 2 }, { 0, 5, 2 } } },
		{ { { 3, 0, 1 }, { 0, 6, 1 } } },
		{ { { 3, 0, 0 }, { 0, 6, 0 } } },
		{ { { 3, 0, 1 }, { 0, 6, 1 } } },
		{ { { 3, 0, 2 }, { 0, 6, 2 } } },
		{ { { 3, 0, 3 }, { 0, 7, 1 } } },
		{ { { 3, 0, 4 }, { 0, 7, 0 } } },
		{ { { 4, 0, 4 }, { 0, 7, 1 } } },
		{ { { 4, 0, 3 }, { 0, 7, 2 } } },
		{ { { 4, 0, 2 }, { 1, 7, 1 } } },
		{ { { 4, 0, 1 }, { 1, 7, 0 } } },
		{ { { 4, 0, 0 }, { 0, 8, 0 } } },
		{ { { 4, 0, 1 }, { 0, 8, 1 } } },
		{ { { 4, 0, 2 }, { 2, 7, 1 } } },
		{ { { 4, 0, 3 }, { 2, 7, 0 } } },
		{ { { 4, 0, 4 }, { 0, 9, 0 } } },
		{ { { 5, 0, 3 }, { 0, 9, 1 } } },
		{ { { 5, 0, 2 }, { 3, 7, 1 } } },
		{ { { 5, 0, 1 }, { 3, 7, 0 } } },
		{ { { 5, 0, 0 }, { 0, 10, 0 } } },
		{ { { 5, 0, 1 }, { 0, 10, 1 } } },
		{ { { 5, 0, 2 }, { 0, 10, 2 } } },
		{ { { 5, 0, 3 }, { 0, 11, 1 } } },
		{ { { 5, 0, 4 }, { 0, 11, 0 } } },
		{ { { 6, 0, 3 }, { 0, 11, 1 } } },
		{ { { 6, 0, 2 }, { 0, 11, 2 } } },
		{ { { 6, 0, 1 }, { 0, 12, 1 } } },
		{ { { 6, 0, 0 }, { 0, 12, 0 } } },
		{ { { 6, 0, 1 }, { 0, 12, 1 } } },
		{ { { 6, 0, 2 }, { 0, 12, 2 } } },
		{ { { 6, 0, 3 }, { 0, 13, 1 } } },
		{ { { 6, 0, 4 }, { 0, 13, 0 } } },
		{ { { 7, 0, 3 }, { 0, 13, 1 } } },
		{ { { 7, 0, 2 }, { 0, 13, 2 } } },
		{ { { 7, 0, 1 }, { 0, 14, 1 } } },
		{ { { 7, 0, 0 }, { 0, 14, 0 } } },
		{ { { 7, 0, 1 }, { 0, 14, 1 } } },
		{ { { 7, 0, 2 }, { 0, 14, 2 } } },
		{ { { 7, 0, 3 }, { 0, 15, 1 } } },
		{ { { 7, 0, 4 }, { 0, 15, 0 } } },
		{ { { 8, 0, 4 }, { 0, 15, 1 } } },
		{ { { 8, 0, 3 }, { 0, 15, 2 } } },
		{ { { 8, 0, 2 }, { 1, 15, 1 } } },
		{ { { 8, 0, 1 }, { 1, 15, 0 } } },
		{ { { 8, 0, 0 }, { 0, 16, 0 } } },
		{ { { 8, 0, 1 }, { 0, 16, 1 } } },
		{ { { 8, 0, 2 }, { 2, 15, 1 } } },
		{ { { 8, 0, 3 }, { 2, 15, 0 } } },
		{ { { 8, 0, 4 }, { 0, 17, 0 } } },
		{ { { 9, 0, 3 }, { 0, 17, 1 } } },
		{ { { 9, 0, 2 }, { 3, 15, 1 } } },
		{ { { 9, 0, 1 }, { 3, 15, 0 } } },
		{ { { 9, 0, 0 }, { 0, 18, 0 } } },
		{ { { 9, 0, 1 }, { 0, 18, 1 } } },
		{ { { 9, 0, 2 }, { 0, 18, 2 } } },
		{ { { 9, 0, 3 }, { 0, 19, 1 } } },
		{ { { 9, 0, 4 }, { 0, 19, 0 } } },
		{ { { 10, 0, 3 }, { 0, 19, 1 } } },
		{ { { 10, 0, 2 }, { 0, 19, 2 } } },
		{ { { 10, 0, 1 }, { 0, 20, 1 } } },
		{ { { 10, 0, 0 }, { 0, 20, 0 } } },
		{ { { 10, 0, 1 }, { 0, 20, 1 } } },
		{ { { 10, 0, 2 }, { 0, 20, 2 } } },
		{ { { 10, 0, 3 }, { 0, 21, 1 } } },
		{ { { 10, 0, 4 }, { 0, 21, 0 } } },
		{ { { 11, 0, 3 }, { 0, 21, 1 } } },
		{ { { 11, 0, 2 }, { 0, 21, 2 } } },
		{ { { 11, 0, 1 }, { 0, 22, 1 } } },
		{ { { 11, 0, 0 }, { 0, 22, 0 } } },
		{ { { 11, 0, 1 }, { 0, 22, 1 } } },
		{ { { 11, 0, 2 }, { 0, 22, 2 } } },
		{ { { 11, 0, 3 }, { 0, 23, 1 } } },
		{ { { 11, 0, 4 }, { 0, 23, 0 } } },
		{ { { 12, 0, 4 }, { 0, 23, 1 } } },
		{ { { 12, 0, 3 }, { 0, 23, 2 } } },
		{ { { 12, 0, 2 }, { 1, 23, 1 } } },
		{ { { 12, 0, 1 }, { 1, 23, 0 } } },
		{ { { 12, 0, 0 }, { 0, 24, 0 } } },
		{ { { 12, 0, 1 }, { 0, 24, 1 } } },
		{ { { 12, 0, 2 }, { 2, 23, 1 } } },
		{ { { 12, 0, 3 }, { 2, 23, 0 } } },
		{ { { 12, 0, 4 }, { 0, 25, 0 } } },
		{ { { 13, 0, 3 }, { 0, 25, 1 } } },
		{ { { 13, 0, 2 }, { 3, 23, 1 } } },
		{ { { 13, 0, 1 }, { 3, 23, 0 } } },
		{ { { 13, 0, 0 }, { 0, 26, 0 } } },
		{ { { 13, 0, 1 }, { 0, 26, 1 } } },
		{ { { 13, 0, 2 }, { 0, 26, 2 } } },
		{ { { 13, 0, 3 }, { 0, 27, 1 } } },
		{ { { 13, 0, 4 }, { 0, 27, 0 } } },
		{ { { 14, 0, 3 }, { 0, 27, 1 } } },
		{ { { 14, 0, 2 }, { 0, 27, 2 } } },
		{ { { 14, 0, 1 }, { 0, 28, 1 } } },
		{ { { 14, 0, 0 }, { 0, 28, 0 } } },
		{ { { 14, 0, 1 }, { 0, 28, 1 } } },
		{ { { 14, 0, 2 }, { 0, 28, 2 } } },
		{ { { 14, 0, 3 }, { 0, 29, 1 } } },
		{ { { 14, 0, 4 }, { 0, 29, 0 } } },
		{ { { 15, 0, 3 }, { 0, 29, 1 } } },
		{ { { 15, 0, 2 }, { 0, 29, 2 } } },
		{ { { 15, 0, 1 }, { 0, 30, 1 } } },
		{ { { 15, 0, 0 }, { 0, 30, 0 } } },
		{ { { 15, 0, 1 }, { 0, 30, 1 } } },
		{ { { 15, 0, 2 }, { 0, 30, 2 } } },
		{ { { 15, 0, 3 }, { 0, 31, 1 } } },
		{ { { 15, 0, 4 }, { 0, 31, 0 } } },
		{ { { 16, 0, 4 }, { 0, 31, 1 } } },
		{ { { 16, 0, 3 }, { 0, 31, 2 } } },
		{ { { 16, 0, 2 }, { 1, 31, 1 } } },
		{ { { 16, 0, 1 }, { 1, 31, 0 } } },
		{ { { 16, 0, 0 }, { 4, 28, 0 } } },
		{ { { 16, 0, 1 }, { 4, 28, 1 } } },
		{ { { 16, 0, 2 }, { 2, 31, 1 } } },
		{ { { 16, 0, 3 }, { 2, 31, 0 } } },
		{ { { 16, 0, 4 }, { 4, 29, 0 } } },
		{ { { 17, 0, 3 }, { 4, 29, 1 } } },
		{ { { 17, 0, 2 }, { 3, 31, 1 } } },
		{ { { 17, 0, 1 }, { 3, 31, 0 } } },
		{ { { 17, 0, 0 }, { 4, 30, 0 } } },
		{ { { 17, 0, 1 }, { 4, 30, 1 } } },
		{ { { 17, 0, 2 }, { 4, 30, 2 } } },
		{ { { 17, 0, 3 }, { 4, 31, 1 } } },
		{ { { 17, 0, 4 }, { 4, 31, 0 } } },
		{ { { 18, 0, 3 }, { 4, 31, 1 } } },
		{ { { 18, 0, 2 }, { 4, 31, 2 } } },
		{ { { 18, 0, 1 }, { 5, 31, 1 } } },
		{ { { 18, 0, 0 }, { 5, 31, 0 } } },
		{ { { 18, 0, 1 }, { 5, 31, 1 } } },
		{ { { 18, 0, 2 }, { 5, 31, 2 } } },
		{ { { 18, 0, 3 }, { 6, 31, 1 } } },
		{ { { 18, 0, 4 }, { 6, 31, 0 } } },
		{ { { 19, 0, 3 }, { 6, 31, 1 } } },
		{ { { 19, 0, 2 }, { 6, 31, 2 } } },
		{ { { 19, 0, 1 }, { 7, 31, 1 } } },
		{ { { 19, 0, 0 }, { 7, 31, 0 } } },
		{ { { 19, 0, 1 }, { 7, 31, 1 } } },
		{ { { 19, 0, 2 }, { 7, 31, 2 } } },
		{ { { 19, 0, 3 }, { 8, 31, 1 } } },
		{ { { 19, 0, 4 }, { 8, 31, 0 } } },
		{ { { 20, 0, 4 }, { 8, 31, 1 } } },
		{ { { 20, 0, 3 }, { 8, 31, 2 } } },
		{ { { 20, 0, 2 }, { 9, 31, 1 } } },
		{ { { 20, 0, 1 }, { 9, 31, 0 } } },
		{ { { 20, 0, 0 }, { 12, 28, 0 } } },
		{ { { 20, 0, 1 }, { 12, 28, 1 } } },
		{ { { 20, 0, 2 }, { 10, 31, 1 } } },
		{ { { 20, 0, 3 }, { 10, 31, 0 } } },
		{ { { 20, 0, 4 }, { 12, 29, 0 } } },
		{ { { 21, 0, 3 }, { 12, 29, 1 } } },
		{ { { 21, 0, 2 }, { 11, 31, 1 } } },
		{ { { 21, 0, 1 }, { 11, 31, 0 } } },
		{ { { 21, 0, 0 }, { 12, 30, 0 } } },
		{ { { 21, 0, 1 }, { 12, 30, 1 } } },
		{ { { 21, 0, 2 }, { 12, 30, 2 } } },
		{ { { 21, 0, 3 }, { 12, 31, 1 } } },
		{ { { 21, 0, 4 }, { 12, 31, 0 } } },
		{ { { 22, 0, 3 }, { 12, 31, 1 } } },
		{ { { 22, 0, 2 }, { 12, 31, 2 } } },
		{ { { 22, 0, 1 }, { 13, 31, 1 } } },
		{ { { 22, 0, 0 }, { 13, 31, 0 } } },
		{ { { 22, 0, 1 }, { 13, 31, 1 } } },
		{ { { 22, 0, 2 }, { 13, 31, 2 } } },
		{ { { 22, 0, 3 }, { 14, 31, 1 } } },
		{ { { 22, 0, 4 }, { 14, 31, 0 } } },
		{ { { 23, 0, 3 }, { 14, 31, 1 } } },
		{ { { 23, 0, 2 }, { 14, 31, 2 } } },
		{ { { 23, 0, 1 }, { 15, 31, 1 } } },
		{ { { 23, 0, 0 }, { 15, 31, 0 } } },
		{ { { 23, 0, 1 }, { 15, 31, 1 } } },
		{ { { 23, 0, 2 }, { 15, 31, 2 } } },
		{ { { 23, 0, 3 }, { 16, 31, 1 } } },
		{ { { 23, 0, 4 }, { 16, 31, 0 } } },
		{ { { 24, 0, 4 }, { 16, 31, 1 } } },
		{ { { 24, 0, 3 }, { 16, 31, 2 } } },
		{ { { 24, 0, 2 }, { 17, 31, 1 } } },
		{ { { 24, 0, 1 }, { 17, 31, 0 } } },
		{ { { 24, 0, 0 }, { 20, 28, 0 } } },
		{ { { 24, 0, 1 }, { 20, 28, 1 } } },
		{ { { 24, 0, 2 }, { 18, 31, 1 } } },
		{ { { 24, 0, 3 }, { 18, 31, 0 } } },
		{ { { 24, 0, 4 }, { 20, 29, 0 } } },
		{ { { 25, 0, 3 }, { 20, 29, 1 } } },
		{ { { 25, 0, 2 }, { 19, 31, 1 } } },
		{ { { 25, 0, 1 }, { 19, 31, 0 } } },
		{ { { 25, 0, 0 }, { 20, 30, 0 } } },
		{ { { 25, 0, 1 }, { 20, 30, 1 } } },
		{ { { 25, 0, 2 }, { 20, 30, 2 } } },
		{ { { 25, 0, 3 }, { 20, 31, 1 } } },
		{ { { 25, 0, 4 }, { 20, 31, 0 } } },
		{ { { 26, 0, 3 }, { 20, 31, 1 } } },
		{ { { 26, 0, 2 }, { 20, 31, 2 } } },
		{ { { 26, 0, 1 }, { 21, 31, 1 } } },
		{ { { 26, 0, 0 }, { 21, 31, 0 } } },
		{ { { 26, 0, 1 }, { 21, 31, 1 } } },
		{ { { 26, 0, 2 }, { 21, 31, 2 } } },
		{ { { 26, 0, 3 }, { 22, 31, 1 } } },
		{ { { 26, 0, 4 }, { 22, 31, 0 } } },
		{ { { 27, 0, 3 }, { 22, 31, 1 } } },
		{ { { 27, 0, 2 }, { 22, 31, 2 } } },
		{ { { 27, 0, 1 }, { 23, 31, 1 } } },
		{ { { 27, 0, 0 }, { 23, 31, 0 } } },
		{ { { 27, 0, 1 }, { 23, 31, 1 } } },
		{ { { 27, 0, 2 }, { 23, 31, 2 } } },
		{ { { 27, 0, 3 }, { 24, 31, 1 } } },
		{ { { 27, 0, 4 }, { 24, 31, 0 } } },
		{ { { 28, 0, 4 }, { 24, 31, 1 } } },
		{ { { 28, 0, 3 }, { 24, 31, 2 } } },
		{ { { 28, 0, 2 }, { 25, 31, 1 } } },
		{ { { 28, 0, 1 }, { 25, 31, 0 } } },
		{ { { 28, 0, 0 }, { 28, 28, 0 } } },
		{ { { 28, 0, 1 }, { 28, 28, 1 } } },
		{ { { 28, 0, 2 }, { 26, 31, 1 } } },
		{ { { 28, 0, 3 }, { 26, 31, 0 } } },
		{ { { 28, 0, 4 }, { 28, 29, 0 } } },
		{ { { 29, 0, 3 }, { 28, 29, 1 } } },
		{ { { 29, 0, 2 }, { 27, 31, 1 } } },
		{ { { 29, 0, 1 }, { 27, 31, 0 } } },
		{ { { 29, 0, 0 }, { 28, 30, 0 } } },
		{ { { 29, 0, 1 }, { 28, 30, 1 } } },
		{ { { 29, 0, 2 }, { 28, 30, 2 } } },
		{ { { 29, 0, 3 }, { 28, 31, 1 } } },
		{ { { 29, 0, 4 }, { 28, 31, 0 } } },
		{ { { 30, 0, 3 }, { 28, 31, 1 } } },
		{ { { 30, 0, 2 }, { 28, 31, 2 } } },
		{ { { 30, 0, 1 }, { 29, 31, 1 } } },
		{ { { 30, 0, 0 }, { 29, 31, 0 } } },
		{ { { 30, 0, 1 }, { 29, 31, 1 } } },
		{ { { 30, 0, 2 }, { 29, 31, 2 } } },
		{ { { 30, 0, 3 }, { 30, 31, 1 } } },
		{ { { 30, 0, 4 }, { 30, 31, 0 } } },
		{ { { 31, 0, 3 }, { 30, 31, 1 } } },
		{ { { 31, 0, 2 }, { 30, 31, 2 } } },
		{ { { 31, 0, 1 }, { 31, 31, 1 } } },
		{ { { 31, 0, 0 }, { 31, 31, 0 } } }
	};

	const SingleColourLookup lookup_6_3[] =
	{
		{ { { 0, 0, 0 }, { 0, 0, 0 } } },
		{ { { 0, 0, 1 }, { 0, 1, 1 } } },
		{ { { 0, 0, 2 }, { 0, 1, 0 } } },
		{ { { 1, 0, 1 }, { 0, 2, 1 } } },
		{ { { 1, 0, 0 }, { 0, 2, 0 } } },
		{ { { 1, 0, 1 }, { 0, 3, 1 } } },
		{ { { 1, 0, 2 }, { 0, 3, 0 } } },
		{ { { 2, 0, 1 }, { 0, 4, 1 } } },
		{ { { 2, 0, 0 }, { 0, 4, 0 } } },
		{ { { 2, 0, 1 }, { 0, 5, 1 } } },
		{ { { 2, 0, 2 }, { 0, 5, 0 } } },
		{ { { 3, 0, 1 }, { 0, 6, 1 } } },
		{ { { 3, 0, 0 }, { 0, 6, 0 } } },
		{ { { 3, 0, 1 }, { 0, 7, 1 } } },
		{ { { 3, 0, 2 }, { 0, 7, 0 } } },
		{ { { 4, 0, 1 }, { 0, 8, 1 } } },
		{ { { 4, 0, 0 }, { 0, 8, 0 } } },
		{ { { 4, 0, 1 }, { 0, 9, 1 } } },
		{ { { 4, 0, 2 }, { 0, 9, 0 } } },
		{ { { 5, 0, 1 }, { 0, 10, 1 } } },
		{ { { 5, 0, 0 }, { 0, 10, 0 } } },
		{ { { 5, 0, 1 }, { 0, 11, 1 } } },
		{ { { 5, 0, 2 }, { 0, 11, 0 } } },
		{ { { 6, 0, 1 }, { 0, 12, 1 } } },
		{ { { 6, 0, 0 }, { 0, 12, 0 } } },
		{ { { 6, 0, 1 }, { 0, 13, 1 } } },
		{ { { 6, 0, 2 }, { 0, 13, 0 } } },
		{ { { 7, 0, 1 }, { 0, 14, 1 } } },
		{ { { 7, 0, 0 }, { 0, 14, 0 } } },
		{ { { 7, 0, 1 }, { 0, 15, 1 } } },
		{ { { 7, 0, 2 }, { 0, 15, 0 } } },
		{ { { 8, 0, 1 }, { 0, 16, 1 } } },
		{ { { 8, 0, 0 }, { 0, 16, 0 } } },
		{ { { 8, 0, 1 }, { 0, 17, 1 } } },
		{ { { 8, 0, 2 }, { 0, 17, 0 } } },
		{ { { 9, 0, 1 }, { 0, 18, 1 } } },
		{ { { 9, 0, 0 }, { 0, 18, 0 } } },
		{ { { 9, 0, 1 }, { 0, 19, 1 } } },
		{ { { 9, 0, 2 }, { 0, 19, 0 } } },
		{ { { 10, 0, 1 }, { 0, 20, 1 } } },
		{ { { 10, 0, 0 }, { 0, 20, 0 } } },
		{ { { 10, 0, 1 }, { 0, 21, 1 } } },
		{ { { 10, 0, 2 }, { 0, 21, 0 } } },
		{ { { 11, 0, 1 }, { 0, 22, 1 } } },
		{ { { 11, 0, 0 }, { 0, 22, 0 } } },
		{ { { 11, 0, 1 }, { 0, 23, 1 } } },
		{ { { 11, 0, 2 }, { 0, 23, 0 } } },
		{ { { 12, 0, 1 }, { 0, 24, 1 } } },
		{ { { 12, 0, 0 }, { 0, 24, 0 } } },
		{ { { 12, 0, 1 }, { 0, 25, 1 } } },
		{ { { 12, 0, 2 }, { 0, 25, 0 } } },
		{ { { 13, 0, 1 }, { 0, 26, 1 } } },
		{ { { 13, 0, 0 }, { 0, 26, 0 } } },
		{ { { 13, 0, 1 }, { 0, 27, 1 } } },
		{ { { 13, 0, 2 }, { 0, 27, 0 } } },
		{ { { 14, 0, 1 }, { 0, 28, 1 } } },
		{ { { 14, 0, 0 }, { 0, 28, 0 } } },
		{ { { 14, 0, 1 }, { 0, 29, 1 } } },
		{ { { 14, 0, 2 }, { 0, 29, 0 } } },
		{ { { 15, 0, 1 }, { 0, 30, 1 } } },
		{ { { 15, 0, 0 }, { 0, 30, 0 } } },
		{ { { 15, 0, 1 }, { 0, 31, 1 } } },
		{ { { 15, 0, 2 }, { 0, 31, 0 } } },
		{ { { 16, 0, 2 }, { 1, 31, 1 } } },
		{ { { 16, 0, 1 }, { 1, 31, 0 } } },
		{ { { 16, 0, 0 }, { 0, 32, 0 } } },
		{ { { 16, 0, 1 }, { 2, 31, 0 } } },
		{ { { 16, 0, 2 }, { 0, 33, 0 } } },
		{ { { 17, 0, 1 }, { 3, 31, 0 } } },
		{ { { 17, 0, 0 }, { 0, 34, 0 } } },
		{ { { 17, 0, 1 }, { 4, 31, 0 } } },
		{ { { 17, 0, 2 }, { 0, 35, 0 } } },
		{ { { 18, 0, 1 }, { 5, 31, 0 } } },
		{ { { 18, 0, 0 }, { 0, 36, 0 } } },
		{ { { 18, 0, 1 }, { 6, 31, 0 } } },
		{ { { 18, 0, 2 }, { 0, 37, 0 } } },
		{ { { 19, 0, 1 }, { 7, 31, 0 } } },
		{ { { 19, 0, 0 }, { 0, 38, 0 } } },
		{ { { 19, 0, 1 }, { 8, 31, 0 } } },
		{ { { 19, 0, 2 }, { 0, 39, 0 } } },
		{ { { 20, 0, 1 }, { 9, 31, 0 } } },
		{ { { 20, 0, 0 }, { 0, 40, 0 } } },
		{ { { 20, 0, 1 }, { 10, 31, 0 } } },
		{ { { 20, 0, 2 }, { 0, 41, 0 } } },
		{ { { 21, 0, 1 }, { 11, 31, 0 } } },
		{ { { 21, 0, 0 }, { 0, 42, 0 } } },
		{ { { 21, 0, 1 }, { 12, 31, 0 } } },
		{ { { 21, 0, 2 }, { 0, 43, 0 } } },
		{ { { 22, 0, 1 }, { 13, 31, 0 } } },
		{ { { 22, 0, 0 }, { 0, 44, 0 } } },
		{ { { 22, 0, 1 }, { 14, 31, 0 } } },
		{ { { 22, 0, 2 }, { 0, 45, 0 } } },
		{ { { 23, 0, 1 }, { 15, 31, 0 } } },
		{ { { 23, 0, 0 }, { 0, 46, 0 } } },
		{ { { 23, 0, 1 }, { 0, 47, 1 } } },
		{ { { 23, 0, 2 }, { 0, 47, 0 } } },
		{ { { 24, 0, 1 }, { 0, 48, 1 } } },
		{ { { 24, 0, 0 }, { 0, 48, 0 } } },
		{ { { 24, 0, 1 }, { 0, 49, 1 } } },
		{ { { 24, 0, 2 }, { 0, 49, 0 } } },
		{ { { 25, 0, 1 }, { 0, 50, 1 } } },
		{ { { 25, 0, 0 }, { 0, 50, 0 } } },
		{ { { 25, 0, 1 }, { 0, 51, 1 } } },
		{ { { 25, 0, 2 }, { 0, 51, 0 } } },
		{ { { 26, 0, 1 }, { 0, 52, 1 } } },
		{ { { 26, 0, 0 }, { 0, 52, 0 } } },
		{ { { 26, 0, 1 }, { 0, 53, 1 } } },
		{ { { 26, 0, 2 }, { 0, 53, 0 } } },
		{ { { 27, 0, 1 }, { 0, 54, 1 } } },
		{ { { 27, 0, 0 }, { 0, 54, 0 } } },
		{ { { 27, 0, 1 }, { 0, 55, 1 } } },
		{ { { 27, 0, 2 }, { 0, 55, 0 } } },
		{ { { 28, 0, 1 }, { 0, 56, 1 } } },
		{ { { 28, 0, 0 }, { 0, 56, 0 } } },
		{ { { 28, 0, 1 }, { 0, 57, 1 } } },
		{ { { 28, 0, 2 }, { 0, 57, 0 } } },
		{ { { 29, 0, 1 }, { 0, 58, 1 } } },
		{ { { 29, 0, 0 }, { 0, 58, 0 } } },
		{ { { 29, 0, 1 }, { 0, 59, 1 } } },
		{ { { 29, 0, 2 }, { 0, 59, 0 } } },
		{ { { 30, 0, 1 }, { 0, 60, 1 } } },
		{ { { 30, 0, 0 }, { 0, 60, 0 } } },
		{ { { 30, 0, 1 }, { 0, 61, 1 } } },
		{ { { 30, 0, 2 }, { 0, 61, 0 } } },
		{ { { 31, 0, 1 }, { 0, 62, 1 } } },
		{ { { 31, 0, 0 }, { 0, 62, 0 } } },
		{ { { 31, 0, 1 }, { 0, 63, 1 } } },
		{ { { 31, 0, 2 }, { 0, 63, 0 } } },
		{ { { 32, 0, 2 }, { 1, 63, 1 } } },
		{ { { 32, 0, 1 }, { 1, 63, 0 } } },
		{ { { 32, 0, 0 }, { 16, 48, 0 } } },
		{ { { 32, 0, 1 }, { 2, 63, 0 } } },
		{ { { 32, 0, 2 }, { 16, 49, 0 } } },
		{ { { 33, 0, 1 }, { 3, 63, 0 } } },
		{ { { 33, 0, 0 }, { 16, 50, 0 } } },
		{ { { 33, 0, 1 }, { 4, 63, 0 } } },
		{ { { 33, 0, 2 }, { 16, 51, 0 } } },
		{ { { 34, 0, 1 }, { 5, 63, 0 } } },
		{ { { 34, 0, 0 }, { 16, 52, 0 } } },
		{ { { 34, 0, 1 }, { 6, 63, 0 } } },
		{ { { 34, 0, 2 }, { 16, 53, 0 } } },
		{ { { 35, 0, 1 }, { 7, 63, 0 } } },
		{ { { 35, 0, 0 }, { 16, 54, 0 } } },
		{ { { 35, 0, 1 }, { 8, 63, 0 } } },
		{ { { 35, 0, 2 }, { 16, 55, 0 } } },
		{ { { 36, 0, 1 }, { 9, 63, 0 } } },
		{ { { 36, 0, 0 }, { 16, 56, 0 } } },
		{ { { 36, 0, 1 }, { 10, 63, 0 } } },
		{ { { 36, 0, 2 }, { 16, 57, 0 } } },
		{ { { 37, 0, 1 }, { 11, 63, 0 } } },
		{ { { 37, 0, 0 }, { 16, 58, 0 } } },
		{ { { 37, 0, 1 }, { 12, 63, 0 } } },
		{ { { 37, 0, 2 }, { 16, 59, 0 } } },
		{ { { 38, 0, 1 }, { 13, 63, 0 } } },
		{ { { 38, 0, 0 }, { 16, 60, 0 } } },
		{ { { 38, 0, 1 }, { 14, 63, 0 } } },
		{ { { 38, 0, 2 }, { 16, 61, 0 } } },
		{ { { 39, 0, 1 }, { 15, 63, 0 } } },
		{ { { 39, 0, 0 }, { 16, 62, 0 } } },
		{ { { 39, 0, 1 }, { 16, 63, 1 } } },
		{ { { 39, 0, 2 }, { 16, 63, 0 } } },
		{ { { 40, 0, 1 }, { 17, 63, 1 } } },
		{ { { 40, 0, 0 }, { 17, 63, 0 } } },
		{ { { 40, 0, 1 }, { 18, 63, 1 } } },
		{ { { 40, 0, 2 }, { 18, 63, 0 } } },
		{ { { 41, 0, 1 }, { 19, 63, 1 } } },
		{ { { 41, 0, 0 }, { 19, 63, 0 } } },
		{ { { 41, 0, 1 }, { 20, 63, 1 } } },
		{ { { 41, 0, 2 }, { 20, 63, 0 } } },
		{ { { 42, 0, 1 }, { 21, 63, 1 } } },
		{ { { 42, 0, 0 }, { 21, 63, 0 } } },
		{ { { 42, 0, 1 }, { 22, 63, 1 } } },
		{ { { 42, 0, 2 }, { 22, 63, 0 } } },
		{ { { 43, 0, 1 }, { 23, 63, 1 } } },
		{ { { 43, 0, 0 }, { 23, 63, 0 } } },
		{ { { 43, 0, 1 }, { 24, 63, 1 } } },
		{ { { 43, 0, 2 }, { 24, 63, 0 } } },
		{ { { 44, 0, 1 }, { 25, 63, 1 } } },
		{ { { 44, 0, 0 }, { 25, 63, 0 } } },
		{ { { 44, 0, 1 }, { 26, 63, 1 } } },
		{ { { 44, 0, 2 }, { 26, 63, 0 } } },
		{ { { 45, 0, 1 }, { 27, 63, 1 } } },
		{ { { 45, 0, 0 }, { 27, 63, 0 } } },
		{ { { 45, 0, 1 }, { 28, 63, 1 } } },
		{ { { 45, 0, 2 }, { 28, 63, 0 } } },
		{ { { 46, 0, 1 }, { 29, 63, 1 } } },
		{ { { 46, 0, 0 }, { 29, 63, 0 } } },
		{ { { 46, 0, 1 }, { 30, 63, 1 } } },
		{ { { 46, 0, 2 }, { 30, 63, 0 } } },
		{ { { 47, 0, 1 }, { 31, 63, 1 } } },
		{ { { 47, 0, 0 }, { 31, 63, 0 } } },
		{ { { 47, 0, 1 }, { 32, 63, 1 } } },
		{ { { 47, 0, 2 }, { 32, 63, 0 } } },
		{ { { 48, 0, 2 }, { 33, 63, 1 } } },
		{ { { 48, 0, 1 }, { 33, 63, 0 } } },
		{ { { 48, 0, 0 }, { 48, 48, 0 } } },
		{ { { 48, 0, 1 }, { 34, 63, 0 } } },
		{ { { 48, 0, 2 }, { 48, 49, 0 } } },
		{ { { 49, 0, 1 }, { 35, 63, 0 } } },
		{ { { 49, 0, 0 }, { 48, 50, 0 } } },
		{ { { 49, 0, 1 }, { 36, 63, 0 } } },
		{ { { 49, 0, 2 }, { 48, 51, 0 } } },
		{ { { 50, 0, 1 }, { 37, 63, 0 } } },
		{ { { 50, 0, 0 }, { 48, 52, 0 } } },
		{ { { 50, 0, 1 }, { 38, 63, 0 } } },
		{ { { 50, 0, 2 }, { 48, 53, 0 } } },
		{ { { 51, 0, 1 }, { 39, 63, 0 } } },
		{ { { 51, 0, 0 }, { 48, 54, 0 } } },
		{ { { 51, 0, 1 }, { 40, 63, 0 } } },
		{ { { 51, 0, 2 }, { 48, 55, 0 } } },
		{ { { 52, 0, 1 }, { 41, 63, 0 } } },
		{ { { 52, 0, 0 }, { 48, 56, 0 } } },
		{ { { 52, 0, 1 }, { 42, 63, 0 } } },
		{ { { 52, 0, 2 }, { 48, 57, 0 } } },
		{ { { 53, 0, 1 }, { 43, 63, 0 } } },
		{ { { 53, 0, 0 }, { 48, 58, 0 } } },
		{ { { 53, 0, 1 }, { 44, 63, 0 } } },
		{ { { 53, 0, 2 }, { 48, 59, 0 } } },
		{ { { 54, 0, 1 }, { 45, 63, 0 } } },
		{ { { 54, 0, 0 }, { 48, 60, 0 } } },
		{ { { 54, 0, 1 }, { 46, 63, 0 } } },
		{ { { 54, 0, 2 }, { 48, 61, 0 } } },
		{ { { 55, 0, 1 }, { 47, 63, 0 } } },
		{ { { 55, 0, 0 }, { 48, 62, 0 } } },
		{ { { 55, 0, 1 }, { 48, 63, 1 } } },
		{ { { 55, 0, 2 }, { 48, 63, 0 } } },
		{ { { 56, 0, 1 }, { 49, 63, 1 } } },
		{ { { 56, 0, 0 }, { 49, 63, 0 } } },
		{ { { 56, 0, 1 }, { 50, 63, 1 } } },
		{ { { 56, 0, 2 }, { 50, 63, 0 } } },
		{ { { 57, 0, 1 }, { 51, 63, 1 } } },
		{ { { 57, 0, 0 }, { 51, 63, 0 } } },
		{ { { 57, 0, 1 }, { 52, 63, 1 } } },
		{ { { 57, 0, 2 }, { 52, 63, 0 } } },
		{ { { 58, 0, 1 }, { 53, 63, 1 } } },
		{ { { 58, 0, 0 }, { 53, 63, 0 } } },
		{ { { 58, 0, 1 }, { 54, 63, 1 } } },
		{ { { 58, 0, 2 }, { 54, 63, 0 } } },
		{ { { 59, 0, 1 }, { 55, 63, 1 } } },
		{ { { 59, 0, 0 }, { 55, 63, 0 } } },
		{ { { 59, 0, 1 }, { 56, 63, 1 } } },
		{ { { 59, 0, 2 }, { 56, 63, 0 } } },
		{ { { 60, 0, 1 }, { 57, 63, 1 } } },
		{ { { 60, 0, 0 }, { 57, 63, 0 } } },
		{ { { 60, 0, 1 }, { 58, 63, 1 } } },
		{ { { 60, 0, 2 }, { 58, 63, 0 } } },
		{ { { 61, 0, 1 }, { 59, 63, 1 } } },
		{ { { 61, 0, 0 }, { 59, 63, 0 } } },
		{ { { 61, 0, 1 }, { 60, 63, 1 } } },
		{ { { 61, 0, 2 }, { 60, 63, 0 } } },
		{ { { 62, 0, 1 }, { 61, 63, 1 } } },
		{ { { 62, 0, 0 }, { 61, 63, 0 } } },
		{ { { 62, 0, 1 }, { 62, 63, 1 } } },
		{ { { 62, 0, 2 }, { 62, 63, 0 } } },
		{ { { 63, 0, 1 }, { 63, 63, 1 } } },
		{ { { 63, 0, 0 }, { 63, 63, 0 } } }
	};

	const SingleColourLookup lookup_5_4[] =
	{
		{ { { 0, 0, 0 }, { 0, 0, 0 } } },
		{ { { 0, 0, 1 }, { 0, 1, 1 } } },
		{ { { 0, 0, 2 }, { 0, 1, 0 } } },
		{ { { 0, 0, 3 }, { 0, 1, 1 } } },
		{ { { 0, 0, 4 }, { 0, 2, 1 } } },
		{ { { 1, 0, 3 }, { 0, 2, 0 } } },
		{ { { 1, 0, 2 }, { 0, 2, 1 } } },
		{ { { 1, 0, 1 }, { 0, 3, 1 } } },
		{ { { 1, 0, 0 }, { 0, 3, 0 } } },
		{ { { 1, 0, 1 }, { 1, 2, 1 } } },
		{ { { 1, 0, 2 }, { 1, 2, 0 } } },
		{ { { 1, 0, 3 }, { 0, 4, 0 } } },
		{ { { 1, 0, 4 }, { 0, 5, 1 } } },
		{ { { 2, 0, 3 }, { 0, 5, 0 } } },
		{ { { 2, 0, 2 }, { 0, 5, 1 } } },
		{ { { 2, 0, 1 }, { 0, 6, 1 } } },
		{ { { 2, 0, 0 }, { 0, 6, 0 } } },
		{ { { 2, 0, 1 }, { 2, 3, 1 } } },
		{ { { 2, 0, 2 }, { 2, 3, 0 } } },
		{ { { 2, 0, 3 }, { 0, 7, 0 } } },
		{ { { 2, 0, 4 }, { 1, 6, 1 } } },
		{ { { 3, 0, 3 }, { 1, 6, 0 } } },
		{ { { 3, 0, 2 }, { 0, 8, 0 } } },
		{ { { 3, 0, 1 }, { 0, 9, 1 } } },
		{ { { 3, 0, 0 }, { 0, 9, 0 } } },
		{ { { 3, 0, 1 }, { 0, 9, 1 } } },
		{ { { 3, 0, 2 }, { 0, 10, 1 } } },
		{ { { 3, 0, 3 }, { 0, 10, 0 } } },
		{ { { 3, 0, 4 }, { 2, 7, 1 } } },
		{ { { 4, 0, 4 }, { 2, 7, 0 } } },
		{ { { 4, 0, 3 }, { 0, 11, 0 } } },
		{ { { 4, 0, 2 }, { 1, 10, 1 } } },
		{ { { 4, 0, 1 }, { 1, 10, 0 } } },
		{ { { 4, 0, 0 }, { 0, 12, 0 } } },
		{ { { 4, 0, 1 }, { 0, 13, 1 } } },
		{ { { 4, 0, 2 }, { 0, 13, 0 } } },
		{ { { 4, 0, 3 }, { 0, 13, 1 } } },
		{ { { 4, 0, 4 }, { 0, 14, 1 } } },
		{ { { 5, 0, 3 }, { 0, 14, 0 } } },
		{ { { 5, 0, 2 }, { 2, 11, 1 } } },
		{ { { 5, 0, 1 }, { 2, 11, 0 } } },
		{ { { 5, 0, 0 }, { 0, 15, 0 } } },
		{ { { 5, 0, 1 }, { 1, 14, 1 } } },
		{ { { 5, 0, 2 }, { 1, 14, 0 } } },
		{ { { 5, 0, 3 }, { 0, 16, 0 } } },
		{ { { 5, 0, 4 }, { 0, 17, 1 } } },
		{ { { 6, 0, 3 }, { 0, 17, 0 } } },
		{ { { 6, 0, 2 }, { 0, 17, 1 } } },
		{ { { 6, 0, 1 }, { 0, 18, 1 } } },
		{ { { 6, 0, 0 }, { 0, 18, 0 } } },
		{ { { 6, 0, 1 }, { 2, 15, 1 } } },
		{ { { 6, 0, 2 }, { 2, 15, 0 } } },
		{ { { 6, 0, 3 }, { 0, 19, 0 } } },
		{ { { 6, 0, 4 }, { 1, 18, 1 } } },
		{ { { 7, 0, 3 }, { 1, 18, 0 } } },
		{ { { 7, 0, 2 }, { 0, 20, 0 } } },
		{ { { 7, 0, 1 }, { 0, 21, 1 } } },
		{ { { 7, 0, 0 }, { 0, 21, 0 } } },
		{ { { 7, 0, 1 }, { 0, 21, 1 } } },
		{ { { 7, 0, 2 }, { 0, 22, 1 } } },
		{ { { 7, 0, 3 }, { 0, 22, 0 } } },
		{ { { 7, 0, 4 }, { 2, 19, 1 } } },
		{ { { 8, 0, 4 }, { 2, 19, 0 } } },
		{ { { 8, 0, 3 }, { 0, 23, 0 } } },
		{ { { 8, 0, 2 }, { 1, 22, 1 } } },
		{ { { 8, 0, 1 }, { 1, 22, 0 } } },
		{ { { 8, 0, 0 }, { 0, 24, 0 } } },
		{ { { 8, 0, 1 }, { 0, 25, 1 } } },
		{ { { 8, 0, 2 }, { 0, 25, 0 } } },
		{ { { 8, 0, 3 }, { 0, 25, 1 } } },
		{ { { 8, 0, 4 }, { 0, 26, 1 } } },
		{ { { 9, 0, 3 }, { 0, 26, 0 } } },
		{ { { 9, 0, 2 }, { 2, 23, 1 } } },
		{ { { 9, 0, 1 }, { 2, 23, 0 } } },
		{ { { 9, 0, 0 }, { 0, 27, 0 } } },
		{ { { 9, 0, 1 }, { 1, 26, 1 } } },
		{ { { 9, 0, 2 }, { 1, 26, 0 } } },
		{ { { 9, 0, 3 }, { 0, 28, 0 } } },
		{ { { 9, 0, 4 }, { 0, 29, 1 } } },
		{ { { 10, 0, 3 }, { 0, 29, 0 } } },
		{ { { 10, 0, 2 }, { 0, 29, 1 } } },
		{ { { 10, 0, 1 }, { 0, 30, 1 } } },
		{ { { 10, 0, 0 }, { 0, 30, 0 } } },
		{ { { 10, 0, 1 }, { 2, 27, 1 } } },
		{ { { 10, 0, 2 }, { 2, 27, 0 } } },
		{ { { 10, 0, 3 }, { 0, 31, 0 } } },
		{ { { 10, 0, 4 }, { 1, 30, 1 } } },
		{ { { 11, 0, 3 }, { 1, 30, 0 } } },
		{ { { 11, 0, 2 }, { 4, 24, 0 } } },
		{ { { 11, 0, 1 }, { 1, 31, 1 } } },
		{ { { 11, 0, 0 }, { 1, 31, 0 } } },
		{ { { 11, 0, 1 }, { 1, 31, 1 } } },
		{ { { 11, 0, 2 }, { 2, 30, 1 } } },
		{ { { 11, 0, 3 }, { 2, 30, 0 } } },
		{ { { 11, 0, 4 }, { 2, 31, 1 } } },
		{ { { 12, 0, 4 }, { 2, 31, 0 } } },
		{ { { 12, 0, 3 }, { 4, 27, 0 } } },
		{ { { 12, 0, 2 }, { 3, 30, 1 } } },
		{ { { 12, 0, 1 }, { 3, 30, 0 } } },
		{ { { 12, 0, 0 }, { 4, 28, 0 } } },
		{ { { 12, 0, 1 }, { 3, 31, 1 } } },
		{ { { 12, 0, 2 }, { 3, 31, 0 } } },
		{ { { 12, 0, 3 }, { 3, 31, 1 } } },
		{ { { 12, 0, 4 }, { 4, 30, 1 } } },
		{ { { 13, 0, 3 }, { 4, 30, 0 } } },
		{ { { 13, 0, 2 }, { 6, 27, 1 } } },
		{ { { 13, 0, 1 }, { 6, 27, 0 } } },
		{ { { 13, 0, 0 }, { 4, 31, 0 } } },
		{ { { 13, 0, 1 }, { 5, 30, 1 } } },
		{ { { 13, 0, 2 }, { 5, 30, 0 } } },
		{ { { 13, 0, 3 }, { 8, 24, 0 } } },
		{ { { 13, 0, 4 }, { 5, 31, 1 } } },
		{ { { 14, 0, 3 }, { 5, 31, 0 } } },
		{ { { 14, 0, 2 }, { 5, 31, 1 } } },
		{ { { 14, 0, 1 }, { 6, 30, 1 } } },
		{ { { 14, 0, 0 }, { 6, 30, 0 } } },
		{ { { 14, 0, 1 }, { 6, 31, 1 } } },
		{ { { 14, 0, 2 }, { 6, 31, 0 } } },
		{ { { 14, 0, 3 }, { 8, 27, 0 } } },
		{ { { 14, 0, 4 }, { 7, 30, 1 } } },
		{ { { 15, 0, 3 }, { 7, 30, 0 } } },
		{ { { 15, 0, 2 }, { 8, 28, 0 } } },
		{ { { 15, 0, 1 }, { 7, 31, 1 } } },
		{ { { 15, 0, 0 }, { 7, 31, 0 } } },
		{ { { 15, 0, 1 }, { 7, 31, 1 } } },
		{ { { 15, 0, 2 }, { 8, 30, 1 } } },
		{ { { 15, 0, 3 }, { 8, 30, 0 } } },
		{ { { 15, 0, 4 }, { 10, 27, 1 } } },
		{ { { 16, 0, 4 }, { 10, 27, 0 } } },
		{ { { 16, 0, 3 }, { 8, 31, 0 } } },
		{ { { 16, 0, 2 }, { 9, 30, 1 } } },
		{ { { 16, 0, 1 }, { 9, 30, 0 } } },
		{ { { 16, 0, 0 }, { 12, 24, 0 } } },
		{ { { 16, 0, 1 }, { 9, 31, 1 } } },
		{ { { 16, 0, 2 }, { 9, 31, 0 } } },
		{ { { 16, 0, 3 }, { 9, 31, 1 } } },
		{ { { 16, 0, 4 }, { 10, 30, 1 } } },
		{ { { 17, 0, 3 }, { 10, 30, 0 } } },
		{ { { 17, 0, 2 }, { 10, 31, 1 } } },
		{ { { 17, 0, 1 }, { 10, 31, 0 } } },
		{ { { 17, 0, 0 }, { 12, 27, 0 } } },
		{ { { 17, 0, 1 }, { 11, 30, 1 } } },
		{ { { 17, 0, 2 }, { 11, 30, 0 } } },
		{ { { 17, 0, 3 }, { 12, 28, 0 } } },
		{ { { 17, 0, 4 }, { 11, 31, 1 } } },
		{ { { 18, 0, 3 }, { 11, 31, 0 } } },
		{ { { 18, 0, 2 }, { 11, 31, 1 } } },
		{ { { 18, 0, 1 }, { 12, 30, 1 } } },
		{ { { 18, 0, 0 }, { 12, 30, 0 } } },
		{ { { 18, 0, 1 }, { 14, 27, 1 } } },
		{ { { 18, 0, 2 }, { 14, 27, 0 } } },
		{ { { 18, 0, 3 }, { 12, 31, 0 } } },
		{ { { 18, 0, 4 }, { 13, 30, 1 } } },
		{ { { 19, 0, 3 }, { 13, 30, 0 } } },
		{ { { 19, 0, 2 }, { 16, 24, 0 } } },
		{ { { 19, 0, 1 }, { 13, 31, 1 } } },
		{ { { 19, 0, 0 }, { 13, 31, 0 } } },
		{ { { 19, 0, 1 }, { 13, 31, 1 } } },
		{ { { 19, 0, 2 }, { 14, 30, 1 } } },
		{ { { 19, 0, 3 }, { 14, 30, 0 } } },
		{ { { 19, 0, 4 }, { 14, 31, 1 } } },
		{ { { 20, 0, 4 }, { 14, 31, 0 } } },
		{ { { 20, 0, 3 }, { 16, 27, 0 } } },
		{ { { 20, 0, 2 }, { 15, 30, 1 } } },
		{ { { 20, 0, 1 }, { 15, 30, 0 } } },
		{ { { 20, 0, 0 }, { 16, 28, 0 } } },
		{ { { 20, 0, 1 }, { 15, 31, 1 } } },
		{ { { 20, 0, 2 }, { 15, 31, 0 } } },
		{ { { 20, 0, 3 }, { 15, 31, 1 } } },
		{ { { 20, 0, 4 }, { 16, 30, 1 } } },
		{ { { 21, 0, 3 }, { 16, 30, 0 } } },
		{ { { 21, 0, 2 }, { 18, 27, 1 } } },
		{ { { 21, 0, 1 }, { 18, 27, 0 } } },
		{ { { 21, 0, 0 }, { 16, 31, 0 } } },
		{ { { 21, 0, 1 }, { 17, 30, 1 } } },
		{ { { 21, 0, 2 }, { 17, 30, 0 } } },
		{ { { 21, 0, 3 }, { 20, 24, 0 } } },
		{ { { 21, 0, 4 }, { 17, 31, 1 } } },
		{ { { 22, 0, 3 }, { 17, 31, 0 } } },
		{ { { 22, 0, 2 }, { 17, 31, 1 } } },
		{ { { 22, 0, 1 }, { 18, 30, 1 } } },
		{ { { 22, 0, 0 }, { 18, 30, 0 } } },
		{ { { 22, 0, 1 }, { 18, 31, 1 } } },
		{ { { 22, 0, 2 }, { 18, 31, 0 } } },
		{ { { 22, 0, 3 }, { 20, 27, 0 } } },
		{ { { 22, 0, 4 }, { 19, 30, 1 } } },
		{ { { 23, 0, 3 }, { 19, 30, 0 } } },
		{ { { 23, 0, 2 }, { 20, 28, 0 } } },
		{ { { 23, 0, 1 }, { 19, 31, 1 } } },
		{ { { 23, 0, 0 }, { 19, 31, 0 } } },
		{ { { 23, 0, 1 }, { 19, 31, 1 } } },
		{ { { 23, 0, 2 }, { 20, 30, 1 } } },
		{ { { 23, 0, 3 }, { 20, 30, 0 } } },
		{ { { 23, 0, 4 }, { 22, 27, 1 } } },
		{ { { 24, 0, 4 }, { 22, 27, 0 } } },
		{ { { 24, 0, 3 }, { 20, 31, 0 } } },
		{ { { 24, 0, 2 }, { 21, 30, 1 } } },
		{ { { 24, 0, 1 }, { 21, 30, 0 } } },
		{ { { 24, 0, 0 }, { 24, 24, 0 } } },
		{ { { 24, 0, 1 }, { 21, 31, 1 } } },
		{ { { 24, 0, 2 }, { 21, 31, 0 } } },
		{ { { 24, 0, 3 }, { 21, 31, 1 } } },
		{ { { 24, 0, 4 }, { 22, 30, 1 } } },
		{ { { 25, 0, 3 }, { 22, 30, 0 } } },
		{ { { 25, 0, 2 }, { 22, 31, 1 } } },
		{ { { 25, 0, 1 }, { 22, 31, 0 } } },
		{ { { 25, 0, 0 }, { 24, 27, 0 } } },
		{ { { 25, 0, 1 }, { 23, 30, 1 } } },
		{ { { 25, 0, 2 }, { 23, 30, 0 } } },
		{ { { 25, 0, 3 }, { 24, 28, 0 } } },
		{ { { 25, 0, 4 }, { 23, 31, 1 } } },
		{ { { 26, 0, 3 }, { 23, 31, 0 } } },
		{ { { 26, 0, 2 }, { 23, 31, 1 } } },
		{ { { 26, 0, 1 }, { 24, 30, 1 } } },
		{ { { 26, 0, 0 }, { 24, 30, 0 } } },
		{ { { 26, 0, 1 }, { 26, 27, 1 } } },
		{ { { 26, 0, 2 }, { 26, 27, 0 } } },
		{ { { 26, 0, 3 }, { 24, 31, 0 } } },
		{ { { 26, 0, 4 }, { 25, 30, 1 } } },
		{ { { 27, 0, 3 }, { 25, 30, 0 } } },
		{ { { 27, 0, 2 }, { 28, 24, 0 } } },
		{ { { 27, 0, 1 }, { 25, 31, 1 } } },
		{ { { 27, 0, 0 }, { 25, 31, 0 } } },
		{ { { 27, 0, 1 }, { 25, 31, 1 } } },
		{ { { 27, 0, 2 }, { 26, 30, 1 } } },
		{ { { 27, 0, 3 }, { 26, 30, 0 } } },
		{ { { 27, 0, 4 }, { 26, 31, 1 } } },
		{ { { 28, 0, 4 }, { 26, 31, 0 } } },
		{ { { 28, 0, 3 }, { 28, 27, 0 } } },
		{ { { 28, 0, 2 }, { 27, 30, 1 } } },
		{ { { 28, 0, 1 }, { 27, 30, 0 } } },
		{ { { 28, 0, 0 }, { 28, 28, 0 } } },
		{ { { 28, 0, 1 }, { 27, 31, 1 } } },
		{ { { 28, 0, 2 }, { 27, 31, 0 } } },
		{ { { 28, 0, 3 }, { 27, 31, 1 } } },
		{ { { 28, 0, 4 }, { 28, 30, 1 } } },
		{ { { 29, 0, 3 }, { 28, 30, 0 } } },
		{ { { 29, 0, 2 }, { 30, 27, 1 } } },
		{ { { 29, 0, 1 }, { 30, 27, 0 } } },
		{ { { 29, 0, 0 }, { 28, 31, 0 } } },
		{ { { 29, 0, 1 }, { 29, 30, 1 } } },
		{ { { 29, 0, 2 }, { 29, 30, 0 } } },
		{ { { 29, 0, 3 }, { 29, 30, 1 } } },
		{ { { 29, 0, 4 }, { 29, 31, 1 } } },
		{ { { 30, 0, 3 }, { 29, 31, 0 } } },
		{ { { 30, 0, 2 }, { 29, 31, 1 } } },
		{ { { 30, 0, 1 }, { 30, 30, 1 } } },
		{ { { 30, 0, 0 }, { 30, 30, 0 } } },
		{ { { 30, 0, 1 }, { 30, 31, 1 } } },
		{ { { 30, 0, 2 }, { 30, 31, 0 } } },
		{ { { 30, 0, 3 }, { 30, 31, 1 } } },
		{ { { 30, 0, 4 }, { 31, 30, 1 } } },
		{ { { 31, 0, 3 }, { 31, 30, 0 } } },
		{ { { 31, 0, 2 }, { 31, 30, 1 } } },
		{ { { 31, 0, 1 }, { 31, 31, 1 } } },
		{ { { 31, 0, 0 }, { 31, 31, 0 } } }
	};

	const SingleColourLookup lookup_6_4[] =
	{
		{ { { 0, 0, 0 }, { 0, 0, 0 } } },
		{ { { 0, 0, 1 }, { 0, 1, 0 } } },
		{ { { 0, 0, 2 }, { 0, 2, 0 } } },
		{ { { 1, 0, 1 }, { 0, 3, 1 } } },
		{ { { 1, 0, 0 }, { 0, 3, 0 } } },
		{ { { 1, 0, 1 }, { 0, 4, 0 } } },
		{ { { 1, 0, 2 }, { 0, 5, 0 } } },
		{ { { 2, 0, 1 }, { 0, 6, 1 } } },
		{ { { 2, 0, 0 }, { 0, 6, 0 } } },
		{ { { 2, 0, 1 }, { 0, 7, 0 } } },
		{ { { 2, 0, 2 }, { 0, 8, 0 } } },
		{ { { 3, 0, 1 }, { 0, 9, 1 } } },
		{ { { 3, 0, 0 }, { 0, 9, 0 } } },
		{ { { 3, 0, 1 }, { 0, 10, 0 } } },
		{ { { 3, 0, 2 }, { 0, 11, 0 } } },
		{ { { 4, 0, 1 }, { 0, 12, 1 } } },
		{ { { 4, 0, 0 }, { 0, 12, 0 } } },
		{ { { 4, 0, 1 }, { 0, 13, 0 } } },
		{ { { 4, 0, 2 }, { 0, 14, 0 } } },
		{ { { 5, 0, 1 }, { 0, 15, 1 } } },
		{ { { 5, 0, 0 }, { 0, 15, 0 } } },
		{ { { 5, 0, 1 }, { 0, 16, 0 } } },
		{ { { 5, 0, 2 }, { 1, 15, 0 } } },
		{ { { 6, 0, 1 }, { 0, 17, 0 } } },
		{ { { 6, 0, 0 }, { 0, 18, 0 } } },
		{ { { 6, 0, 1 }, { 0, 19, 0 } } },
		{ { { 6, 0, 2 }, { 3, 14, 0 } } },
		{ { { 7, 0, 1 }, { 0, 20, 0 } } },
		{ { { 7, 0, 0 }, { 0, 21, 0 } } },
		{ { { 7, 0, 1 }, { 0, 22, 0 } } },
		{ { { 7, 0, 2 }, { 4, 15, 0 } } },
		{ { { 8, 0, 1 }, { 0, 23, 0 } } },
		{ { { 8, 0, 0 }, { 0, 24, 0 } } },
		{ { { 8, 0, 1 }, { 0, 25, 0 } } },
		{ { { 8, 0, 2 }, { 6, 14, 0 } } },
		{ { { 9, 0, 1 }, { 0, 26, 0 } } },
		{ { { 9, 0, 0 }, { 0, 27, 0 } } },
		{ { { 9, 0, 1 }, { 0, 28, 0 } } },
		{ { { 9, 0, 2 }, { 7, 15, 0 } } },
		{ { { 10, 0, 1 }, { 0, 29, 0 } } },
		{ { { 10, 0, 0 }, { 0, 30, 0 } } },
		{ { { 10, 0, 1 }, { 0, 31, 0 } } },
		{ { { 10, 0, 2 }, { 9, 14, 0 } } },
		{ { { 11, 0, 1 }, { 0, 32, 0 } } },
		{ { { 11, 0, 0 }, { 0, 33, 0 } } },
		{ { { 11, 0, 1 }, { 2, 30, 0 } } },
		{ { { 11, 0, 2 }, { 0, 34, 0 } } },
		{ { { 12, 0, 1 }, { 0, 35, 0 } } },
		{ { { 12, 0, 0 }, { 0, 36, 0 } } },
		{ { { 12, 0, 1 }, { 3, 31, 0 } } },
		{ { { 12, 0, 2 }, { 0, 37, 0 } } },
		{ { { 13, 0, 1 }, { 0, 38, 0 } } },
		{ { { 13, 0, 0 }, { 0, 39, 0 } } },
		{ { { 13, 0, 1 }, { 5, 30, 0 } } },
		{ { { 13, 0, 2 }, { 0, 40, 0 } } },
		{ { { 14, 0, 1 }, { 0, 41, 0 } } },
		{ { { 14, 0, 0 }, { 0, 42, 0 } } },
		{ { { 14, 0, 1 }, { 6, 31, 0 } } },
		{ { { 14, 0, 2 }, { 0, 43, 0 } } },
		{ { { 15, 0, 1 }, { 0, 44, 0 } } },
		{ { { 15, 0, 0 }, { 0, 45, 0 } } },
		{ { { 15, 0, 1 }, { 8, 30, 0 } } },
		{ { { 15, 0, 2 }, { 0, 46, 0 } } },
		{ { { 16, 0, 2 }, { 0, 47, 0 } } },
		{ { { 16, 0, 1 }, { 1, 46, 0 } } },
		{ { { 16, 0, 0 }, { 0, 48, 0 } } },
		{ { { 16, 0, 1 }, { 0, 49, 0 } } },
		{ { { 16, 0, 2 }, { 0, 50, 0 } } },
		{ { { 17, 0, 1 }, { 2, 47, 0 } } },
		{ { { 17, 0, 0 }, { 0, 51, 0 } } },
		{ { { 17, 0, 1 }, { 0, 52, 0 } } },
		{ { { 17, 0, 2 }, { 0, 53, 0 } } },
		{ { { 18, 0, 1 }, { 4, 46, 0 } } },
		{ { { 18, 0, 0 }, { 0, 54, 0 } } },
		{ { { 18, 0, 1 }, { 0, 55, 0 } } },
		{ { { 18, 0, 2 }, { 0, 56, 0 } } },
		{ { { 19, 0, 1 }, { 5, 47, 0 } } },
		{ { { 19, 0, 0 }, { 0, 57, 0 } } },
		{ { { 19, 0, 1 }, { 0, 58, 0 } } },
		{ { { 19, 0, 2 }, { 0, 59, 0 } } },
		{ { { 20, 0, 1 }, { 7, 46, 0 } } },
		{ { { 20, 0, 0 }, { 0, 60, 0 } } },
		{ { { 20, 0, 1 }, { 0, 61, 0 } } },
		{ { { 20, 0, 2 }, { 0, 62, 0 } } },
		{ { { 21, 0, 1 }, { 8, 47, 0 } } },
		{ { { 21, 0, 0 }, { 0, 63, 0 } } },
		{ { { 21, 0, 1 }, { 1, 62, 0 } } },
		{ { { 21, 0, 2 }, { 1, 63, 0 } } },
		{ { { 22, 0, 1 }, { 10, 46, 0 } } },
		{ { { 22, 0, 0 }, { 2, 62, 0 } } },
		{ { { 22, 0, 1 }, { 2, 63, 0 } } },
		{ { { 22, 0, 2 }, { 3, 62, 0 } } },
		{ { { 23, 0, 1 }, { 11, 47, 0 } } },
		{ { { 23, 0, 0 }, { 3, 63, 0 } } },
		{ { { 23, 0, 1 }, { 4, 62, 0 } } },
		{ { { 23, 0, 2 }, { 4, 63, 0 } } },
		{ { { 24, 0, 1 }, { 13, 46, 0 } } },
		{ { { 24, 0, 0 }, { 5, 62, 0 } } },
		{ { { 24, 0, 1 }, { 5, 63, 0 } } },
		{ { { 24, 0, 2 }, { 6, 62, 0 } } },
		{ { { 25, 0, 1 }, { 14, 47, 0 } } },
		{ { { 25, 0, 0 }, { 6, 63, 0 } } },
		{ { { 25, 0, 1 }, { 7, 62, 0 } } },
		{ { { 25, 0, 2 }, { 7, 63, 0 } } },
		{ { { 26, 0, 1 }, { 16, 45, 0 } } },
		{ { { 26, 0, 0 }, { 8, 62, 0 } } },
		{ { { 26, 0, 1 }, { 8, 63, 0 } } },
		{ { { 26, 0, 2 }, { 9, 62, 0 } } },
		{ { { 27, 0, 1 }, { 16, 48, 0 } } },
		{ { { 27, 0, 0 }, { 9, 63, 0 } } },
		{ { { 27, 0, 1 }, { 10, 62, 0 } } },
		{ { { 27, 0, 2 }, { 10, 63, 0 } } },
		{ { { 28, 0, 1 }, { 16, 51, 0 } } },
		{ { { 28, 0, 0 }, { 11, 62, 0 } } },
		{ { { 28, 0, 1 }, { 11, 63, 0 } } },
		{ { { 28, 0, 2 }, { 12, 62, 0 } } },
		{ { { 29, 0, 1 }, { 16, 54, 0 } } },
		{ { { 29, 0, 0 }, { 12, 63, 0 } } },
		{ { { 29, 0, 1 }, { 13, 62, 0 } } },
		{ { { 29, 0, 2 }, { 13, 63, 0 } } },
		{ { { 30, 0, 1 }, { 16, 57, 0 } } },
		{ { { 30, 0, 0 }, { 14, 62, 0 } } },
		{ { { 30, 0, 1 }, { 14, 63, 0 } } },
		{ { { 30, 0, 2 }, { 15, 62, 0 } } },
		{ { { 31, 0, 1 }, { 16, 60, 0 } } },
		{ { { 31, 0, 0 }, { 15, 63, 0 } } },
		{ { { 31, 0, 1 }, { 24, 46, 0 } } },
		{ { { 31, 0, 2 }, { 16, 62, 0 } } },
		{ { { 32, 0, 2 }, { 16, 63, 0 } } },
		{ { { 32, 0, 1 }, { 17, 62, 0 } } },
		{ { { 32, 0, 0 }, { 25, 47, 0 } } },
		{ { { 32, 0, 1 }, { 17, 63, 0 } } },
		{ { { 32, 0, 2 }, { 18, 62, 0 } } },
		{ { { 33, 0, 1 }, { 18, 63, 0 } } },
		{ { { 33, 0, 0 }, { 27, 46, 0 } } },
		{ { { 33, 0, 1 }, { 19, 62, 0 } } },
		{ { { 33, 0, 2 }, { 19, 63, 0 } } },
		{ { { 34, 0, 1 }, { 20, 62, 0 } } },
		{ { { 34, 0, 0 }, { 28, 47, 0 } } },
		{ { { 34, 0, 1 }, { 20, 63, 0 } } },
		{ { { 34, 0, 2 }, { 21, 62, 0 } } },
		{ { { 35, 0, 1 }, { 21, 63, 0 } } },
		{ { { 35, 0, 0 }, { 30, 46, 0 } } },
		{ { { 35, 0, 1 }, { 22, 62, 0 } } },
		{ { { 35, 0, 2 }, { 22, 63, 0 } } },
		{ { { 36, 0, 1 }, { 23, 62, 0 } } },
		{ { { 36, 0, 0 }, { 31, 47, 0 } } },
		{ { { 36, 0, 1 }, { 23, 63, 0 } } },
		{ { { 36, 0, 2 }, { 24, 62, 0 } } },
		{ { { 37, 0, 1 }, { 24, 63, 0 } } },
		{ { { 37, 0, 0 }, { 32, 47, 0 } } },
		{ { { 37, 0, 1 }, { 25, 62, 0 } } },
		{ { { 37, 0, 2 }, { 25, 63, 0 } } },
		{ { { 38, 0, 1 }, { 26, 62, 0 } } },
		{ { { 38, 0, 0 }, { 32, 50, 0 } } },
		{ { { 38, 0, 1 }, { 26, 63, 0 } } },
		{ { { 38, 0, 2 }, { 27, 62, 0 } } },
		{ { { 39, 0, 1 }, { 27, 63, 0 } } },
		{ { { 39, 0, 0 }, { 32, 53, 0 } } },
		{ { { 39, 0, 1 }, { 28, 62, 0 } } },
		{ { { 39, 0, 2 }, { 28, 63, 0 } } },
		{ { { 40, 0, 1 }, { 29, 62, 0 } } },
		{ { { 40, 0, 0 }, { 32, 56, 0 } } },
		{ { { 40, 0, 1 }, { 29, 63, 0 } } },
		{ { { 40, 0, 2 }, { 30, 62, 0 } } },
		{ { { 41, 0, 1 }, { 30, 63, 0 } } },
		{ { { 41, 0, 0 }, { 32, 59, 0 } } },
		{ { { 41, 0, 1 }, { 31, 62, 0 } } },
		{ { { 41, 0, 2 }, { 31, 63, 0 } } },
		{ { { 42, 0, 1 }, { 32, 61, 0 } } },
		{ { { 42, 0, 0 }, { 32, 62, 0 } } },
		{ { { 42, 0, 1 }, { 32, 63, 0 } } },
		{ { { 42, 0, 2 }, { 41, 46, 0 } } },
		{ { { 43, 0, 1 }, { 33, 62, 0 } } },
		{ { { 43, 0, 0 }, { 33, 63, 0 } } },
		{ { { 43, 0, 1 }, { 34, 62, 0 } } },
		{ { { 43, 0, 2 }, { 42, 47, 0 } } },
		{ { { 44, 0, 1 }, { 34, 63, 0 } } },
		{ { { 44, 0, 0 }, { 35, 62, 0 } } },
		{ { { 44, 0, 1 }, { 35, 63, 0 } } },
		{ { { 44, 0, 2 }, { 44, 46, 0 } } },
		{ { { 45, 0, 1 }, { 36, 62, 0 } } },
		{ { { 45, 0, 0 }, { 36, 63, 0 } } },
		{ { { 45, 0, 1 }, { 37, 62, 0 } } },
		{ { { 45, 0, 2 }, { 45, 47, 0 } } },
		{ { { 46, 0, 1 }, { 37, 63, 0 } } },
		{ { { 46, 0, 0 }, { 38, 62, 0 } } },
		{ { { 46, 0, 1 }, { 38, 63, 0 } } },
		{ { { 46, 0, 2 }, { 47, 46, 0 } } },
		{ { { 47, 0, 1 }, { 39, 62, 0 } } },
		{ { { 47, 0, 0 }, { 39, 63, 0 } } },
		{ { { 47, 0, 1 }, { 40, 62, 0 } } },
		{ { { 47, 0, 2 }, { 48, 46, 0 } } },
		{ { { 48, 0, 2 }, { 40, 63, 0 } } },
		{ { { 48, 0, 1 }, { 41, 62, 0 } } },
		{ { { 48, 0, 0 }, { 41, 63, 0 } } },
		{ { { 48, 0, 1 }, { 48, 49, 0 } } },
		{ { { 48, 0, 2 }, { 42, 62, 0 } } },
		{ { { 49, 0, 1 }, { 42, 63, 0 } } },
		{ { { 49, 0, 0 }, { 43, 62, 0 } } },
		{ { { 49, 0, 1 }, { 48, 52, 0 } } },
		{ { { 49, 0, 2 }, { 43, 63, 0 } } },
		{ { { 50, 0, 1 }, { 44, 62, 0 } } },
		{ { { 50, 0, 0 }, { 44, 63, 0 } } },
		{ { { 50, 0, 1 }, { 48, 55, 0 } } },
		{ { { 50, 0, 2 }, { 45, 62, 0 } } },
		{ { { 51, 0, 1 }, { 45, 63, 0 } } },
		{ { { 51, 0, 0 }, { 46, 62, 0 } } },
		{ { { 51, 0, 1 }, { 48, 58, 0 } } },
		{ { { 51, 0, 2 }, { 46, 63, 0 } } },
		{ { { 52, 0, 1 }, { 47, 62, 0 } } },
		{ { { 52, 0, 0 }, { 47, 63, 0 } } },
		{ { { 52, 0, 1 }, { 48, 61, 0 } } },
		{ { { 52, 0, 2 }, { 48, 62, 0 } } },
		{ { { 53, 0, 1 }, { 56, 47, 0 } } },
		{ { { 53, 0, 0 }, { 48, 63, 0 } } },
		{ { { 53, 0, 1 }, { 49, 62, 0 } } },
		{ { { 53, 0, 2 }, { 49, 63, 0 } } },
		{ { { 54, 0, 1 }, { 58, 46, 0 } } },
		{ { { 54, 0, 0 }, { 50, 62, 0 } } },
		{ { { 54, 0, 1 }, { 50, 63, 0 } } },
		{ { { 54, 0, 2 }, { 51, 62, 0 } } },
		{ { { 55, 0, 1 }, { 59, 47, 0 } } },
		{ { { 55, 0, 0 }, { 51, 63, 0 } } },
		{ { { 55, 0, 1 }, { 52, 62, 0 } } },
		{ { { 55, 0, 2 }, { 52, 63, 0 } } },
		{ { { 56, 0, 1 }, { 61, 46, 0 } } },
		{ { { 56, 0, 0 }, { 53, 62, 0 } } },
		{ { { 56, 0, 1 }, { 53, 63, 0 } } },
		{ { { 56, 0, 2 }, { 54, 62, 0 } } },
		{ { { 57, 0, 1 }, { 62, 47, 0 } } },
		{ { { 57, 0, 0 }, { 54, 63, 0 } } },
		{ { { 57, 0, 1 }, { 55, 62, 0 } } },
		{ { { 57, 0, 2 }, { 55, 63, 0 } } },
		{ { { 58, 0, 1 }, { 56, 62, 1 } } },
		{ { { 58, 0, 0 }, { 56, 62, 0 } } },
		{ { { 58, 0, 1 }, { 56, 63, 0 } } },
		{ { { 58, 0, 2 }, { 57, 62, 0 } } },
		{ { { 59, 0, 1 }, { 57, 63, 1 } } },
		{ { { 59, 0, 0 }, { 57, 63, 0 } } },
		{ { { 59, 0, 1 }, { 58, 62, 0 } } },
		{ { { 59, 0, 2 }, { 58, 63, 0 } } },
		{ { { 60, 0, 1 }, { 59, 62, 1 } } },
		{ { { 60, 0, 0 }, { 59, 62, 0 } } },
		{ { { 60, 0, 1 }, { 59, 63, 0 } } },
		{ { { 60, 0, 2 }, { 60, 62, 0 } } },
		{ { { 61, 0, 1 }, { 60, 63, 1 } } },
		{ { { 61, 0, 0 }, { 60, 63, 0 } } },
		{ { { 61, 0, 1 }, { 61, 62, 0 } } },
		{ { { 61, 0, 2 }, { 61, 63, 0 } } },
		{ { { 62, 0, 1 }, { 62, 62, 1 } } },
		{ { { 62, 0, 0 }, { 62, 62, 0 } } },
		{ { { 62, 0, 1 }, { 62, 63, 0 } } },
		{ { { 62, 0, 2 }, { 63, 62, 0 } } },
		{ { { 63, 0, 1 }, { 63, 63, 1 } } },
		{ { { 63, 0, 0 }, { 63, 63, 0 } } }
	};


	class SingleColourFit : public ColourFit
	{
	public:
		SingleColourFit(ColourSet const* colours, int flags)
			: ColourFit(colours, flags)
		{
			// grab the single colour
			Vec3 const* values = m_colours->GetPoints();
			m_colour[0] = (u8)FloatToInt(255.0f*values->X(), 255);
			m_colour[1] = (u8)FloatToInt(255.0f*values->Y(), 255);
			m_colour[2] = (u8)FloatToInt(255.0f*values->Z(), 255);

			// initialise the best error
			m_besterror = INT_MAX;
		}

	private:
		virtual void Compress3(void* block)
		{
			// build the table of lookups
			SingleColourLookup const* const lookups[] =
			{
				lookup_5_3,
				lookup_6_3,
				lookup_5_3
			};

			// find the best end-points and index
			ComputeEndPoints(lookups);

			// build the block if we win
			if (m_error < m_besterror)
			{
				// remap the indices
				u8 indices[16];
				m_colours->RemapIndices(&m_index, indices);

				// save the block
				WriteColourBlock3(m_start, m_end, indices, block);

				// save the error
				m_besterror = m_error;
			}
		}
		virtual void Compress4(void* block)
		{
			// build the table of lookups
			SingleColourLookup const* const lookups[] =
			{
				lookup_5_4,
				lookup_6_4,
				lookup_5_4
			};

			// find the best end-points and index
			ComputeEndPoints(lookups);

			// build the block if we win
			if (m_error < m_besterror)
			{
				// remap the indices
				u8 indices[16];
				m_colours->RemapIndices(&m_index, indices);

				// save the block
				WriteColourBlock4(m_start, m_end, indices, block);

				// save the error
				m_besterror = m_error;
			}
		}

		void ComputeEndPoints(SingleColourLookup const* const* lookups)
		{
			// check each index combination (endpoint or intermediate)
			m_error = INT_MAX;
			for (int index = 0; index < 2; ++index)
			{
				// check the error for this codebook index
				SourceBlock const* sources[3];
				int error = 0;
				for (int channel = 0; channel < 3; ++channel)
				{
					// grab the lookup table and index for this channel
					SingleColourLookup const* lookup = lookups[channel];
					int target = m_colour[channel];

					// store a pointer to the source for this channel
					sources[channel] = lookup[target].sources + index;

					// accumulate the error
					int diff = sources[channel]->error;
					error += diff*diff;
				}

				// keep it if the error is lower
				if (error < m_error)
				{
					m_start = Vec3(
						(float)sources[0]->start / 31.0f,
						(float)sources[1]->start / 63.0f,
						(float)sources[2]->start / 31.0f
						);
					m_end = Vec3(
						(float)sources[0]->end / 31.0f,
						(float)sources[1]->end / 63.0f,
						(float)sources[2]->end / 31.0f
						);
					m_index = (u8)(2 * index);
					m_error = error;
				}
			}
		}


		u8 m_colour[3];
		Vec3 m_start;
		Vec3 m_end;
		u8 m_index;
		int m_error;
		int m_besterror;
	};


	// alpha.h/cpp
	static void CompressAlphaDxt3(u8 const* rgba, int mask, void* block)
	{
		u8* bytes = reinterpret_cast<u8*>(block);

		// quantise and pack the alpha values pairwise
		for (int i = 0; i < 8; ++i)
		{
			// quantise down to 4 bits
			float alpha1 = (float)rgba[8 * i + 3] * (15.0f / 255.0f);
			float alpha2 = (float)rgba[8 * i + 7] * (15.0f / 255.0f);
			int quant1 = FloatToInt(alpha1, 15);
			int quant2 = FloatToInt(alpha2, 15);

			// set alpha to zero where masked
			int bit1 = 1 << (2 * i);
			int bit2 = 1 << (2 * i + 1);
			if ((mask & bit1) == 0)
				quant1 = 0;
			if ((mask & bit2) == 0)
				quant2 = 0;

			// pack into the byte
			bytes[i] = (u8)(quant1 | (quant2 << 4));
		}
	}

	static void DecompressAlphaDxt3(u8* rgba, void const* block)
	{
		u8 const* bytes = reinterpret_cast<u8 const*>(block);

		// unpack the alpha values pairwise
		for (int i = 0; i < 8; ++i)
		{
			// quantise down to 4 bits
			u8 quant = bytes[i];

			// unpack the values
			u8 lo = quant & 0x0f;
			u8 hi = quant & 0xf0;

			// convert back up to bytes
			rgba[8 * i + 3] = lo | (lo << 4);
			rgba[8 * i + 7] = hi | (hi >> 4);
		}
	}

	static void FixRange(int& min, int& max, int steps)
	{
		if (max - min < steps)
			max = std::min(min + steps, 255);
		if (max - min < steps)
			min = std::max(0, max - steps);
	}

	static int FitCodes(u8 const* rgba, int mask, u8 const* codes, u8* indices)
	{
		// fit each alpha value to the codebook
		int err = 0;
		for (int i = 0; i < 16; ++i)
		{
			// check this pixel is valid
			int bit = 1 << i;
			if ((mask & bit) == 0)
			{
				// use the first code
				indices[i] = 0;
				continue;
			}

			// find the least error and corresponding index
			int value = rgba[4 * i + 3];
			int least = INT_MAX;
			int index = 0;
			for (int j = 0; j < 8; ++j)
			{
				// get the squared error from this code
				int dist = (int)value - (int)codes[j];
				dist *= dist;

				// compare with the best so far
				if (dist < least)
				{
					least = dist;
					index = j;
				}
			}

			// save this index and accumulate the error
			indices[i] = (u8)index;
			err += least;
		}

		// return the total error
		return err;
	}

	static void WriteAlphaBlock(int alpha0, int alpha1, u8 const* indices, void* block)
	{
		u8* bytes = reinterpret_cast<u8*>(block);

		// write the first two bytes
		bytes[0] = (u8)alpha0;
		bytes[1] = (u8)alpha1;

		// pack the indices with 3 bits each
		u8* dest = bytes + 2;
		u8 const* src = indices;
		for (int i = 0; i < 2; ++i)
		{
			// pack 8 3-bit values
			int value = 0;
			for (int j = 0; j < 8; ++j)
			{
				int index = *src++;
				value |= (index << 3 * j);
			}

			// store in 3 bytes
			for (int j = 0; j < 3; ++j)
			{
				int byte = (value >> 8 * j) & 0xff;
				*dest++ = (u8)byte;
			}
		}
	}

	static void WriteAlphaBlock5(int alpha0, int alpha1, u8 const* indices, void* block)
	{
		// check the relative values of the endpoints
		if (alpha0 > alpha1)
		{
			// swap the indices
			u8 swapped[16];
			for (int i = 0; i < 16; ++i)
			{
				u8 index = indices[i];
				if (index == 0)
					swapped[i] = 1;
				else if (index == 1)
					swapped[i] = 0;
				else if (index <= 5)
					swapped[i] = 7 - index;
				else
					swapped[i] = index;
			}

			// write the block
			WriteAlphaBlock(alpha1, alpha0, swapped, block);
		}
		else
		{
			// write the block
			WriteAlphaBlock(alpha0, alpha1, indices, block);
		}
	}

	static void WriteAlphaBlock7(int alpha0, int alpha1, u8 const* indices, void* block)
	{
		// check the relative values of the endpoints
		if (alpha0 < alpha1)
		{
			// swap the indices
			u8 swapped[16];
			for (int i = 0; i < 16; ++i)
			{
				u8 index = indices[i];
				if (index == 0)
					swapped[i] = 1;
				else if (index == 1)
					swapped[i] = 0;
				else
					swapped[i] = 9 - index;
			}

			// write the block
			WriteAlphaBlock(alpha1, alpha0, swapped, block);
		}
		else
		{
			// write the block
			WriteAlphaBlock(alpha0, alpha1, indices, block);
		}
	}

	static void CompressAlphaDxt5(u8 const* rgba, int mask, void* block)
	{
		// get the range for 5-alpha and 7-alpha interpolation
		int min5 = 255;
		int max5 = 0;
		int min7 = 255;
		int max7 = 0;
		for (int i = 0; i < 16; ++i)
		{
			// check this pixel is valid
			int bit = 1 << i;
			if ((mask & bit) == 0)
				continue;

			// incorporate into the min/max
			int value = rgba[4 * i + 3];
			if (value < min7)
				min7 = value;
			if (value > max7)
				max7 = value;
			if (value != 0 && value < min5)
				min5 = value;
			if (value != 255 && value > max5)
				max5 = value;
		}

		// handle the case that no valid range was found
		if (min5 > max5)
			min5 = max5;
		if (min7 > max7)
			min7 = max7;

		// fix the range to be the minimum in each case
		FixRange(min5, max5, 5);
		FixRange(min7, max7, 7);

		// set up the 5-alpha code book
		u8 codes5[8];
		codes5[0] = (u8)min5;
		codes5[1] = (u8)max5;
		for (int i = 1; i < 5; ++i)
			codes5[1 + i] = (u8)(((5 - i)*min5 + i*max5) / 5);
		codes5[6] = 0;
		codes5[7] = 255;

		// set up the 7-alpha code book
		u8 codes7[8];
		codes7[0] = (u8)min7;
		codes7[1] = (u8)max7;
		for (int i = 1; i < 7; ++i)
			codes7[1 + i] = (u8)(((7 - i)*min7 + i*max7) / 7);

		// fit the data to both code books
		u8 indices5[16];
		u8 indices7[16];
		int err5 = FitCodes(rgba, mask, codes5, indices5);
		int err7 = FitCodes(rgba, mask, codes7, indices7);

		// save the block with least error
		if (err5 <= err7)
			WriteAlphaBlock5(min5, max5, indices5, block);
		else
			WriteAlphaBlock7(min7, max7, indices7, block);
	}

	static void DecompressAlphaDxt5(u8* rgba, void const* block)
	{
		// get the two alpha values
		u8 const* bytes = reinterpret_cast<u8 const*>(block);
		int alpha0 = bytes[0];
		int alpha1 = bytes[1];

		// compare the values to build the codebook
		u8 codes[8];
		codes[0] = (u8)alpha0;
		codes[1] = (u8)alpha1;
		if (alpha0 <= alpha1)
		{
			// use 5-alpha codebook
			for (int i = 1; i < 5; ++i)
				codes[1 + i] = (u8)(((5 - i)*alpha0 + i*alpha1) / 5);
			codes[6] = 0;
			codes[7] = 255;
		}
		else
		{
			// use 7-alpha codebook
			for (int i = 1; i < 7; ++i)
				codes[1 + i] = (u8)(((7 - i)*alpha0 + i*alpha1) / 7);
		}

		// decode the indices
		u8 indices[16];
		u8 const* src = bytes + 2;
		u8* dest = indices;
		for (int i = 0; i < 2; ++i)
		{
			// grab 3 bytes
			int value = 0;
			for (int j = 0; j < 3; ++j)
			{
				int byte = *src++;
				value |= (byte << 8 * j);
			}

			// unpack 8 3-bit values from it
			for (int j = 0; j < 8; ++j)
			{
				int index = (value >> 3 * j) & 0x7;
				*dest++ = (u8)index;
			}
		}

		// write out the indexed codebook values
		for (int i = 0; i < 16; ++i)
			rgba[4 * i + 3] = codes[indices[i]];
	}


	// squish.h
	static int FixFlags(int flags)
	{
		// grab the flag bits
		int method = flags & (kDxt1 | kDxt3 | kDxt5);
		int fit = flags & (kColourIterativeClusterFit | kColourClusterFit | kColourRangeFit);
		int metric = flags & (kColourMetricPerceptual | kColourMetricUniform);
		int extra = flags & kWeightColourByAlpha;

		// set defaults
		if (method != kDxt3 && method != kDxt5)
			method = kDxt1;
		if (fit != kColourRangeFit)
			fit = kColourClusterFit;
		if (metric != kColourMetricUniform)
			metric = kColourMetricPerceptual;

		// done
		return method | fit | metric | extra;
	}

	void Compress(u8 const* rgba, void* block, int flags)
	{
		// compress with full mask
		CompressMasked(rgba, 0xffff, block, flags);
	}

	void CompressMasked(u8 const* rgba, int mask, void* block, int flags)
	{
		// fix any bad flags
		flags = FixFlags(flags);

		// get the block locations
		void* colourBlock = block;
		void* alphaBock = block;
		if ((flags & (kDxt3 | kDxt5)) != 0)
			colourBlock = reinterpret_cast<u8*>(block)+8;

		// create the minimal point set
		ColourSet colours(rgba, mask, flags);

		// check the compression type and compress colour
		if (colours.GetCount() == 1)
		{
			// always do a single colour fit
			SingleColourFit fit(&colours, flags);
			fit.Compress(colourBlock);
		}
		else if ((flags & kColourRangeFit) != 0 || colours.GetCount() == 0)
		{
			// do a range fit
			RangeFit fit(&colours, flags);
			fit.Compress(colourBlock);
		}
		else
		{
			// default to a cluster fit (could be iterative or not)
			ClusterFit fit(&colours, flags);
			fit.Compress(colourBlock);
		}

		// compress alpha separately if necessary
		if ((flags & kDxt3) != 0)
			CompressAlphaDxt3(rgba, mask, alphaBock);
		else if ((flags & kDxt5) != 0)
			CompressAlphaDxt5(rgba, mask, alphaBock);
	}

	void Decompress(u8* rgba, void const* block, int flags)
	{
		// fix any bad flags
		flags = FixFlags(flags);

		// get the block locations
		void const* colourBlock = block;
		void const* alphaBock = block;
		if ((flags & (kDxt3 | kDxt5)) != 0)
			colourBlock = reinterpret_cast<u8 const*>(block)+8;

		// decompress colour
		DecompressColour(rgba, colourBlock, (flags & kDxt1) != 0);

		// decompress alpha separately if necessary
		if ((flags & kDxt3) != 0)
			DecompressAlphaDxt3(rgba, alphaBock);
		else if ((flags & kDxt5) != 0)
			DecompressAlphaDxt5(rgba, alphaBock);
	}

	int GetStorageRequirements(int width, int height, int flags)
	{
		// fix any bad flags
		flags = FixFlags(flags);

		// compute the storage requirements
		int blockcount = ((width + 3) / 4) * ((height + 3) / 4);
		int blocksize = ((flags & kDxt1) != 0) ? 8 : 16;
		return blockcount*blocksize;
	}

	void CompressImage(u8 const* rgba, int width, int height, void* blocks, int flags)
	{
		// fix any bad flags
		flags = FixFlags(flags);

		// initialise the block output
		u8* targetBlock = reinterpret_cast<u8*>(blocks);
		int bytesPerBlock = ((flags & kDxt1) != 0) ? 8 : 16;

		// loop over blocks
		for (int y = 0; y < height; y += 4)
		{
			for (int x = 0; x < width; x += 4)
			{
				// build the 4x4 block of pixels
				u8 sourceRgba[16 * 4];
				u8* targetPixel = sourceRgba;
				int mask = 0;
				for (int py = 0; py < 4; ++py)
				{
					for (int px = 0; px < 4; ++px)
					{
						// get the source pixel in the image
						int sx = x + px;
						int sy = y + py;

						// enable if we're in the image
						if (sx < width && sy < height)
						{
							// copy the rgba value
							u8 const* sourcePixel = rgba + 4 * (width*sy + sx);
							for (int i = 0; i < 4; ++i)
								*targetPixel++ = *sourcePixel++;

							// enable this pixel
							mask |= (1 << (4 * py + px));
						}
						else
						{
							// skip this pixel as its outside the image
							targetPixel += 4;
						}
					}
				}

				// compress it into the output
				CompressMasked(sourceRgba, mask, targetBlock, flags);

				// advance
				targetBlock += bytesPerBlock;
			}
		}
	}

	void DecompressImage(u8* rgba, int width, int height, void const* blocks, int flags)
	{
		// fix any bad flags
		flags = FixFlags(flags);

		// initialise the block input
		u8 const* sourceBlock = reinterpret_cast<u8 const*>(blocks);
		int bytesPerBlock = ((flags & kDxt1) != 0) ? 8 : 16;

		// loop over blocks
		for (int y = 0; y < height; y += 4)
		{
			for (int x = 0; x < width; x += 4)
			{
				// decompress the block
				u8 targetRgba[4 * 16];
				Decompress(targetRgba, sourceBlock, flags);

				// write the decompressed pixels to the correct image locations
				u8 const* sourcePixel = targetRgba;
				for (int py = 0; py < 4; ++py)
				{
					for (int px = 0; px < 4; ++px)
					{
						// get the target location
						int sx = x + px;
						int sy = y + py;
						if (sx < width && sy < height)
						{
							u8* targetPixel = rgba + 4 * (width*sy + sx);

							// copy the rgba value
							for (int i = 0; i < 4; ++i)
								*targetPixel++ = *sourcePixel++;
						}
						else
						{
							// skip this pixel as its outside the image
							sourcePixel += 4;
						}
					}
				}

				// advance
				sourceBlock += bytesPerBlock;
			}
		}
	}

} // namespace squish
