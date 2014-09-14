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
#include "Scrollbar.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Button.h"

#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
#pragma region FormVScrollBar
		ScrollBar::ScrollBar(const ScrollBarVisualSettings& settings, const Point& position, ScrollBarType type, int32 length)
			: Control(nullptr, position), m_type(type)
		{
			m_decrButton = new Button(settings.DecrButton, Point(0, 0), settings.DecrButton.NormalGraphic.getSize(), L"");
			m_incrButton = new Button(settings.IncrButton, Point(0, 0), m_decrButton->getSize(), L"");
			
			if (settings.HasHandleGraphic)
				HandleGraphic = settings.HandleGraphic;
			if (settings.HasBackgroundGraphic)
				BackgroundGraphic = settings.BackgroundGraphic;

			if (settings.HasDisabledHandleGraphic)
				DisabledHandleGraphic = settings.DisabledHandleGraphic;
			if (settings.HasDisabledBackgroundGraphic)
				DisabledBackgroundGraphic = settings.DisabledBackgroundGraphic;

			PostInit(length);
		}

		ScrollBar::ScrollBar(const StyleSkin* skin, const Point& position, ScrollBarType type, int32 length)
			: Control(skin, position), m_type(type)
		{
			if (type == SCRBAR_Vertical)
			{
				ButtonVisualSettings bvs;
				bvs.HasNormalGraphic = true;
				bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarUp);

				m_decrButton = new Button(bvs, Point(0, 0), skin->VScrollBarUp.getSize(), L"");
				//m_decrButton->NormalGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarUp);
				//m_decrButton->MouseDownGraphic = UIGraphic(skin->SkinTexture, gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarUpD);
				//m_decrButton->MouseOverGraphic = UIGraphic(skin->SkinTexture, gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarUpH);
				//m_decrButton->DisabledGraphic = m_decrButton->NormalGraphic;
				//m_decrButton->DisabledGraphic.ModColor = CV_PackColor(0xff, 0xff, 0xff, m_disabledAlpha);

				bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarDown);

				m_incrButton = new Button(bvs, Point(0, 0), m_decrButton->getSize(), L"");
				//m_incrButton->NormalGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarDown);
				//m_incrButton->MouseDownGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarDownD);
				//m_incrButton->MouseOverGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarDownH);
				//m_incrButton->DisabledGraphic = m_incrButton->NormalGraphic;
				//m_incrButton->DisabledGraphic.ModColor = m_incrButton->DisabledGraphic.ModColor;

				HandleGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarCursor);
				BackgroundGraphic = UIGraphic(skin->SkinTexture, skin->VScrollBarBG);
			}
			else
			{
				ButtonVisualSettings bvs;
				bvs.HasNormalGraphic = true;
				bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarLeft);

				m_decrButton = new Button(bvs, Point(0, 0), skin->HScrollBarLeft.getSize(), L"");
				//m_decrButton->NormalGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarLeft);// gui_ControlAssets.VScrollBarUpN);
				//m_decrButton->MouseDownGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarUpD);
				//m_decrButton->MouseOverGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarUpH);
				//m_decrButton->DisabledGraphic = m_decrButton->NormalGraphic;
				//m_decrButton->DisabledGraphic.ModColor = CV_PackColor(0xff, 0xff, 0xff, m_disabledAlpha);

				bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarRight);
				m_incrButton = new Button(bvs, Point(0, 0), skin->HScrollBarRight.getSize(), L"");
				//m_incrButton->NormalGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarRight); // gui_ControlAssets.VScrollBarDownN
				//m_incrButton->MouseDownGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarDownD);
				//m_incrButton->MouseOverGraphic = UIGraphic(gui_ControlAssets.Graphic, gui_ControlAssets.VScrollBarDownH);
				//m_incrButton->DisabledGraphic = m_incrButton->NormalGraphic;
				//m_incrButton->DisabledGraphic.ModColor = m_incrButton->DisabledGraphic.ModColor;


				HandleGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarCursor);
				BackgroundGraphic = UIGraphic(skin->SkinTexture, skin->HScrollBarBG);
			}

			PostInit(length);
		}
		ScrollBar::~ScrollBar()
		{
			m_decrButton->eventRelease.Reset();
			m_incrButton->eventRelease.Reset();

			DELETE_AND_NULL(m_decrButton);
			DELETE_AND_NULL(m_incrButton);
		}

		void ScrollBar::PostInit(int32 length)
		{
			m_decrButton->eventRelease.Bind(this, &ScrollBar::DecrButton_Pressed);
			m_incrButton->eventRelease.Bind(this, &ScrollBar::IncrButton_Pressed);

			UpdateButtonPosition();

			if (m_type != SCRBAR_Vertical)
			{
				m_size.X = length;
				m_size.Y = BackgroundGraphic.SourceRects[0].Height;
			}
			else
			{
				m_size.X = BackgroundGraphic.SourceRects[0].Width;
				m_size.Y = length;
			}
		}

		void ScrollBar::Draw(Sprite* sprite)
		{
			if (!Visible)
				return;

			{
				UIGraphic* g = Enabled ? &BackgroundGraphic : &DisabledBackgroundGraphic;

				g->Draw(sprite, getAbsoluteArea(), m_type == SCRBAR_Vertical);
			}

			if (m_decrButton->Enabled && Maximum > 0)
			{
				Apoc3D::Math::Rectangle handleArea = CalculateHandleArea();
				handleArea = HandlePadding.InflateRect(handleArea);

				UIGraphic* g = Enabled ? &HandleGraphic : &DisabledHandleGraphic;

				g->Draw(sprite, handleArea, m_type == SCRBAR_Vertical);
			}


			m_decrButton->Draw(sprite);
			m_incrButton->Draw(sprite);
		}
		void ScrollBar::Update(const GameTime* time)
		{
			m_isMouseHovering = false;

			if (m_value > Maximum)
				m_value = Maximum;
			
			UpdateButtonPosition();

			if (!Enabled || !Visible)
				return;

			int32 scrLen = GetScrollableLength();
			if (Maximum > 0)
			{
				m_decrButton->Enabled = true;
				m_incrButton->Enabled = true;

				Mouse* mouse = InputAPIManager::getSingleton().getMouse();

				Apoc3D::Math::Rectangle handleArea = CalculateHandleArea();
				int32 handleLen = m_type == SCRBAR_Vertical ? handleArea.Height : handleArea.Width;

				if (m_isDragging)
				{
					if (handleLen < scrLen)
					{
						int32 dm = m_type == SCRBAR_Horizontal ? mouse->getDX() : mouse->getDY();
						int32 dv = dm * Maximum / (scrLen - handleLen);

						m_value += IsInverted ? -dv : dv;
						m_value = Math::Clamp(m_value, 0, Maximum);
					}

					if (mouse->IsLeftUp())
					{
						m_isDragging = false;
					}

					m_isMouseHovering = true;
					m_mouseHoverArea = handleArea;
				}
				else
				{
					m_decrButton->Update(time);
					m_incrButton->Update(time);

					if (m_decrButton->isMouseHover())
					{
						m_isMouseHovering = true;
						m_mouseHoverArea = m_decrButton->getAbsoluteArea();
					}
					if (m_incrButton->isMouseHover())
					{
						m_isMouseHovering = true;
						m_mouseHoverArea = m_incrButton->getAbsoluteArea();
					}

					if (!m_isMouseHovering && handleArea.Contains(mouse->getX(), mouse->getY()))
					{
						m_isMouseHovering = true;
						m_mouseHoverArea = handleArea;

						if (mouse->IsLeftPressed())
						{
							m_isDragging = true;
						}
					}
				}

			}
			else
			{
				m_decrButton->Enabled = false;
				m_incrButton->Enabled = false;
			}
		}
		void ScrollBar::UpdateButtonPosition()
		{
			Point decrButtonOffset(BorderPadding.Left, BorderPadding.Top);
			Point incrButtonOffset(BorderPadding.Left, BorderPadding.Bottom);

			m_decrButton->Position = Position + decrButtonOffset;
			m_incrButton->Position = Position + incrButtonOffset;

			m_decrButton->BaseOffset = m_incrButton->BaseOffset = BaseOffset;

			if (m_type == SCRBAR_Vertical)
			{
				m_incrButton->Position.Y += m_size.Y - m_incrButton->getSize().Y;
			}
			else
			{
				m_incrButton->Position.X += m_size.X - m_incrButton->getSize().X;
			}
		}

		int32 ScrollBar::GetScrollableLength() const
		{
			int32 result;
			if (m_type == SCRBAR_Vertical)
				result = m_size.Y - m_decrButton->getSize().Y - m_incrButton->getSize().Y;
			else
				result = m_size.X - m_decrButton->getSize().X - m_incrButton->getSize().X;

			if (result < 0)
				result = 0;
			return result;
			//return Height - m_upButton->getSize().Y - m_downButton->getSize().Y;
		}

		int32 ScrollBar::CalculateBarLength() const
		{
			int32 scrollHeight = GetScrollableLength();
			if (Maximum + scrollHeight == 0)
				return 100;

			int32 r = (int32)(scrollHeight * scrollHeight / (Maximum + scrollHeight));
			if (r > scrollHeight)
				r = scrollHeight;
			if (r < 100)
				r = 100;
			return r;
		}
		Apoc3D::Math::Rectangle ScrollBar::CalculateHandleArea() const
		{
			Point pos = GetAbsolutePosition();

			if (m_type == SCRBAR_Vertical)
			{
				Apoc3D::Math::Rectangle handleArea;

				handleArea.X = pos.X + BorderPadding.Left;// +1;
				handleArea.Width = BackgroundGraphic.getWidth() - BorderPadding.getHorizontalSum();// -2;

				handleArea.Height = CalculateBarLength();
				int32 scrH = GetScrollableLength() - handleArea.Height;

				if (IsInverted)
					handleArea.Y = m_decrButton->getAbsoluteArea().getBottom() + Maximum - m_value * scrH / Maximum;
				else
					handleArea.Y = m_decrButton->getAbsoluteArea().getBottom() + m_value * scrH / Maximum;

				return handleArea;
			}
			else
			{
				Apoc3D::Math::Rectangle handleArea;

				handleArea.Y = pos.Y + BorderPadding.Top;
				handleArea.Height = BackgroundGraphic.getHeight() - BorderPadding.getVerticalSum();

				handleArea.Width = CalculateBarLength();
				int32 scrW = GetScrollableLength() - handleArea.Width;

				if (IsInverted)
					handleArea.X = m_decrButton->getAbsoluteArea().getRight() + Maximum - m_value * scrW / Maximum;
				else
					handleArea.X = m_decrButton->getAbsoluteArea().getRight() + m_value * scrW / Maximum;

				return handleArea;
			}
		}

		void ScrollBar::DecrButton_Pressed(Button* btn)
		{
			ForceScroll(IsInverted ? Step : -Step);
		}
		void ScrollBar::IncrButton_Pressed(Button* btn)
		{
			ForceScroll(IsInverted ? -Step : Step);
		}

		void ScrollBar::ForceScroll(int32 chg)
		{
			m_value += chg;
			m_value = Math::Clamp(m_value, 0, Maximum);
		}
		void ScrollBar::SetValue(int32 val)
		{
			m_value = Math::Clamp(val, 0, Maximum);
		}

		void ScrollBar::SetLength(int32 len)
		{
			(m_type != SCRBAR_Vertical ? m_size.X : m_size.Y) = len;
		}
		int32 ScrollBar::GetLength() const
		{
			return m_type != SCRBAR_Vertical ? m_size.X : m_size.Y;
			//return 
		}
#pragma endregion


	}
}