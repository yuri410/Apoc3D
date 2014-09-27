/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#include "Control.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Menu.h"
#include "Button.h"
#include "Text.h"
#include "PictureBox.h"
#include "CheckBox.h"
#include "List.h"
#include "Bar.h"

#include "apoc3d/Core/GameTime.h"

#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"

namespace Apoc3D
{
	namespace UI
	{
		void MultiClickChecker::Update(const GameTime* time, Input::Mouse* mouse)
		{
			m_timeSinceLastClick += time->getElapsedTime();
			
			if (m_clickCount > 0 && m_timeSinceLastClick >= SystemUI::DoubleClickInterval)
				m_clickCount = 0;

			if (m_clickCount > 0)
			{
				m_distance.Track(mouse);
				if (m_distance.getDistance() > 7)
					m_clickCount = 0;
			}
		}

		bool MultiClickChecker::Check(Mouse* mouse, int32 clickCount)
		{
			if (mouse == nullptr)
				mouse = InputAPIManager::getSingleton().getMouse();

			if (mouse->IsLeftPressed())
			{
				if (m_clickCount == clickCount - 1)
					return true;

				if (m_clickCount == 0)
					m_distance.Reset(mouse);

				m_timeSinceLastClick = 0;
				m_clickCount++;
			}
			return false;
		}
		/************************************************************************/
		/*  Control                                                             */
		/************************************************************************/

		Control::Control()
			: Control(nullptr) { }

		Control::Control(const Point& position)
			: Control(nullptr, position) { }

		Control::Control(const Point& position, int32 width)
			: Control(nullptr, position, width) { }

		Control::Control(const Point& position, const Point& size)
			: Control(nullptr, position, size) { }


		Control::Control(const StyleSkin* skin)
			: Control(skin, Point::Zero, Point(0,0)) { }

		Control::Control(const StyleSkin* skin, const Point& position)
			: Control(skin, position, Point(0,0)) { }

		Control::Control(const StyleSkin* skin, const Point& position, int32 width)
			: Control(skin, position, Point(width, 0)) { }

		Control::Control(const StyleSkin* skin, const Point& position, const Point& size)
			: Position(position), m_size(size)
		{
			Initialze(skin);
		}


		Control::~Control()
		{

		}

		void Control::Initialze(const StyleSkin* skin)
		{
			if (skin)
				m_fontRef = skin->ContentTextFont;
		}

		void Control::SetFont(Font* fontRef)
		{
			m_fontRef = fontRef;
		}

		
		template <typename T>
		void Control::UpdateEvents_StandardButton(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
			void (T::*onMouseHover)(), void (T::*onMouseOut)(), void (T::*onPress)(), void (T::*onRelease)())
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point cursorPos = mouse->GetPosition();

			T* obj = static_cast<T*>(this);

			if (Visible && Enabled && IsInteractive && area.Contains(cursorPos))
			{
				if (!mouseHover)
				{
					mouseHover = true;
					(obj->*onMouseHover)();
				}
				if (!mouseDown && mouse && mouse->IsLeftPressed())
				{
					mouseDown = true;
					(obj->*onPress)();
				}
				else if (mouseDown && mouse && mouse->IsLeftUp())
				{
					mouseDown = false;
					(obj->*onRelease)();
				}
			}
			else if (mouseHover)
			{
				mouseHover = false;
				mouseDown = false;
				(obj->*onMouseOut)();
			}
		}

		template void Control::UpdateEvents_StandardButton<Button>(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area, 
			void (Button::*onMouseHover)(), void (Button::*onMouseOut)(), void (Button::*onPress)(), void (Button::*onRelease)());

		template void Control::UpdateEvents_StandardButton<Label>(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
			void (Label::*onMouseHover)(), void (Label::*onMouseOut)(), void (Label::*onPress)(), void (Label::*onRelease)());
		
		template void Control::UpdateEvents_StandardButton<PictureBox>(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
			void (PictureBox::*onMouseHover)(), void (PictureBox::*onMouseOut)(), void (PictureBox::*onPress)(), void (PictureBox::*onRelease)());

		template void Control::UpdateEvents_StandardButton<CheckBox>(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
			void (CheckBox::*onMouseHover)(), void (CheckBox::*onMouseOut)(), void (CheckBox::*onPress)(), void (CheckBox::*onRelease)());

		template void Control::UpdateEvents_StandardButton<TreeView>(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
			void (TreeView::*onMouseHover)(), void (TreeView::*onMouseOut)(), void (TreeView::*onPress)(), void (TreeView::*onRelease)());

