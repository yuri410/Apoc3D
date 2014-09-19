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

#include "Form.h"
#include "Menu.h"
#include "Button.h"
#include "StyleSkin.h"
#include "FontManager.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Core/GameTime.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		Form::Form(const StyleSkin* skin, RenderDevice* device, BorderStyle border, const String& title)
			: ControlContainer(skin), m_device(device), m_skin(skin),
			m_titleOffset(12,0), m_minimumSize(100, 40), m_minimizedSize(100, 20),
			m_dragArea(0,0,0,0), m_resizeArea(0,0,15,15), 
			m_borderStyle(border), m_title(title)
		{
			m_size = Point(200,100);
			Visible = false;

			Initialize(skin);
		}

		Form::~Form()
		{
			if (SystemUI::ModalForm == this)
			{
				SystemUI::ModalForm = nullptr;
			}
			if (SystemUI::TopMostForm == this)
			{
				SystemUI::TopMostForm = nullptr;
			}
			delete m_btClose;
			delete m_btMaximize;
			delete m_btMinimize;
			delete m_btRestore;
			delete m_border;
		}

		void Form::ShowModal()
		{
			SystemUI::ModalForm = this;
			Show();
		}
		void Form::Show()
		{
			m_state = FWS_Normal;

			if (m_previousPosition != Point::Zero)
				Position = m_previousPosition;
			if (m_previousSize != Point::Zero)
				m_size = m_previousSize;

			if (!Visible)
			{
				Visible = true;
				ApplyBaseOffset();
			}

			Focus();
		}
		void Form::Hide()
		{
			if (SystemUI::ModalForm == this)
			{
				SystemUI::ModalForm = nullptr;
			}
			Visible = false;
		}
		void Form::Close()
		{
			if (MenuBar)
			{
				MenuBar->Close();
			}

			if (SystemUI::TopMostForm == this)
				SystemUI::TopMostForm = nullptr;

			Hide();
			
			eventClosed.Invoke(this);
		}
		void Form::Focus()
		{
			if (SystemUI::TopMostForm && SystemUI::TopMostForm != this && 
				SystemUI::TopMostForm->MenuBar && SystemUI::TopMostForm->MenuBar->getState() == MENU_Open)
			{
				return;
			}

			SystemUI::BringToFirst(this);
			SystemUI::TopMostForm = this;

		}
		void Form::Unfocus()
		{
			if (MenuBar && MenuBar->getState() != MENU_Closed)
			{
				MenuBar->Close();
			}

			if (SystemUI::TopMostForm == this)
				SystemUI::TopMostForm = nullptr;
		}
		void Form::Minimize()
		{
			Point minPos;
			bool ret = SystemUI::GetMinimizedPosition(m_device, this, minPos);
			if (ret)
				m_minimizedPos = minPos;
			else
				return;


			if (m_state == FWS_Normal)
			{
				m_previousPosition = Position;
				m_previousSize = m_size;
			}
			
			if (MenuBar)
			{
				MenuBar->Close();
				MenuBar->Visible = false;
			}

			m_state = FWS_Minimized;
			m_isMinimizing = true;
		}
		void Form::Maximize()
		{
			if (m_state == FWS_Normal)
			{
				m_previousPosition = Position;
				m_previousSize = m_size;
			}

			if (MenuBar)
			{
				MenuBar->Close();
			}

			Focus();

			Apoc3D::Math::Rectangle rect = SystemUI::GetMaximizedRect(m_device, this);
			m_maximizedPos.X = rect.X; m_maximizedPos.Y = rect.Y;

			m_maximumSize.X = rect.Width; m_maximumSize.Y = rect.Height;// UIRoot::GetMaximizedSize(m_device, this);
			m_state = FWS_Maximized;
		}
		void Form::Restore()
		{
			Focus();

			if (MenuBar)
			{
				MenuBar->Visible = true;
				MenuBar->Close();
			}

			m_state = FWS_Normal;
		}

		void Form::Initialize(const StyleSkin* skin)
		{
			Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(m_device);
			m_maximumSize = Point(rect.Width, rect.Height);
			
			m_border = new Border(m_borderStyle, m_skin);

			m_fontRef = m_skin->TitleTextFont;

			if (m_borderStyle != FBS_Pane)
				m_titleOffset.Y = (m_skin->FormTitle->Height - m_fontRef->getLineHeightInt())/2 - 1;

			m_titleOffset.X = m_skin->FormTitlePadding.Left;

			InitializeButtons();

			m_initialized = true;
		}

		void Form::InitializeButtons()
		{
			ButtonVisualSettings bvs;
			bvs.NormalGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseNormal);
			bvs.MouseHoverGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseHover);
			bvs.MouseDownGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseDown);
			bvs.OverlayIcon = UIGraphicSimple(m_skin->SkinTexture, m_skin->FormCBIconClose);

			m_btClose = new Button(bvs, Point(m_size.X - m_skin->FormTitlePadding.Right - m_skin->FormCBCloseNormal.Width, m_skin->FormTitlePadding.Top), L"");
			m_btClose->eventRelease.Bind(this, &Form::btClose_Release);

			
			if (m_hasMinimizeButton)
			{
				bvs.NormalGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinNormal);
				bvs.MouseHoverGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinHover);
				bvs.MouseDownGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinDown);
				bvs.OverlayIcon = UIGraphicSimple(m_skin->SkinTexture, m_skin->FormCBIconMin);

				m_btMinimize = new Button(bvs, Point(0, 0), L"");
				m_btMinimize->eventRelease.Bind(this, &Form::btMinimize_Release);
			}
			if (m_hasMaximizeButton)
			{
				bvs.NormalGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxNormal);
				bvs.MouseHoverGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxHover);
				bvs.MouseDownGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxDown);
				bvs.OverlayIcon = UIGraphicSimple(m_skin->SkinTexture, m_skin->FormCBIconMax);

				m_btMaximize = new Button(bvs, Point(0, 0), L"");
				m_btMaximize->eventRelease.Bind(this, &Form::btMaximize_Release);
			}
			if (m_hasMinimizeButton || m_hasMaximizeButton)
			{
				bvs.NormalGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreNormal);
				bvs.MouseHoverGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreHover);
				bvs.MouseDownGraphic = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreDown);
				bvs.OverlayIcon = UIGraphicSimple(m_skin->SkinTexture, m_skin->FormCBIconRestore);

				m_btRestore = new Button(bvs, Point(0, 0), L"");
				m_btRestore->eventRelease.Bind(this, &Form::btRestore_Release);
			}
			ApplyBaseOffset();
		}


		void Form::btClose_Release(Button* sender)
		{
			Close();
		}
		void Form::btMinimize_Release(Button* sender)
		{
			Minimize();
		}
		void Form::btMaximize_Release(Button* sender)
		{
			Maximize();
		}
		void Form::btRestore_Release(Button* sender)
		{
			Restore();
		}

		void Form::Update(const GameTime* time)
		{
			if (!Visible)
				return;

			if (MenuBar)
				MenuBar->Position = Point(0, m_skin->FormTitle->Height);

			UpdateState();
			UpdateTopMost();


			if (m_state == FWS_Normal && !m_isMaximized && !m_isMinimized)
			{
				if (!m_isResizeing && m_borderStyle != FBS_Pane)
				{
					CheckDragging();
				}
				if (!m_isDragging && m_borderStyle == FBS_Sizable)
				{
					CheckResize();
				}
			}

			ApplyBaseOffset();
			CheckDoubleClick(time);

			if (m_state != FWS_Minimized && !m_isMinimized)
			{
				if (m_size.X < m_minimumSize.X)
					m_size.X = m_minimumSize.X;
				if (m_size.Y < m_minimumSize.Y)
					m_size.Y = m_minimumSize.Y;
			}

			if (SystemUI::TopMostForm == this || (m_isMinimized && SystemUI::ActiveForm == this))
			{
				if (MenuBar && MenuBar->Visible)
				{
					MenuBar->Update(time);
				}

				bool blocked = false;
				if (m_btClose && m_borderStyle != FBS_Pane)
				{
					m_btClose->Update(time);
					blocked |= m_btClose->isMouseHover();
				}

				if (!blocked && m_btMaximize && ((m_hasMaximizeButton && !m_isMaximized) || (m_hasMinimizeButton && m_isMinimized)))
				{
					m_btMaximize->Update(time);
					blocked |= m_btMaximize->isMouseHover();
				}
				if (!blocked && m_btMinimize && ((m_hasMinimizeButton && !m_isMinimized) || (m_hasMaximizeButton && m_isMaximized)))
				{
					m_btMinimize->Update(time);
					blocked |= m_btMinimize->isMouseHover();
				}

				if (!blocked && m_btRestore && ((m_hasMinimizeButton && m_isMinimized) || (m_hasMaximizeButton && m_isMaximized)))
				{
					m_btRestore->Update(time);
					blocked |= m_btRestore->isMouseHover();
				}

				if (MenuBar == nullptr || !MenuBar->Visible || MenuBar->getState() == MENU_Closed)
				{
					if (m_state == FWS_Normal || m_state == FWS_Maximized)
					{
						bool skip = false;
						for (int32 i = 0; i < m_controls.getCount();i++)
						{
							Control* ct = m_controls[i];
							if (ct->IsOverriding())
							{
								ct->Update(time);
								skip = true;
							}
						}

						if (!skip)
						{
							for (int32 i = 0; i < m_controls.getCount(); i++)
							{
								Control* ct = m_controls[i];
								if (ct->Enabled)
								{
									ct->Update(time);
								}
							}
						}
					}
				}
			}
			
			UpdateActive();
			
			if (SystemUI::ActiveForm != this)
			{
				if (m_isResizeing) m_isResizeing = false;
				if (m_isDragging) m_isDragging = false;
			}
		}

		void Form::UpdateTopMost()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();	

			if (mouse->IsLeftPressedState() && getArea().Contains(mouse->GetPosition()) && SystemUI::ActiveForm == nullptr)
			{
				SystemUI::ActiveForm = this;
				Focus();
			}
		}

		void Form::UpdateActive()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (SystemUI::ActiveForm == this && mouse->IsLeftReleasedState())
			{
				SystemUI::ActiveForm = nullptr;
			}
		}

		static Point Limit(Point d)
		{
			if (d.X > 5 || d.X < -5) d.X /= 5;
			if (d.Y > 5 || d.Y < -5) d.Y /= 5;
			return d;
		}

		void Form::UpdateState()
		{
			//Restore the window to its original size and position
			if (m_state == FWS_Normal && (m_isMaximized || m_isMinimized))
			{
				if (Point::Distance(Position, m_previousPosition) > 2.0f)
				{
					Position += Limit(m_previousPosition - Position);
				}
				else
				{
					Position = m_previousPosition;
				}

				if (Point::Distance(m_size, m_previousSize) > 2.0f)
				{
					m_size += Limit(m_previousSize - m_size);
				}
				else
				{
					m_size = m_previousSize;
				}

				if (Position == m_previousPosition && m_size == m_previousSize)
				{
					m_isMaximized = false;
					m_isMinimized = false;

					Focus();

					eventResized.Invoke(this);
				}
			}
			//Minimize the window
			else if (m_state == FWS_Minimized && !m_isMinimized)
			{
				if (Point::Distance(Position, m_minimizedPos) > 2.0f)
				{
					Position += Limit(m_minimizedPos - Position);
				}
				else
				{
					Position = m_minimizedPos;
				}

				if (Point::Distance(m_size, m_minimizedSize) > 2.0f)
				{
					m_size += Limit(m_minimizedSize - m_size);
				}
				else
				{
					m_size = m_minimizedSize;
				}

				if (Position == m_minimizedPos && m_size == m_minimizedSize)
				{
					m_isMinimized = true;
					m_isMaximized = false;
					m_isMinimizing = false;

					eventResized.Invoke(this);
				}
			}
			//Maximize the window
			else if (m_state == FWS_Maximized && !m_isMaximized)
			{
				if (Point::Distance(m_maximizedPos, Position) > 2.0f)
				{
					Position += Limit(m_maximizedPos - Position);
				}
				else
				{
					Position = m_maximizedPos;
				}

				if (Point::Distance(m_size, m_maximumSize) > 2.0f)
				{
					m_size += Limit(m_maximumSize - m_size);
				}
				else
				{
					m_size = m_maximumSize;
				}

				if (Position == m_maximizedPos && m_size == m_maximumSize)
				{
					m_isMinimized = false;
					m_isMaximized = true;

					eventResized.Invoke(this);
				}
			}
		}
		void Form::CheckDragging()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_dragArea.X = Position.X + m_skin->FormTitle[0].Width;
			m_dragArea.Y = Position.Y;
			m_dragArea.Width = m_size.X - m_skin->FormCBCloseNormal.Width - m_skin->FormTitlePadding.Right;
			if (m_hasMinimizeButton)
				m_dragArea.Width -= m_skin->FormCBMinNormal.Width;
			if (m_hasMaximizeButton)
				m_dragArea.Width -= m_skin->FormCBMaxNormal.Width;
			m_dragArea.Height = m_skin->FormTitle[0].Height;

			if (m_dragArea.Contains(mouse->GetPosition()) &&
				mouse->IsLeftPressed() && SystemUI::ActiveForm == this)
			{
				m_isDragging = true;
				Focus();
				m_posOffset = mouse->GetPosition() - Position;
			}

			if (m_isDragging)
			{
				Point dif = mouse->GetPosition() - m_posOffset - Position;

				dif = SystemUI::ClampFormMovementOffset(this, dif);
				Position.X += dif.X;
				Position.Y += dif.Y;

				if (mouse->IsLeftReleasedState())
					m_isDragging = false;
			}
			// snapping?
		}

		void Form::CheckDoubleClick(const GameTime* time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (m_state != FWS_Normal)
			{
				m_dragArea.X = Position.X + 7;
				m_dragArea.Y = Position.Y;

				m_dragArea.Width = m_size.X - 29;
				if (m_hasMinimizeButton)
					m_dragArea.Width -= 15;
				if (m_hasMaximizeButton)
					m_dragArea.Width -= 15;
				m_dragArea.Height = 20;
			}

			if (m_borderStyle == FBS_Sizable && m_dragArea.Contains(mouse->GetPosition()))
			{
				if (mouse->IsLeftPressed())
				{
					if (SystemUI::ActiveForm != this)
						SystemUI::ActiveForm = this;
				}
				if (m_clickChecker.Check(mouse))
				{
					ToggleWindowState();
				}
			}
			m_clickChecker.Update(time, mouse);
		}

		void Form::CheckResize()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_resizeArea.setPosition(Position + m_size - m_resizeArea.getSize());

			if (m_resizeArea.Contains(mouse->GetPosition()) &&
				SystemUI::TopMostForm == this)
			{
				if (m_isInReiszeArea)
				{
					m_isInReiszeArea = true;
				}

				if (mouse->IsLeftPressed() &&
					SystemUI::ActiveForm == this)
				{
					m_isResizeing = true;
					Focus();
					
					m_posOffset = mouse->GetPosition();
					m_oldSize = m_size;
				}
			}
			else if (m_isInReiszeArea && !m_isResizeing)
			{
				m_isInReiszeArea = false;
			}

			if (m_isResizeing)
			{
				Point dif = m_oldSize + mouse->GetPosition() - m_posOffset - m_size;

				dif = SystemUI::ClampFormMovementOffset(this, dif);

				m_size += dif;

				if (dif.X != 0 || dif.Y != 0)
				{
					eventResized.Invoke(this);
				}

				if (mouse->IsLeftReleasedState())
				{
					m_isResizeing = false;
				}
			}
		}

		void Form::ToggleWindowState()
		{
			if (m_state == FWS_Normal)
				Maximize();
			else if (m_state == FWS_Maximized)
				Restore();
			else if (m_state == FWS_Minimized)
				Restore();
		}

		void Form::Draw(Sprite* sprite)
		{
			if (!Visible)
				return;

			Apoc3D::Math::Rectangle uiArea = SystemUI::GetUIArea(m_device);

			m_borderAlpha = 0.3f - SystemUI::getForms().IndexOf(this) * 0.04f;
			m_border->Draw(sprite, Position, m_size, m_borderAlpha);

			DrawTitle(sprite);
			DrawButtons(sprite);

			if (m_state != FWS_Minimized)
			{
				sprite->Flush();

				Apoc3D::Math::Rectangle rect= getAbsoluteArea();
				if (rect.getBottom()>uiArea.getBottom())
				{
					rect.Height -= rect.getBottom() - uiArea.getBottom();
				}
				if (rect.getRight() > uiArea.getRight())
				{
					rect.Width -= rect.getRight() - uiArea.getRight();
				}
				m_device->getRenderState()->setScissorTest(true, &rect);

				bool hasOverridingControl = false;
				int overlay = 0;
				for (int i = 0; i < m_controls.getCount(); i++)
				{
					Control* ctl = m_controls[i];
					if (ctl->IsOverriding())
					{
						overlay = i;
						hasOverridingControl = true;
					}
					if (ctl->Visible)
					{
						ctl->Draw(sprite);
					}
				}

				sprite->Flush();
				m_device->getRenderState()->setScissorTest(false,0);
				if (hasOverridingControl)
				{
					m_controls[overlay]->DrawOverlay(sprite);
				}

				if (MenuBar && MenuBar->Visible)
					MenuBar->Draw(sprite);

			}
		}
		void Form::DrawButtons(Sprite* sprite)
		{
			if (m_borderStyle != FBS_None && m_borderStyle != FBS_Pane)
			{
				m_btClose->Position.X = m_size.X - 22;
				m_btClose->Position.Y = 4;
				m_btClose->Draw(sprite);
			}

			if (m_borderStyle == FBS_Sizable)
			{
				if (m_hasMaximizeButton)
				{					
					if (m_state != FWS_Maximized)
					{
						m_btMaximize->Position.X = m_size.X - 37;
						m_btMaximize->Position.Y = 4;
						
						m_btMaximize->Draw(sprite);
					}
					else
					{
						m_btRestore->Position.X = m_size.X - 37;
						m_btRestore->Position.Y = 4;
						m_btRestore->Draw(sprite);
					}

					if (m_hasMinimizeButton)
					{
						if (m_state != FWS_Minimized)
						{
							m_btMinimize->Position.X = m_size.X - 52;
							m_btMinimize->Position.Y = 4;
							m_btMinimize->Draw(sprite);
						}
						else
						{
							m_btRestore->Position.X = m_size.X - 52;
							m_btRestore->Position.Y = 4;
							m_btRestore->Draw(sprite);
						}
					}
				}
				else if (m_hasMinimizeButton)
				{
					if (m_state != FWS_Minimized)
					{
						m_btMinimize->Position.X = m_size.X - 37;
						m_btMinimize->Position.Y = 4;
						m_btMinimize->Draw(sprite);
					}
					else
					{
						m_btRestore->Position.X = m_size.X - 37;
						m_btRestore->Position.Y = 4;
						m_btRestore->Draw(sprite);
					}
				}
			}

		}

		void Form::DrawTitle(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition() + m_titleOffset;

			Point size = m_fontRef->MeasureString(m_title);
			int32 padding = m_skin->FormTitlePadding.getHorizontalSum();
			padding += m_skin->FormCBCloseNormal.Width;
			if (m_hasMinimizeButton)
				padding += m_skin->FormCBMinNormal.Width;
			if (m_hasMaximizeButton)
				padding += m_skin->FormCBMaxNormal.Width;

			if (size.X >= m_size.X - padding)
			{
				String title = m_title;
				guiOmitLineText(m_fontRef, m_size.X - padding, title);
				
				m_fontRef->DrawString(sprite, title, drawPos, m_skin->TextColor);
			}
			else
			{
				m_fontRef->DrawString(sprite, m_title, drawPos, m_skin->TextColor);
			}

		}

		void Form::ApplyBaseOffset()
		{
			Point subOffset = GetAbsolutePosition();
			if (m_btClose)
				m_btClose->BaseOffset = subOffset;
			if (m_btMinimize)
				m_btMinimize->BaseOffset = subOffset;
			if (m_btMaximize)
				m_btMaximize->BaseOffset = subOffset;
			if (m_btRestore)
				m_btRestore->BaseOffset = subOffset;

			bool isFocus = SystemUI::TopMostForm == this || SystemUI::ActiveForm == this;

			for (Control* ct : m_controls)
			{
				ct->ParentFocused = isFocus;
				ct->BaseOffset = subOffset;
			}
		}
			 
		/************************************************************************/
		/*  Border                                                              */
		/************************************************************************/


		Border::Border(BorderStyle style, const StyleSkin* skin)
			: m_skin(skin), m_style(style)
		{
			for (int i = 0; i < countof(m_dstRect); i++)
			{
				//Texture* tex;
				if (m_style == FBS_Pane)
				{
					//tex = m_skin->WhitePixelTexture;
					m_dstRect[i] = Apoc3D::Math::Rectangle(0, 0, 1, 1);
					m_srcRect[i] = m_dstRect[i];
				}
				else
				{
					if (i < 3)
						m_srcRect[i] = m_skin->FormTitle[i];
					else
						m_srcRect[i] = m_skin->FormBody[i];
					m_dstRect[i] = m_srcRect[i];
				}

				//m_dstRect[i] = Apoc3D::Math::Rectangle(0,0,tex->getWidth(), tex->getHeight());
			}
			
			/*if (style == FBS_Sizable)
			{
				m_dstRect[8] = Apoc3D::Math::Rectangle(0,0,m_skin->FormBorderTexture[9]->getWidth(), m_skin->FormBorderTexture[9]->getHeight());
			}*/
			//if (m_style == FBS_Pane)
			//	m_shadowOffset = Point(1,1);
		}

		void Border::Draw(Sprite* sprite, const Point& pt, const Point& size, float shadowAlpha)
		{
			UpdateRects(pt, size);

			//if (size.Y > m_dstRect[0].Height)
			//{
				//Point shdPos = pt;
				//shdPos.Y += 15;
				//DrawShadow(sprite, shdPos, shadowAlpha);
			//}

			DrawUpper(sprite);
			if (size.Y > m_dstRect[0].Height)
			{
				DrawMiddle(sprite);
				DrawLower(sprite);
			}

			if (m_style == FBS_Sizable)
			{
				Apoc3D::Math::Rectangle dstRect = m_skin->FormResizer;
				dstRect.X = pt.X + size.X - dstRect.Width - 5;
				dstRect.Y = pt.Y + size.Y - dstRect.Height - 5;
				sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->FormResizer, CV_White);

			}
		}

		void Border::UpdateRects(const Point& position, const Point& size)
		{
			// upper
			m_dstRect[0].X = position.X;
			m_dstRect[0].Y = position.Y;

			m_dstRect[1].X = m_dstRect[0].X + m_dstRect[0].Width;
			m_dstRect[1].Y = m_dstRect[0].Y;
			m_dstRect[1].Width = size.X - (m_dstRect[0].Width + m_dstRect[2].Width);

			m_dstRect[2].X = m_dstRect[1].X + m_dstRect[1].Width;
			m_dstRect[2].Y = m_dstRect[1].Y;

			// middle
			if (m_style == FBS_Pane)
			{
				m_dstRect[3].X = m_dstRect[0].X;
				m_dstRect[3].Y = m_dstRect[0].Y + m_dstRect[0].Height;
				m_dstRect[3].Height = size.Y - (m_dstRect[0].Height + SystemUI::GetWhitePixel()->getHeight());

			}
			else
			{
				m_dstRect[3].X = m_dstRect[0].X;
				m_dstRect[3].Y = m_dstRect[0].Y + m_dstRect[0].Height;
				m_dstRect[3].Height = size.Y - (m_dstRect[0].Height + m_skin->FormBody[6].Height);

			}
			
			m_dstRect[4].X = m_dstRect[1].X;
			m_dstRect[4].Y = m_dstRect[3].Y;
			m_dstRect[4].Width = m_dstRect[1].Width;
			m_dstRect[4].Height = m_dstRect[3].Height;

			m_dstRect[5].X = m_dstRect[2].X;
			m_dstRect[5].Y = m_dstRect[3].Y;
			m_dstRect[5].Height = m_dstRect[3].Height;

			// lower
			m_dstRect[6].X = m_dstRect[0].X;
			m_dstRect[6].Y = m_dstRect[3].Y + m_dstRect[3].Height;

			if (m_style == FBS_Pane)
			{
				if (size.Y > m_dstRect[0].Height + SystemUI::GetWhitePixel()->getHeight())
				{
					m_dstRect[6].Height = SystemUI::GetWhitePixel()->getHeight();
				}
				else
				{
					m_dstRect[6].Y = m_dstRect[0].Y + m_dstRect[0].Height;
					m_dstRect[6].Height = size.Y - SystemUI::GetWhitePixel()->getHeight();
				}
			}
			else
			{
				if (size.Y > m_dstRect[0].Height + m_skin->FormBody[6].Height)
				{
					m_dstRect[6].Height = m_skin->FormBody[6].Height;
				}
				else
				{
					m_dstRect[6].Y = m_dstRect[0].Y + m_dstRect[0].Height;
					m_dstRect[6].Height = size.Y - m_skin->FormBody[6].Height;
				}
			}
			

			m_dstRect[7].X = m_dstRect[1].X;
			m_dstRect[7].Y = m_dstRect[6].Y;
			m_dstRect[7].Width = m_dstRect[1].Width;
			m_dstRect[7].Height = m_dstRect[6].Height;

			m_dstRect[8].X = m_dstRect[2].X;
			m_dstRect[8].Y = m_dstRect[6].Y;
			m_dstRect[8].Height = m_dstRect[6].Height;
		}
		void Border::DrawUpper(Sprite* sprite)
		{
			if (m_style == FBS_Pane)
			{
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[0], m_skin->BorderColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[1], m_skin->BorderColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[2], m_skin->BorderColor);
			}
			else
			{
				sprite->Draw(m_skin->SkinTexture, m_dstRect[0], &m_srcRect[0], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[1], &m_srcRect[1], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[2], &m_srcRect[2], CV_White);
			}
			
		}
		void Border::DrawMiddle(Sprite* sprite)
		{
			if (m_style == FBS_Pane)
			{
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[3], m_skin->BorderColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[4], m_skin->ControlFaceColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[5], m_skin->BorderColor);
			}
			else
			{
				sprite->Draw(m_skin->SkinTexture, m_dstRect[3], &m_srcRect[3], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[4], &m_srcRect[4], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[5], &m_srcRect[5], CV_White);
			}
			
		}
		void Border::DrawLower(Sprite* sprite)
		{
			if (m_style == FBS_Pane)
			{
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[6], m_skin->BorderColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[7], m_skin->BorderColor);
				sprite->Draw(SystemUI::GetWhitePixel(), m_dstRect[8], m_skin->BorderColor);
			}
			else
			{
				sprite->Draw(m_skin->SkinTexture, m_dstRect[6], &m_srcRect[6], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[7], &m_srcRect[7], CV_White);
				sprite->Draw(m_skin->SkinTexture, m_dstRect[8], &m_srcRect[8], CV_White);
			}
		}
		void Border::DrawShadow(Sprite* sprite, const Point& pos, float alpha)
		{
			/*if (m_style == FBS_Pane)
				return;
			
			if (alpha > 1)
				alpha = 1;
			if (alpha<0)
				alpha=0;
			byte a = (byte)(alpha * 0xff);

			ColorValue shdClr = PACK_COLOR(0,0,0,a);

			Apoc3D::Math::Rectangle shadowRect = m_dstRect[2];
			shadowRect.X += m_shadowOffset.X;
			shadowRect.Y += m_shadowOffset.Y;
			sprite->Draw(m_skin->FormBorderTexture[2], shadowRect, shdClr);

			shadowRect = m_dstRect[5];
			shadowRect.X += m_shadowOffset.X;
			shadowRect.Y += m_shadowOffset.Y;
			sprite->Draw(m_skin->FormBorderTexture[5], shadowRect, shdClr);


			shadowRect = m_dstRect[6];
			shadowRect.X += m_shadowOffset.X;
			shadowRect.Y += m_shadowOffset.Y;
			sprite->Draw(m_skin->FormBorderTexture[6], shadowRect, shdClr);


			shadowRect = m_dstRect[7];
			shadowRect.X += m_shadowOffset.X;
			shadowRect.Y += m_shadowOffset.Y;
			sprite->Draw(m_skin->FormBorderTexture[7], shadowRect, shdClr);

			shadowRect = m_dstRect[8];
			shadowRect.X += m_shadowOffset.X;
			shadowRect.Y += m_shadowOffset.Y;
			if (m_style == FBS_Sizable)
			{
				sprite->Draw(m_skin->FormBorderTexture[9], shadowRect, shdClr);
			}
			else
			{
				sprite->Draw(m_skin->FormBorderTexture[8], shadowRect, shdClr);
			}
			*/
		}

	}
}