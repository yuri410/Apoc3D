#include "CheckBox.h"
#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"
#include "Label.h"
#include "Button.h"
#include "List.h"

#include "Input/InputAPI.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		CheckBox::CheckBox(const Point& position, const String& text, bool checked)
			: Control(position, text), m_check(checked)
		{
			
		}

		void CheckBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			Size.X = m_skin->CheckBoxTextures[0]->getWidth() + m_fontRef->MeasureString(Text).X + 15;
			Size.Y = max(m_skin->CheckBoxTextures[0]->getHeight(), m_fontRef->getLineHeight());

			m_textOffset.X = m_skin->CheckBoxTextures[0]->getWidth() + 5;
			m_textOffset.Y = abs(m_skin->CheckBoxTextures[0]->getHeight() - m_fontRef->getLineHeight()) - 1;
		}
		void CheckBox::Update(const GameTime* const time)
		{
			UpdateEvents();
		}
		void CheckBox::UpdateEvents()
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
					Toggle();

					if (!m_mouseDown)
					{
						m_mouseDown = true;
						OnPress();
					}
				}
				else if (m_mouseDown && mouse->IsLeftUp())
				{
					OnRelease();
				}
			}
			else
			{
				if (m_mouseOver)
				{
					m_mouseOver = false;
					OnMouseOut();
				}
				m_mouseDown = false;
			}
		}
		void CheckBox::Toggle()
		{
			if (m_check && m_canUncheck)
				m_check = false;
			else if (!m_check)
				m_check = true;
		}

		void CheckBox::Draw(Sprite* sprite)
		{
			if (m_check)
			{
				sprite->Draw(m_skin->CheckBoxTextures[0], Position, m_skin->BackColor);
			}
			else
				sprite->Draw(m_skin->CheckBoxTextures[1], Position, m_skin->BackColor);

			m_fontRef->DrawString(sprite, Text, Position + m_textOffset, m_skin->ForeColor);
		}
	}
}