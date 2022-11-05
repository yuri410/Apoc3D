#include "DistributionHistogram.h"

#include "apoc3d/Math/Math.h"

namespace Apoc3D
{
	namespace Utility
	{
		DistributionHistogram::DistributionHistogram()
		{

		}

		DistributionHistogram::~DistributionHistogram()
		{

		}

		void DistributionHistogram::BuildWithBinSize(const List<double>& values, double binSize, double capSize)
		{
			double minValue, maxValue;

			if (!values.GetMinMax(minValue, maxValue))
				return;

			const double step = binSize;

			List<HistogramBinRange> binRanges;

			if (capSize > 0)
			{
				const double capStep = (maxValue - minValue) * capSize;

				minValue += capStep;
				maxValue -= capStep;

				const int32 binConut = Math::Max(0, (int)((maxValue - minValue) / step + 0.5));
				const int32 coreBinCount = binConut - 2;

				binRanges.Add({ -std::numeric_limits<double>::infinity(), minValue });
				for (int32 i = 0; i < coreBinCount; i++)
				{
					HistogramBinRange binRange = { minValue + step*i, minValue + step*(i + 1) };
					binRange.m_end = Math::Min(binRange.m_end, maxValue);
					binRanges.Add(binRange);
				}
				binRanges.Add({ maxValue, std::numeric_limits<double>::infinity() });
			}
			else
			{
				const int32 binConut = Math::Max(0, (int)((maxValue - minValue) / step + 0.5));

				for (int32 i = 0; i < binConut; i++)
				{
					HistogramBinRange binRange = { minValue + step*i, minValue + step*(i + 1) };
					binRange.m_end = Math::Min(binRange.m_end, maxValue);
					binRanges.Add(binRange);
				}
			}
		
			Build(values, binRanges);
		}

		void DistributionHistogram::BuildWithBinCount(const List<double>& values, int32 binCount, double capSize)
		{
			double minValue, maxValue;

			if (!values.GetMinMax(minValue, maxValue))
				return;

			List<HistogramBinRange> binRanges;

			if (capSize > 0)
			{
				const double capStep = (maxValue - minValue) * capSize;
				const double step = (maxValue - minValue - 2 * capStep) / binCount;

				const int32 coreBinCount = binCount - 2;

				binRanges.Add({ -std::numeric_limits<double>::infinity(), minValue });
				for (int32 i = 0; i < coreBinCount; i++)
				{
					binRanges.Add({ minValue + step*i, minValue + step*(i + 1) });
				}
				binRanges.Add({ maxValue, std::numeric_limits<double>::infinity() });
			}
			else
			{
				const double step = (maxValue - minValue) / binCount;

				for (int32 i = 0; i < binCount; i++)
				{
					binRanges.Add({ minValue + step*i, minValue + step*(i + 1) });
				}
			}
		
			Build(values, binRanges);
		}

		void DistributionHistogram::Build(const List<double>& values, const List<HistogramBinRange>& binRanges)
		{
			Clear();

			m_binRanges = binRanges;
			m_binCount = binRanges.getCount();

			bool firstRange = true;
			for (const HistogramBinRange& br : binRanges)
			{
				double vals[] = { br.m_start, br.m_end };
				
				for (double v : vals)
				{
					if (isfinite(v))
					{
						if (firstRange)
						{
							m_minBin = m_maxBin = v;
							firstRange = false;
						}
						else
						{
							m_minBin = Math::Min(v, m_minBin);
							m_maxBin = Math::Max(v, m_maxBin);
						}
					}
				}

				m_bins.Add(0);
			}
			assert(!firstRange);

			const double invSampleCount = 1.0 / values.getCount();

			for (int32 i = 0; i < binRanges.getCount(); i++)
			{
				const auto& r = binRanges[i];

				for (double d : values)
				{
					bool isInBand = i == binRanges.getCount() - 1 ? 
						(d >= r.m_start && d <= r.m_end) : (d >= r.m_start && d < r.m_end);

					if (isInBand)
					{
						m_bins[i] += invSampleCount;
					}
				}
			}

			m_generated = true;
		}

		void DistributionHistogram::Clear()
		{
			m_binRanges.Clear();
			m_bins.Clear();
			m_generated = false;
		}

		double DistributionHistogram::CalculateXSpanAround(double value, double probability) const
		{
			int startIndex = (int)(CalculateXPosition(value) * (m_binCount - 1));
			startIndex = m_bins.ClampIndexInRange(startIndex);
			
			int minIndex = startIndex;
			int maxIndex = startIndex;

			double curProb = 0;
			for (int i = 0; i < m_binCount; i++)
			{
				if (i == 0)
				{
					curProb += m_bins[startIndex];
				}
				else
				{
					if (m_bins.isIndexInRange(startIndex + i))
					{
						maxIndex = startIndex + i;
						curProb += m_bins[maxIndex];
					}
					if (m_bins.isIndexInRange(startIndex - i))
					{
						minIndex = startIndex - i;
						curProb += m_bins[minIndex];
					}
				}

				if (curProb >= probability)
					break;
			}

			return (maxIndex - minIndex + 1) / (double)m_binCount;
		}

		double DistributionHistogram::CalculateXPosition(double value) const
		{
			return (value - m_minBin) / (m_maxBin - m_minBin);
		}
	}
}