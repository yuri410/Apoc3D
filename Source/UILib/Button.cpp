#include "Button.h"
#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"

#include "Input/InputAPI.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		void Button::UpdateEvents()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Keyboard* keyb = InputAPIManager::getSingleton().getKeyboard();

			Apoc3D::Math::Rectangle rect = getArea();
			if (m_owner && m_owner->getArea().Contains(rect) && rect.Contains(mouse->GetCurrentPosition()))
			{
				if (!m_mouseOver)
				{
					m_mouseOver = true;
					OnMouseOver();
				}
				if (!m_mouseDown && mouse && mouse->IsLeftPressed())
				{
					m_mouseDown = true;
					OnPress();
				}
				else if (m_mouseDown && mouse && mouse->IsLeftUp())
				{
					m_mouseDown = false;
					OnRelease();
				}
			}
			else if (m_mouseOver)
			{
				m_mouseOver = false;
				m_mouseDown = false;
				OnMouseOut();
			}
		}

		void Button::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
		}


		void Button::DrawDefaultButton(Sprite* sprite)
		{
			m_btnDestRect[0].X = (int)(Position.X);
			m_btnDestRect[0].Y = (int)(Position.Y);
			m_btnDestRect[0].Width = m_skin->ButtonTexture->getWidth() - 1;
			m_btnDestRect[0].Height = (int)Size.Y;

			m_btnDestRect[1].X = m_btnDestRect[0].X + m_btnDestRect[0].Width;
			m_btnDestRect[1].Y = m_btnDestRect[0].Y;
			m_btnDestRect[1].Width = (int)Size.X - m_btnDestRect[0].Width * 2;
			m_btnDestRect[1].Height = m_btnDestRect[0].Height;

			m_btnDestRect[2].X = m_btnDestRect[1].X + m_btnDestRect[1].Width;
			m_btnDestRect[2].Y = m_btnDestRect[0].Y;
			m_btnDestRect[2].Width = m_btnDestRect[0].Width;
			m_btnDestRect[2].Height = m_btnDestRect[0].Height;

			if (Text.size())
			{
				m_textSize = m_fontRef->MeasureString(Text);
				if (Size.X < m_textSize.X + m_skin->BtnVertPadding)
					Size.X = m_textSize.X + m_skin->BtnVertPadding;
				if (Size.Y < m_textSize.Y + m_skin->BtnHozPadding)
					Size.Y = m_textSize.Y + m_skin->BtnHozPadding;

				m_textPos.X = (int)(m_btnDestRect[0].X + (Size.X - m_textSize.X) / 2.0f);
				m_textPos.Y = (int)(m_btnDestRect[0].Y + (m_btnDestRect[0].Height - m_textSize.Y) / 2.0f);
			}

			if (Size.X < m_textSize.X+20)
			{
				Size.X = m_textSize.X + 20;
			}
			if (Size.Y < m_skin->ButtonTexture->getHeight())
			{
				Size.Y = m_skin->ButtonTexture->getHeight();
			}
			if (Size.Y < m_textSize.Y + 5)
			{
				Size.Y = m_textSize.Y + 5;
			}
			

			if (m_mouseOver)
			{
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[0], &m_skin->BtnSrcRect[0], m_skin->BtnDimColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[1], &m_skin->BtnSrcRect[1], m_skin->BtnDimColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[2], &m_skin->BtnSrcRect[2], m_skin->BtnDimColor);
				if (Text.size())
				{
					m_fontRef->DrawString(sprite, Text, m_textPos, m_skin->BtnTextDimColor);
				}
			}
			else
			{
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[0], &m_skin->BtnSrcRect[0], m_skin->BtnHighLightColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[1], &m_skin->BtnSrcRect[1], m_skin->BtnHighLightColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[2], &m_skin->BtnSrcRect[2], m_skin->BtnHighLightColor);
				if (Text.size())
				{
					m_fontRef->DrawString(sprite, Text, m_textPos, m_skin->BtnTextDimColor);
				}
			}
		}

		void Button::DrawCustomButton(Sprite* spriteBatch)
		{
			if (Enabled)
			{
				if (m_mouseDown)
				{
					if (m_MouseDownTexture)
						spriteBatch->Draw(m_MouseDownTexture, getArea(), 0, 1);
					else
						spriteBatch->Draw(m_NormalTexture, getArea(), 0, 1);
				}
				else if (m_mouseOver)
				{
					if (m_MouseOverTexture)
						spriteBatch->Draw(m_MouseOverTexture, getArea(), 0, 1);
					else
						spriteBatch->Draw(m_NormalTexture, getArea(), 0, 1);
				}
				else
				{
					spriteBatch->Draw(m_NormalTexture, getArea(), 0, 1);
				}
			}
			else
			{
				if (m_DisabledTexture)
					spriteBatch->Draw(m_DisabledTexture, getArea(), 0, 1);
				else
					spriteBatch->Draw(m_NormalTexture, getArea(), 0, 1);
			}
			//if (bMouseOver)
			//{
			//	if (bMouseDown)
			//		spriteBatch.Draw(texture, Position + offset, srcRect[0], dimColor, 0f, Vector2.Zero, scale, effect, 0f);
			//	else
			//		spriteBatch.Draw(texture, Position + offset, srcRect[0], highlight, 0f, Vector2.Zero, scale, effect, 0f);
			//}
			//else
			//	spriteBatch.Draw(texture, Position + offset, srcRect[0], BackColor, 0f, Vector2.Zero, scale, effect, 0f);
		}

		void Button::Draw(Sprite* sprite)
		{
			if (m_skin)
			{
				DrawDefaultButton(sprite);
			}
			else
			{
				DrawCustomButton(sprite);
			}
		}
		void Button::Update(const GameTime* const time)
		{
			Control::Update(time);

			UpdateEvents();
		}


		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		ButtonGroup::ButtonGroup(const FastList<Button*> buttons)
			: m_selectedIndex(0)
		{
			for (int i=0;i<buttons.getCount();i++)
			{
				m_button.Add(buttons[i]);
			}
		}

		ButtonGroup::ButtonGroup(const FastList<Button*> buttons, int selected)
			: m_selectedIndex(selected)
		{
			for (int i=0;i<buttons.getCount();i++)
			{
				m_button.Add(buttons[i]);
			}
		}

		const String& ButtonGroup::getSelectedText() const
		{
			return m_button[m_selectedIndex]->Text;
		}

		void ButtonGroup::setSelectedText(const String& text)
		{
			for (int i=0;i<m_button.getCount();i++)
			{
				if (m_button[i]->Text == text)
				{
					m_selectedIndex = i;
					break;
				}
			}
		}

		void ButtonGroup::Initialize(RenderDevice* device)
		{
			for (int i=0;i<m_button.getCount();i++)
			{
				m_button[i]->Initialize(device);
				m_button[i]->eventRelease().bind(this, &ButtonGroup::Button_OnRelease);
				m_button[i]->setOwner(m_owner)	;
			}
			Control::Initialize(device);
		}

		void ButtonGroup::Button_OnRelease(Control* sender)
		{
			Button* button = static_cast<Button*>(sender);

			for (int i=0;i<m_button.getCount();i++)
			{
				if (m_button[i] == button)
				{
					if (m_selectedIndex != i)
					{
						m_selectedIndex = i;
						if (!m_eChangeSelection.empty())
						{
							m_eChangeSelection(this);
						}
					}
				}
			}
		}

		void ButtonGroup::Draw(Sprite* sprite)
		{
			for (int i=0;i<m_button.getCount();i++)
			{
				//if (m_selectedIndex == i)
				//{
					m_button[i]->Draw(sprite);
				//}
			}
		}

		void ButtonGroup::Update(const GameTime* const time)
		{
			Control::Update(time);

			for (int i=0;i<m_button.getCount();i++)
			{
				m_button[i]->Update(time);
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		ButtonRow::ButtonRow(const Vector2& position, float width, const List<String>& titles)
			: m_count(titles.getCount())
		{ 
			Size.X = (int)width;
			Size.Y = m_skin->ButtonTexture->getHeight();
			m_texPos = new Point[m_count];
			m_titles = new String[m_count];
			m_btRect = new Apoc3D::Math::Rectangle[m_count];
			m_rect = new Apoc3D::Math::Rectangle[m_count];
		}
		ButtonRow::~ButtonRow()
		{
			delete[] m_texPos;
			delete[] m_titles;
			delete[] m_btRect;
			delete[] m_rect;
		}
		void ButtonRow::Initialize(RenderDevice* device)
		{
			
			Point s = m_fontRef->MeasureString(Text);
			float cellWidth = (float)Size.X / m_count;
			Apoc3D::Math::Rectangle area = getArea();

			for (int i=0;i<m_count;i++)
			{
				m_btRect[i] = Apoc3D::Math::Rectangle(
					area.X + (int)(cellWidth*i), area.Y, (int)cellWidth, m_skin->ButtonTexture->getHeight());
				m_texPos[i] = Point((int)(m_btRect[i].X + (m_btRect[i].Width - s.X) * 0.5f),
					(int)(m_btRect[i].Y + (m_btRect[i].Height - s.Y) * 0.5f));

				if (i==0)
				{
					m_rect[i] = Apoc3D::Math::Rectangle(
						Position.X+m_skin->ButtonTexture->getWidth() + (int)(cellWidth*i),
						Position.Y, 
						(int)cellWidth-m_skin->ButtonTexture->getWidth(), 
						m_skin->ButtonTexture->getHeight());
				}
				else if (i == m_count-1)
				{
					m_rect[i] = Apoc3D::Math::Rectangle(
						Position.X + (int)(cellWidth*i), 
						Position.Y, 
						(int)cellWidth - m_skin->ButtonTexture->getWidth(), 
						m_skin->ButtonTexture->getHeight());
				}
				else
				{
					m_rect[i] = Apoc3D::Math::Rectangle(Position.X + (int)(cellWidth*i),
						Position.Y,
						(int)cellWidth,
						m_skin->ButtonTexture->getHeight());
				}
			}
			m_tailPos = Point(m_rect[m_count-1].getRight(), Position.Y);
			Text = m_titles[0];
			Control::Initialize(device);
		}
		void ButtonRow::Draw(Sprite* sprite)
		{

		}
		void ButtonRow::Update(const GameTime* const time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point rectPos;
			Apoc3D::Math::Rectangle rect;// = Apoc3D::Math::Rectangle::Empty;
			rectPos.X = Position.X + m_owner->Position.X;
			rectPos.Y = Position.Y + m_owner->Position.Y;

			m_hoverIndex = -1;

			for (int i=0;i<m_count;i++)
			{
				rect.X = rectPos.X;
				rect.Y = rectPos.Y;
				rect.Width = m_btRect[i].Width;
				rect.Height = m_btRect[i].Height;

				if (rect.Contains(mouse->GetCurrentPosition().X, mouse->GetCurrentPosition().Y))
				{
					m_hoverIndex = i;
					// TODO
				}
			}
		}
	}
}

