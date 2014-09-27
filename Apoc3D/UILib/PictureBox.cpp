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
#include "PictureBox.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "FontManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "StyleSkin.h"
#include "Form.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Input;


namespace Apoc3D
{
	namespace UI
	{
		PictureBox::PictureBox(const Point& position, int border)
			: PictureBox(nullptr, position, border, nullptr)
		{ }

		PictureBox::PictureBox(const Point& position, int border, Texture* texture)
			: PictureBox(nullptr, position, border, nullptr)
		{ }

		PictureBox::PictureBox(const StyleSkin* skin, const Point& position, int border)
			: PictureBox(nullptr, position, border, nullptr)
		{ }

		PictureBox::PictureBox(const StyleSkin* skin, const Point& position, int border, Texture* texture)
			: Control(skin, position), m_border(border)
		{
			if (texture)
			{
				m_size = Point(texture->getWidth(), texture->getHeight());

				Graphic = UIGraphicSimple(texture);
			}
		}

		PictureBox::~PictureBox()
		{

		}

		void PictureBox::Update(const GameTime* time)
		{
			UpdateEvents_StandardButton(m_mouseHover, m_mouseDown, getAbsoluteArea(),
				&PictureBox::OnMouseHover, &PictureBox::OnMouseOut, &PictureBox::OnPress, &PictureBox::OnRelease);
		}

		void PictureBox::Draw(Sprite* sprite)
		{
			RenderDevice* m_device = sprite->getRenderDevice();

			Apoc3D::Math::Rectangle destRect = getAbsoluteArea();
			if (m_border)
			{
				Apoc3D::Math::Rectangle borderRect = destRect;
				borderRect.Inflate(m_border, m_border);

				sprite->Draw(SystemUI::GetWhitePixel(), borderRect, nullptr, CV_Black);
			}
			if (Graphic.isSet())
			{
				Graphic.Draw(sprite, destRect);
			}

			if (eventPictureDraw.getCount())
			{
				sprite->Flush();

				Apoc3D::Math::Rectangle uiArea = SystemUI::GetUIArea(m_device);

				Apoc3D::Math::Rectangle rect = getAbsoluteArea();
				if (rect.getBottom() > uiArea.getBottom())
				{
					rect.Height -= rect.getBottom() - uiArea.getBottom();
				}
				if (rect.getRight() > uiArea.getRight())
				{
					rect.Width -= rect.getRight() - uiArea.getRight();
				}
				m_device->getRenderState()->setScissorTest(true, &rect);

				eventPictureDraw.Invoke(sprite, &destRect);

				sprite->Flush();
				m_device->getRenderState()->setScissorTest(false, 0);
			}
		}

		void PictureBox::OnMouseHover() { eventMouseHover.Invoke(this); } 
		void PictureBox::OnMouseOut() { eventMouseOut.Invoke(this); }
		void PictureBox::OnPress() { eventPress.Invoke(this); }
		void PictureBox::OnRelease() { eventRelease.Invoke(this); }
	}
}