#pragma once

#include "UICommon.h"
#include "apoc3d/Collections/List2D.h"
#include "apoc3d/Core/BackgroundWorker.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class Chart;

		struct ChartStyle
		{
			ColorValue m_chartAxisAlignedLineColor; // CV_UIChartGrid
			ColorValue m_chartBorderColor;			// CV_UIChartBorder
			ColorValue m_chartZeroLineColor;		// CV_UIChartGrid

			String m_chartLegendFontName;			// FNT_UI8
			String m_chartTitleFontName;			// FNT_UI8

			ColorValue m_histoChartCenterLine;		// CV_UIChartGrid

			ColorValue m_pieChartLabelLineColor;	// CV_UIChartBorder
			String m_pieChartLabelFontName;			// FNT_UI8

			ColorValue m_axisLabelColor;			// CV_UIChartBorder
			String m_axisLabelFontName;				// FNT_UI7

			ColorValue m_fieldChartGeometryColor;			// CV_UIChartBorder
			ColorValue m_fieldChartAxisLineColor;			// CV_UIChartBorder
			ColorValue m_fieldChartVerticalAxisLineColor;	// CV_UIChartGrid

			ColorValue m_UIGreen;
			ColorValue m_UIRed;

			int32 m_loadingIconID;
		};

		enum struct ChartSeriesMode
		{
			Line,
			Candlestick,
			Dots
		};

		struct ChartDataSourceDesc
		{
			IChartDataSource* m_data = nullptr;
			ColorValue m_color = CV_White;
			ChartSeriesMode m_mode;
			String m_name;

			ChartDataSourceDesc() { }
			ChartDataSourceDesc(IChartDataSource* series, ColorValue seriresColor, ChartSeriesMode mode, const String& name = L"")
				: m_data(series), m_color(seriresColor), m_mode(mode), m_name(name)
			{ }
		};

		struct ChartCustomLegend
		{
			String m_text;
			ColorValue m_color;
		};

		struct ChartFormatDesc
		{
			IChartAxisFomatter* m_axisX = nullptr;
			IChartAxisFomatter* m_axisY = nullptr;
			Font* m_titleFont = nullptr;
			LayoutLocation m_legendLocation;

			//ChartFormatDesc() { }
			ChartFormatDesc(IChartAxisFomatter* axisX, IChartAxisFomatter* axisY,
				const String& titleFontName /* FNT_UI8 */, LayoutLocation legendLocation = LayoutLocation::Left);
		};

		struct ChartAxisAlignedLine
		{
			double m_position = 0;
			LayoutDirection m_dir = LayoutDirection::Horizontal;
			ColorValue m_color = CV_White;

			ChartAxisAlignedLine() { }

			ChartAxisAlignedLine(double pos, ColorValue color, LayoutDirection dir = LayoutDirection::Horizontal)
				: m_dir(dir), m_position(pos), m_color(color) { }
		};

		struct ChartCustomLine
		{
			PointD m_start;
			PointD m_end;
			ColorValue m_color = CV_White;
		};

		struct ChartGraphicObject
		{
			UIGraphicSimple m_graphic;
			//Point m_textureSrcAnchor;
			PointD m_position;
			double m_rotation = 0;
		};

		struct ChartObjectsDesc
		{
			List<ChartCustomLegend> m_customLegends;
			List<ChartAxisAlignedLine> m_customAxisLines;
			List<ChartCustomLine> m_customLines;

			List<ChartGraphicObject> m_customGraphics;
		};

		struct ChartBackgroundTask
		{
			Chart* m_subject = nullptr;
			bool m_init = false;

			ChartBackgroundTask() { }
			ChartBackgroundTask(Chart* subject, bool init = false) : m_subject(subject), m_init(init) { }
		};

		class ChartBackgroundUpdater : public BackgroundWorker<ChartBackgroundTask>
		{
			SINGLETON_DECL(ChartBackgroundUpdater);

		public:
			ChartBackgroundUpdater();
			~ChartBackgroundUpdater();

		private:

			void BackgroundMainProcess(ChartBackgroundTask& item) override;
		};

		enum struct ChartAutoViewMode
		{
			Local,
			Global
		};

		class Chart : public Control
		{
			friend class ChartBackgroundUpdater;

			RTTI_DERIVED(Chart, Control);
		public:
			static const int32 MaxCandleWidth = 7;
			static const int32 MinCandleWidth = 1;

			Chart(const List<ChartDataSourceDesc>& sources, const ChartFormatDesc& format, const ChartStyle& style, Point size);
			Chart(const List<ChartDataSourceDesc>& sources, const ChartFormatDesc& format, const ChartStyle& style, const ChartObjectsDesc& objects, Point size);
			~Chart();

			void Draw(Sprite* sprite) override;
			void Update(const AppTime* time) override;
			void RefreshData(bool async);

			void RefreshObjects(const ChartObjectsDesc& objects);

			bool isReady() const { return m_isReady; }

			const String& getTitle() const { return m_title; }
			void setTitleFont(Font* font) { m_format.m_titleFont = font; }
			void setTitle(const String& title) { m_title = title; }
			void showDefaultLegend(bool v) { m_showDefaultLegend = v; }

			double getXStart() const { return m_xStart; }
			double getXEnd() const { return m_xStart + m_size.X / m_xZoom; }

			double getYStart() const { return m_yStart; }
			double getYEnd() const { return m_yStart + m_size.Y / m_yZoom; }

			void SetXView(double xStart, double xSpan);
			void SetYView(double yStart, double ySpan);

			void SetXViewMinMax(double xMin, double xMax);
			void SetYViewMinMax(double yMin, double yMax);

			void SetYViewAuto(ChartAutoViewMode mode, bool v);
			
			void SetScrollable(bool v) { m_scrollable = v; }

			void SetCachedRender(bool v) { m_cachedRender = v; }

			int32 TransformX(double x) const;
			int32 TransformY(double y) const;
			Point Transform(PointD p) const;
			Point Transform(double x, double y) const;

			double InvTransformX(int32 x) const;

			int32 GetTitleHeight() const;

			void DrawCandleStick(Sprite* sprite, double open, double high, double low, double close, float width, Point basePos);

			static int32 ComputeFitCandlestickWidth(int32 width, int32 barCount);
		private:
			struct DataSource final
			{
				ChartDataSourceDesc m_desc;
				List<ChartDataPoint> m_points;

				SpriteDrawEntries m_cachedDraw;

				double m_currentXStart = 0;
				double m_currentXEnd = 0;

				DataSource(ChartDataSourceDesc info);
				~DataSource();

				DataSource(DataSource&& o);
				DataSource& operator=(DataSource&& o);

				DataSource(const DataSource&) = delete;
				DataSource& operator=(const DataSource&) = delete;

				void RecacheData(double xStart, double xEnd);
				void ExpandCachedData(double xStartExpansion, double xEndExpansion);

			};

			void DrawDataSeries(Sprite* sprite, Rectangle bgArea);
			void DrawObjects(Sprite* sprite, Rectangle bgArea);

			Point Transform(const ChartDataPoint& pt) const;
			
			void BackgroundDataThread(bool init);
			void _RecacheData();
			void _CalculateAutoYRange();

			List<DataSource> m_data;
			ChartObjectsDesc m_objects;
			ChartFormatDesc m_format;
			ChartStyle m_style;

			double m_xLowbound = 0;
			double m_xHighbound = 0;

			bool m_isReady = false;
			bool m_scrollable = false;
			bool m_cachedRender = false;
			bool m_autoFitX = false;
			
			bool m_autoFitYLocal = true;
			bool m_autoFitYGlobal = false;

			bool m_showDefaultLegend = false;
			bool m_showCustomLegend = false;
			
			double m_xStart = 0;
			double m_xZoom = 1;
			
			double m_yStart = 0;
			double m_yZoom = 1;

			String m_title;
			Rectangle m_lastDataSeriesRect;
			AnimatedIconInstance m_loadingIcon;
		};

		//////////////////////////////////////////////////////////////////////////

		struct DistributionChartLine
		{
			LayoutDirection m_direction;
			String m_text;
			double m_position;
			ColorValue m_color;

			DistributionChartLine(const String& text, double pos, ColorValue color)
				: m_direction(LayoutDirection::Vertical), m_text(text), m_position(pos), m_color(color) {  }
		};

		class DistributionChartSerires : public Control
		{
			RTTI_DERIVED(DistributionChartSerires, Control);

		public:
			DistributionChartSerires(const ChartStyle& style, int32 chartSize, int32 digits, LayoutDirection layoutDirection);
			~DistributionChartSerires();

			void Draw(Sprite* sprite) override;
			void Update(const AppTime* time) override;

			void Clear();
			void AddHistogram(const Utility::DistributionHistogram* h, const String& title, const List<DistributionChartLine>& lines = {});

			void UnifyScales();

			int32 getViewCount() const { return m_views.getCount(); }

			void setGrandTitle(const String& title) { m_grandTitle = title; }
			void setChartSpacing(int32 v) { m_chartSpacing = Math::Max(0, v); }
			void showLegend(bool v) { m_showLegend = v; }

		private:
			struct SubView
			{
				SubView(const ChartStyle* style, const Utility::DistributionHistogram* histogramRef, int32 chartSize, int32 digits, const String& title, const List<DistributionChartLine>& lines);
				~SubView();

				SubView(SubView&&);
				SubView& operator=(SubView&&);

				SubView(const SubView&) = delete;
				SubView& operator=(const SubView&) = delete;

				void Draw(Sprite* sprite);

				void DrawVerticalLine(Sprite* sprite, double value, ColorValue cv) const;
				int32 CalculateXPosition(double value) const;

				Chart* m_chart;
				NumberRangeAxisFormatter* m_xAxis;
				const Utility::DistributionHistogram* m_histoRef;

				List<DistributionChartLine> m_lines;

				const ChartStyle* m_style;
			};

			void UpdateLayout();

			List<SubView> m_views;
			int32 m_chartSize;
			int32 m_chartSpacing = 25;
			int32 m_digits;
			LayoutDirection m_layoutDirection;
			bool m_showLegend = false;

			ChartStyle m_style;

			String m_grandTitle;
			List<ChartCustomLegend> m_legends;
		};

		//////////////////////////////////////////////////////////////////////////

		struct PiechartComponent
		{
			String m_name;
			double m_value;
			ColorValue m_color;
		};

		class Piechart : public Control
		{
			RTTI_DERIVED(Piechart, Control);

		public:
			Piechart(const StyleSkin* skin, const ChartStyle& style, const Point& pos, const Point& size, const List<PiechartComponent>& cmps);
			~Piechart();

			void Draw(Sprite* sprite) override;

			void SetComponents(const List<PiechartComponent>& cmps);
            void SetSize(Point sz);

			void SetLabelFont(Font* font);

			void ForEachComponentName(FunctorReference<void(String&)> func);

		private:
			struct ComponentData
			{
				double m_percent;

				float m_startAngle;
				float m_endAngle;
			};

			struct PiechartLabel
			{
				String m_name;
				String m_value;
				Point m_pieExitPos;
				int m_yPos;
			};

            void UpdateLabels();
			void Relax(List<PiechartLabel>& lbls, int labelHeight);
			void DrawLabels(Sprite* sprite, const List<PiechartLabel>& lbls, int32 xpos, const Rectangle& circleArea, bool lr);

			List<PiechartComponent> m_components;
			List<ComponentData> m_data;
			List<PiechartLabel> m_leftLabels;
			List<PiechartLabel> m_rightLabels;

			Font* m_labelFont;
			ChartStyle m_style;
		};

        //////////////////////////////////////////////////////////////////////////

        class FieldChart : public Control
        {
			RTTI_DERIVED(FieldChart, Control);

        public:
            FieldChart(const StyleSkin* skin, const ChartStyle& style, const Point& pos, const Point& size, const List2D<double>& data);
            ~FieldChart();

            void Draw(Sprite* sprite) override;
			void Update(const AppTime* time) override;

            void SetFieldData(const List2D<double>& data);
            void SetSize(Point sz);

        private:
            struct FieldVertex
            {
                Vector3 m_pos;
				Vector3 m_normal;
				Vector3 m_data;
            };

			struct AxisLineVertex
			{
				Vector3 m_pos;
				uint m_color;
			};

            void UpdateFieldData(RenderDevice* dev);
            void UpdateRenderTarget(RenderDevice* dev);

            bool m_visualDataDirty = false;
            bool m_sizeDirty = false;

            VertexBuffer* m_fieldVertices = nullptr;
            IndexBuffer* m_fieldIndices = nullptr;

			VertexBuffer* m_axisVertices = nullptr;

			VertexDeclaration* m_fieldVtxDecl = nullptr;
			VertexDeclaration* m_axisVtxDecl = nullptr;
			
            List2D<double> m_fieldData;

            int32 m_dataWidth = 0;
            int32 m_dataHeight = 0;
			float m_fieldRot = Math::Half_PI*0.5;

            RenderTarget* m_fieldImage = nullptr;
            DepthStencilBuffer* m_fieldImageDS = nullptr;

			ChartStyle m_style;
        };
	}
}