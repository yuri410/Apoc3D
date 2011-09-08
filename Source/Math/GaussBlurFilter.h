/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#ifndef GAUSSBLURFILTER_H
#define GAUSSBLURFILTER_H

#include "Common.h"
#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI GaussBlurFilter
		{
		private:
			float BlurAmount;
			int32 SampleCount;
			float* SampleWeights;
			Vector4* SampleOffsetsX;
			Vector4* SampleOffsetsY;

			float ComputeGaussian(float n) const
			{
				float theta = BlurAmount;
				
				return (float)((1.0 / sqrtf(2 * Math::PI * theta)) *
					expf(-(n * n) / (2 * theta * theta)));
			}
			void ComputeFilter(float dx, float dy, float* &sampleWeights, Vector4* &sampleOffsets)
			{
				// Create temporary arrays for computing our filter settings.
				if (!sampleWeights)
					sampleWeights = new float[SampleCount];
				if (!sampleOffsets)
					sampleOffsets = new Vector4[SampleCount];

				// The first sample always has a zero offset.
				sampleWeights[0] = ComputeGaussian(0);
				sampleOffsets[0] = Vector4(0,0,0,0);

				// Maintain a sum of all the weighting values.
				float totalWeights = sampleWeights[0];

				// Add pairs of additional sample taps, positioned
				// along a line in both directions from the center.
				for (int i = 0; i < SampleCount / 2; i++)
				{
					// Store weights for the positive and negative taps.
					float weight = ComputeGaussian(static_cast<float>(i + 1));

					sampleWeights[i * 2 + 1] = weight;
					sampleWeights[i * 2 + 2] = weight;

					totalWeights += weight * 2;

					// To get the maximum amount of blurring from a limited number of
					// pixel shader samples, we take advantage of the bilinear filtering
					// hardware inside the texture fetch unit. If we position our texture
					// coordinates exactly halfway between two texels, the filtering unit
					// will average them for us, giving two samples for the price of one.
					// This allows us to step in units of two texels per sample, rather
					// than just one at a time. The 1.5 offset kicks things off by
					// positioning us nicely in between two texels.
					float sampleOffset = i * 2 + 1.5f;

					Vector4 delta = Vector4(dx* sampleOffset, dy* sampleOffset,0,0);

					// Store texture coordinate offsets for the positive and negative taps.
					sampleOffsets[i * 2 + 1] = delta;
					sampleOffsets[i * 2 + 2] = Vector4Utils::Negate(delta);
				}

				// Normalize the list of sample weightings, so they will always sum to one.
				for (int i = 0; i < SampleCount; i++)
				{
					sampleWeights[i] /= totalWeights;
				}
			}

		public:
			const float* const getSampleWeights() const { return SampleWeights; }
			const Vector4* const getSampleOffsetX() const { return SampleOffsetsX; }
			const Vector4* const getSampleOffsetY() const { return SampleOffsetsY; }

			const int32 getSampleCount() const { return SampleCount; }

			GaussBlurFilter(int32 sampleCount, float blurAmount, int32 mapWidth, int32 mapHeight)
				: SampleCount(sampleCount), BlurAmount(blurAmount), SampleWeights(0), SampleOffsetsX(0), SampleOffsetsY(0)
			{
				ComputeFilter(1.0f / (float)mapWidth, 0, SampleWeights, SampleOffsetsX);
				ComputeFilter(0, 1.0f / (float)mapHeight, SampleWeights, SampleOffsetsY);
			}
			~GaussBlurFilter(void)
			{
				delete[] SampleWeights;
				delete[] SampleOffsetsX;
				delete[] SampleOffsetsY;
			}
		};

	}
}
#endif