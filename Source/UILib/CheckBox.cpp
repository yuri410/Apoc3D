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
			: Control(position, text), 
			m_check(checked), m_canUncheck(true), m_mouseOver(false), m_mouseDown(false), m_textOffset(0,0)
		{
			
		}
		CheckBox::~CheckBox()
		{

		}

		void CheckBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			Size.X = m_skin->CheckBoxTextures[0]->getWidth() + m_fontRef->MeasureString(Text).X + 15;
			Size.Y = max(m_skin->CheckBoxTextures[0]->getHeight(), m_fontRef->getLineHeight());

			m_textOffset.X = m_skin->CheckBoxTextures[0]->getWidth() + 5;
			m_textOffset.Y = (m_skin->CheckBoxTextures[0]->getHeight() - m_fontRef->getLineHeight())/2 ;
		}
		void CheckBox::Update(const GameTime* const time)
		{
			UpdateEvents();
		}
		void CheckBox::UpdateEvents()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Apoc3D::Math::Rectangle rect = getAbsoluteArea();
			
			if (rect.Contains(mouse->GetCurrentPosition()) && (!getOwner() || getOwner()->getAbsoluteArea().Contains(rect)))
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
			{
				m_check = false;
				if (!m_eToggled.empty())
					m_eToggled(this);
			}
			else if (!m_check)
			{
				m_check = true;
				if (!m_eToggled.empty())
					m_eToggled(this);
			}
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

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		CheckboxGroup::CheckboxGroup(const FastList<CheckBox*>& checkbox)
			: m_checkbox(checkbox), m_selectedIndex(-1)
		{

		}
		CheckboxGroup::~CheckboxGroup()
		{

		}

		void CheckboxGroup::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			for (int i=0;i<m_checkbox.getCount();i++)
			{
				m_checkbox[i]->SetSkin(m_skin);
				m_checkbox[i]->setOwner(getOwner());
				m_checkbox[i]->setCanUncheck(false);
				m_checkbox[i]->eventPress().bind(this, &CheckboxGroup::Checkbox_Press);
				m_checkbox[i]->Initialize(device);

				if (m_checkbox[i]->getValue())
					m_selectedIndex = i;

				if (Position.X < m_checkbox[i]->Position.X)
					Position.X = m_checkbox[i]->Position.X;
				if (Position.Y < m_checkbox[i]->Position.Y)
					Position.Y = m_checkbox[i]->Position.Y;
				if (Size.X < m_checkbox[i]->Size.X)
					Size.X = m_checkbox[i]->Size.X;

				Size.Y += m_checkbox[i]->Size.Y;
			}
		}
		void CheckboxGroup::Update(const GameTime* const time)
		{
			for (int i=0;i<m_checkbox.getCount();i++)
			{
				m_checkbox[i]->Update(time);
			}
		}
		void CheckboxGroup::Draw(Sprite* sprite)
		{
			for (int i=0;i<m_checkbox.getCount();i++)
			{
				m_checkbox[i]->Draw(sprite);
			}
		}

		void CheckboxGroup::Checkbox_Press(Control* ctrl)
		{
			CheckBox* cb = static_cast<CheckBox*>(ctrl);

			int checkIndex = -1;
			for (int i=0;i<m_checkbox.getCount();i++)
			{
				if (m_checkbox[i] == cb)
				{
					checkIndex = i;
					break;
				}
			}

			if (checkIndex != m_selectedIndex)
			{
				if (!m_eSelectionChanged.empty())
					m_eSelectionChanged(this);
				m_selectedIndex = checkIndex;
			}

			for (int i=0;i<m_checkbox.getCount();i++)
			{
				if (i!=checkIndex)
				{
					m_checkbox[i]->setValue(false);
				}
			}
		}
	}
}