		void Control::SetControlBasicStates(std::initializer_list<Control*> ptrs, Point baseOffset, bool parentFocused)
		{
			for (Control* c : ptrs)
			{
				if (c == nullptr)
					continue;

				c->BaseOffset = baseOffset;
				c->IsInteractive = parentFocused;
			}
		}
		void Control::SetControlBasicStates(std::initializer_list<Control*> ptrs, Point baseOffset, Point pos, bool parentFocused)
		{
			for (Control* c : ptrs)
			{
				if (c == nullptr)
					continue;

				c->Position = pos;
				c->BaseOffset = baseOffset;
				c->IsInteractive = parentFocused;
			}
		}

		void Control::SetControlBasicStates(std::initializer_list<Control*> ptrs)
		{
			SetControlBasicStates(ptrs, BaseOffset, IsInteractive);
		}
		void Control::SetControlBasicStates(std::initializer_list<Control*> ptrs, Point pos)
		{
			SetControlBasicStates(ptrs, BaseOffset, pos, IsInteractive);
		}

		/************************************************************************/
		/* ScrollableControl                                                    */
		/************************************************************************/

		ScrollableControl::ScrollableControl()
			: Control() { }

		ScrollableControl::ScrollableControl(const Point& position)
			: Control(position) { }

		ScrollableControl::ScrollableControl(const Point& position, const Point& size)
			: Control(position, size) { }

		ScrollableControl::ScrollableControl( const Point& position, int32 width)
			: Control(position, width) { }


		ScrollableControl::ScrollableControl(const StyleSkin* skin)
			: Control(skin) { }

		ScrollableControl::ScrollableControl(const StyleSkin* skin, const Point& position)
			: Control(skin, position) { }

		ScrollableControl::ScrollableControl(const StyleSkin* skin, const Point& position, const Point& size)
			: Control(skin, position, size) { }

		ScrollableControl::ScrollableControl(const StyleSkin* skin, const Point& position, int32 width)
			: Control(skin, position, width) { }


		ScrollableControl::~ScrollableControl()
		{
			DELETE_AND_NULL(m_vscrollbar);
			DELETE_AND_NULL(m_hscrollbar);
		}

		void ScrollableControl::InitScrollbars(const StyleSkin* skin)
		{
			DELETE_AND_NULL(m_vscrollbar);
			DELETE_AND_NULL(m_hscrollbar);

			GUIUtils::ScrollBarPositioning hs = skin->HScrollBarBG.Y;
			GUIUtils::ScrollBarPositioning vs = skin->VScrollBarBG.X;

			GUIUtils::CalculateScrollBarPositions(getArea(), &vs, &hs);
			m_hscrollbar = new ScrollBar(skin, hs.Position, BarDirection::Horizontal, hs.Length);
			m_vscrollbar = new ScrollBar(skin, vs.Position, BarDirection::Vertical, vs.Length);
		}

		void ScrollableControl::InitScrollbars(const ScrollBarVisualSettings& hss, const ScrollBarVisualSettings& vss)
		{
			DELETE_AND_NULL(m_vscrollbar);
			DELETE_AND_NULL(m_hscrollbar);

			GUIUtils::ScrollBarPositioning hs = hss.BackgroundGraphic.getHeight();
			GUIUtils::ScrollBarPositioning vs = vss.BackgroundGraphic.getWidth();

			GUIUtils::CalculateScrollBarPositions(getArea(), &vs, &hs);
			m_hscrollbar = new ScrollBar(hss, hs.Position, BarDirection::Horizontal, hs.Length);
			m_vscrollbar = new ScrollBar(vss, vs.Position, BarDirection::Vertical, vs.Length);
		}

		void ScrollableControl::UpdateScrollBarsLength(const Apoc3D::Math::Rectangle& area)
		{
			bool hasHSB = EnableHScrollBar && (m_alwaysShowHS || m_hscrollbar->Maximum > 0);
			bool hasVSB = EnableVScrollBar && (m_alwaysShowVS || m_vscrollbar->Maximum > 0);
			
			m_hscrollbar->Visible = hasHSB;
			m_vscrollbar->Visible = hasVSB;

			GUIUtils::ScrollBarPositioning hs = hasHSB ? m_hscrollbar->getHeight() : 0;
			GUIUtils::ScrollBarPositioning vs = hasVSB ? m_vscrollbar->getWidth() : 0;

			GUIUtils::CalculateScrollBarPositions(area, hasVSB ? &vs : nullptr, hasHSB ? &hs : nullptr);

			m_vscrollbar->Position = vs.Position;
			m_hscrollbar->Position = hs.Position;

			if (m_vscrollbar->GetLength() != vs.Length)
				m_vscrollbar->SetLength(vs.Length);

			if (m_hscrollbar->GetLength() != hs.Length)
				m_hscrollbar->SetLength(hs.Length);
		}
		void ScrollableControl::UpdateScrollBarsGeneric(const Apoc3D::Math::Rectangle& area, const GameTime* time)
		{
			UpdateScrollBarsLength(area);

			SetControlBasicStates({ m_hscrollbar, m_vscrollbar });

			if (m_vscrollbar->Visible)
			{
				m_vscrollbar->Update(time);
			}
			if (m_hscrollbar->Visible)
			{
				m_hscrollbar->Update(time);
			}
		}

