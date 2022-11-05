#include "Layout.h"
#include "WorkerPanel.h"
#include "Presentation.h"
#include "TabRenderer.h"
#include "Chart.h"

#include "apoc3d/Core/AppTime.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"

#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/Bar.h"
#include "apoc3d/UILib/FontManager.h"

#include "apoc3d/Math/Math.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
#pragma region TabLoadingQueue

		TabLoadingQueue::TabLoadingQueue()
		{
			StartBackground(L"Tab loader", 1);
		}

		TabLoadingQueue::~TabLoadingQueue()
		{

		}

		void TabLoadingQueue::BackgroundMainProcess(TabLoadingTask& item)
		{
			item.m_subject->LoadImpl();
		}

#pragma endregion

#pragma region LayoutManager

		//////////////////////////////////////////////////////////////////////////

		const ControlBounds paneTitlePadding = { 10,5,7,7 };
		const int32 paneTitleHeight = 25;

		const int32 tabSpacing = 10;

		LayoutManager::LayoutManager(const StyleSkin* skin, const LayoutManagerStyle& style)
			: m_skin(skin)
			, m_style(style)
		{
			m_loadingIcon = PresentationManager::getSingleton().GetAnimatedIcon(style.m_loadingIconID)->CreateInstance();
		}
		
		LayoutManager::~LayoutManager()
		{
			m_tabLoadingQueue.WaitUntilClear();

			m_panes.DeleteAndClear();
			m_tabs.DeleteAndClear();
		}

		void LayoutManager::RegisterPane(Pane* p)
		{
			m_panes.Add(p);
		}
		void LayoutManager::UnregisterPane(Pane* p)
		{
			m_panes.Remove(p);
		}

		void LayoutManager::RegisterTab(Tab* t)
		{
			m_tabs.Add(t);

			if (t->GetLoadingStatus().m_state == LoadingState::Unloaded)
			{
				t->Load();
			}
		}
		void LayoutManager::UnregisterTab(Tab* t)
		{
			m_tabs.Remove(t);
		}

		void LayoutManager::SetBounds(Point tl, Point br)
		{
			m_tl = tl;
			m_br = br;
		}

		Point LayoutManager::GetPanePosition(PaneDock dockPos) const
		{
			switch (dockPos)
			{
				case PaneDock::Left: return Point(m_tl.X + BarWidth, m_tl.Y);
				case PaneDock::Right: return Point(m_br.X - BarWidth, m_tl.Y);
				case PaneDock::Bottom: return Point(m_tl.X, m_br.Y - BarWidth);
			}
			return m_tl;
		}

		Point LayoutManager::GetPaneSize(PaneDock dockPos, int32 size) const
		{
			if (dockPos == PaneDock::Bottom)
			{
				Point tl = m_tl;
				Point br = m_br;

				if (m_leftLabels.getCount() > 0)
					tl = GetPanePosition(PaneDock::Left);

				if (m_rightLabels.getCount() > 0)
					br = GetPanePosition(PaneDock::Right);

				return Point(m_br.X - m_tl.X, size);
			}
			
			return Point(size, m_br.Y - m_tl.Y);
		}

		Point LayoutManager::GetTabPosition() const
		{
			if (m_leftLabels.getCount() > 0)
			{
				return GetPanePosition(PaneDock::Left) + Point(0, BarWidth) + m_tl;
			}
			return Point(0, BarWidth) + m_tl;
		}

		Point LayoutManager::GetTabSize() const
		{
			Point tl = m_tl;
			Point br = m_br;
			
			if (m_leftLabels.getCount() > 0)
			{
				tl += Point(BarWidth, 0);
			}
			if (m_rightLabels.getCount() > 0)
			{
				br -= Point(BarWidth, 0);
			}
			if (m_bottomLabels.getCount() > 0)
			{
				br -= Point(0, BarWidth);
			}
			if (m_tabLabels.getCount() > 0)
			{
				tl += Point(0, BarWidth);
			}

			return br - tl;
		}

		Tab* LayoutManager::GetSelectedTab() const
		{
			return m_tabs.isIndexInRange(m_selectedTab) ? m_tabs[m_selectedTab] : nullptr;
		}

		void LayoutManager::ExportCurrentTab(RenderDevice* dev, const String& fp)
		{
			if (m_tabRenderer)
				return;

			Tab* curTab = GetSelectedTab();
			if (curTab)
			{
				m_tabRenderer = new TabRenderer(dev, curTab, m_style.m_clearColor, fp);
			}
		}

		void LayoutManager::Update(const AppTime* time)
		{
			for (Pane* p : m_panes)
			{
				p->IsInteractive = IsInteractive;
				p->Update(time);
			}

			int32 idx = 0;
			for (Tab* t : m_tabs)
			{
				LoadingState status = t->GetLoadingStatus().m_state;
				if (status == LoadingState::Loaded)
				{
					t->IsInteractive = idx == m_selectedTab;
					t->Update(time);
				}
				else if (status == LoadingState::Loading && idx == m_selectedTab)
				{
					m_tabLoadingQueue.MoveToFront([&t](const TabLoadingTask& tsk)
					{
						return tsk.m_subject == t;
					});
				}
				idx++;
			}

			const int32 titlePadding = 20;

			m_leftLabels.Clear();
			m_rightLabels.Clear();
			m_bottomLabels.Clear();
			m_tabLabels.Clear();

			for (Pane* p : m_panes)
			{
				SideLabel lbl = { p->getTitle(), p };

				lbl.m_measuredLength = m_skin->ButtonFont->MeasureString(lbl.m_title).X;
				lbl.m_length = lbl.m_measuredLength + titlePadding;

				PaneDock dock = p->getDock();
				if (dock == PaneDock::Left)
					m_leftLabels.Add(lbl);
				else if (dock == PaneDock::Right)
					m_rightLabels.Add(lbl);
				else if (dock == PaneDock::Bottom)
					m_bottomLabels.Add(lbl);
			}

			for (Tab* t : m_tabs)
			{
				SideLabel lbl = { t->getTitle(), t };

				lbl.m_measuredLength = m_skin->ButtonFont->MeasureString(lbl.m_title).X;
				lbl.m_length = lbl.m_measuredLength + titlePadding;

				m_tabLabels.Add(lbl);
			}

			m_rightBar = { GetPanePosition(PaneDock::Right), { BarWidth, m_br.Y - m_tl.Y } };
			m_tabBar = { GetTabPosition() - Point(0, BarWidth), { GetTabSize().X, BarWidth } };

			if (m_rightLabels.getCount() > 0)
			{
				UpdateLabelMeasurements(m_rightLabels);

				if (IsInteractive)
				{
					Pane* paneToOpen = nullptr;

					Mouse* mouse = InputAPIManager::getSingleton().getMouse();

					for (SideLabel& lbl : m_rightLabels)
					{
						Rectangle area = { m_rightBar.getPosition() + Point(0, lbl.m_position), Point(BarWidth, lbl.m_length) };
						if (area.Contains(mouse->GetPosition()))
						{
							lbl.m_mouseHover = true;
							lbl.m_mouseDown = mouse->IsLeftPressedState();

							paneToOpen = lbl.m_refPane;
						}
					}

					if (paneToOpen)
					{
						for (SideLabel& lbl : m_rightLabels)
						{
							if (lbl.m_refPane != paneToOpen)
							{
								lbl.m_refPane->Close();
							}
							else
							{
								lbl.m_refPane->Open();
							}
						}
					}
				}
			}

			if (m_tabLabels.getCount() > 0)
			{
				UpdateLabelMeasurements(m_tabLabels);

				if (IsInteractive)
				{
					Mouse* mouse = InputAPIManager::getSingleton().getMouse();

					for (SideLabel& lbl : m_tabLabels)
					{
						Rectangle area = { m_tabBar.getPosition() + Point(lbl.m_position, 0), Point(lbl.m_length, BarWidth) };
						if (area.Contains(mouse->GetPosition()))
						{
							lbl.m_mouseHover = true;
							lbl.m_mouseDown = mouse->IsLeftPressedState();

							if (mouse->IsLeftPressed())
							{
								m_selectedTab = m_tabs.IndexOf(lbl.m_refTab);
							}
						}
					}
				}
			}
		}

		static void DrawStringWithShadow(Sprite* spr, const String& txt, const Point& pos, const StyleSkin* skin)
		{
			skin->ButtonFont->DrawString(spr, txt, pos + Point(1, 0), CV_Black);
			skin->ButtonFont->DrawString(spr, txt, pos, skin->TextColor);
		}

		void LayoutManager::Draw(Sprite* sprite)
		{
			if (m_tabs.isIndexInRange(m_selectedTab))
			{
				Tab* tab = m_tabs[m_selectedTab];
				LoadingStatus status = tab->GetLoadingStatus();
				
				if (status.m_state == LoadingState::Loaded)
					tab->Draw(sprite);
				else
					PresentationManager::getSingleton().GetAnimatedIcon(m_style.m_loadingIconID)->Draw(sprite, m_loadingIcon, Rectangle(GetTabPosition(), GetTabSize()).getCenter(), 64, status.m_progress);
			}
			
			if (m_tabRenderer)
			{
				if (!m_tabRenderer->isFinished())
				{
					m_tabRenderer->DoStep(sprite);
				}
				else
				{
					DELETE_AND_NULL(m_tabRenderer);
				}
			}

			if (m_tabLabels.getCount() > 0)
			{
				sprite->Flush();
				sprite->getRenderDevice()->getRenderState()->setScissorTest(true, &m_tabBar);
				sprite->Draw(SystemUI::GetWhitePixel(), m_tabBar, nullptr, 0xff404040);

				bool isAnyOpen;
				DrawLabels(sprite, m_tabLabels, 0, m_tabBar.getPosition(), Point(1, 0), true, isAnyOpen);

				sprite->SetTransform(Matrix::Identity);
				sprite->Flush();
				sprite->getRenderDevice()->getRenderState()->setScissorTest(false, nullptr);

				sprite->DrawLine(SystemUI::GetWhitePixel(), m_tabBar.getBottomLeft(), m_tabBar.getBottomRight(),
					m_skin->BorderColor, 1, LineCapOptions::Butt);
			}

			for (Pane* p : m_panes)
			{
				p->Draw(sprite);
			}

			if (m_rightLabels.getCount() > 0)
			{
				sprite->Flush();
				sprite->getRenderDevice()->getRenderState()->setScissorTest(true, &m_rightBar);
				sprite->Draw(SystemUI::GetWhitePixel(), m_rightBar, nullptr, 0xff404040);

				bool isAnyOpen;
				DrawLabels(sprite, m_rightLabels, 90, m_rightBar.getTopRight(), Point(0,1), false, isAnyOpen);

				sprite->SetTransform(Matrix::Identity);
				sprite->Flush();
				sprite->getRenderDevice()->getRenderState()->setScissorTest(false, nullptr);

				sprite->DrawLine(SystemUI::GetWhitePixel(), m_rightBar.getTopLeft(), m_rightBar.getBottomLeft(), 
					!isAnyOpen ? m_skin->BorderColor : m_skin->ControlDarkShadeColor, 1, LineCapOptions::Butt);
			}
		}

		void LayoutManager::DrawLabels(Sprite* sprite, const List<SideLabel>& lbls, float angle, Point basePos, Point dir, bool isTab, bool& anyOpen)
		{
			anyOpen = false;

			Matrix t;
			Matrix::CreateRotationZ(t, ToRadian(angle));

			for (const SideLabel& lbl : lbls)
			{
				Point labelPos = basePos + dir * lbl.m_position;
				t.SetTranslation((float)labelPos.X, (float)labelPos.Y, 0);

				sprite->SetTransform(t);

				bool isOpen = false;
				if (lbl.m_refPane)
					isOpen |= lbl.m_refPane->isOpen();
				if (lbl.m_refTab && m_tabs.isIndexInRange(m_selectedTab))
					isOpen |= lbl.m_refTab == m_tabs[m_selectedTab];

				bool isMouseHoverOrOpen = lbl.m_mouseHover || isOpen;
				ColorValue color;
				if (isTab)
				{
					color = isMouseHoverOrOpen ? 0xff646464 : 0xff525252;
					if (lbl.m_mouseDown || isOpen)
						color = 0xff757575;
				}
				else
				{
					color = isMouseHoverOrOpen ? 0xff757575 : 0xff525252;
				}
				Rectangle localArea = { { 0, TabPush }, { lbl.m_length, BarWidth } };
				sprite->DrawRoundedRect(SystemUI::GetWhitePixel(), localArea, nullptr, 5, 2, color);

				Rectangle localAreaBorder = localArea;
				localAreaBorder.Inflate(1, 1);
				sprite->DrawRoundedRectBorder(SystemUI::GetWhitePixel(), localAreaBorder, nullptr, 1, 5, 2, m_skin->ControlDarkShadeColor);

				// draw progress bar
				LoadingStatus loadingStatus;
				if (lbl.m_refTab)
				{
					loadingStatus = lbl.m_refTab->GetLoadingStatus();
				}
				else if (lbl.m_refPane)
				{
					loadingStatus = lbl.m_refPane->GetAggregatedLoadingStatus();
				}

				if (loadingStatus.m_state != LoadingState::Loaded)
				{
					const int32 MinBarWidth = TabPush;

					Rectangle barArea = localArea;
					barArea.Y = BarWidth - TabPush;
					barArea.Width = MinBarWidth + Math::Round((localArea.Width - MinBarWidth) * loadingStatus.m_progress);
					sprite->Draw(SystemUI::GetWhitePixel(), barArea, m_style.m_tabProgressLineColor);
				}

				DrawStringWithShadow(sprite, lbl.m_title, Point((lbl.m_length - lbl.m_measuredLength) / 2, TabPush), m_skin);

				anyOpen |= isOpen;
			}
		}
		
		void LayoutManager::UpdateLabelMeasurements(List<SideLabel>& lbls)
		{
			int32 pos = tabSpacing;
			for (SideLabel& lbl : lbls)
			{
				lbl.m_position = pos;
				pos += lbl.m_length + tabSpacing;
				lbl.m_mouseHover = false;
				lbl.m_mouseDown = false;
			}
		}

		void LayoutManager::DrawOverlay(Sprite* sprite)
		{
			if (m_tabRenderer)
			{
				Point pbPos = Point(m_br.X - 100 - 5, 5);
				ProgressBar pb(m_skin, pbPos, 100);
				pb.CurrentValue = m_tabRenderer->GetProgress();
				pb.Draw(sprite);
			}
		}
