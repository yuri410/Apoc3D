#pragma once
#ifndef APOC3D_GAUSSBLURFILTER_H
#define APOC3D_GAUSSBLURFILTER_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "Vector.h"
#include "MathCommon.h"

namespace Apoc3D
{
	namespace Math
	{
		class GaussBlurFilterInterface
		{
		public:
			virtual void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight) = 0;
			virtual void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight, float kernelXScale) = 0;

			virtual int32 getSampleCount() const = 0;
			virtual float getBlurAmount() const = 0;
			virtual float getKernelXScale() const = 0;

			virtual int getMapWidth() const = 0;
			virtual int getMapHeight() const = 0;

		protected:
			~GaussBlurFilterInterface() { }
		};

		class GaussBlurFilter final : public GaussBlurFilterInterface
		{
		public:
			GaussBlurFilter(int32 sampleCount, float blurAmount, int32 mapWidth, int32 mapHeight, float kernelXScale = 1)
				: SampleCount(sampleCount + 1 - (sampleCount % 2)), BlurAmount(blurAmount),
				m_mapWidth(mapWidth), m_mapHeight(mapHeight),
				m_kernelXScale(kernelXScale)
			{
				ComputeFilters();
			}
			~GaussBlurFilter()
			{
				delete[] SampleWeights;
				delete[] SampleOffsetsX;
				delete[] SampleOffsetsY;
			}

			void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight) override
			{
				BlurAmount = blurAmount;
				m_mapHeight = mapHeight;
				m_mapWidth = mapWidth;
				
				ComputeFilters();
			}
			void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight, float kernelXScale) override
			{
				m_kernelXScale = kernelXScale;

				ChangeSettings(blurAmount, mapWidth, mapHeight);
			}

			const float* const getSampleWeights() const { return SampleWeights; }
			const Vector4* const getSampleOffsetX() const { return SampleOffsetsX; }
			const Vector4* const getSampleOffsetY() const { return SampleOffsetsY; }

			int32 getSampleCount() const override { return SampleCount; }
			float getBlurAmount() const override { return BlurAmount; }
			float getKernelXScale() const override { return m_kernelXScale; }
			
			int getMapWidth() const override { return m_mapWidth; }
			int getMapHeight() const override { return m_mapHeight; }
			
		private:
			void ComputeFilters()
			{
				if (SampleWeights == nullptr)
				{
					SampleWeights = new float[SampleCount];
				}
				if (SampleOffsetsX == nullptr)
				{
					SampleOffsetsX = new Vector4[SampleCount];
				}
				if (SampleOffsetsY == nullptr)
				{
					SampleOffsetsY = new Vector4[SampleCount];
				}
				ComputeGaussianFilter(1.0f / m_mapWidth, 0, SampleCount, BlurAmount, m_kernelXScale, SampleWeights, SampleOffsetsX);
				ComputeGaussianFilter(0, 1.0f / m_mapHeight, SampleCount, BlurAmount, m_kernelXScale, nullptr, SampleOffsetsY);
			}

			float BlurAmount;
			int32 SampleCount;
			float* SampleWeights = nullptr;
			Vector4* SampleOffsetsX = nullptr;
			Vector4* SampleOffsetsY = nullptr;

			float m_kernelXScale;

			int m_mapWidth;
			int m_mapHeight;
		};

		template <int32 _SampleCount>
		class GaussBlurFilterSized final : public GaussBlurFilterInterface
		{
			static const int SampleCount = _SampleCount + 1 - (_SampleCount % 2);
		public:
			GaussBlurFilterSized() 
			{
				ZeroArray(SampleWeights);
				ZeroArray(SampleOffsetsX);
				ZeroArray(SampleOffsetsY);
			}

			GaussBlurFilterSized(float blurAmount, int32 mapWidth, int32 mapHeight, float kernelXScale = 1)
				: BlurAmount(blurAmount),
				m_mapWidth(mapWidth), m_mapHeight(mapHeight),
				m_kernelXScale(kernelXScale)
			{
				ComputeFilters();
			}


			void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight) override
			{
				BlurAmount = blurAmount;
				m_mapHeight = mapHeight;
				m_mapWidth = mapWidth;

				ComputeFilters();
			}
			void ChangeSettings(float blurAmount, int32 mapWidth, int32 mapHeight, float kernelXScale) override
			{
				m_kernelXScale = kernelXScale;

				ChangeSettings(blurAmount, mapWidth, mapHeight);
			}

			float SampleWeights[SampleCount];
			Vector4 SampleOffsetsX[SampleCount];
			Vector4 SampleOffsetsY[SampleCount];

			int32 getSampleCount() const override { return SampleCount; }
			float getBlurAmount() const override { return BlurAmount; }
			float getKernelXScale() const override { return m_kernelXScale; }

			int getMapWidth() const override { return m_mapWidth; }
			int getMapHeight() const override { return m_mapHeight; }
			
		private:
			void ComputeFilters()
			{
				ComputeGaussianFilter(1.0f / m_mapWidth, 0, SampleCount, BlurAmount, m_kernelXScale, SampleWeights, SampleOffsetsX);
				ComputeGaussianFilter(0, 1.0f / m_mapHeight, SampleCount, BlurAmount, m_kernelXScale, nullptr, SampleOffsetsY);
			}

			float BlurAmount = 0;
			float m_kernelXScale = 1;

			int m_mapWidth = 0;
			int m_mapHeight = 0;


		};
	}
}
#endif