		void ScrollableControl::DrawScrollBars(Sprite* sprite)
		{
			if (m_hscrollbar && m_hscrollbar->Visible)
				m_hscrollbar->Draw(sprite);

			if (m_vscrollbar && m_vscrollbar->Visible)
				m_vscrollbar->Draw(sprite);
		}

		Apoc3D::Math::Rectangle ScrollableControl::GetContentArea() const
		{
			Apoc3D::Math::Rectangle rect = getAbsoluteArea();
			
			if (m_vscrollbar && (m_alwaysShowVS || (m_vscrollbar->Visible && m_vscrollbar->Maximum > 0)))
			{
				rect.Width -= m_vscrollbar->getWidth();
			}

			if (m_hscrollbar && (m_alwaysShowHS || (m_hscrollbar->Visible && m_hscrollbar->Maximum > 0)))
			{
				rect.Height -= m_hscrollbar->getHeight();
			}

			return rect;
		}


		/************************************************************************/
		/* ControlCollection                                                    */
		/************************************************************************/

		void ControlCollection::SetElementsBaseOffset(Point bo)
		{
			for (int32 i = 0; i < m_count; i++)
			{
				Control* ctrl = m_elements[i];
				ctrl->BaseOffset = bo;
			}
		}
		void ControlCollection::SetElementsInteractive(bool isInteractive)
		{
			for (int32 i = 0; i < m_count; i++)
			{
				Control* ctrl = m_elements[i];
				ctrl->IsInteractive = isInteractive;
			}
		}
		void ControlCollection::SetElementsBasicStates(Point baseOffset, bool isInteractive)
		{
			for (int32 i = 0; i < m_count; i++)
			{
				Control* ctrl = m_elements[i];
				ctrl->BaseOffset = baseOffset;
				ctrl->IsInteractive = isInteractive;
			}
		}

		void ControlCollection::Update(const GameTime* time)
		{
			Control* overridingControl = nullptr;
			for (int32 i = 0; i < m_count; i++)
			{
				Control* ctrl = m_elements[i];
				if (ctrl->IsOverriding())
				{
					ctrl->Update(time);
					overridingControl = ctrl;
				}
			}

			if (overridingControl)
			{
				SetElementsInteractive(false);
			}

			for (int32 i = 0; i < m_count; i++)
			{
				Control* ctrl = m_elements[i];

				if (ctrl != overridingControl && ctrl->Enabled)
				{
					ctrl->Update(time);
				}
			}
		}
		void ControlCollection::Draw(Sprite* sprite)
		{
			Control* overridingControl = nullptr;

			for (int32 i = 0; i < m_count;i++)
			{
				Control* ctrl = m_elements[i];
				if (ctrl->IsOverriding())
				{
					overridingControl = ctrl;
				}
				if (ctrl->Enabled)
				{
					ctrl->Draw(sprite);
				}
			}

			if (overridingControl)
			{
				overridingControl->DrawOverlay(sprite);
			}
		}

		/************************************************************************/
		/* ControlContainer                                                     */
		/************************************************************************/

		ControlContainer::ControlContainer(const StyleSkin* skin)
			: Control(skin) { }

		ControlContainer::~ControlContainer()
		{
			if (ReleaseControls)
				m_controls.DeleteAndClear();
		}

		void ControlContainer::Draw(Sprite* sprite)
		{
			m_controls.Draw(sprite);
			
			if (MenuBar && MenuBar->Visible)
			{
				MenuBar->Draw(sprite);
			}
		}

		void ControlContainer::Update(const GameTime* time)
		{
			m_controls.SetElementsBaseOffset(GetAbsolutePosition());
			
			m_controls.Update(time);

			if (MenuBar && MenuBar->Visible)
			{
				MenuBar->Update(time);
			}
		}

	}
}