#pragma endregion

#pragma region Pane

		//////////////////////////////////////////////////////////////////////////

		Pane::Pane(LayoutManager* mgr, const String& title, PaneDock dock, int32 size)
			: m_manager(mgr), m_title(title), m_dock(dock), m_size(size)
		{
			UpdatePaneStatus(nullptr);
		}
		Pane::~Pane()
		{
			m_controls.DeleteAndClear();
		}

		void Pane::Update(const Core::AppTime* time)
		{
			UpdatePaneStatus(time);

			m_controls.SetElementsBasicStates(m_contentOffset, m_opened && IsInteractive);
			m_controls.Update(time);
		}

		void Pane::Draw(Sprite* sprite)
		{
			const StyleSkin* skin = m_manager->getSkin();

			Rectangle area = { m_position, m_manager->GetPaneSize(m_dock, m_size) + Point(25,0) };
			sprite->DrawRoundedRect(SystemUI::GetWhitePixel(), area, nullptr, 5, 2, 0xff404040);

			Rectangle areaBorder = area;
			areaBorder.Inflate(1, 1);
			sprite->DrawRoundedRectBorder(SystemUI::GetWhitePixel(), areaBorder, nullptr, 1, 5, 2, skin->BorderColor);

			Point titleOffset;
			titleOffset.X = paneTitlePadding.Left;
			titleOffset.Y = (paneTitleHeight - skin->TitleTextFont->getLineHeightInt()) / 2 - 1;

			skin->TitleTextFont->DrawString(sprite, m_title, m_position + titleOffset, skin->TextColor);

			m_controls.Draw(sprite);
		}

		void Pane::UpdatePaneStatus(const Core::AppTime* time)
		{
			const float OpenSpeed = 5;
			if (time)
			{
				m_openProgress = Math::Saturate(m_openProgress + time->ElapsedTime * (m_opened ? 1 : -1) * OpenSpeed);
				//m_opened = m_openProgress >= 1;
			}

			Point basePos = m_manager->GetPanePosition(m_dock);

			PointF openDir = PointF(0, 0);
			switch (m_dock)
			{
				case PaneDock::Left: openDir = PointF(1, 0); break;
				case PaneDock::Right: openDir = PointF(-1, 0); break;
				case PaneDock::Bottom: openDir = PointF(0, -1); break;
			}

			const StyleSkin* skin = m_manager->getSkin();

			m_position = Math::Round((PointF)basePos + openDir * (m_openProgress*m_size));
			m_contentOffset = m_position + Point(5, paneTitleHeight);
			m_contentAreaSize = m_manager->GetPaneSize(m_dock, m_size) - Point(5, paneTitleHeight) - Point(5, 5);

			if (m_openProgress >= 1)
			{
				Mouse* mouse = InputAPIManager::getSingleton().getMouse();

				if (mouse)
				{
					Rectangle area = { m_position, m_manager->GetPaneSize(m_dock, m_size) };

					if (!area.Contains(mouse->GetPosition()))
					{
						m_activeTimer += time->ElapsedTime;
						if (m_activeTimer > 1)
						{
							Close();
						}
					}
					else
					{
						m_activeTimer = 0;
					}
				}
			}
		}

		void Pane::Open() 
		{
			m_opened = true;
			m_activeTimer = 0;
		}

		void Pane::Close()
		{
			m_activeTimer = 0;
			m_opened = false;
		}

		void Pane::AddControl(Control* ctrl)
		{
			m_controls.Add(ctrl); 
		}
		
		void Pane::RemoveControl(Control* ctrl)
		{
			m_controls.Remove(ctrl);
		}

		LoadingStatus Pane::GetAggregatedLoadingStatus() const
		{
			return{ LoadingState::Loaded, 0.0f };
		}

