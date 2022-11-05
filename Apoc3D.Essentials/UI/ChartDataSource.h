#pragma once

#include "UICommon.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace UI
	{
		class IChartDataSource
		{
		public:
			virtual ~IChartDataSource() { }

			virtual void GetData(double start, double end, List<ChartDataPoint>& data) = 0;
			virtual bool GetDomain(double& minX, double& maxX) = 0;
		};

		class RawDataSource : public IChartDataSource
		{
		public:
			RawDataSource(const List<double>* rates);
			~RawDataSource();

			void GetData(double start, double end, List<ChartDataPoint>& data) override;
			bool GetDomain(double& minX, double& maxX) override;

		private:
			const List<double>* m_rates = nullptr;

			bool m_noTime = false;
		};

	}
}