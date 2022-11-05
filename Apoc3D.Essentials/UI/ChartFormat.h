#pragma once

#include "UICommon.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d.Essentials/Utils/DistributionHistogram.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		enum struct ChartAxisSide
		{
			Min,
			Max,
		};

		struct AxisLine
		{
			double m_value;
			bool m_omitted;

			AxisLine(double x) : m_value(x), m_omitted(false) { }
			AxisLine(double x, bool omitted) : m_value(x), m_omitted(omitted) { }
		};

		class IChartAxisFomatter
		{
		public:
			virtual ~IChartAxisFomatter() { }

			virtual List<AxisLine> GetAxisLines(double minX, double maxX, int length) = 0;
			virtual void DrawAxisLine(Sprite* spr, const AxisLine& x) = 0;

			bool hasZeroLine() const { return m_hasZeroLine; }
		protected:
			IChartAxisFomatter(ChartAxisSide side, bool hasZeroLine)
				: m_axisSide(side), m_hasZeroLine(hasZeroLine) { }

			ChartAxisSide m_axisSide;
			bool m_hasZeroLine;
		};

		class DateAxisFormatter : public IChartAxisFomatter
		{
		public:
			DateAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor);
			~DateAxisFormatter();

			List<AxisLine> GetAxisLines(double minX, double maxX, int length) override;
			void DrawAxisLine(Sprite* spr, const AxisLine& x) override;

		private:
			Font* m_labelFont;
			ColorValue m_labelColor;
		};

		class NumberAxisFormatter : public IChartAxisFomatter
		{
		public:
			NumberAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor, int32 digits, int32 significantDigits = -1, bool percentage = false);
			~NumberAxisFormatter();

			List<AxisLine> GetAxisLines(double minX, double maxX, int length) override;
			void DrawAxisLine(Sprite* spr, const AxisLine& x) override;

		private:
			Font* m_labelFont;
			int32 m_digits;
			int32 m_signifiantDigits;
			ColorValue m_labelColor;
			bool m_usePercentage;
		};
		
		class NumberRangeAxisFormatter : public IChartAxisFomatter
		{
		public:
			NumberRangeAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor, const List<Utility::HistogramBinRange>* ranges, int32 digits);
			~NumberRangeAxisFormatter();

			List<AxisLine> GetAxisLines(double minX, double maxX, int length) override;
			void DrawAxisLine(Sprite* spr, const AxisLine& x) override;

		private:
			String ToString(double x, int xpos) const;
				
			Font* m_labelFont;
			const List<Utility::HistogramBinRange>* m_ranges;

			int32 m_digits;
			ColorValue m_labelColor;
		};

	}
}