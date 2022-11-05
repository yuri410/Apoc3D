#pragma once

#include "UICommon.h"
#include "apoc3d/Core/BackgroundWorker.h"
#include "apoc3d/UILib/Control.h"

namespace Apoc3D
{
	namespace UI
	{
		enum struct PaneDock
		{
			Left,
			Right,
			Bottom
		};

		struct TabLoadingTask { Tab* m_subject; };

		class TabLoadingQueue : public BackgroundWorker<TabLoadingTask>
		{
		public:
			TabLoadingQueue();
			~TabLoadingQueue();

		private:
			void BackgroundMainProcess(TabLoadingTask& item) override;
		};

		struct LayoutManagerStyle
		{
			ColorValue m_clearColor;			// CV_UIClearColor
			ColorValue m_tabProgressLineColor;  // CV_UIBlue

			int32 m_loadingIconID;
		};

		class LayoutManager
		{
		public:
			LayoutManager(const StyleSkin* skin, const LayoutManagerStyle& style);
			~LayoutManager();

			void RegisterPane(Pane* p);
			void UnregisterPane(Pane* p);
			void RegisterTab(Tab* t);
			void UnregisterTab(Tab* t);

			Point GetPanePosition(PaneDock dockPos) const;
			Point GetPaneSize(PaneDock dockPos, int32 size) const;
			
			Point GetTabPosition() const;
			Point GetTabSize() const;

			void SetBounds(Point tl, Point br);
			
			Tab* GetSelectedTab() const;

			void ExportCurrentTab(RenderDevice* dev, const String& fp);

			void Update(const AppTime* time);
			void Draw(Sprite* sprite);
			void DrawOverlay(Sprite* sprite);

			const StyleSkin* getSkin() const { return m_skin; }
			TabLoadingQueue& getTabLoadingQueue() { return m_tabLoadingQueue; }

			bool IsInteractive = false;
		private:
			static const int32 BarWidth = 25;
			static const int32 TabPush = 3;

			// labels on the side
			struct SideLabel
			{
				String m_title;
				Pane* m_refPane = nullptr;
				Tab* m_refTab = nullptr;
				int32 m_length = 0;
				int32 m_measuredLength = 0;
				int32 m_position = 0;
				bool m_mouseHover = false;
				bool m_mouseDown = false;

				SideLabel(const String& title, Pane* ref) : m_title(title), m_refPane(ref) { }
				SideLabel(const String& title, Tab* ref) : m_title(title), m_refTab(ref) { }
			};

			void DrawLabels(Sprite* sprite, const List<SideLabel>& lbls, float angle, Point basePos, Point dir, bool isTab, bool& anyOpen);
			void UpdateLabelMeasurements(List<SideLabel>& lbls);

			const StyleSkin* m_skin;

			Point m_tl;
			Point m_br;

			List<Pane*> m_panes;
			List<Tab*> m_tabs;
			TabRenderer* m_tabRenderer = nullptr;

			List<SideLabel> m_tabLabels;
			List<SideLabel> m_leftLabels;
			List<SideLabel> m_rightLabels;
			List<SideLabel> m_bottomLabels;

			int32 m_selectedTab = 0;

			Rectangle m_tabBar;
			Rectangle m_rightBar;

			LayoutManagerStyle m_style;
			AnimatedIconInstance m_loadingIcon;
			TabLoadingQueue m_tabLoadingQueue;
		};

		class Pane
		{
		public:
			Pane(LayoutManager* mgr, const String& title, PaneDock dock, int32 size);
			virtual ~Pane();

			virtual void Update(const Core::AppTime* time);
			virtual void Draw(Sprite* sprite);
			
			virtual LoadingStatus GetAggregatedLoadingStatus() const;

			void Open();
			void Close();

			void AddControl(Control* ctrl);
			void RemoveControl(Control* ctrl);

			PaneDock getDock() const { return m_dock; }
			const String& getTitle() const { return m_title; }
			bool isOpen() const { return m_opened; }

			bool IsInteractive = false;
		protected:

			void UpdatePaneStatus(const Core::AppTime* time);

			LayoutManager* m_manager;
			String m_title;
			PaneDock m_dock;

			Point m_position;
			Point m_contentOffset;
			Point m_contentAreaSize;

			int32 m_size;
			float m_openProgress = 0;
			float m_activeTimer = 0;
			bool m_opened = false;

		private:
			ControlCollection m_controls;
		};

		class Tab
		{
			friend class TabLoadingQueue;
		public:
			Tab(LayoutManager* mgr, const String& title);
			virtual ~Tab();

			void Load();
			void DeferredReload();

			virtual void Update(const Core::AppTime* time);
			virtual void Draw(Sprite* sprite);

			void AddControlAtBegining(Control* ctrl);
			void AddControl(Control* ctrl);
			void RemoveControl(Control* ctrl);
			void ReleaseAllControls();

			LoadingStatus GetLoadingStatus() const;

			Point CalculateMaxContentSize() const;
			void SetScroll(int32 yPos);

			const String& getTitle() const { return m_title; }
			const LayoutManager* getLayoutManager() const { return m_manager; }

			bool IsInteractive = false;
			bool HideScroll = false;
		protected:
			virtual void LoadImpl();
			virtual bool ShouldLoadAsync() { return false; }

			void SetLoadingProgress(float p);
			void SetLoadingFinished();

			const ControlCollection& getControls() const { return m_controls; }

			LayoutManager* m_manager;
			String m_title;
			Point m_baseOffset;
			Point m_contentOffset;
			ScrollBar* m_vscroll;

			int32 m_maxContentHeightHint = 0;

		private:
			ControlCollection m_controls;

			bool m_pendingReload = false;
			std::atomic<LoadingState> m_loadingState = LoadingState::Unloaded;
			std::atomic<float> m_loadingProgress = 0;
		};


		class WorkerPaneBase : public Pane
		{
		public:
			WorkerPaneBase(LayoutManager* mgr, const String& title);
			virtual ~WorkerPaneBase();

			void Update(const AppTime* time) override;

			LoadingStatus GetAggregatedLoadingStatus() const override;

		protected:

			List<UI::WorkerPanel*> m_fetchPanels;
		};


	}
}