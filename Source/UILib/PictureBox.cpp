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
#include "PictureBox.h"
#include "Utility/StringUtils.h"
#include "Input/InputAPI.h"
#include "Input/Mouse.h"
#include "FontManager.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "StyleSkin.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Input;


namespace Apoc3D
{
	namespace UI
	{
		void PictureBox::Initialize(RenderDevice* device)
		{
			if (m_texture)
			{
				if (Size == Point::Zero)
				{
					Size = Point(m_texture->getWidth(),m_texture->getHeight());
				}
				m_srcRect = Apoc3D::Math::Rectangle(0,0,m_texture->getWidth(),m_texture->getHeight());
			}
		}
		void PictureBox::Update(const GameTime* const time)
		{
			UpdateEvents();
		}
		void PictureBox::UpdateEvents()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Apoc3D::Math::Rectangle rect = getAbsoluteArea();
			if (rect.Contains(mouse->GetCurrentPosition()) && getOwner()->getAbsoluteArea().Contains(rect))
			{
				if (!m_mouseOver)
				{
					m_mouseOver = true;
					OnMouseOver();
				}

				if (mouse->IsLeftPressed())
				{
					OnPress();
				}
				else if (mouse->IsLeftUp())
				{
					OnRelease();
				}
			}
			else if (m_mouseOver)
			{
				m_mouseOver = false;
				OnMouseOut();
			}
		}
		void PictureBox::Draw(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle destRect(Position.X, Position.Y, Size.X, Size.Y);
			if (m_border)
			{
				Apoc3D::Math::Rectangle borderRect(destRect.X - m_border, 
					destRect.Y - m_border, Size.X+m_border*2, Size.Y+m_border*2);
				sprite->Draw(m_skin->WhitePixelTexture, borderRect, 0 , CV_Black);
			}
			if (m_texture)
			{
				sprite->Draw(m_texture, destRect, &m_srcRect, CV_White);
			}

			if (!m_eDraw.empty())
				m_eDraw(sprite, &destRect);
		}
	}
}