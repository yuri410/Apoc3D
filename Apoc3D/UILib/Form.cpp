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

		Form::Form(BorderStyle border, const String& title)
			: m_titleOffset(12,0), m_minimumSize(100, 40), m_minimizedSize(100, 20),
			m_maximumSize(0,0), m_maximizedPos(0,0), m_isMinimized(false), m_isMaximized(false),
			m_previousPosition(0,0), m_previousSize(0,0), m_minimizedPos(0,0), 
			m_hasMinimizeButton(true), m_hasMaximizeButton(true), 
			m_dragArea(0,0,0,0), m_resizeArea(0,0,15,15), m_isDragging(false), m_isResizeing(false),
			m_isMinimizing(false), m_isInReiszeArea(false), m_posOffset(0,0), m_oldSize(0,0), m_initialized(false), m_lastClickTime(0),
			m_borderStyle(border), m_state(FWS_Normal), m_title(title),
			m_borderAlpha(1), m_border(nullptr),
			m_device(nullptr), m_btClose(nullptr), m_btMinimize(nullptr),
			m_btMaximize(nullptr), m_btRestore(nullptr),
			IsBackgroundForm(false)
		{
			Size = Point(200,100);
			Visible = false;
		}

		Form::~Form()
		{
			if (UIRoot::getModalForm() == this)
			{
				UIRoot::setModalForm(0);
			}
			if (UIRoot::getTopMostForm() == this)
			{
				UIRoot::setTopMostForm(0);
			}
			delete m_btClose;
			delete m_btMaximize;
			delete m_btMinimize;
			delete m_btRestore;
			delete m_border;
		}
		void Form::ShowModal()
		{
			UIRoot::setModalForm(this);
			Show();
		}
		void Form::Show()
		{
			m_state = FWS_Normal;

			if (m_previousPosition!= Point::Zero)
				Position = m_previousPosition;
			if (m_previousSize!=Point::Zero)
				Size = m_previousSize;

			Visible = true;
			Focus();
		}
		void Form::Hide()
		{
			if (UIRoot::getModalForm() == this)
			{
				UIRoot::setModalForm(0);
			}
			Visible = false;
		}
		void Form::Close()
		{
			if (m_menu)
			{
				m_menu->Close();
			}

			if (UIRoot::getTopMostForm() == this)
				UIRoot::setTopMostForm(0);

			Hide();
			
			m_eClosed.Invoke(this);
		}
		void Form::Focus()
		{
			if (UIRoot::getTopMostForm() && UIRoot::getTopMostForm() != this && 
				UIRoot::getTopMostForm()->getMenu() && UIRoot::getTopMostForm()->getMenu()->getState() == MENU_Open)
			{
				return;
			}

			UIRoot::getForms().Remove(this);
			UIRoot::getForms().Insert(0, this);
			UIRoot::setTopMostForm(this);

		}
		void Form::Unfocus()
		{
			if (m_menu && m_menu->getState() != MENU_Closed)
			{
				m_menu->Close();
			}
			if (UIRoot::getTopMostForm() == this)
				UIRoot::setTopMostForm(0);
		}
		void Form::Minimize()
		{
			Point minPos;
			bool ret = UIRoot::GetMinimizedPosition(m_device, this, minPos);
			if (ret)
				m_minimizedPos = minPos;
			else
				return;


			if (m_state == FWS_Normal)
			{
				m_previousPosition = Position;
				m_previousSize = Size;
			}
			
			if (m_menu)
			{
				m_menu->Close();
				m_menu->Visible = false;
			}

			m_state = FWS_Minimized;
			m_isMinimizing = true;
		}
		void Form::Maximize()
		{
			if (m_state == FWS_Normal)
			{
				m_previousPosition = Position;
				m_previousSize = Size;
			}

			if (m_menu)
			{
				m_menu->Close();
			}

			Focus();

			Apoc3D::Math::Rectangle rect = UIRoot::GetMaximizedRect(m_device, this);
			m_maximizedPos.X = rect.X; m_maximizedPos.Y = rect.Y;

			m_maximumSize.X = rect.Width; m_maximumSize.Y = rect.Height;// UIRoot::GetMaximizedSize(m_device, this);
			m_state = FWS_Maximized;
		}
		void Form::Restore()
		{
			Focus();

			if (m_menu)
			{
				m_menu->Visible = true;
				m_menu->Close();
			}

			m_state = FWS_Normal;
		}

		void Form::Initialize(RenderDevice* device)
		{
			m_device = device;

			Apoc3D::Math::Rectangle rect = UIRoot::GetUIArea(device);
			m_maximumSize = Point(rect.Width, rect.Height);

			if (m_menu)
			{
				m_minimumSize.Y = 60;
				m_menu->setOwner(this);
				m_menu->Position = m_menuOffset;
				m_menu->Initialize(m_device);
			}

			m_border = new Border(m_borderStyle, m_skin);

			m_fontRef = m_skin->TitleTextFont;

			if (m_borderStyle != FBS_Pane)
				m_titleOffset.Y = (m_skin->FormTitle->Height - m_fontRef->getLineHeightInt())/2 - 1;

			m_titleOffset.X = m_skin->FormTitlePadding[StyleSkin::SI_Left];

			InitializeButtons(device);

			ControlContainer::Initialize(device);

			m_initialized = true;
		}

		void Form::InitializeButtons(RenderDevice* device)
		{
			m_btClose = new Button(Point(Size.X - m_skin->FormTitlePadding[StyleSkin::SI_Right] - m_skin->FormCBCloseNormal.Width, m_skin->FormTitlePadding[StyleSkin::SI_Top]), L"");
			m_btClose->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseNormal);
			m_btClose->MouseOverTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseHover);
			m_btClose->MouseDownTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBCloseDown);
			m_btClose->OverlayIcon = UIGraphic(m_skin->SkinTexture, m_skin->FormCBIconClose);
			m_btClose->setOwner(this);
			m_btClose->Initialize(device);
			m_btClose->eventRelease().Bind(this, &Form::btClose_Release);

			
			if (m_hasMinimizeButton)
			{
				m_btMinimize = new Button(Point(0,0),L"");
				//m_btMinimize->setNormalTexture(m_skin->FormMinimizeButton);
				m_btMinimize->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinNormal);
				m_btMinimize->MouseOverTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinHover);
				m_btMinimize->MouseDownTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMinDown);
				m_btMinimize->OverlayIcon = UIGraphic(m_skin->SkinTexture, m_skin->FormCBIconMin);
				m_btMinimize->setOwner(this);
				m_btMinimize->Initialize(device);
				m_btMinimize->eventRelease().Bind(this, &Form::btMinimize_Release);
			}
			if (m_hasMaximizeButton)
			{
				m_btMaximize = new Button(Point(0,0),L"");
				m_btMaximize->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxNormal);
				m_btMaximize->MouseOverTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxHover);
				m_btMaximize->MouseDownTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBMaxDown);
				m_btMaximize->OverlayIcon = UIGraphic(m_skin->SkinTexture, m_skin->FormCBIconMax);
				m_btMaximize->setOwner(this);
				m_btMaximize->Initialize(device);
				m_btMaximize->eventRelease().Bind(this, &Form::btMaximize_Release);
			}
			if (m_hasMinimizeButton || m_hasMaximizeButton)
			{
				m_btRestore = new Button(Point(0,0), L"");
				m_btRestore->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreNormal);
				m_btRestore->MouseOverTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreHover);
				m_btRestore->MouseDownTexture = UIGraphic(m_skin->SkinTexture, m_skin->FormCBRestoreDown);
				m_btRestore->OverlayIcon = UIGraphic(m_skin->SkinTexture, m_skin->FormCBIconRestore);
				m_btRestore->setOwner(this);
				m_btRestore->Initialize(device);
				m_btRestore->eventRelease().Bind(this, &Form::btRestore_Release);
			}
		}


		void Form::btClose_Release(Control* sender)
		{
			Close();
		}
		void Form::btMinimize_Release(Control* sender)
		{
			Minimize();
		}
		void Form::btMaximize_Release(Control* sender)
		{
			Maximize();
		}
		void Form::btRestore_Release(Control* sender)
		{
			Restore();
		}

		void Form::Update(const GameTime* const time)
		{
			if (!Visible)
				return;

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

			CheckDoubleClick(time);

			if (m_state != FWS_Minimized && !m_isMinimized)
			{
				if (Size.X < m_minimumSize.X)
					Size.X = m_minimumSize.X;
				if (Size.Y < m_minimumSize.Y)
					Size.Y = m_minimumSize.Y;
			}

			if (UIRoot::getTopMostForm() == this || (m_isMinimized && UIRoot::getActiveForm() == this))
			{
				if (m_menu && m_menu->Visible)
				{
					m_menu->Update(time);
				}

				bool blocked = false;
				if (m_btClose && m_borderStyle != FBS_Pane)
				{
					m_btClose->Update(time);
					blocked |= m_btClose->isMouseHover();
				}

				if (!blocked && m_btMaximize && ((m_hasMaximizeButton && !m_isMaximized)||(m_hasMinimizeButton && m_isMinimized)))
				{
					m_btMaximize->Update(time);
					blocked |= m_btMaximize->isMouseHover();
				}
				if (!blocked && m_btMinimize && ((m_hasMinimizeButton && !m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btMinimize->Update(time);
					blocked |= m_btMinimize->isMouseHover();
				}

				if (!blocked && m_btRestore && ((m_hasMinimizeButton && m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btRestore->Update(time);
					blocked |= m_btRestore->isMouseHover();
				}

				if (!m_menu || !m_menu->Visible || m_menu->getState() == MENU_Closed)
				{
					if (m_state == FWS_Normal || m_state == FWS_Maximized)
					{
						bool skip = false;
						for (int i=0;i<m_controls->getCount();i++)
						{
							if (m_controls->operator[](i)->IsOverriding())
							{
								m_controls->operator[](i)->Update(time);
								skip = true;
							}
						}
						if (!skip)
						{
							for (int i=0;i<m_controls->getCount();i++)
							{
								if (m_controls->operator[](i)->Enabled)
								{
									m_controls->operator[](i)->Update(time);
								}
							}
						}
					}
				}
			}
			UpdateActive();
			if (UIRoot::getActiveForm()!=this)
			{
				if (m_isResizeing) m_isResizeing = false;
				if (m_isDragging) m_isDragging = false;
			}
		}

		void Form::UpdateTopMost()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();	

			if (mouse->IsLeftPressedState() && getArea().Contains(mouse->GetCurrentPosition()) && !UIRoot::getActiveForm())
			{
				UIRoot::setActiveForm(this);
				Focus();
			}
		}

		void Form::UpdateActive()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (UIRoot::getActiveForm() == this && mouse->IsLeftReleasedState())
			{
				UIRoot::setActiveForm(0);
			}
		}

		void Form::UpdateState()
		{
			//Restore the window to its original size and position
			if (m_state == FWS_Normal && (m_isMaximized || m_isMinimized))
			{
				if (Vector2::Distance(
					Vector2((float)Position.X, (float)Position.Y), 
					Vector2((float)m_previousPosition.X, (float)m_previousPosition.Y))>2.0f)
				{
					int dx = (int)((m_previousPosition.X-Position.X)*0.2f);
					if (!dx) dx = m_previousPosition.X-Position.X;
					Position.X += dx;

					int dy = (int)((m_previousPosition.Y-Position.Y)*0.2f);
					if (!dy) dy = m_previousPosition.Y-Position.Y;
					Position.Y += dy;
				}
				else
				{
					Position = m_previousPosition;
				}

				if (Vector2::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_previousPosition.X, (float)m_previousPosition.Y))>2.0f)
				{
					int dx = (int)((m_previousSize.X-Size.X)*0.2f);
					if (!dx) dx = m_previousSize.X-Size.X;
					Size.X += dx;

					int dy = (int)((m_previousSize.Y-Size.Y)*0.2f);
					if (!dy) dy = m_previousSize.Y-Size.Y;
					Size.Y += dy;
				}
				else
				{
					Size = m_previousSize;
				}

				if (Position == m_previousPosition && Size == m_previousSize)
				{
					m_isMaximized = false;
					m_isMinimized = false;

					Focus();
					
					m_eResized.Invoke(this);
				}
			}
			 //Minimize the window
			else if (m_state == FWS_Minimized && !m_isMinimized)
			{
				if (Vector2::Distance(
					Vector2((float)Position.X,(float)Position.Y),
					Vector2((float)m_minimizedPos.X, (float)m_minimizedPos.Y))>2.0f)
				{
					int dx = (int)((m_minimizedPos.X-Position.X)*0.2f);
					if (!dx) dx = m_minimizedPos.X-Position.X;
					Position.X += dx;

					int dy = (int)((m_minimizedPos.Y-Position.Y)*0.2f);
					if (!dy) dy = m_minimizedPos.Y-Position.Y;
					Position.Y += dy;
				}
				else
				{
					Position = m_minimizedPos;
				}

				if (Vector2::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_minimizedSize.X, (float)m_minimizedSize.Y))>2.0f)
				{
					int dx = (int)((m_minimizedSize.X-Size.X)*0.2f);
					if (!dx) dx = m_minimizedSize.X-Size.X;
					Size.X += dx;

					int dy = (int)((m_minimizedSize.Y-Size.Y)*0.2f);
					if (!dy) dy = m_minimizedSize.Y-Size.Y;
					Size.Y += dy;
				}
				else
				{
					Size = m_minimizedSize;
				}

				if (Position == m_minimizedPos && Size == m_minimizedSize)
				{
					m_isMinimized = true;
					m_isMaximized = false;
					m_isMinimizing = false;
					
					m_eResized.Invoke(this);
				}
			}
			//Maximize the window
			else if (m_state == FWS_Maximized && !m_isMaximized)
			{
				if (Vector2::Distance(
					Vector2((float)m_maximizedPos.X, (float)m_maximizedPos.Y), 
					Vector2((float)Position.X, (float)Position.Y))>2.0f)
				{
					int dx = (int)((m_maximizedPos.X-Position.X)*0.2f);
					if (!dx) dx = m_maximizedPos.X-Position.X;
					Position.X += dx;

					int dy = (int)((m_maximizedPos.Y-Position.Y)*0.2f);
					if (!dy) dy = m_maximizedPos.Y-Position.Y;
					Position.Y += dy;
				}
				else
				{
					Position = m_maximizedPos;
				}
				
				if (Vector2::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_maximumSize.X, (float)m_maximumSize.Y))>2.0f)
				{
					int dx = (int)((m_maximumSize.X-Size.X)*0.2f);
					if (!dx) dx = m_maximumSize.X-Size.X;
					Size.X += dx;

					int dy = (int)((m_maximumSize.Y-Size.Y)*0.2f);
					if (!dy) dy = m_maximumSize.Y-Size.Y;
					Size.Y += dy;
				}
				else
				{
					Size = m_maximumSize;
				}

				if (Position == m_maximizedPos && Size == m_maximumSize)
				{
					m_isMinimized = false;
					m_isMaximized = true;
					
					m_eResized.Invoke(this);
				}
			}
		}
		void Form::CheckDragging()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_dragArea.X = Position.X + m_skin->FormTitle[0].Width;
			m_dragArea.Y = Position.Y;
			m_dragArea.Width = Size.X - m_skin->FormCBCloseNormal.Width - m_skin->FormTitlePadding[StyleSkin::SI_Right];
			if (m_hasMinimizeButton)
				m_dragArea.Width -= m_skin->FormCBMinNormal.Width;
			if (m_hasMaximizeButton)
				m_dragArea.Width -= m_skin->FormCBMaxNormal.Width;
			m_dragArea.Height = m_skin->FormTitle[0].Height;

			if (m_dragArea.Contains(mouse->GetCurrentPosition()) &&
				mouse->IsLeftPressed() && UIRoot::getActiveForm()==this)
			{
				m_isDragging = true;
				Focus();
				m_posOffset.X = mouse->GetCurrentPosition().X - Position.X;
				m_posOffset.Y = mouse->GetCurrentPosition().Y - Position.Y;
			}

			if (m_isDragging)
			{
				Point dif(mouse->GetCurrentPosition().X - m_posOffset.X - Position.X, 
					mouse->GetCurrentPosition().Y - m_posOffset.Y - Position.Y);

				dif = UIRoot::ClampFormMovementOffset(this, dif);
				Position.X += dif.X;
				Position.Y += dif.Y;

				if (mouse->IsLeftReleasedState())
					m_isDragging = false;
			}
			// snapping?
		}

		void Form::CheckDoubleClick(const GameTime* const time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (m_state != FWS_Normal)
			{
				m_dragArea.X = Position.X + 7;
				m_dragArea.Y = Position.Y;

				m_dragArea.Width = Size.X - 29;
				if (m_hasMinimizeButton)
					m_dragArea.Width -= 15;
				if (m_hasMaximizeButton)
					m_dragArea.Width -= 15;
				m_dragArea.Height = 20;
			}

			if (m_borderStyle == FBS_Sizable && m_dragArea.Contains(mouse->GetCurrentPosition()))
			{
				if (mouse->IsLeftPressed())
				{
					if (UIRoot::getActiveForm() != this)
						UIRoot::setActiveForm(this);

					if (UIRoot::getActiveForm()==this)
					{
						if (time->getTotalTime() - m_lastClickTime < 0.2f)
						{
							ToggleWindowState();
						}

						m_lastClickTime = time->getTotalTime();
					}
				}
			}
		}

		void Form::CheckResize()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_resizeArea.X = Position.X + Size.X - m_resizeArea.Width;
			m_resizeArea.Y = Position.Y + Size.Y - m_resizeArea.Height;

			if (m_resizeArea.Contains(mouse->GetCurrentPosition()) &&
				UIRoot::getTopMostForm()==this)
			{
				if (m_isInReiszeArea)
				{
					m_isInReiszeArea = true;
				}

				if (mouse->IsLeftPressed() &&
					UIRoot::getActiveForm() == this)
				{
					m_isResizeing = true;
					Focus();
					m_posOffset.X = mouse->GetCurrentPosition().X;
					m_posOffset.Y = mouse->GetCurrentPosition().Y;
					m_oldSize = Size;
				}
			}
			else if (m_isInReiszeArea && !m_isResizeing)
			{
				m_isInReiszeArea = false;
			}

			if (m_isResizeing)
			{
				Point dif(
					m_oldSize.X + mouse->GetCurrentPosition().X - m_posOffset.X - Size.X, 
					m_oldSize.Y + mouse->GetCurrentPosition().Y - m_posOffset.Y - Size.Y);

				dif = UIRoot::ClampFormMovementOffset(this, dif);

				Size.X += dif.X;
				Size.Y += dif.Y;

				if ((dif.X !=0 || dif.Y !=0))
				{
					m_eResized.Invoke(this);
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

			Apoc3D::Math::Rectangle uiArea = UIRoot::GetUIArea(m_device);

			m_borderAlpha = 0.3f - UIRoot::getForms().IndexOf(this) * 0.04f;
			m_border->Draw(sprite, Position, Size, m_borderAlpha);

			DrawTitle(sprite);
			DrawButtons(sprite);

			if (m_state != FWS_Minimized)
			{
				sprite->Flush();

				Matrix matrix;
				Matrix::CreateTranslation(matrix, (float)Position.X, (float)Position.Y,0);
				sprite->MultiplyTransform(matrix);
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

				int overlay = 0;
				for (int i=0;i<m_controls->getCount();i++)
				{
					Control* ctl = m_controls->operator[](i);
					if (ctl->IsOverriding())
					{
						overlay = i;
					}
					if (ctl->Visible)
					{
						ctl->Draw(sprite);
					}
				}

				sprite->Flush();
				m_device->getRenderState()->setScissorTest(false,0);
				if (overlay)
				{
					m_controls->operator[](overlay)->DrawOverlay(sprite);
				}

				if (m_menu && m_menu->Visible)
					m_menu->Draw(sprite);

				if(sprite->isUsingStack())
				{
					sprite->PopTransform();
				}
				else
				{
					sprite->SetTransform(Matrix::Identity);
				}
			}
		}
		void Form::DrawButtons(Sprite* sprite)
		{
			Matrix matrix;
			Matrix::CreateTranslation(matrix, (float)Position.X, (float)Position.Y,0);

			sprite->MultiplyTransform(matrix);
			if (m_borderStyle != FBS_None && m_borderStyle != FBS_Pane)
			{
				m_btClose->Position.X = Size.X - 22;
				m_btClose->Position.Y = 4;
				m_btClose->Draw(sprite);
			}

			if (m_borderStyle == FBS_Sizable)
			{
				
				if (m_hasMaximizeButton)
				{					
					if (m_state != FWS_Maximized)
					{
						m_btMaximize->Position.X = Size.X - 37;
						m_btMaximize->Position.Y = 4;
						
						m_btMaximize->Draw(sprite);
					}
					else
					{
						m_btRestore->Position.X = Size.X - 37;
						m_btRestore->Position.Y = 4;
						m_btRestore->Draw(sprite);
					}

					if (m_hasMinimizeButton)
					{
						if (m_state != FWS_Minimized)
						{
							m_btMinimize->Position.X = Size.X - 52;
							m_btMinimize->Position.Y = 4;
							m_btMinimize->Draw(sprite);
						}
						else
						{
							m_btRestore->Position.X = Size.X - 52;
							m_btRestore->Position.Y = 4;
							m_btRestore->Draw(sprite);
						}
					}
				}
				else if (m_hasMinimizeButton)
				{
					if (m_state != FWS_Minimized)
					{
						m_btMinimize->Position.X = Size.X - 37;
						m_btMinimize->Position.Y = 4;
						m_btMinimize->Draw(sprite);
					}
					else
					{
						m_btRestore->Position.X = Size.X - 37;
						m_btRestore->Position.Y = 4;
						m_btRestore->Draw(sprite);
					}
				}
			}
			if(sprite->isUsingStack())
			{
				sprite->PopTransform();
			}
			else
			{
				sprite->SetTransform(Matrix::Identity);
			}

		}
		void Form::DrawTitle(Sprite* sprite)
		{
			Point size = m_fontRef->MeasureString(m_title);
			int32 padding = m_skin->FormTitlePadding[StyleSkin::SI_Right] + m_skin->FormTitlePadding[StyleSkin::SI_Left];
			padding += m_skin->FormCBCloseNormal.Width;
			if (m_hasMinimizeButton)
				padding += m_skin->FormCBMinNormal.Width;
			if (m_hasMaximizeButton)
				padding += m_skin->FormCBMaxNormal.Width;
			
			if (size.X >= Size.X-padding)
			{
				padding += m_fontRef->MeasureString(L"..").X;

				for (size_t i=0;i<m_title.size()+1;i++)
				{
					String subStr = m_title.substr(0,i);
					size = m_fontRef->MeasureString(subStr);
					if (size.X >= Size.X-padding)
					{
						Point pos = Position;
						pos.X += m_titleOffset.X;
						pos.Y += m_titleOffset.Y;
						m_fontRef->DrawString(sprite, subStr + L"..", pos, m_skin->TextColor);
						break;
					}
				}
			}
			else
			{
				Point pos = Position;
				pos.X += m_titleOffset.X;
				pos.Y += m_titleOffset.Y;
				m_fontRef->DrawString(sprite, m_title, pos, m_skin->TextColor);
			}

		}

		/************************************************************************/
		/*  Border                                                              */
		/************************************************************************/


		Border::Border(BorderStyle style, const StyleSkin* skin)
			: m_skin(skin), m_style(style)
		{
			for (int i=0;i<9;i++)
			{
				//Texture* tex;
				if (m_style == FBS_Pane)
				{
					//tex = m_skin->WhitePixelTexture;
					m_dstRect[i] = Apoc3D::Math::Rectangle(0,0,1,1);
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
				m_dstRect[3].Height = size.Y - (m_dstRect[0].Height + m_skin->WhitePixelTexture->getHeight());

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
				if (size.Y > m_dstRect[0].Height + m_skin->WhitePixelTexture->getHeight())
				{
					m_dstRect[6].Height = m_skin->WhitePixelTexture->getHeight();
				}
				else
				{
					m_dstRect[6].Y = m_dstRect[0].Y + m_dstRect[0].Height;
					m_dstRect[6].Height = size.Y - m_skin->WhitePixelTexture->getHeight();
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
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[0], m_skin->BorderColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[1], m_skin->BorderColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[2], m_skin->BorderColor);
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
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[3], m_skin->BorderColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[4], m_skin->ControlFaceColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[5], m_skin->BorderColor);
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
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[6], m_skin->BorderColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[7], m_skin->BorderColor);
				sprite->Draw(m_skin->WhitePixelTexture, m_dstRect[8], m_skin->BorderColor);
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
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		FastList<Form*> UIRoot::m_forms(10);
		FastList<ControlContainer*> UIRoot::m_containers(10);
		Form* UIRoot::m_activeForm = 0;
		Form* UIRoot::m_topMostForm = 0;
		RectangleF UIRoot::UIArea(0,0,1,1);
		RectangleF UIRoot::MaximizedArea(0,0,1,1);
		SubMenu* UIRoot::m_contextMenu = 0;
		Sprite* UIRoot::m_sprite = 0;
		Menu* UIRoot::m_mainMenu = 0;
		Form* UIRoot::m_modalForm = 0;
		int UIRoot::m_modalAnim = 0;

		Apoc3D::Math::Rectangle UIRoot::GetUIArea(RenderDevice* device)
		{
			Viewport vp = device->getViewport();

			Apoc3D::Math::Rectangle rect;
			rect.X = (int)(UIArea.X * vp.Width);
			rect.Y = (int)(UIArea.Y * vp.Height);
			rect.Width = (int)(UIArea.Width * vp.Width);
			rect.Height = (int)(UIArea.Height * vp.Height);
			return rect;
		}

		bool UIRoot::GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos)
		{
			for (int i=0;i<m_forms.getCount();i++)
			{
				if (m_forms[i] != form && m_forms[i]->isMinimizing())
				{
					return false;
				}
			}

			Apoc3D::Math::Rectangle rect = GetUIArea(dev);

			for (int y = rect.Height -20;y>0;y-=20)
			{
				for (int x=0;x<rect.Width-99;x+=100)
				{
					bool isOccupied = false;
					for (int i=0;i<m_forms.getCount();i++)
					{
						if (m_forms[i] != form && m_forms[i]->Visible &&
							m_forms[i]->Position.X == x && m_forms[i]->Position.Y == y)
						{
							isOccupied = true;
							break;
						}
					}

					if (!isOccupied)
					{
						pos = Point(x,y);
						return true;
					}
				}
			}
			pos = Point::Zero;
			return true;
		}

		Apoc3D::Math::Rectangle UIRoot::GetMaximizedRect(RenderDevice* dev, Form* form)
		{
			Point size = GetMaximizedSize(dev, form);

			if (m_mainMenu)
			{
				size.Y -= m_mainMenu->Size.Y;
				Apoc3D::Math::Rectangle result(0, m_mainMenu->Size.Y, size.X, size.Y);
				return result;
			}
			else
			{
				Apoc3D::Math::Rectangle result(0,0,size.X,size.Y);
				return result;
			}
		}
		Point UIRoot::GetMaximizedSize(RenderDevice* dev, Form* form)
		{
			Viewport vp = dev->getViewport();

			Apoc3D::Math::Rectangle rect;
			rect.X = (int)(UIArea.X * MaximizedArea.X * vp.Width);
			rect.Y = (int)(UIArea.Y * MaximizedArea.Y * vp.Height);
			rect.Width = (int)(UIArea.Width * MaximizedArea.Width * vp.Width);
			rect.Height = (int)(UIArea.Height * MaximizedArea.Height * vp.Height);

			for (int i=0;i<m_forms.getCount();i++)
			{
				if (m_forms[i] != form && m_forms[i]->Visible && m_forms[i]->getState() == Form::FWS_Minimized)
					if (m_forms[i]->Position.Y < rect.getBottom())
						rect.Height = m_forms[i]->Position.Y - rect.Y;
			}

			return Point(rect.Width, rect.Height);
		}
		bool UIRoot::IsObstructed(Control* control, const Point& point)
		{
			for (int i=0;i<m_forms.getCount();i++)
			{
				Apoc3D::Math::Rectangle area = control->getArea();
				if (control->getOwner() && m_forms[i] != control->getOwner())
				{
					if (m_forms[i]->getArea().Contains(area) || m_forms[i]->getArea().Intersects(area))
					{
						if (m_forms[i]->getArea().Contains(point))
						{
							return true;
						}
					}
				}
				else if (!control->getOwner())
				{
					if (m_forms[i]->getArea().Contains(area) || m_forms[i]->getArea().Intersects(area))
					{
						if (m_forms[i]->getArea().Contains(point))
							return true;
					}
				}
			}
			return false;
		}
		
		Point UIRoot::ClampFormMovementOffset(Form* frm, const Point& vec)
		{
			if (frm != m_modalForm)
			{
				RenderDevice* dev = frm->getRenderDevice();

				Apoc3D::Math::Rectangle rect = GetMaximizedRect(dev, frm);
				Apoc3D::Math::Rectangle formArea = frm->getAbsoluteArea();
				formArea.Width = 80;
				formArea.Height = 20;

				Point result = vec;
				
				if (formArea.X + result.X < rect.X)
					result.X = rect.X - formArea.X;

				if (formArea.Y + result.Y < rect.Y)
					result.Y = rect.Y - formArea.Y;

				if (formArea.getRight() + result.X > rect.getRight())
					result.X = rect.getRight() - formArea.getRight();

				if (formArea.getBottom() + result.Y > rect.getBottom())
					result.Y = rect.getBottom() - formArea.getBottom();

				//Apoc3D::Math::Rectangle formArea(newPos.X, newPos.Y, newSize.X, newSize.Y);

				return result;
			}
			return vec;
		}
		void UIRoot::Form_SizeChanged(Control* ctl)
		{
			Form* form = static_cast<Form*>(ctl);

			Apoc3D::Math::Rectangle area = GetUIArea(form->getRenderDevice());
			//for (int i=0;i<m_forms.getCount();i++)
			{
				//Form* form = m_forms[i];
				//If a form is out of the working area,
				//we need to put it back where the user can see it.
				if (!area.Contains(form->getArea()))
				{
					if (form->Position.X + form->Size.X > area.Width)
						form->Position.X = area.Width - form->Size.X;

					if (form->Position.X + form->Size.Y > area.Height)
						form->Position.Y = area.Height - form->Size.Y;
				}

				//If a form was maximized
				if (form->getState() == Form::FWS_Maximized)
				{
					//resize it
					form->Size.X = area.Width;
					form->Size.Y = area.Height;
				}
			}
		}



		void UIRoot::Initialize(RenderDevice* device)
		{
			ObjectFactory* fac = device->getObjectFactory();
			m_sprite = fac->CreateSprite();

		}
		void UIRoot::Finalize()
		{
			delete m_sprite;
		}
		void UIRoot::Draw()
		{
			FontManager::getSingleton().StartFrame();

			m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_AlphaBlended | Sprite::SPR_UsePostTransformStack | Sprite::SPR_RestoreState));
			
			// first background forms
			for (int i=m_forms.getCount()-1;i>-1;i--)
			{
				Form* frm = m_forms[i];
				if (frm->Visible && 
					frm->getState() != Form::FWS_Minimized && frm->IsBackgroundForm)
				{
					frm->Draw(m_sprite);
				}
			}


			// regular forms
			for (int i=m_forms.getCount()-1;i>-1;i--)
			{
				Form* frm = m_forms[i];
				if (frm->Visible && frm != m_topMostForm && 
					frm->getState() != Form::FWS_Minimized && !frm->IsBackgroundForm)
				{
					frm->Draw(m_sprite);
				}
			}

			if (m_topMostForm && m_topMostForm->Visible && !m_topMostForm->IsBackgroundForm)
			{
				m_topMostForm->Draw(m_sprite);
			}

			// minimized always last
			for (int i=m_forms.getCount()-1;i>=0;i--)
			{
				Form* frm = m_forms[i];
				if (frm->getState() == Form::FWS_Minimized &&
					frm->Visible && frm != m_topMostForm)
				{
					frm->Draw(m_sprite);
				}
			}

			if (m_contextMenu && m_contextMenu->Visible && m_contextMenu->getState() != MENU_Closed)
			{
				m_contextMenu->Draw(m_sprite);
			}
			if (m_mainMenu && m_mainMenu->Visible)
			{
				m_mainMenu->Draw(m_sprite);
			}

			if (m_modalForm)
			{
				const StyleSkin* skin = m_modalForm->getSkin();
				if (m_modalAnim>3)
				{
					m_sprite->Draw(skin->WhitePixelTexture, GetUIArea(m_sprite->getRenderDevice()),0, PACK_COLOR(0,0,0, 120));
				}
				else if (m_modalAnim)
				{
					m_sprite->Draw(skin->WhitePixelTexture, GetUIArea(m_sprite->getRenderDevice()),0, PACK_COLOR(0,0,0, 200));
				}

				m_modalForm->Draw(m_sprite);
			}
			

			// cursor
			m_sprite->End();
		}
		void UIRoot::Update(const GameTime* const time)
		{
			if (m_modalForm)
			{
				m_modalForm->Update(time);
				m_modalAnim++;
				return;
			}

			m_modalAnim = 0;
			bool menuOverriden = false;
			if (m_mainMenu)
			{
				for (int i=0;i<m_mainMenu->getItems().getCount();i++)
				{
					if (m_mainMenu->getItems()[i]->getSubMenu() && m_mainMenu->getItems()[i]->getSubMenu()->getState() == MENU_Open)
					{
						menuOverriden = true;
					}
				}
			}
			
			if (!menuOverriden)
			{
				Form* alreadyUpdatedForm = nullptr;
				if (m_activeForm)
				{
					alreadyUpdatedForm = m_activeForm;
					m_activeForm->Update(time);
				}
				else if (m_topMostForm && !m_topMostForm->IsBackgroundForm)
				{
					alreadyUpdatedForm = m_topMostForm;
					m_topMostForm->Update(time);
				}
				//else if (!m_topMostForm && m_forms.getCount())
				//{
				//	m_topMostForm = m_forms[0];
				//	m_topMostForm->Focus();
				//}

				for (int i=0;i<m_forms.getCount();i++)
				{
					Form* frm = m_forms[i];
					if (frm->Enabled && frm != alreadyUpdatedForm)
					{
						frm->Update(time);
					}
				}

				Mouse* mouse = InputAPIManager::getSingleton().getMouse();
				if (mouse->IsLeftPressedState())
				{
					if (!m_activeForm && m_topMostForm)
					{
						// empty selection
						m_topMostForm->Unfocus();
					}
					if (m_topMostForm && m_topMostForm->IsBackgroundForm && m_topMostForm != m_activeForm)
					{
						// deselsect background form
						m_topMostForm->Unfocus();
						m_activeForm->Focus();
					}
				}
			}
			

			//Update Context Menu
			if (m_contextMenu && m_contextMenu->getState() != MENU_Closed &&
				m_contextMenu->Visible)
			{
				m_contextMenu->Update(time);
			}
			if (m_mainMenu && m_mainMenu->Visible)
			{
				m_mainMenu->Update(time);
			}
		}
	

		void UIRoot::Add(ControlContainer* cc)
		{
			Form* form = dynamic_cast<Form*>(cc);
			if (form)
			{
				m_forms.Insert(0, form);
			}
			else
			{
				m_containers.Add(cc);
			}
		}
		void UIRoot::Remove(ControlContainer* cc)
		{
			Form* form = dynamic_cast<Form*>(cc);
			if (form)
			{
				m_forms.Remove(form);
			}
			else
			{
				m_containers.Remove(cc);
			}
		}
		void UIRoot::RemoveForm(const String& name)
		{
			for (int i=0;i<m_forms.getCount();i++)
			{
				if (m_forms[i]->Name == name)
				{
					m_forms.RemoveAt(i);
					break;
				}
			}
		}
		void UIRoot::RemoveContainer(const String& name)
		{
			for (int i=0;i<m_containers.getCount();i++)
			{
				if (m_containers[i]->Name == name)
				{
					m_containers.RemoveAt(i);
					break;
				}
			}
		}

	}
}