#pragma endregion

#pragma region Tab

		//////////////////////////////////////////////////////////////////////////

		Tab::Tab(LayoutManager* mgr, const String& title)
			: m_manager(mgr), m_title(title)
		{
			m_vscroll = new ScrollBar(mgr->getSkin(), Point(), BarDirection::Vertical, 100);
		}
		Tab::~Tab()
		{
			ReleaseAllControls();
		}

		void Tab::Load()
		{
			m_loadingProgress = 0;
			m_loadingState = LoadingState::Loading;
			
			if (ShouldLoadAsync())
			{
				m_manager->getTabLoadingQueue().AddWorkItem({ this });
			}
			else
			{
				LoadImpl();
			}
		}
		void Tab::DeferredReload()
		{
			m_pendingReload = true;
		}

		void Tab::Update(const Core::AppTime* time)
		{
			m_baseOffset = m_manager->GetTabPosition();

			Point contentSize = m_manager->GetTabSize();
			Point actualSize = contentSize;

			for (Control* p : m_controls)
			{
				Point br = p->getArea().getBottomRight();
				actualSize.X = Math::Max(br.X, actualSize.X);
				actualSize.Y = Math::Max(br.Y, actualSize.Y);
			}
			
			actualSize.Y = Math::Max(m_maxContentHeightHint, actualSize.Y);

			m_vscroll->Maximum = Math::Max(0, actualSize.Y - contentSize.Y);
			m_vscroll->VisibleRange = contentSize.Y;
			
			GUIUtils::ScrollBarPositioning vs = m_vscroll->getWidth();
			GUIUtils::CalculateScrollBarPositions({ m_baseOffset, contentSize }, &vs, nullptr);

			m_vscroll->Position = vs.Position;
			if (m_vscroll->GetLength() != vs.Length)
				m_vscroll->SetLength(vs.Length);


			m_contentOffset = m_baseOffset + Point(0, m_vscroll->Maximum > 0 ? -m_vscroll->getValue() : 0);

			m_controls.SetElementsBasicStates(m_contentOffset, IsInteractive);
			m_controls.Update(time);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			bool isInteractingControls = false;

			for (Control* p : m_controls)
			{
				if (p->IsOverriding())
				{
					isInteractingControls = true;
					break;
				}
				
				bool isScrollable = up_cast<Chart*>(p) != nullptr || up_cast<ScrollableControl*>(p) != nullptr;
				if (isScrollable && p->getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					isInteractingControls = true;
					break;
				}
			}

			if (IsInteractive && !isInteractingControls)
			{
				if (mouse->getDZ())
				{
					m_vscroll->ForceScroll(-mouse->getDZ() * 30 / 120);
				}
			}

			//m_vscroll->BaseOffset = m_baseOffset;
			m_vscroll->IsInteractive = IsInteractive && !HideScroll;
			m_vscroll->Update(time);

			if (m_pendingReload && (m_loadingState == LoadingState::Unloaded || m_loadingState == LoadingState::Loaded))
			{
				m_pendingReload = false;
				Load();
			}
		}
		void Tab::Draw(Sprite* sprite)
		{
			m_controls.Draw(sprite);

			if (!HideScroll)
				m_vscroll->Draw(sprite);
		}

		void Tab::AddControl(Control* ctrl)
		{
			m_controls.Add(ctrl);
		}

		void Tab::AddControlAtBegining(Control* ctrl)
		{
			m_controls.Insert(0, ctrl);
		}

		void Tab::RemoveControl(Control* ctrl)
		{
			m_controls.Remove(ctrl);
		}

		void Tab::ReleaseAllControls()
		{
			m_controls.DeleteAndClear();
		}

		LoadingStatus Tab::GetLoadingStatus() const
		{
			return { m_loadingState, m_loadingProgress };
		}

		Point Tab::CalculateMaxContentSize() const
		{
			Point actualSize(0, 0);

			for (Control* p : m_controls)
			{
				Point br = p->getArea().getBottomRight();
				actualSize.X = Math::Max(br.X, actualSize.X);
				actualSize.Y = Math::Max(br.Y, actualSize.Y);
			}

			actualSize.Y = Math::Max(m_maxContentHeightHint, actualSize.Y);
			return actualSize;
		}
		
		void Tab::SetScroll(int32 yPos)
		{
			m_vscroll->SetValue(yPos);
		}

		void Tab::LoadImpl()
		{
			SetLoadingFinished();
		}

		void Tab::SetLoadingProgress(float p)
		{
			m_loadingProgress = Math::Saturate(p);
		}
		void Tab::SetLoadingFinished()
		{
			m_loadingState = LoadingState::Loaded;
		}
