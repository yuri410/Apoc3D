#include "ChartDataSource.h"

namespace Apoc3D
{
	namespace UI
	{

#pragma region RawDataSource
		//////////////////////////////////////////////////////////////////////////

		RawDataSource::RawDataSource(const List<double>* rates)
			: m_rates(rates) { }

		RawDataSource::~RawDataSource()
		{

		}

		void RawDataSource::GetData(double start, double end, List<ChartDataPoint>& data)
		{
			if (m_rates && m_rates->getCount() > 0)
			{
				for (int32 i = 0; i < m_rates->getCount(); i++)
				{
					if (i >= start && i <= end)
					{
						ChartDataPoint p;
						p.m_x = i;
						p.m_y = m_rates->operator[](i);
						p.m_y_open = p.m_y_low = p.m_y_high = p.m_y;
						data.Add(p);
					}
				}
			}
		}

		bool RawDataSource::GetDomain(double& minX, double& maxX)
		{
			if (m_rates && m_rates->getCount() > 0)
			{
				minX = 0;
				maxX = m_rates->getCount();
				return true;
			}
			return false;
		}

#pragma endregion
	}
}