#include "CheckBox.h"
#include "StyleSkin.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"
#include "Label.h"
#include "Button.h"
#include "List.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/Keyboard.h"

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

			int32 hozMg = m_skin->CheckBoxMargin.getHorizontalSum();
			int32 vertMg = m_skin->CheckBoxMargin.getVerticalSum();
			Size.X = m_skin->CheckBoxNormal.Width - hozMg + m_fontRef->MeasureString(Text).X + m_skin->CheckBoxTextSpacing;
			Size.Y = Math::Max(m_skin->CheckBoxNormal.Height,m_fontRef->getLineHeightInt()) - vertMg;

			m_textOffset.X = m_skin->CheckBoxNormal.Width + m_skin->CheckBoxTextSpacing;
			m_textOffset.Y = (m_skin->CheckBoxNormal.Height - m_fontRef->getLineHeightInt())/2 - vertMg;
		}
		void CheckBox::Update(const GameTime* time)
		{
			UpdateEvents();
		}
		void CheckBox::UpdateEvents()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Apoc3D::Math::Rectangle rect = getAbsoluteArea();
			
			if (rect.Contains(mouse->GetPosition()) && (!getOwner() || getOwner()->getAbsoluteArea().Contains(rect)))
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
					m_mouseDown = false;
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
				eventToggled.Invoke(this);
			}
			else if (!m_check)
			{
				m_check = true;
				eventToggled.Invoke(this);
			}
		}

		void CheckBox::Draw(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle dstRect(Position.X - m_skin->CheckBoxMargin.Left, Position.Y - m_skin->CheckBoxMargin.Top, 0,0);
			dstRect.Width = m_skin->CheckBoxNormal.Width;
			dstRect.Height = m_skin->CheckBoxNormal.Height;

			if (Enabled)
			{
				if (m_mouseDown)
					sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxDown, CV_White);
				else if (m_mouseOver)
					sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxHover, CV_White);
				else
					sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxNormal, CV_White);

				if (m_check)
					sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxChecked, CV_White);
			}
			else
			{
				sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxDisable, CV_White);

				if (m_check)
					sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->CheckBoxChecked, 0x7fffffffU);
			}
			
			m_fontRef->DrawString(sprite, Text, Position + m_textOffset, m_skin->TextColor);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		CheckboxGroup::CheckboxGroup(const List<CheckBox*>& checkbox)
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
				m_checkbox[i]->eventPress.Bind(this, &CheckboxGroup::Checkbox_Press);
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
		void CheckboxGroup::Update(const GameTime* time)
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
				eventSelectionChanged.Invoke(this);
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