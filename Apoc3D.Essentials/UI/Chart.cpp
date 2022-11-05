#include "Chart.h"
#include "ChartDataSource.h"
#include "ChartFormat.h"
#include "Presentation.h"
#include "apoc3d/Graphics/Camera.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"
#include "apoc3d/Graphics/RenderSystem/VertexDeclaration.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Math/Math.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace UI
	{
		SINGLETON_IMPL(ChartBackgroundUpdater);

		const bool MultithreadedChart = false;

		void SetScissor(Sprite* sprite, Rectangle* bgArea)
		{
			RenderStateManager* rsm = sprite->getRenderDevice()->getRenderState();

			sprite->Flush();
			if (bgArea)
				rsm->setScissorTest(true, bgArea);
			else
				rsm->setScissorTest(false, nullptr);
		}

		ChartFormatDesc::ChartFormatDesc(IChartAxisFomatter* axisX, IChartAxisFomatter* axisY,
			const String& titleFontName /* FNT_UI8 */, LayoutLocation legendLocation)
			: m_axisX(axisX)
			, m_axisY(axisY)
			, m_titleFont(FontManager::getSingleton().getFont(titleFontName))
			, m_legendLocation(legendLocation)
		{ }

#pragma region Chart

		Chart::DataSource::DataSource(ChartDataSourceDesc info)
			: m_desc(info)
		{
		}

		Chart::DataSource::~DataSource()
		{
			DELETE_AND_NULL(m_desc.m_data);
		}

		Chart::DataSource::DataSource(DataSource&& o)
			: m_desc(o.m_desc), m_points(std::move(o.m_points))
		{
			o.m_desc.m_data = nullptr;
		}

		Chart::DataSource& Chart::DataSource::operator=(DataSource&& o)
		{
			if (this != &o)
			{
				this->~DataSource();
				new (this)DataSource(std::move(o));
			}
			return *this;
		}

		void Chart::DataSource::RecacheData(double xStart, double xEnd)
		{
			m_points.Clear();
			m_cachedDraw.Clear();

			m_desc.m_data->GetData(xStart, xEnd, m_points);
			
			m_currentXStart = xStart;
			m_currentXEnd = xEnd;
		}

		void Chart::DataSource::ExpandCachedData(double xStartExpansion, double xEndExpansion)
		{
			List<ChartDataPoint> leftExpPoints, rightExpPoints;

			if (xStartExpansion > 0)
			{
				m_desc.m_data->GetData(m_currentXStart - xStartExpansion, m_currentXStart, leftExpPoints);

				m_currentXStart -= xStartExpansion;
			}

			if (xEndExpansion > 0)
			{
				m_desc.m_data->GetData(m_currentXEnd, m_currentXEnd + xEndExpansion, rightExpPoints);

				m_currentXEnd += xEndExpansion;
			}

			if (m_points.getCount())
			{
				int32 idx = leftExpPoints.IndexOf([this](const ChartDataPoint& pt) { return pt.m_x == m_points[0].m_x; });
			
				m_points.InsertArray(0, &leftExpPoints[0], idx == -1 ? leftExpPoints.getCount() : idx);
			}
			
			if (m_points.getCount())
			{
				int32 idx = rightExpPoints.IndexOf([this](const ChartDataPoint& pt) { return pt.m_x == m_points.LastItem().m_x; });
			
				if (idx == -1)
					m_points.AddArray(&rightExpPoints[0], rightExpPoints.getCount());
				else if (idx < rightExpPoints.getCount() - 1)
					m_points.AddArray(&rightExpPoints[idx + 1], rightExpPoints.getCount() - idx - 1);
			}

			m_cachedDraw.Clear();
		}

		//////////////////////////////////////////////////////////////////////////

		ChartBackgroundUpdater::ChartBackgroundUpdater()
		{
			StartBackground(L"Chart worker", 1);
		}

		ChartBackgroundUpdater::~ChartBackgroundUpdater()
		{
			StopBackground();
		}

		void ChartBackgroundUpdater::BackgroundMainProcess(ChartBackgroundTask& item)
		{
			item.m_subject->BackgroundDataThread(item.m_init);
		}

		//////////////////////////////////////////////////////////////////////////
		
		Chart::Chart(const List<ChartDataSourceDesc>& sources, const ChartFormatDesc& format, const ChartStyle& style, Point size)
			: Chart(sources, format, style, {}, size)
		{ }

		Chart::Chart(const List<ChartDataSourceDesc>& sources, const ChartFormatDesc& format, const ChartStyle& style, const ChartObjectsDesc& objects, Point size)
			: Control({}, size), m_format(format), m_objects(objects)
			, m_style(style)
		{
			m_size = size;
			m_loadingIcon = PresentationManager::getSingleton().GetAnimatedIcon(style.m_loadingIconID)->CreateInstance();

			if (sources.getCount())
			{
				for (ChartDataSourceDesc& src : sources)
				{
					m_data.Add(DataSource(src));
				}

				if (MultithreadedChart)
				{
					ChartBackgroundUpdater::getSingleton().AddWorkItem(ChartBackgroundTask(this, true));
				}
				else
				{
					BackgroundDataThread(true);
				}

			}
		}

		Chart::~Chart()
		{
			
		}

		void Chart::Draw(Sprite* sprite)
		{
			RenderStateManager* rsm = sprite->getRenderDevice()->getRenderState();

			const ColorValue borderColor = m_style.m_chartBorderColor;

			// draw border
			const Point pos = GetAbsolutePosition();
			Rectangle bgArea = { pos, m_size };
			sprite->DrawLine(SystemUI::GetWhitePixel(), bgArea.getTopRight(), bgArea.getBottomRight(), borderColor, 1, LineCapOptions::Butt);
			sprite->DrawLine(SystemUI::GetWhitePixel(), bgArea.getBottomLeft(), bgArea.getBottomRight(), borderColor, 1, LineCapOptions::Butt);

			if (m_isReady) 
			{
				DrawDataSeries(sprite, bgArea);

				Font* titleFont = m_format.m_titleFont;

				if (m_format.m_axisX)
				{
					Matrix prevT = sprite->getTransform();

					List<AxisLine> lines = m_format.m_axisX->GetAxisLines(getXStart(), getXEnd(), m_size.X);
					for (AxisLine l : lines)
					{
						int32 xpos = TransformX(l.m_value) + pos.X;
						int32 ypos = m_size.Y + pos.Y;

						Matrix t;
						Matrix::CreateRotationZ(t, ToRadian(90));
						t.SetTranslation((float)xpos, (float)ypos, 0);

						sprite->SetTransform(t);
						m_format.m_axisX->DrawAxisLine(sprite, l);
					}

					sprite->SetTransform(prevT);

					if (m_format.m_axisX->hasZeroLine())
					{
						SetScissor(sprite, &bgArea);

						int32 x0 = TransformX(0) + pos.X;
						sprite->DrawLine(SystemUI::GetWhitePixel(), Point(x0, pos.Y), Point(x0, m_size.Y+pos.Y), m_style.m_chartZeroLineColor, 1, LineCapOptions::Butt);

						SetScissor(sprite, nullptr);
					}
				}
				if (m_format.m_axisY)
				{
					Matrix prevT = sprite->getTransform();

					List<AxisLine> lines = m_format.m_axisY->GetAxisLines(getYStart(), getYEnd(), m_size.Y);
					for (AxisLine l : lines)
					{
						int32 xpos = m_size.X + pos.X;
						int32 ypos = TransformY(l.m_value) + pos.Y;

						Matrix t;
						Matrix::CreateTranslation(t, (float)xpos, (float)ypos, 0);

						sprite->SetTransform(t);
						m_format.m_axisY->DrawAxisLine(sprite, l);
					}

					sprite->SetTransform(prevT);

					if (m_format.m_axisY->hasZeroLine())
					{
						SetScissor(sprite, &bgArea);

						int32 y0 = TransformY(0) + pos.Y;
						sprite->DrawLine(SystemUI::GetWhitePixel(), Point(pos.X, y0), Point(pos.X + m_size.X, y0), m_style.m_chartZeroLineColor, 1, LineCapOptions::Butt);

						SetScissor(sprite, nullptr);
					}
				}
				
				DrawObjects(sprite, bgArea);

				if (titleFont && m_title.size())
				{
					Point sz = titleFont->MeasureString(m_title);
					Point titlePos = Point(bgArea.getRight() - sz.X - 4, bgArea.Y - sz.Y);

					sprite->DrawLine(SystemUI::GetWhitePixel(), { bgArea.getRight(), titlePos.Y }, bgArea.getBottomRight(), borderColor, 1, LineCapOptions::Butt);
					sprite->DrawLine(SystemUI::GetWhitePixel(), { titlePos.X, bgArea.getTop() }, bgArea.getTopRight(), borderColor, 1, LineCapOptions::Butt);

					titleFont->DrawString(sprite, m_title, titlePos, CV_White);
				}

				if (titleFont && (m_showDefaultLegend || m_showCustomLegend))
				{
					List<ChartCustomLegend> legends;

					if (m_showDefaultLegend)
					{
						for (const auto& d : m_data)
						{
							if (d.m_desc.m_name.empty())
								continue;

							legends.Add({ d.m_desc.m_name, d.m_desc.m_color });
						}
					}
					else if (m_showCustomLegend)
					{
						legends = m_objects.m_customLegends;
					}

					int32 legendOffset = (titleFont->getLineHeightInt() - 5) / 2;

					if (m_format.m_legendLocation == LayoutLocation::Left)
					{
						int32 maxLegendItemWidth = 0;
						for (const auto& le : legends)
						{
							const String& name = le.m_text;

							Point sz = titleFont->MeasureString(name);
							maxLegendItemWidth = Math::Max(sz.X, maxLegendItemWidth);
						}

						int32 legendAreaWidth = Math::Min(maxLegendItemWidth, 100);
						Point legendPos = bgArea.getPosition() - Point(legendAreaWidth, 0);

						for (const auto& le : legends)
						{
							int32 lineCount = 0;
							String name = titleFont->LineBreakString(le.m_text, legendAreaWidth, true, lineCount);
							Point sz = titleFont->MeasureString(name);

							sprite->Draw(SystemUI::GetWhitePixel(), Rectangle(legendPos + Point(0, legendOffset), { 5,5 }), nullptr, le.m_color);
							titleFont->DrawString(sprite, name, legendPos + Point(10, 0), borderColor);

							legendPos.Y += sz.Y + (lineCount > 1 ? 5 : 0);
						}
					}
					else
					{
						Point legendPos = bgArea.getPosition() - Point(0, titleFont->getLineHeightInt());

						for (const auto& le : legends)
						{
							const String& name = le.m_text;

							Point sz = titleFont->MeasureString(name);

							sprite->Draw(SystemUI::GetWhitePixel(), Rectangle(legendPos + Point(0, legendOffset), { 5,5 }), nullptr, le.m_color);
							titleFont->DrawString(sprite, name, legendPos + Point(10, 0), borderColor);

							legendPos.X += sz.X + 20;
						}
						
					}
				}
			}
			else
			{
				PresentationManager::getSingleton().GetAnimatedIcon(m_style.m_loadingIconID)->Draw(sprite, m_loadingIcon, getAbsoluteArea().getCenter(), 64);
			}
		}

		void Chart::Update(const AppTime* time)
		{
			if (m_scrollable && IsInteractive)
			{
				Mouse* mouse = InputAPIManager::getSingleton().getMouse();

				if (getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					if (mouse->IsLeftPressedState())
					{
						double xShift = -mouse->getDX() / m_xZoom;

						double xStart = getXStart();
						double xEnd = getXEnd();

						double newXStart = xStart + xShift;
						double newXEnd = xEnd + xShift;

						if (newXStart < m_xLowbound || newXEnd > m_xHighbound)
							xShift = 0;

						m_xStart += xShift;

						if (m_autoFitYLocal || m_autoFitYGlobal)
							_CalculateAutoYRange();
					}
					else if (mouse->getDZ())
					{
						double xStart = getXStart();
						double xEnd = getXEnd();
						double zoomCenter = InvTransformX(mouse->getX() - GetAbsolutePosition().X);

						double zoomStep = mouse->getDZ() / 120.0;
						double zoomScale = pow(2, zoomStep * 0.1);

						xStart = zoomCenter + (xStart - zoomCenter) * zoomScale;
						xEnd = zoomCenter + (xEnd - zoomCenter) * zoomScale;

						xStart = Math::Max(m_xLowbound, xStart);
						xEnd = Math::Min(m_xHighbound, xEnd);

						xStart = Math::Min(xEnd, xStart);
						xEnd = Math::Max(xEnd, xStart);

						double xSpan = xEnd - xStart > 0 ? (xEnd - xStart) : 1.0;

						m_xStart = xStart;
						m_xZoom = m_size.X / xSpan;

						if (m_autoFitYLocal || m_autoFitYGlobal)
							_CalculateAutoYRange();
					}
				}
			}
		}

		void Chart::RefreshData(bool async)
		{
			m_isReady = false;

			if (async)
			{
				ChartBackgroundUpdater::getSingleton().AddWorkItem(ChartBackgroundTask(this));
			}
			else
			{
				BackgroundDataThread(false);
			}
		}

		void Chart::BackgroundDataThread(bool init)
		{
			if (init)
			{
				bool noData = true;

				for (DataSource& src : m_data)
				{
					double xMin, xMax;
					if (src.m_desc.m_data->GetDomain(xMin, xMax))
					{
						if (noData)
						{
							noData = false;
							m_xLowbound = xMin;
							m_xHighbound = xMax;
						}
						else
						{
							m_xLowbound = Math::Min(xMin, m_xLowbound);
							m_xHighbound = Math::Max(xMin, m_xHighbound);
						}
					}
				}

				m_xStart = m_xHighbound - m_size.X / m_xZoom;
			}

			_RecacheData();

			if (m_autoFitYLocal || m_autoFitYGlobal)
				_CalculateAutoYRange();

			m_isReady = true;
		}

		void Chart::_RecacheData()
		{
			double xStart = getXStart();
			double xEnd = getXEnd();

			for (DataSource& ds : m_data)
			{
				ds.RecacheData(xStart, xEnd);
			}
		}

		void Chart::_CalculateAutoYRange()
		{
			const double xStart = getXStart();
			const double xEnd = getXEnd();

			bool noData = true;
			double yMin = 0;
			double yMax = 0;

			for (const DataSource& ds : m_data)
			{
				for (const auto& p : ds.m_points)
				{
					bool isInRange = m_autoFitYGlobal ? true : p.m_x >= xStart && p.m_x <= xEnd;

					if (isInRange)
					{
						if (noData)
						{
							noData = false;

							if (ds.m_desc.m_mode == ChartSeriesMode::Candlestick)
							{
								yMin = p.m_y_low;
								yMax = p.m_y_high;
							}
							else
							{
								yMin = yMax = p.m_y;
							}
						}
						else
						{
							if (ds.m_desc.m_mode == ChartSeriesMode::Candlestick)
							{
								yMin = Math::Min(yMin, p.m_y_low);
								yMax = Math::Max(yMax, p.m_y_high);
							}
							else
							{
								yMin = Math::Min(yMin, p.m_y);
								yMax = Math::Max(yMax, p.m_y);
							}
						}
						
					}
				}
			}

			if (!noData)
			{
				m_yStart = yMin;
				m_yZoom = yMax - yMin > 0 ? m_size.Y / (yMax - yMin) : 1;
			}
		}

		void Chart::RefreshObjects(const ChartObjectsDesc& objects)
		{
			m_objects = objects;
		}

		void Chart::SetXView(double xStart, double xSpan)
		{
			m_xStart = xStart;
			m_xZoom = m_size.X / xSpan;

			RefreshData(MultithreadedChart);
			//RecacheData();
			//CalculateAutoYRange();
		}

		void Chart::SetYView(double yStart, double ySpan)
		{
			m_autoFitYLocal = false;
			m_autoFitYGlobal = false;
			m_yStart = yStart;
			m_yZoom = ySpan;
		}

		void Chart::SetXViewMinMax(double xMin, double xMax)
		{
			SetXView(xMin, xMax - xMin > 0 ? m_size.X / (xMax - xMin) : 1);
		}
		void Chart::SetYViewMinMax(double yMin, double yMax)
		{
			SetYView(yMin, yMax - yMin > 0 ? m_size.Y / (yMax - yMin) : 1);
		}

		void Chart::SetYViewAuto(ChartAutoViewMode mode, bool v)
		{
			m_autoFitYLocal = false;
			m_autoFitYGlobal = false;

			if (mode == ChartAutoViewMode::Global)
				m_autoFitYGlobal = v;
			else
				m_autoFitYLocal = v;

			if (m_autoFitYLocal || m_autoFitYGlobal)
				_CalculateAutoYRange();
		}

		int32 Chart::TransformX(double x) const
		{
			return (int32)((x - m_xStart) * m_xZoom);
		}
		int32 Chart::TransformY(double y) const
		{
			return (int32)(m_size.Y - (y - m_yStart) * m_yZoom);
		}
		Point Chart::Transform(const ChartDataPoint& pt) const
		{
			return Transform(pt.m_x, pt.m_y);
		}
		Point Chart::Transform(PointD p) const
		{
			return Transform(p.X, p.Y);
		}
		Point Chart::Transform(double x, double y) const
		{
			return Point(TransformX(x), TransformY(y));
		}

		double Chart::InvTransformX(int32 x) const
		{
			return x / m_xZoom + m_xStart;
		}

		int32 Chart::GetTitleHeight() const
		{
			Font* titleFont = m_format.m_titleFont;
			if (titleFont && m_title.size())
			{
				return titleFont->MeasureString(m_title).Y;
			}
			return 0;
		}

		void Chart::DrawDataSeries(Sprite* sprite, Rectangle bgArea)
		{
			Rectangle bgAreaExpanded = bgArea;
			bgAreaExpanded.Inflate(1, 1);

			Point pos = bgArea.getPosition();

			SetScissor(sprite, &bgAreaExpanded);

			for (DataSource& ds : m_data)
			{
				ChartSeriesMode seriesMode = ds.m_desc.m_mode;
				int32 candleWidth = 0;

				if (seriesMode == ChartSeriesMode::Candlestick)
				{
					candleWidth = ComputeFitCandlestickWidth(m_size.X, ds.m_points.getCount());
					if (candleWidth <= 0)
						seriesMode = ChartSeriesMode::Line;
				}

				bool updateCache = false;
				if (m_cachedRender)
				{
					updateCache = m_lastDataSeriesRect != bgArea || ds.m_cachedDraw.getCount() == 0;

					if (updateCache)
					{
						sprite->End();
						sprite->Begin((Sprite::SpriteSettings)(Sprite::SPRMix_ManageStateAlphaBlended | Sprite::SPR_RecordBatch));
					}
					else
					{
						sprite->DrawBatch(ds.m_cachedDraw);
						continue;
					}
				}
				
				if (seriesMode == ChartSeriesMode::Line)
				{
					ColorValue color = ds.m_desc.m_color;

					for (int32 i = 0; i < ds.m_points.getCount() - 1; i++)
					{
						const ChartDataPoint& cur = ds.m_points[i];
						const ChartDataPoint& nxt = ds.m_points[i + 1];

						Point curPos = Transform(cur) + pos;
						Point nxtPos = Transform(nxt) + pos;

						sprite->DrawLine(SystemUI::GetWhitePixel(), curPos, nxtPos, color, 1, LineCapOptions::Butt);
					}
				}
				else if (seriesMode == ChartSeriesMode::Dots)
				{
					ColorValue color = ds.m_desc.m_color;

					for (const ChartDataPoint& pt : ds.m_points)
					{
						Point curPos = Transform(pt) + pos;

						Rectangle dstRect = { curPos - Point(4,4),Point(8,8) };

						sprite->DrawCircle(SystemUI::GetWhitePixel(), dstRect, nullptr, 1, color);
					}
				}
				else
				{
					for (const ChartDataPoint& pt : ds.m_points)
					{
						Point candlePos = pos;
						candlePos.X += TransformX(pt.m_x);

						DrawCandleStick(sprite, pt.m_y_open, pt.m_y_high, pt.m_y_low, pt.m_y, (float)candleWidth, candlePos);
					}
				}

				if (updateCache)
				{
					sprite->End();
					
					ds.m_cachedDraw = sprite->getDrawEntries();

					sprite->Begin(Sprite::SPRMix_ManageStateAlphaBlended);
					sprite->DrawBatch(ds.m_cachedDraw);
				}
			}

			SetScissor(sprite, nullptr);

			m_lastDataSeriesRect = bgArea;
		}

		void Chart::DrawObjects(Sprite* sprite, Rectangle bgArea)
		{
			Point pos = bgArea.getPosition();

			SetScissor(sprite, &bgArea);

			if (m_objects.m_customAxisLines.getCount() > 0)
			{
				for (auto& line : m_objects.m_customAxisLines)
				{
					if (line.m_dir == LayoutDirection::Horizontal)
					{
						int32 y0 = TransformY(line.m_position) + pos.Y;
						sprite->DrawLine(SystemUI::GetWhitePixel(), Point(pos.X, y0), Point(pos.X + m_size.X, y0), line.m_color, 1, LineCapOptions::Butt);
					}
					else
					{
						int32 x0 = TransformX(line.m_position) + pos.X;
						sprite->DrawLine(SystemUI::GetWhitePixel(), Point(x0, pos.Y), Point(x0, pos.Y + m_size.Y), line.m_color, 1, LineCapOptions::Butt);
					}
				}
			}

			if (m_objects.m_customLines.getCount() > 0)
			{
				for (auto& line : m_objects.m_customLines)
				{
					Point p0 = Transform(line.m_start) + pos;
					Point p1 = Transform(line.m_end) + pos;

					sprite->DrawLine(SystemUI::GetWhitePixel(), p0, p1, line.m_color, 1, LineCapOptions::Butt);
				}
			}

			if (m_objects.m_customGraphics.getCount() > 0)
			{
				for (auto& obj : m_objects.m_customGraphics)
				{
					Point p = Transform(obj.m_position) + pos;
					
					PointF ofs(0, 0);// = (PointF)obj.m_graphic.getSize() * 0.5f - obj.m_textureSrcAnchor;

					Matrix t;
					Matrix::CreateRotationZ(t, (float)obj.m_rotation);
					t.SetTranslation(p.X + ofs.X, p.Y + ofs.Y, 0);

					SpriteTransformScope sts(sprite, t, false);
					obj.m_graphic.DrawCentered(sprite, Point(0, 0), Point(0, 0));
				}
			}

			SetScissor(sprite, nullptr);
		}

		void Chart::DrawCandleStick(Sprite* sprite, double open, double high, double low, double close, float width, Point basePos)
		{
			const ColorValue bullColor = m_style.m_UIGreen;
			const ColorValue bearColor = m_style.m_UIRed;
			const ColorValue color = open <= close ? bullColor : bearColor;

			if (low < Math::Min(open, close) || high > Math::Max(open, close))
			{
				Point a = basePos;
				Point b = basePos;
				a.Y += TransformY(low);
				b.Y += TransformY(high);

				sprite->DrawLine(SystemUI::GetWhitePixel(), a, b, color, 1, LineCapOptions::Butt);
			}

			Point a = basePos;
			Point b = basePos;
			a.Y += TransformY(open);
			b.Y += TransformY(close);

			if (a.Y == b.Y)
			{
				int32 off = (int32)Math::Sign((float)(close - open));
				if (off == 0)
					off = 1;
				b.Y += off;
			}

			sprite->DrawLine(SystemUI::GetWhitePixel(), a, b, color, width, LineCapOptions::Butt);
		}

		int32 Chart::ComputeFitCandlestickWidth(int32 width, int32 barCount)
		{
			barCount = Math::Max(1, barCount);

			int32 padding = 1;
			int32 perBarSpace = width / barCount;
			perBarSpace -= padding;
			
			int32 barWidth = (Math::Max(1, perBarSpace) - 1) / 2 * 2 + 1;
			barWidth = Math::Min(MaxCandleWidth, barWidth);
			return barWidth;
		}

		//////////////////////////////////////////////////////////////////////////

#pragma endregion

#pragma region DistributionChartSerires

		DistributionChartSerires::DistributionChartSerires(const ChartStyle& style, int32 chartSize, int32 digits, LayoutDirection layoutDirection)
			: m_style(style)
			, m_chartSize(chartSize)
			, m_digits(digits)
			, m_layoutDirection(layoutDirection)
		{

		}
		DistributionChartSerires::~DistributionChartSerires()
		{
			Clear();
		}

		void DistributionChartSerires::Draw(Sprite* sprite)
		{
			int32 subViewMaxTitleHeight = 0;
			for (SubView& sv : m_views)
			{
				sv.Draw(sprite);
				subViewMaxTitleHeight = Math::Max(subViewMaxTitleHeight, sv.m_chart->GetTitleHeight());
			}

			if (m_views.getCount() > 0)
			{
				// draw grand title
				Font* fnt = FontManager::getSingleton().getFont(L"english");
				Point sz = fnt->MeasureString(m_grandTitle);

				Rectangle titleArea = getAbsoluteArea();
				titleArea.Y -= subViewMaxTitleHeight;
				titleArea.Height = sz.Y;

				fnt->DrawString(sprite, m_grandTitle, titleArea.GetCenterRegion(sz).getPosition(), CV_White);

				// draw legends
				Font* fnt8 = FontManager::getSingleton().getFont(m_style.m_chartLegendFontName);
				int32 legendOffset = (fnt8->getLineHeightInt() - 5) / 2;
				int32 legendSpacing = 5;

				Point legendPos = GetAbsolutePosition() - Point(100, 0);
				legendPos += m_views[0].m_chart->Position - Position;

				for (const auto& l : m_legends)
				{
					sprite->Draw(SystemUI::GetWhitePixel(), Rectangle(legendPos + Point(0, legendOffset), { 5,5 }), nullptr, l.m_color);
					fnt8->DrawString(sprite, l.m_text, legendPos + Point(10, 0), CV_White);

					legendPos.Y += legendSpacing + fnt8->MeasureString(l.m_text).Y;
				}
			}
		}
		void DistributionChartSerires::Update(const AppTime* time)
		{
			UpdateLayout();

			for (SubView& sv : m_views)
			{
				sv.m_chart->Update(time);
			}
		}

		void DistributionChartSerires::UpdateLayout()
		{
			const Point offset = m_layoutDirection == LayoutDirection::Horizontal ? Point(m_chartSize + m_chartSpacing, 0) : Point(0, m_chartSize + m_chartSpacing);
			const Point chartOffset = { 0, 50 };

			int32 i = 0;
			for (SubView& sv : m_views)
			{
				sv.m_chart->BaseOffset = BaseOffset;
				sv.m_chart->Position = Position + offset * i + chartOffset;

				i++;
			}

			m_size = Point(0, 0);
			for (const SubView& sv : m_views)
			{
				Point sz = sv.m_chart->getArea().getBottomRight() - Position;

				m_size.X = Math::Max(sz.X, m_size.X);
				m_size.Y = Math::Max(sz.Y + sv.m_chart->GetTitleHeight(), m_size.Y);
			}
			m_size.Y += 80;
		}

		void DistributionChartSerires::Clear()
		{
			m_views.Clear();
			m_legends.Clear();
		}

		void DistributionChartSerires::AddHistogram(const Utility::DistributionHistogram* h, const String& title, const List<DistributionChartLine>& lines)
		{
			m_views.Add(SubView(&m_style, h, m_chartSize, m_digits, title, lines));

			UpdateLayout();

			// update legends
			for (const auto& e : lines)
			{
				bool found = false;
				for (const auto& l : m_legends)
				{
					if (l.m_text == e.m_text)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					m_legends.Add({ e.m_text, e.m_color });
				}
			}
		}

		void DistributionChartSerires::UnifyScales()
		{
			double maxValue = 0;
			double minValue = 0;

			const SubView* largestView = nullptr;
			for (SubView& sv : m_views)
			{
				double y = sv.m_chart->getYStart();
				maxValue = Math::Max(maxValue, y);
				minValue = Math::Min(minValue, y);

				y = sv.m_chart->getYEnd();
				maxValue = Math::Max(maxValue, y);
				minValue = Math::Min(minValue, y);
			}

			if (maxValue > minValue)
			{
				for (SubView& sv : m_views)
				{
					sv.m_chart->SetYViewMinMax(minValue, maxValue);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////

		DistributionChartSerires::SubView::SubView(const ChartStyle* style, const Utility::DistributionHistogram* histogramRef, int32 chartSize, int32 digits, const String& title, const List<DistributionChartLine>& lines)
			: m_histoRef(histogramRef)
			, m_lines(lines)
			, m_style(style)
		{
			int32 binCount = m_histoRef->getBinCount();

			m_xAxis = new NumberRangeAxisFormatter(ChartAxisSide::Min, style->m_axisLabelFontName, style->m_axisLabelColor, &m_histoRef->getBinRanges(), digits);
			m_chart = new Chart(
				{ { new RawDataSource(&m_histoRef->getBins()), CV_White, ChartSeriesMode::Line } },
				{ m_xAxis, new NumberAxisFormatter(ChartAxisSide::Min, style->m_axisLabelFontName, style->m_axisLabelColor, 1, -1, true), style->m_chartTitleFontName },
				*style,
				Point(chartSize, chartSize));

			m_chart->setTitle(title);
			m_chart->SetXView(0, binCount - 1);
		}

		DistributionChartSerires::SubView::~SubView()
		{
			DELETE_AND_NULL(m_chart);
			DELETE_AND_NULL(m_xAxis);
			m_histoRef = nullptr;
		}

		DistributionChartSerires::SubView::SubView(SubView&& o)
			: m_chart(o.m_chart), m_xAxis(o.m_xAxis), m_histoRef(o.m_histoRef), m_lines(std::move(o.m_lines))
		{
			o.m_chart = nullptr;
			o.m_xAxis = nullptr;
			o.m_histoRef = nullptr;
		}
		DistributionChartSerires::SubView& DistributionChartSerires::SubView::operator=(SubView&& o)
		{
			if (this != &o)
			{
				this->~SubView();
				new (this)SubView(std::move(o));
			}
			return *this;
		}

		void DistributionChartSerires::SubView::Draw(Sprite* sprite)
		{
			DrawVerticalLine(sprite, 0, m_style->m_histoChartCenterLine);

			for (const auto& l : m_lines)
			{
				if (l.m_direction == LayoutDirection::Vertical)
				{
					DrawVerticalLine(sprite, l.m_position, l.m_color);
				}
			}

			m_chart->Draw(sprite);
		}

		int32 DistributionChartSerires::SubView::CalculateXPosition(double value) const
		{
			return Math::Round(m_chart->getSize().X * m_histoRef->CalculateXPosition(value));
		}

		void DistributionChartSerires::SubView::DrawVerticalLine(Sprite* sprite, double value, ColorValue cv) const
		{
			Rectangle chartArea = m_chart->getAbsoluteArea();

			int32 x0 = CalculateXPosition(value) + chartArea.getLeft();
			int32 yb = chartArea.getBottom();
			int32 yt = chartArea.getTop();

			sprite->DrawLine(SystemUI::GetWhitePixel(), Point(x0, yt), Point(x0, yb), cv, 1, LineCapOptions::Butt);
		}

		//////////////////////////////////////////////////////////////////////////

#pragma endregion

#pragma region Piechart

		Piechart::Piechart(const StyleSkin* skin, const ChartStyle& style, const Point& pos, const Point& size, const List<PiechartComponent>& cmps)
			: Control(skin, pos, size)
			, m_style(style)
		{
			m_labelFont = FontManager::getSingleton().getFont(style.m_pieChartLabelFontName);
			SetComponents(cmps);
		}

		Piechart::~Piechart()
		{

		}

		void Piechart::Draw(Sprite* sprite)
		{
			Rectangle area = getAbsoluteArea();

			int32 minLen = Math::Min(area.Width, area.Height);

			Rectangle circleArea = area.GetCenterRegion(Point(minLen, minLen));

			for (int32 i = 0; i < m_components.getCount(); i++)
			{
				const PiechartComponent& c = m_components[i];
				const ComponentData& d = m_data[i];

				if (c.m_value > 0)
				{
					sprite->DrawCircle(SystemUI::GetWhitePixel(), circleArea, nullptr, d.m_startAngle, d.m_endAngle, 8, c.m_color);
				}
			}

			DrawLabels(sprite, m_leftLabels, area.X + 10, circleArea, true);
			DrawLabels(sprite, m_rightLabels, area.getRight() - 10, circleArea, false);
		}

		void Piechart::DrawLabels(Sprite* sprite, const List<PiechartLabel>& lbls, int xPos, const Rectangle& circleArea, bool lr)
		{
			Point center = Math::Round(circleArea.getCenter());

			Font* labelFont = m_labelFont;

			int32 circleBorderX = lr ? (circleArea.X - 10) : (circleArea.getRight() + 10);

			for (const PiechartLabel& l : lbls)
			{
				Point lp1;
				Point lp2;

				if (lr)
				{
					lp1 = Point(xPos, l.m_yPos + center.Y);
					lp2 = lp1 + Point(0, labelFont->getLineHeightInt());
				}
				else
				{
					Point sz = labelFont->MeasureString(l.m_name);
					lp1 = Point(xPos - sz.X, l.m_yPos + center.Y);

					sz = labelFont->MeasureString(l.m_value);
					lp2 = Point(xPos - sz.X, l.m_yPos + labelFont->getLineHeightInt() + center.Y);
				}

				labelFont->DrawString(sprite, l.m_name, lp1, CV_White);
				labelFont->DrawString(sprite, l.m_value, lp2, CV_White);

				Point lineStartPt = Point(xPos, lp2.Y);
				Point lineMidPt = Point(circleBorderX, lp2.Y);
				sprite->DrawLine(SystemUI::GetWhitePixel(), lineStartPt, lineMidPt, m_style.m_pieChartLabelLineColor, 1, LineCapOptions::Butt);
				sprite->DrawLine(SystemUI::GetWhitePixel(), lineMidPt, Math::Round(center + l.m_pieExitPos), m_style.m_pieChartLabelLineColor, 1, LineCapOptions::Butt);
			}
		}

		void Piechart::SetComponents(const List<PiechartComponent>& cmps)
		{
			m_components = cmps;
			m_data.ResizeDiscard(cmps.getCount());

			double total = 0;
			for (auto& e : cmps)
			{
				total += e.m_value;
			}
			total = 1.0 / total;

			double accumulatedPercent = 0;
			for (auto& e : cmps)
			{
				ComponentData d;
				d.m_percent = e.m_value * total;
				d.m_endAngle = (float)(Math::Two_PI - accumulatedPercent * Math::Two_PI + Math::Half_PI);
				accumulatedPercent += d.m_percent;
				d.m_startAngle = (float)(Math::Two_PI - accumulatedPercent * Math::Two_PI + Math::Half_PI);

				m_data.Add(d);
			}

            UpdateLabels();
		}

        void Piechart::SetSize(Point sz)
        {
            if (m_size != sz)
            {
                m_size = sz;

                UpdateLabels();
            }
        }

		void Piechart::SetLabelFont(Font* font)
		{
			if (m_labelFont != font)
			{
				m_labelFont = font;

				UpdateLabels();
			}
		}

		void Piechart::ForEachComponentName(FunctorReference<void(String&)> func)
		{
			for (int32 i = 0; i < m_components.getCount(); i++)
			{
				PiechartComponent& c = m_components[i];
				func(c.m_name);
			}
		}

        void Piechart::UpdateLabels()
        {
            m_leftLabels.Clear();
            m_rightLabels.Clear();

            Font* labelFont = m_labelFont;
            int32 minLen = Math::Min(getWidth(), getHeight());
            float radius = minLen * 0.5f;

            for (int32 i = 0; i < m_components.getCount(); i++)
            {
                const PiechartComponent& c = m_components[i];
                const ComponentData& d = m_data[i];

                if (c.m_value > 0)
                {
                    float centerAngle = 0.5f * (d.m_startAngle + d.m_endAngle);

                    PointF pp = { radius * cosf(centerAngle), -radius * sinf(centerAngle) };

                    bool lr = centerAngle > Math::Half_PI&& centerAngle < Math::Two_PI - Math::Half_PI;

                    PiechartLabel l;
                    l.m_name = c.m_name;
                    l.m_value = fmt::format(L"{0:0.1f}%", d.m_percent * 100);
                    l.m_yPos = (int)(pp.Y);

                    if (radius > 20)
                    {
                        pp = { (radius - 5) * cosf(centerAngle), -(radius - 5) * sinf(centerAngle) };
                    }

                    l.m_pieExitPos = Math::Round(pp);
                    (lr ? m_leftLabels : m_rightLabels).Add(l);
                }
            }

            m_leftLabels.Sort([](const auto& a, const auto& b)
            {
                return OrderComparer(a.m_yPos, b.m_yPos);
            });

            m_rightLabels.Sort([](const auto& a, const auto& b)
            {
                return OrderComparer(a.m_yPos, b.m_yPos);
            });

            const int LabelHeight = 1 + 2 * labelFont->getLineHeightInt();

            Relax(m_leftLabels, LabelHeight);
            Relax(m_rightLabels, LabelHeight);
        }

		void Piechart::Relax(List<PiechartLabel>& lbls, int labelHeight)
		{
			for (;;)
			{
				bool moved = false;
				for (int i = 0; i < lbls.getCount(); i++)
				{
					const PiechartLabel* prev = i > 0 ? &lbls[i - 1] : nullptr;
					const PiechartLabel* next = i < lbls.getCount() - 1 ? &lbls[i + 1] : nullptr;
					PiechartLabel& cur = lbls[i];

					if (prev)
					{
						int32 d = prev->m_yPos + labelHeight - cur.m_yPos;
						d = Math::Max(d, 0);
						cur.m_yPos += Math::Sign(d);

						moved |= d != 0;
					}
					if (next)
					{
						int32 d = next->m_yPos - (cur.m_yPos + labelHeight);
						d = Math::Min(d, 0);
						cur.m_yPos += Math::Sign(d);

						moved |= d != 0;
					}
				}

				if (!moved)
					break;
			}
		}
#pragma endregion

#pragma region FieldChart

        FieldChart::FieldChart(const StyleSkin* skin, const ChartStyle& style, const Point& pos, const Point& size, const List2D<double>& data)
            : Control(skin, pos, size)
            , m_fieldData(0,0)
            , m_sizeDirty(true)
			, m_style(style)
        {
            SetFieldData(data);
        }

        FieldChart::~FieldChart()
        {

        }

        void FieldChart::Draw(Sprite* sprite)
        {
            RenderDevice* dev = sprite->getRenderDevice();

            if (m_visualDataDirty)
            {
                m_visualDataDirty = false;
                UpdateFieldData(dev);
            }

            if (m_sizeDirty)
            {
                m_sizeDirty = false;
                UpdateRenderTarget(dev);
            }
			
			sprite->End();

            {
                ScopeRenderTargetChange srtc(dev, 0, m_fieldImage, m_fieldImageDS);

				dev->Clear(CLEAR_ColorAndDepth, 0, 1.0f, 0);

				float fieldSizeWidth = m_fieldData.getWidth() - 1.0f;
				float fieldSizeHeight = m_fieldData.getCount() - 1.0f;

				float aspectRatio = (float)getWidth() / getHeight();
				float radius = 0.5f * sqrtf(fieldSizeWidth * fieldSizeWidth + fieldSizeHeight * fieldSizeHeight);
				float distance;

				bool perspective = true;

				Matrix proj;
				if (perspective)
				{
					float fovY = Math::ToRadian(15);
					float fovX = fovY * aspectRatio;
					distance = radius / Math::Min(tanf(fovX * 0.5f), tanf(fovY * 0.5f));

					Matrix::CreatePerspectiveFovLH(proj, fovY, aspectRatio, 1, 1000);
				}
				else
				{
					distance = radius;
					Matrix::CreateOrthoLH(proj, radius * 2 * aspectRatio, radius * 2, 1, 1000);
				}

				Vector3 posOffset(fieldSizeWidth * 0.5f, 0, fieldSizeHeight * 0.5f);

				float phi = Math::Half_PI * 0.5f;
				Vector3 viewDir;
				viewDir.Z = sinf(m_fieldRot) * sinf(phi);
				viewDir.Y = cosf(phi);
				viewDir.X = cosf(m_fieldRot) * sinf(phi);

				Matrix view;
				Matrix::CreateLookAtLH(view, viewDir.Normalized() * distance + posOffset, posOffset, Vector3::UnitY);

				Camera viewCamera;
				viewCamera.setProjMatrix(proj);
				viewCamera.setViewMatrix(view);
				viewCamera.CalculateMatrices();

				RendererEffectParams::CurrentCamera = &viewCamera;
				RendererEffectParams::LightDirection = Vector3(-0.707f, 0, -0).Normalized();

				Effect* fieldFx = EffectManager::getSingleton().getEffect(L"fieldchart");
				Effect* axisFx = EffectManager::getSingleton().getEffect(L"fieldchartaxis");

                Material fieldMtrl(dev);
				Material axisMtrl(dev);
				fieldMtrl.Diffuse = Color4(m_style.m_fieldChartGeometryColor);
				fieldMtrl.SetPassEffect(0, fieldFx);
				axisMtrl.SetPassEffect(0, axisFx);

                GeometryData geoData;
                geoData.VertexBuffer = m_fieldVertices;
                geoData.IndexBuffer = m_fieldIndices;
                geoData.VertexDecl = m_fieldVtxDecl;
                geoData.PrimitiveCount = m_fieldIndices->getIndexCount() / 3;
                geoData.PrimitiveType = PrimitiveType::TriangleList;
                geoData.VertexCount = m_fieldVertices->getVertexCount();
                geoData.VertexSize = m_fieldVtxDecl->GetVertexSize();

                RenderOperation op;
                op.Material = &fieldMtrl;
                op.GeometryData = &geoData;
                op.RootTransform.LoadIdentity();
                op.RootTransformIsFinal = true;

                dev->Render(&fieldMtrl, &op, 1, -1);

				geoData.VertexBuffer = m_axisVertices;
				geoData.IndexBuffer = nullptr;
				geoData.VertexDecl = m_axisVtxDecl;
				geoData.PrimitiveCount = m_axisVertices->getVertexCount() / 2;
				geoData.PrimitiveType = PrimitiveType::LineList;
				geoData.VertexCount = m_axisVertices->getVertexCount();
				geoData.VertexSize = m_axisVtxDecl->GetVertexSize();
				
				op.Material = &axisMtrl;
				op.GeometryData = &geoData;
				op.RootTransform.LoadIdentity();
				op.RootTransformIsFinal = true;

				dev->Render(&axisMtrl, &op, 1, -1);

				RendererEffectParams::CurrentCamera = nullptr;
            }

			sprite->Begin(Sprite::SPRMix_ManageStateAlphaBlended);
            sprite->Draw(m_fieldImage->GetColorTexture(), GetAbsolutePosition(), CV_White);
        }

        void FieldChart::SetFieldData(const List2D<double>& data)
        {
            if (m_dataWidth != data.getWidth() || m_dataHeight != data.getCount())
            {
                m_dataWidth = data.getWidth();
                m_dataHeight = data.getCount();

                DELETE_AND_NULL(m_fieldVertices);
                DELETE_AND_NULL(m_fieldIndices);
            }

            m_fieldData = data;
            m_visualDataDirty = true;
        }

        void FieldChart::SetSize(Point sz)
        {
            if (m_size != sz)
            {
                DELETE_AND_NULL(m_fieldImage);
                DELETE_AND_NULL(m_fieldImageDS);
                m_size = sz;
                m_sizeDirty = true;
            }
        }

        void FieldChart::UpdateFieldData(RenderDevice* dev)
        {
            ObjectFactory* objFac = dev->getObjectFactory();

            int32 fieldWidth = m_fieldData.getWidth();
            int32 fieldHeight = m_fieldData.getCount();

            if (m_fieldVtxDecl == nullptr)
            {
                m_fieldVtxDecl = objFac->CreateVertexDeclaration(
                {
                    { 0, VertexElementFormat::VEF_Vector3, VertexElementUsage::VEU_Position },
					{ 12, VertexElementFormat::VEF_Vector3, VertexElementUsage::VEU_Normal },
					{ 24, VertexElementFormat::VEF_Vector3, VertexElementUsage::VEU_TextureCoordinate, 0 }
                });
            }

			if (m_axisVtxDecl == nullptr)
			{
				m_axisVtxDecl = objFac->CreateVertexDeclaration(
				{
					{ 0, VertexElementFormat::VEF_Vector3, VertexElementUsage::VEU_Position },
					{ 12, VertexElementFormat::VEF_Color, VertexElementUsage::VEU_Color }
				});
			}

            if (m_fieldVertices == nullptr)
            {
                m_fieldVertices = objFac->CreateVertexBuffer(fieldWidth*fieldHeight, m_fieldVtxDecl, BU_WriteOnly);
            }

            if (m_fieldIndices == nullptr)
            {
                m_fieldIndices = objFac->CreateIndexBuffer(IndexBufferFormat::Bit32, 6 * (fieldWidth - 1) * (fieldHeight - 1), BU_WriteOnly);
            }

			if (m_axisVertices == nullptr)
			{
				m_axisVertices = objFac->CreateVertexBuffer(10, m_fieldVtxDecl, BU_WriteOnly);
			}

			double minVal = 0;
			double maxVal = 0.5 * (Math::Max(fieldHeight, fieldWidth) - 1);

            FieldVertex* vtxPtr = (FieldVertex*)m_fieldVertices->Lock(LOCK_None);
            int32* idxPtr = (int32*)m_fieldIndices->Lock(LOCK_None);

            for (int32 i = 0; i < fieldHeight; i++)
            {
                for (int32 j = 0; j < fieldWidth; j++)
                {
					double v = m_fieldData[i][j];
                    vtxPtr->m_pos = Vector3((float)j, (float)v, (float)i);
					vtxPtr->m_normal = Vector3(0, 1, 0);
					vtxPtr->m_data = Vector3(1.0f, 1.0f, 1.0f);// (float)m_fieldData[i][j], 0.0f);
					//vtxPtr->m_data = Vector2((float)m_fieldData[i][j], 0.0f);
                    vtxPtr++;

					minVal = Math::Min(v, minVal);
					maxVal = Math::Max(v, maxVal);
                }
            }

            for (int32 i = 0; i < fieldHeight - 1; i++)
            {
                for (int32 j = 0; j < fieldWidth - 1; j++)
                {
                    *idxPtr++ = i * fieldWidth + j;
                    *idxPtr++ = i * fieldWidth + j + 1;
                    *idxPtr++ = (i + 1) * fieldWidth + j + 1;

                    *idxPtr++ = i * fieldWidth + j;
                    *idxPtr++ = (i + 1) * fieldWidth + j + 1;
                    *idxPtr++ = (i + 1) * fieldWidth + j;
                }
            }

            m_fieldVertices->Unlock();
            m_fieldIndices->Unlock();

			ColorValue axisColor = m_style.m_fieldChartAxisLineColor;
			ColorValue vertAxisColor = m_style.m_fieldChartVerticalAxisLineColor;

			AxisLineVertex* axisPtr = (AxisLineVertex*)m_axisVertices->Lock(LOCK_None);
			
			*axisPtr++ = { Vector3(0, 0, 0), axisColor };
			*axisPtr++ = { Vector3((fieldWidth - 1) * 1.5f, 0, 0), axisColor };

			*axisPtr++ = { Vector3(0, 0, 0), axisColor };
			*axisPtr++ = { Vector3(0, 0, (fieldHeight - 1) * 1.5f), axisColor };

			*axisPtr++ = { Vector3(fieldWidth - 1.0f, 0, 0), vertAxisColor };
			*axisPtr++ = { Vector3(fieldWidth - 1.0f, 0, fieldHeight - 1.0f), vertAxisColor };

			*axisPtr++ = { Vector3(0, 0, fieldHeight - 1.0f), vertAxisColor };
			*axisPtr++ = { Vector3(fieldWidth - 1.0f, 0, fieldHeight - 1.0f), vertAxisColor };

			*axisPtr++ = { Vector3(0, (float)minVal, 0), axisColor };
			*axisPtr++ = { Vector3(0, (float)maxVal, 0), axisColor };

			m_axisVertices->Unlock();
        }

        void FieldChart::UpdateRenderTarget(RenderDevice* dev)
        {
            ObjectFactory* objFac = dev->getObjectFactory();

            if (m_fieldImage == nullptr)
            {
                m_fieldImage = objFac->CreateRenderTarget(m_size.X, m_size.Y, FMT_A8R8G8B8);
            }
            if (m_fieldImageDS == nullptr)
            {
                m_fieldImageDS = objFac->CreateDepthStencilBuffer(m_size.X, m_size.Y, DEPFMT_Depth24Stencil8);
            }
        }

		void FieldChart::Update(const AppTime* time)
		{
			//m_fieldRot += time->ElapsedTime / Math::Half_PI;
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (IsInteractive && getAbsoluteArea().Contains(mouse->GetPosition()) && mouse->IsLeftPressedState())
			{
				m_fieldRot += Math::ToRadian(mouse->getDX());
			}
		}

#pragma endregion


	}
}