#pragma endregion

#pragma region WorkerPaneBase

		//////////////////////////////////////////////////////////////////////////

		WorkerPaneBase::WorkerPaneBase(LayoutManager* mgr, const String& title)
			: Pane(mgr, title, PaneDock::Right, 250)
		{ }

		WorkerPaneBase::~WorkerPaneBase()
		{
			m_fetchPanels.Clear();
		}

		void WorkerPaneBase::Update(const AppTime* time)
		{
			UpdatePaneStatus(time);

			const int paneWidth = m_contentAreaSize.X;
			
			int sx = 0;
			int sy = 0;

			Rectangle area = { sx, sy, paneWidth, 0 };

			for (WorkerPanel* p : m_fetchPanels)
			{
				p->BaseOffset = m_contentOffset;
				p->IsInteractive = m_opened && IsInteractive;

				p->SetArea({ sx, sy, paneWidth, WorkerPanel::PanelHeight });
				p->Update(time);

				sy += WorkerPanel::PanelHeight + 5;
			}
		}

		LoadingStatus WorkerPaneBase::GetAggregatedLoadingStatus() const
		{
			LoadingStatus result;

			float totalProgress = 0;
			float totalCount = 0;

			for (WorkerPanel* p : m_fetchPanels)
			{
				IWorkerAdapter* worker = p->GetWorker();
				if (worker)
				{
					WorkerAdapterStatus status = worker->WA_GetStatus();
					if (status.m_isRunning)
					{
						totalProgress += status.m_totalCount > 0 ? Math::Saturate((float)status.m_currentCount / status.m_totalCount) : 0;
						totalCount++;
					}
				}
			}

			if (totalCount > 0)
			{
				result.m_state = LoadingState::Loading;
				result.m_progress = totalProgress / totalCount;
			}

			return result;
		}



#pragma endregion

	}
}