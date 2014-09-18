#include "ComboBox.h"
#include "StyleSkin.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"
#include "Text.h"
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
		ComboBox::ComboBox(const ComboBoxVisualSettings& settings, const Point& position, int width, const List<String>& items)
			: Control(nullptr, position, width), m_items(items)
		{
			Initialize(settings);
		}

		ComboBox::ComboBox(const StyleSkin* skin, const Point& position, int width, const List<String>& items)
			: Control(skin, position, width), m_items(items)
		{
			Initialize(skin);
		}


		ComboBox::~ComboBox()
		{
			DELETE_AND_NULL(m_textbox);
			DELETE_AND_NULL(m_button);
			DELETE_AND_NULL(m_listBox);
		}

		Point ComboBox::CalculateDropButtonPos(TextBox* ctb, int32 btnWidth)
		{
			Point dropButtonPos = Position + DropdownButtonOffset;
			dropButtonPos.X += m_size.X - btnWidth - ctb->Margin.Right;
			dropButtonPos.Y += -ctb->Margin.Top;
			return dropButtonPos;
		}

		void ComboBox::Initialize(const StyleSkin* skin)
		{
			m_textbox = new TextBox(skin, Position, m_size.X);
			
			ButtonVisualSettings bvs;
			bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->DropdownButtonNormalRegion);
			bvs.MouseHoverGraphic = UIGraphic(skin->SkinTexture, skin->DropdownButtonHoverRegion);
			bvs.MouseDownGraphic = UIGraphic(skin->SkinTexture, skin->DropdownButtonDownRegion);
			bvs.Margin = skin->DropdownButtonMargin;

			Point dropButtonPos = CalculateDropButtonPos(m_textbox, skin->DropdownButtonNormalRegion.Width);
			m_button = new Button(bvs, dropButtonPos, L"");
			m_listBox = new ListBox(skin, Position + Point(0,19), m_size.X, 8*m_fontRef->getLineHeightInt(), m_items);

			DropdownButtonOffset = skin->DropdownButtonOffset;

			PostInit();
		}
		void ComboBox::Initialize(const ComboBoxVisualSettings& settings)
		{
			m_textbox = new TextBox(settings.ContentTextBox, Position, m_size.X);
			
			Point dropButtonPos = CalculateDropButtonPos(m_textbox, settings.DropdownButton.NormalGraphic.getWidth());
			m_button = new Button(settings.DropdownButton, dropButtonPos, L"");
			m_listBox = new ListBox(settings.DropdownList, Position + Point(0, m_textbox->getHeight()), m_size.X, 8 * m_fontRef->getLineHeightInt(), m_items);

			if (settings.DropdownButtonOffset.isSet())
				DropdownButtonOffset = settings.DropdownButtonOffset;

			PostInit();
		}

		void ComboBox::PostInit()
		{
			m_textbox->ReadOnly = true;

			m_button->eventPress.Bind(this, &ComboBox::Button_OnPress);

			m_listBox->Visible = false;
			m_listBox->eventSelectionChanged.Bind(this, &ComboBox::ListBox_SelectionChanged);
			m_listBox->eventPress.Bind(this, &ComboBox::ListBox_OnPress);
		}


		void ComboBox::Update(const GameTime* time)
		{
			m_textbox->Position = Position;
			m_button->Position = CalculateDropButtonPos(m_textbox, m_button->getWidth());
			m_listBox->Position = Position + Point(0, m_textbox->getHeight());

			m_textbox->ParentFocused = m_button->ParentFocused = m_listBox->ParentFocused = ParentFocused;
			m_textbox->BaseOffset = m_button->BaseOffset = m_listBox->BaseOffset = BaseOffset;


			m_textbox->Update(time);
			m_button->Update(time);

			if (m_listBox->Visible)
			{
				m_listBox->Update(time);

				Mouse* mouse = InputAPIManager::getSingleton().getMouse();
				if (!m_justOpened && mouse->IsLeftPressed() &&
					!m_listBox->getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					Close();
				}

				if (mouse->IsLeftPressed())
					m_justOpened = true;
			}
		}
		void ComboBox::Draw(Sprite* sprite)
		{
			m_textbox->Draw(sprite);
			m_button->Draw(sprite);
		}
		void ComboBox::DrawOverlay(Sprite* sprite)
		{
			if (m_listBox->Visible)
			{
				m_listBox->Draw(sprite);
			}
		}
		bool ComboBox::IsOverriding() { return m_listBox->Visible; }

		List<String>& ComboBox::getItems() const { return m_listBox->getItems(); }

		bool ComboBox::isOpened() const { return m_listBox->Visible; }
		bool ComboBox::getLocked() const { return m_textbox->ReadOnly; }
		void ComboBox::setLocked(bool value) const { m_textbox->ReadOnly = value; }

		int ComboBox::getSelectedIndex() const { return m_listBox->getSelectedIndex(); }
		void ComboBox::setSelectedIndex(int v) const
		{
			m_listBox->setSelectedIndex(v);
			if (m_listBox->getSelectedIndex()!=-1)
			{
				m_textbox->SetText(m_listBox->getItems()[m_listBox->getSelectedIndex()]);
			}
		}

		void ComboBox::SetSelectionByName(const String& n)
		{
			int idx = m_listBox->FindEntry(n);
			if (idx != -1)
				m_listBox->setSelectedIndex(idx);
		}
		String* ComboBox::getSelectedItem() const
		{
			int32 idx = m_listBox->getSelectedIndex();
			if (idx != -1)
			{
				return &m_items[idx];
			}
			return nullptr;
		}

		void ComboBox::ListBox_OnPress(ListBox* ctrl) {  }
		void ComboBox::ListBox_SelectionChanged(ListBox* ctrl)
		{
			//String previousItem = m_textbox->Text;
			//int lastIndex = m_listBox->getSelectedIndex();
			if (m_listBox->getSelectedIndex()!=-1)
			{
				m_textbox->SetText(m_listBox->getItems()[m_listBox->getSelectedIndex()]);

				eventSelectionChanged.Invoke(this);
				
				Close();
			}
			
		}
		void ComboBox::Button_OnPress(Button* ctrl)
		{
			if (!m_listBox->Visible)
				Open();
			else
				Close();
		}
		void ComboBox::Open()
		{
			m_justOpened = true;
			m_listBox->Visible = true;
		}
		void ComboBox::Close()
		{
			m_listBox->Visible = false;
		}
	}
}