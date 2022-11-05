#pragma once

#include "apoc3d.Essentials/EssentialCommon.h"
#include "apoc3d/Collections/List.h"

namespace Apoc3D
{
	namespace Utility
	{
		struct HistogramBinRange
		{
			double m_start;
			double m_end;
		};

		class DistributionHistogram
		{
		public:
			DistributionHistogram();
			~DistributionHistogram();

            DistributionHistogram(const DistributionHistogram& o) = default;
            DistributionHistogram& operator=(const DistributionHistogram& o) = default;

			DistributionHistogram(DistributionHistogram&& o) = default;
			DistributionHistogram& operator=(DistributionHistogram&& o) = default;

			void BuildWithBinSize(const List<double>& values, double binSize, double capSize = 0);
			void BuildWithBinCount(const List<double>& values, int32 binCount, double capSize = 0);
			void Build(const List<double>& values, const List<HistogramBinRange>& binRanges);
			void Clear();

			double CalculateXSpanAround(double value, double probability) const;
			double CalculateXPosition(double value) const;
			
			const List<HistogramBinRange>& getBinRanges() const { return m_binRanges; }
			const List<double>& getBins() const { return m_bins; }
			const int32 getBinCount() const { return m_binCount; }

			bool hasGenerated() const { return m_generated; }

		private:

			int32 m_binCount = 0;
			List<double> m_bins;
			List<HistogramBinRange> m_binRanges;

			bool m_generated = false;

			double m_minBin = 0;
			double m_maxBin = 0;
		};
	}
}