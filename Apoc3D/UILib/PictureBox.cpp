/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
			: PictureBox(nullptr, position, border, texture)
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

		void PictureBox::Update(const AppTime* time)
		{
			UpdateEvents_StandardButton(m_mouseHover, m_mouseDown, getAbsoluteArea(),
				&PictureBox::OnMouseHover, &PictureBox::OnMouseOut, &PictureBox::OnPress, &PictureBox::OnRelease);

			if (IsInteractive && m_mouseHover)
			{
				eventInteractiveUpdate.Invoke(this, time);
			}
		}

		void PictureBox::Draw(Sprite* sprite)
		{
			RenderDevice* m_device = sprite->getRenderDevice();

			Apoc3D::Math::Rectangle destRect = getAbsoluteArea();
			if (m_border)
			{
				Apoc3D::Math::Rectangle borderRect = destRect;
				borderRect.Inflate(m_border, m_border);

				sprite->Draw(SystemUI::GetWhitePixel(), borderRect, nullptr, BorderColor);
			}
			if (Graphic.isSet())
			{
				Graphic.Draw(sprite, destRect);
			}

			if (eventPictureDraw.getCount())
			{
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

				{
					ScissorTestScope sts(rect, sprite);

					if (!sts.isEmpty())
					{
						eventPictureDraw.Invoke(sprite, &destRect);
					}
				}
			}
		}

		void PictureBox::OnMouseHover() { eventMouseHover.Invoke(this); } 
		void PictureBox::OnMouseOut() { eventMouseOut.Invoke(this); }
		void PictureBox::OnPress() { eventPress.Invoke(this); }
		void PictureBox::OnRelease() { eventRelease.Invoke(this); }
	}
}