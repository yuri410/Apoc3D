#include "CheckBox.h"
#include "StyleSkin.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"
#include "Text.h"
#include "Button.h"
#include "List.h"
#include "apoc3d/Math/Math.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/Keyboard.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		CheckBox::CheckBoxEvent CheckBox::eventAnyPress;
		CheckBox::CheckBoxEvent CheckBox::eventAnyRelease;

		CheckBox::CheckBox(const CheckboxVisualSettings& settings, const Point& position, const String& text, bool checked)
			: Control(nullptr, position), m_text(text), Checked(checked), AutosizeX(true), AutosizeY(true)
		{
			Initialize(settings);
		}
		CheckBox::CheckBox(const CheckboxVisualSettings& settings, const Point& position, const String& text, const Point& sz, bool checked)
			: Control(nullptr, position), m_text(text), Checked(checked)
		{
			Initialize(settings);
			m_size = sz;
		}

		CheckBox::CheckBox(const StyleSkin* skin, const Point& position, const String& text, bool checked)
			: Control(skin, position), m_text(text), Checked(checked), AutosizeX(true), AutosizeY(true)
		{
			Initialize(skin);
		}

		CheckBox::CheckBox(const StyleSkin* skin, const Point& position, const String& text, const Point& sz, bool checked)
			: Control(skin, position), m_text(text), Checked(checked)
		{
			Initialize(skin);
			m_size = sz;
		}

		CheckBox::~CheckBox()
		{

		}

		void CheckBox::Initialize(const StyleSkin* skin)
		{
			TextSpacing = skin->CheckBoxTextSpacing;
			TextSettings.TextColor = skin->TextColor;
			TextSettings.TextColorDisabled = skin->TextColorDisabled;

			TextSettings.HorizontalAlignment = TextHAlign::Left;

			NormalGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxNormalRegion, skin->CheckBoxNormalColor);
			HoverGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxHoverRegion, skin->CheckBoxHoverColor);
			DownGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxDownRegion, skin->CheckBoxDownColor);
			DisabledGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxDisabledRegion, skin->CheckBoxDisabledColor);
			
			TickGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxCheckedRegion, skin->CheckBoxCheckedColor);
			DisabledTickGraphic = UIGraphicSimple(skin->SkinTexture, skin->CheckBoxDisabledCheckedRegion, skin->CheckBoxDisableCheckedColor);
			
			Margin = skin->CheckBoxMargin;

			SetFont(skin->CheckBoxFont);

			UpdateSize();
		}
		void CheckBox::Initialize(const CheckboxVisualSettings& settings)
		{
			if (settings.TextSpacing.isSet())
				TextSpacing = settings.TextSpacing;

			TextSettings.TextColor = settings.TextColor;
			TextSettings.TextColorDisabled = settings.TextColorDisabled.isSet() ? settings.TextColorDisabled : settings.TextColor;
			TextSettings.HorizontalAlignment = TextHAlign::Left;
			
			NormalGraphic = settings.NormalGraphic;

			if (settings.HoverGraphic.isSet())
				HoverGraphic = settings.HoverGraphic;
			if (settings.DownGraphic.isSet())
				DownGraphic = settings.DownGraphic;
			if (settings.DisabledGraphic.isSet())
				DisabledGraphic = settings.DisabledGraphic;

			TickGraphic = settings.TickGraphic;

			if (settings.DisabledTickGraphic.isSet())
				DisabledTickGraphic = settings.DisabledTickGraphic;

			if (settings.Margin.isSet())
				Margin = settings.Margin;

			if (settings.FontRef)
				SetFont(settings.FontRef);

			UpdateSize();
		}

		void CheckBox::Update(const AppTime* time)
		{
			return UpdateEvents_StandardButton(m_mouseHover, m_mouseDown, getAbsoluteArea(),
				&CheckBox::OnMouseHover, &CheckBox::OnMouseOut, &CheckBox::OnPress, &CheckBox::OnRelease);
		}

		void CheckBox::UpdateSize()
		{
			if (AutosizeX || AutosizeY)
			{
				Point textSize = m_fontRef->MeasureString(m_text);

				if (AutosizeX)
					m_size.X = textSize.X + NormalGraphic.getWidth() + Margin.getHorizontalSum() + TextSpacing;

				if (AutosizeY)
					m_size.Y = Math::Max(textSize.Y, NormalGraphic.getHeight()) + Margin.getVerticalSum();
			}
		}

		void CheckBox::Toggle()
		{
			if (Checked && CanUncheck)
			{
				Checked = false;
				eventToggled.Invoke(this);
			}
			else if (!Checked)
			{
				Checked = true;
				eventToggled.Invoke(this);
			}
		}

		void CheckBox::Draw(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition();

			Apoc3D::Math::Rectangle gfxRect =
			{
				drawPos.X - Margin.Left,
				drawPos.Y - Margin.Top + (m_size.Y - NormalGraphic.getHeight()) / 2,
				NormalGraphic.getWidth(),
				NormalGraphic.getHeight()
			};

			if (Enabled)
			{
				if (m_mouseDown)
					DownGraphic.Draw(sprite, gfxRect);
				else if (m_mouseHover)
					HoverGraphic.Draw(sprite, gfxRect);
				else
					NormalGraphic.Draw(sprite, gfxRect);
					
				if (Checked)
					TickGraphic.Draw(sprite, gfxRect);
					//sprite->Draw(m_skin->SkinTexture, gfxRect, &m_skin->CheckBoxChecked, CV_White);
			}
			else
			{
				DisabledGraphic.Draw(sprite, gfxRect);
				//sprite->Draw(m_skin->SkinTexture, gfxRect, &m_skin->CheckBoxDisable, CV_White);

				if (Checked)
					DisabledTickGraphic.Draw(sprite, gfxRect);
					//sprite->Draw(m_skin->SkinTexture, gfxRect, &m_skin->CheckBoxChecked, 0x7fffffffU);
			}

			Point textOffset;
			textOffset.X = gfxRect.Width + Margin.getHorizontalSum() + TextSpacing;
			textOffset.Y = 0;// (m_size.Y - m_fontRef->getLineHeightInt() - 1) / 2;

			//m_fontRef->DrawString(sprite, Text, Position + m_textOffset, m_skin->TextColor);
			Point textPos = drawPos + textOffset;
			TextSettings.Draw(sprite, m_fontRef, m_text, textPos, Point(m_size.X - textOffset.X, m_size.Y), Enabled);
		}

		void CheckBox::SetFont(Font* fontRef)
		{
			if (fontRef != m_fontRef)
			{
				m_fontRef = fontRef;
				UpdateSize();
			}
		}
		void CheckBox::SetText(const String& text)
		{
			if (m_text != text)
			{
				m_text = text;
				UpdateSize();
			}
		}

		void CheckBox::SetSize(const Point& sz)
		{
			m_size = sz;
			AutosizeX = AutosizeY = false;
		}

		void CheckBox::SetSizeX(int32 v)
		{
			m_size.X = v;
			AutosizeX = false;
		}
		void CheckBox::SetSizeY(int32 v)
		{
			m_size.Y = v;
			AutosizeY = false;
		}

		void CheckBox::OnMouseHover()
		{
			//eventMouseOver.Invoke(this); 
		}
		void CheckBox::OnMouseOut()
		{
			//eventMouseOut.Invoke(this); 
		}
		void CheckBox::OnPress()
		{
			eventPress.Invoke(this);
			eventAnyPress.Invoke(this);
		}
		void CheckBox::OnRelease()
		{
			Toggle();

			eventRelease.Invoke(this);
			eventAnyRelease.Invoke(this);
		}


		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		CheckboxGroup::CheckboxGroup(const List<CheckBox*>& checkbox)
			: m_checkbox(checkbox)
		{

		}
		CheckboxGroup::~CheckboxGroup()
		{

		}

		void CheckboxGroup::Initialize(const StyleSkin* skin)
		{
			for (int i=0;i<m_checkbox.getCount();i++)
			{
				CheckBox* chb = m_checkbox[i];
				//m_checkbox[i]->SetSkin(m_skin);
				//m_checkbox[i]->setOwner(getOwner());
				//m_checkbox[i]->setCanUncheck(false);
				chb->eventPress.Bind(this, &CheckboxGroup::Checkbox_Press);
				//m_checkbox[i]->Initialize(device);

				if (chb->Checked)
					m_selectedIndex = i;

				if (Position.X < chb->Position.X)
					Position.X = chb->Position.X;
				if (Position.Y < chb->Position.Y)
					Position.Y = chb->Position.Y;

				Point sz = chb->getSize();

				if (m_size.X < sz.X)
					m_size.X = sz.X;

				m_size.Y += sz.Y;
			}
		}
		void CheckboxGroup::Update(const AppTime* time)
		{
			for (CheckBox* chb : m_checkbox)
			{
				chb->Update(time);
			}
		}
		void CheckboxGroup::Draw(Sprite* sprite)
		{
			for (CheckBox* chb : m_checkbox)
			{
				chb->Draw(sprite);
			}
		}

		void CheckboxGroup::Checkbox_Press(CheckBox* cb)
		{
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
					m_checkbox[i]->Checked = false;
				}
			}
		}
	}
}