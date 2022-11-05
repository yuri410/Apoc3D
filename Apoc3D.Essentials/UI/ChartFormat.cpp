#include "ChartFormat.h"
#include "UICommon.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d.Essentials/System/TimeSystem.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
#pragma region DateAxisFormatter

		DateAxisFormatter::DateAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor)
			: IChartAxisFomatter(axis, true)
			, m_labelColor(labelColor)
		{
			m_labelFont = FontManager::getSingleton().getFont(fontName);
		}
		DateAxisFormatter::~DateAxisFormatter()
		{

		}

		List<AxisLine> DateAxisFormatter::GetAxisLines(double minX, double maxX, int length)
		{
			const int stepSizeInPixel = 25;
			const int stepCount = (length + stepSizeInPixel - 1) / stepSizeInPixel;
			const double stepSizeInValue = (maxX - minX) / stepCount;

			List<AxisLine> result(stepCount + 1);

			for (int32 i = 0; i <= stepCount; i++)
			{
				double v = minX + stepSizeInValue*i;
				if (v > maxX)
					v = maxX;
				result.Add(v);
			}

			return result;
		}
		void DateAxisFormatter::DrawAxisLine(Sprite* spr, const AxisLine& x)
		{
			Date d = Date((int32)x.m_value);
			Font* fnt = m_labelFont;

			fnt->DrawString(spr, StringUtils::toASCIIWideString(d.ToStringYYYYMMDD()), Point(4, -fnt->getLineHeightInt() / 2), m_labelColor);

			spr->DrawLine(SystemUI::GetWhitePixel(), Point(0, 0), Point(3, 0), m_labelColor, 1, LineCapOptions::Butt);
		}

#pragma endregion

#pragma region NumberAxisFormatter

		NumberAxisFormatter::NumberAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor, int32 digits, int32 significantDigits, bool percentage)
			: IChartAxisFomatter(axis, true), m_digits(digits), m_signifiantDigits(significantDigits), m_usePercentage(percentage)
			, m_labelColor(labelColor)
		{
			m_labelFont = FontManager::getSingleton().getFont(fontName);
		}
		NumberAxisFormatter::~NumberAxisFormatter()
		{
			
		}

		List<AxisLine> NumberAxisFormatter::GetAxisLines(double minX, double maxX, int length)
		{
			const int32 stepSizeInPixel = m_labelFont->getLineHeightInt();
			const double valueRange = maxX - minX;
			const double unitsPerPixel = valueRange / length;
			const double minUnits = stepSizeInPixel * unitsPerPixel;

			if (m_signifiantDigits >= 0)
			{
				double stepSizeInValue = pow(0.1, m_signifiantDigits);

				if (minUnits > stepSizeInValue)
				{
					stepSizeInValue = ceil(minUnits / stepSizeInValue) * stepSizeInValue;
				}

				double roundedStart = floor(minX / stepSizeInValue) * stepSizeInValue;
				int32 stepCount = (int32)ceil(valueRange / stepSizeInValue);

				List<AxisLine> result(stepCount + 2);

				for (int32 i = 0; i < stepCount; i++)
				{
					double v = roundedStart + stepSizeInValue*i;

					if (v > minX && v < maxX)
						result.Add(v);
				}

				if (result.getCount())
				{
					if (result[0].m_value - minX > minUnits)
						result.Insert(0, minX);

					if (maxX - result.LastItem().m_value > minUnits)
						result.Add(maxX);
				}
				
				return result;
			}
			else
			{
				const int32 stepCount = (int32)ceil(valueRange / minUnits);

				const double stepSizeInValue = valueRange / stepCount;
				List<AxisLine> result(stepCount + 1);

				for (int32 i = 0; i <= stepCount; i++)
				{
					double v = minX + stepSizeInValue*i;
					if (v > maxX)
						v = maxX;
					result.Add(v);
				}

				return result;
			}
		}
		void NumberAxisFormatter::DrawAxisLine(Sprite* spr, const AxisLine& x)
		{
			double val = x.m_value;

			Font* fnt = m_labelFont;

			Point pos = Point(4, -fnt->getLineHeightInt() / 2);

			if (m_usePercentage)
			{
				fnt->DrawString(spr, StringUtils::DoubleToString(val * 100, StrFmt::FPDecimal(m_digits)) + L"%", pos, m_labelColor);
			}
			else
			{
				fnt->DrawString(spr, StringUtils::DoubleToString(val, StrFmt::FPDecimal(m_digits)), pos, m_labelColor);
			}
			spr->DrawLine(SystemUI::GetWhitePixel(), Point(0, 0), Point(3, 0), m_labelColor, 1, LineCapOptions::Butt);
		}
#pragma endregion

#pragma region NumberRangeAxisFormatter
		
		NumberRangeAxisFormatter::NumberRangeAxisFormatter(ChartAxisSide axis, const String& fontName, ColorValue labelColor, const List<Utility::HistogramBinRange>* ranges, int32 digits)
			: IChartAxisFomatter(axis, false), m_ranges(ranges), m_digits(digits)
			, m_labelColor(labelColor)
		{
			m_labelFont = FontManager::getSingleton().getFont(fontName);
		}
		NumberRangeAxisFormatter::~NumberRangeAxisFormatter()
		{

		}

		List<AxisLine> NumberRangeAxisFormatter::GetAxisLines(double minX, double maxX, int length)
		{
			int32 startIdx = m_ranges->ClampIndexInRange((int32)minX);
			int32 endIdx = m_ranges->ClampIndexInRange((int32)maxX);
			
			int32 count = Math::Max(0, endIdx - startIdx + 1);

			int32 fitCount = m_labelFont->getLineHeightInt() > 0 ? length / m_labelFont->getLineHeightInt() : 1;
			if (fitCount == 0)
				fitCount = 1;

			int32 skip = Math::Max(0, (count - 1) / fitCount);

			List<AxisLine> result(count);
			int32 s = 0;
			for (int32 i = startIdx; i <= endIdx; i++)
			{
				bool omitted = s != 0 && i != endIdx;

				AxisLine al(i, omitted);
				result.Add(al);

				if (++s > skip)
					s = 0;
			}
			return result;
		}

		String NumberRangeAxisFormatter::ToString(double x, int xpos) const
		{
			String result = StringUtils::DoubleToString(abs(x), StrFmt::FPDecimal(m_digits));
			String spacer = ControlCodes::MakeMoveControl(Point(xpos + 3, 0), true);

			if (x < 0)
				result = L"-" + spacer + result;
			else
				result = spacer + result;

			if (xpos > 0)
				result = ControlCodes::MakeMoveControl(Point(xpos, 0), true) + result;

			return result;
		}

		void NumberRangeAxisFormatter::DrawAxisLine(Sprite* spr, const AxisLine& x)
		{
			int32 idx = (int32)x.m_value;
			if (!m_ranges->isIndexInRange(idx))
				return;

			auto range = m_ranges->operator[](idx);

			if (!x.m_omitted)
			{
				Font* fnt = m_labelFont;
				String msg = ToString(range.m_start, 0) + ToString(range.m_end, 35);
				fnt->DrawString(spr, msg, Point(4, -fnt->getLineHeightInt() / 2), m_labelColor);
			}

			spr->DrawLine(SystemUI::GetWhitePixel(), Point(0, 0), Point(3, 0), m_labelColor, 1, LineCapOptions::Butt);
		}

#pragma endregion

	}
}