/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Graphics/TextureManager.h"
#include "Input/Mouse.h"
#include "Input/InputAPI.h"
#include "Core/GameTime.h"
#include "FontManager.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{

		Form::Form(BorderStyle border, const String& title)
			: m_titleOffset(12,2), m_minimumSize(100, 40), m_minimizedSize(100, 20),
			m_maximumSize(0,0), m_isMinimized(false), m_isMaximized(false),
			m_previousPosition(0,0), m_previousSize(0,0), m_minimizedPos(0,0), 
			m_hasMinimizeButton(true), m_hasMaximizeButton(true), 
			m_dragArea(0,0,0,0), m_resizeArea(0,0,15,15), m_isDragging(false), m_isResizeing(false),
			m_isMinimizing(false), m_isInReiszeArea(false), m_posOffset(0,0), m_oldSize(0,0), m_initialized(false), m_lastClickTime(0),
			m_borderStyle(border), m_state(FWS_Normal), m_title(title),
			m_borderAlpha(1), m_border(0)
		{
			Size = Point(200,100);
			Visible = false;
		}

		Form::~Form()
		{
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
				m_previousSize = Position;
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

			m_maximumSize = UIRoot::GetMaximizedSize(m_device, this);
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

			m_border = new Border(m_borderStyle == FBS_Sizable, m_skin);

			InitializeButtons(device);

			ControlContainer::Initialize(device);

			m_initialized = true;
		}

		void Form::InitializeButtons(RenderDevice* device)
		{
			m_btClose = new Button(Point(Size.X = 22, 4), L"");
			m_btClose->setNormalTexture(m_skin->FormCloseButton);
			m_btClose->setOwner(this);
			m_btClose->Initialize(device);
			m_btClose->eventRelease().bind(this, &Form::btClose_Release);

			
			if (m_hasMinimizeButton)
			{
				m_btMinimize = new Button(Point(0,0),L"");
				m_btMinimize->setNormalTexture(m_skin->FormMinimizeButton);
				m_btMinimize->setOwner(this);
				m_btMinimize->Initialize(device);

				m_btMinimize->eventRelease().bind(this, &Form::btMinimize_Release);
			}
			if (m_hasMaximizeButton)
			{
				m_btMaximize = new Button(Point(0,0),L"");
				m_btMaximize->setNormalTexture(m_skin->FormMaximizeButton);
				m_btMaximize->setOwner(this);
				m_btMaximize->Initialize(device);
				m_btMaximize->eventRelease().bind(this, &Form::btMaximize_Release);
			}
			if (m_hasMinimizeButton || m_hasMaximizeButton)
			{
				m_btRestore = new Button(Point(0,0), L"");
				m_btRestore->setNormalTexture(m_skin->FormRestoreButton);
				m_btRestore->setOwner(this);
				m_btRestore->Initialize(device);
				m_btRestore->eventRelease().bind(this, &Form::btRestore_Release);
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
			UpdateState();
			UpdateTopMost();

			if (m_state == FWS_Normal && !m_isMaximized && !m_isMinimized)
			{
				if (!m_isResizeing)
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

			if (UIRoot::getTopMostForm() == this && (m_isMinimized && UIRoot::getActiveForm() == this))
			{
				if (m_menu && m_menu->Visible)
				{
					m_menu->Update(time);
				}

				if (m_btClose)
				{
					m_btClose->Update(time);
				}

				if (m_btMaximize && ((m_hasMaximizeButton && !m_isMaximized)||(m_hasMinimizeButton && m_isMinimized)))
				{
					m_btMaximize->Update(time);
				}
				if (m_btMinimize && ((m_hasMinimizeButton && !m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btMinimize->Update(time);
				}

				if (m_btRestore && ((m_hasMinimizeButton && m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btRestore->Update(time);
				}

				if (!m_menu || !m_menu->Visible || m_menu->getState() == FWS_Maximized)
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
				if (Vector2Utils::Distance(
					Vector2((float)Position.X, (float)Position.Y), 
					Vector2((float)m_previousPosition.X, (float)m_previousPosition.Y))>2.0f)
				{
					Position.X += (int)((m_previousPosition.X-Position.X)*0.2f);
					Position.Y += (int)((m_previousPosition.Y-Position.Y)*0.2f);
				}
				else
				{
					Position = m_previousPosition;
				}

				if (Vector2Utils::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_previousPosition.X, (float)m_previousPosition.Y))>2.0f)
				{
					Size.X += (int)((m_previousSize.X-Size.X)*0.2f);
					Size.Y += (int)((m_previousSize.Y-Size.Y)*0.2f);
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
					if (!m_eResized.empty())
					{
						m_eResized(this);
					}
				}
			}
			 //Minimize the window
			else if (m_state == FWS_Minimized && !m_isMinimized)
			{
				if (Vector2Utils::Distance(
					Vector2((float)Position.X,(float)Position.Y),
					Vector2((float)m_minimizedPos.X, (float)m_minimizedPos.Y))>2.0f)
				{
					Position.X += (int)((m_minimizedPos.X-Position.X)*0.2f);
					Position.Y += (int)((m_minimizedPos.Y-Position.Y)*0.2f);
				}
				else
				{
					Position = m_minimizedPos;
				}

				if (Vector2Utils::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_minimizedSize.X, (float)m_minimizedSize.Y))>2.0f)
				{
					Size.X += (int)((m_minimizedSize.X-Size.X)*0.2f);
					Size.Y += (int)((m_minimizedSize.Y-Size.Y)*0.2f);
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
					if (!m_eResized.empty())
					{
						m_eResized(this);
					}
				}
			}
			//Maximize the window
			else if (m_state == FWS_Maximized && !m_isMaximized)
			{
				if (Vector2Utils::Distance(
					Vector2Utils::Zero, 
					Vector2((float)Position.X, (float)Position.Y))>2.0f)
				{
					Position.X += (int)((-Position.X)*0.2f);
					Position.Y += (int)((-Position.Y)*0.2f);
				}
				else
				{
					Position = Point::Zero;
				}
				
				if (Vector2Utils::Distance(
					Vector2((float)Size.X,(float)Size.Y), 
					Vector2((float)m_maximumSize.X, (float)m_maximumSize.Y))>2.0f)
				{
					Size.X += (int)((m_maximumSize.X-Size.X)*0.2f);
					Size.Y += (int)((m_maximumSize.Y-Size.Y)*0.2f);
				}
				else
				{
					Size = m_maximumSize;
				}

				if (Position == Point::Zero && Size == m_maximumSize)
				{
					m_isMinimized = false;
					m_isMaximized = true;
					
					if (!m_eResized.empty())
					{
						m_eResized(this);
					}
				}
			}
		}
		void Form::CheckDragging()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_dragArea.X = Position.X + 7;
			m_dragArea.Y = Position.Y;
			m_dragArea.Width = Size.X - 29;
			if (m_hasMinimizeButton)
				m_dragArea.Width -= 15;
			if (m_hasMaximizeButton)
				m_dragArea.Width -= 15;
			m_dragArea.Height = 20;

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
				Position.X = mouse->GetCurrentPosition().X - m_posOffset.X;
				Position.Y = mouse->GetCurrentPosition().Y - m_posOffset.Y;
				
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
					if (!UIRoot::getActiveForm())
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
				Size.X = m_oldSize.X + mouse->GetCurrentPosition().X - m_posOffset.X;
				Size.Y = m_oldSize.Y + mouse->GetCurrentPosition().Y - m_posOffset.Y;

				if (!m_eResized.empty())
				{
					m_eResized(this);
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

			m_borderAlpha = 0.3f - UIRoot::getForms().IndexOf(this) * 0.04f;
			m_border->Draw(sprite, Position, Size, m_borderAlpha);

			DrawTitle(sprite);
			DrawButtons(sprite);

			int overlay = 0;
			for (int i=0;i<m_controls->getCount();i++)
			{
				if (m_controls->operator[](i)->IsOverriding())
				{
					overlay = i;
				}
				if (m_controls->operator[](i)->Enabled)
				{
					m_controls->operator[](i)->Draw(sprite);
				}
			}
			if (overlay)
			{

			}

			
		}
		void Form::DrawButtons(Sprite* sprite)
		{
			if (m_borderStyle != FBS_None)
			{
				m_btClose->Position.X = Size.X - 22;
				m_btClose->Position.Y = 4;
				m_btClose->Draw(sprite);
			}

			if (m_borderStyle == FBS_Sizable)
			{
				Matrix matrix;
				Matrix::CreateTranslation(matrix, (float)Position.X, (float)Position.Y,0);
				sprite->SetTransform(matrix);

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

				sprite->SetTransform(Matrix::Identity);
			}
		}
		void Form::DrawTitle(Sprite* sprite)
		{
			Point size = m_fontRef->MeasureString(m_title);
			if (size.X >= Size.X-75)
			{
				for (size_t i=0;i<m_title.size()+1;i++)
				{
					String subStr = m_title.substr(0,i);
					size = m_fontRef->MeasureString(subStr);
					if (size.X >= Size.X-75)
					{
						Point pos = Position;
						pos.X += m_titleOffset.X;
						pos.Y += m_titleOffset.Y;
						m_fontRef->DrawString(sprite, subStr + L"..", pos, CV_Black);
						break;
					}
				}
			}
			else
			{
				Point pos = Position;
				pos.X += m_titleOffset.X;
				pos.Y += m_titleOffset.Y;
				m_fontRef->DrawString(sprite, m_title, pos, CV_Black);
			}

		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		Border::Border(bool resizable, const StyleSkin* skin)
			: m_skin(skin), m_resizable(resizable), m_shadowOffset(6,4)
		{
			for (int i=0;i<9;i++)
			{
				m_dstRect[i] = Apoc3D::Math::Rectangle(0,0,m_skin->FormBorderTexture[i]->getWidth(), m_skin->FormBorderTexture[i]->getHeight());
			}
			if (resizable)
			{
				m_dstRect[8] = Apoc3D::Math::Rectangle(0,0,m_skin->FormBorderTexture[9]->getWidth(), m_skin->FormBorderTexture[9]->getHeight());
			}
		}

		void Border::Draw(Sprite* sprite, const Point& pt, const Point& size, float shadowAlpha)
		{
			UpdateRects(pt, size);

			if (size.Y > m_dstRect[0].Height)
			{
				Point shdPos = pt;
				shdPos.Y += 15;
				DrawShadow(sprite, shdPos, shadowAlpha);
			}

			DrawUpper(sprite);
			if (size.Y > m_dstRect[0].Height)
			{
				DrawMiddle(sprite);
				DrawLower(sprite);
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
			m_dstRect[3].X = m_dstRect[0].X;
			m_dstRect[3].Y = m_dstRect[0].Y + m_dstRect[0].Height;
			m_dstRect[3].Height = size.Y - (m_dstRect[0].Height + m_skin->FormBorderTexture[6]->getHeight());

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
			if (size.Y > m_dstRect[0].Height + m_skin->FormBorderTexture[6]->getHeight())
			{
				m_dstRect[6].Height = m_skin->FormBorderTexture[6]->getHeight();
			}
			else
			{
				m_dstRect[6].Y = m_dstRect[0].Y + m_dstRect[0].Height;
				m_dstRect[6].Height = size.Y - m_skin->FormBorderTexture[6]->getHeight();
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
			sprite->Draw(m_skin->FormBorderTexture[0], m_dstRect[0], CV_White);
			sprite->Draw(m_skin->FormBorderTexture[1], m_dstRect[1], CV_White);
			sprite->Draw(m_skin->FormBorderTexture[2], m_dstRect[2], CV_White);
		}
		void Border::DrawMiddle(Sprite* sprite)
		{
			sprite->Draw(m_skin->FormBorderTexture[3], m_dstRect[3], CV_White);
			sprite->Draw(m_skin->FormBorderTexture[4], m_dstRect[4], CV_White);
			sprite->Draw(m_skin->FormBorderTexture[5], m_dstRect[5], CV_White);
		}
		void Border::DrawLower(Sprite* sprite)
		{
			sprite->Draw(m_skin->FormBorderTexture[6], m_dstRect[6], CV_White);
			sprite->Draw(m_skin->FormBorderTexture[7], m_dstRect[7], CV_White);
			if (m_resizable)
				sprite->Draw(m_skin->FormBorderTexture[9], m_dstRect[9], CV_White);			
			else
				sprite->Draw(m_skin->FormBorderTexture[8], m_dstRect[8], CV_White);
		}
		void Border::DrawShadow(Sprite* sprite, const Point& pos, float alpha)
		{
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

			if (m_resizable)
			{
				shadowRect = m_dstRect[9];
				shadowRect.X += m_shadowOffset.X;
				shadowRect.Y += m_shadowOffset.Y;
				sprite->Draw(m_skin->FormBorderTexture[9], shadowRect, shdClr);
			}
			else
			{
				shadowRect = m_dstRect[8];
				shadowRect.X += m_shadowOffset.X;
				shadowRect.Y += m_shadowOffset.Y;
				sprite->Draw(m_skin->FormBorderTexture[8], shadowRect, shdClr);
			}

		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		FastList<Form*> UIRoot::m_forms(10);
		Form* UIRoot::m_activeForm = 0;
		Form* UIRoot::m_topMostForm = 0;
		RectangleF UIRoot::UIArea(0,0,1,1);

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

		Point UIRoot::GetMaximizedSize(RenderDevice* dev, Form* form)
		{
			Apoc3D::Math::Rectangle rect = GetUIArea(dev);

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
		void UIRoot::Form_SizeChanged(Control* ctl)
		{

		}

		void UIRoot::Initialize(RenderDevice* device)
		{
			
		}
		void UIRoot::Finalize()
		{

		}
		void UIRoot::Draw()
		{

		}
		void UIRoot::Update(const GameTime* const time)
		{

		}
	}
}