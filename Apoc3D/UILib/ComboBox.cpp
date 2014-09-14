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
		ComboBox::ComboBox(const Point& position, int width, const List<String>& items)
			: ComboBox(nullptr, position, width, items) { }

		ComboBox::ComboBox(const StyleSkin* skin, const Point& position, int width, const List<String>& items)
			: Control(skin, position), m_items(items)
		{
			m_size.X = width;
			Initialize(skin);
		}


		ComboBox::~ComboBox()
		{
			DELETE_AND_NULL(m_textbox);
			DELETE_AND_NULL(m_button);
			DELETE_AND_NULL(m_listBox);
		}

		//void ComboBox::Initialize(RenderDevice* device)
		void ComboBox::Initialize(const StyleSkin* skin)
		{
			//Control::Initialize(device);

			m_textbox = new TextBox(skin, Position, m_size.X);
			//m_textbox->SetSkin(m_skin);
			//m_textbox->setOwner(getOwner());
			m_textbox->setLocked(true);
			//m_textbox->Initialize(device);

			ButtonVisualSettings bvs;
			bvs.HasNormalGraphic = true;
			bvs.HasMouseHoverGraphic = true;
			bvs.HasMouseDownGraphic = true;

			bvs.NormalGraphic = UIGraphic(skin->SkinTexture, skin->DropDownButtonNormal); //m_skin->ComboButton);
			bvs.MouseHoverGraphic = UIGraphic(skin->SkinTexture, skin->DropDownButtonHover);
			bvs.MouseDownGraphic = UIGraphic(skin->SkinTexture, skin->DropDownButtonDown);

			m_button = new Button(bvs, Position + Point(m_size.X - skin->DropDownButtonNormal.Width, 0), L"");
			//m_button->setOwner(getOwner());
			

			//m_button->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->DropDownButtonNormal); //m_skin->ComboButton);
			//m_button->MouseOverTexture = UIGraphic(m_skin->SkinTexture, m_skin->DropDownButtonHover); 
			//m_button->MouseDownTexture = UIGraphic(m_skin->SkinTexture, m_skin->DropDownButtonDown); 

			m_button->eventPress.Bind(this, &ComboBox::Button_OnPress);
			//m_button->Initialize(device);
			

			m_listBox = new ListBox(skin, Position + Point(0,19), m_size.X, 8*m_fontRef->getLineHeightInt(), m_items);
			//m_listBox->SetSkin(m_skin);
			//m_listBox->setOwner(getOwner());
			m_listBox->Visible = false;
			m_listBox->eventSelectionChanged.Bind(this, &ComboBox::ListBox_SelectionChanged);
			m_listBox->eventPress.Bind(this, &ComboBox::ListBox_OnPress);
			//m_listBox->Initialize(device);
		}
		void ComboBox::Update(const GameTime* time)
		{
			m_textbox->Position = Position;
			m_button->Position = Position + Point(m_size.X-16,0);
			m_listBox->Position = Position + Point(0,19);

			m_textbox->ParentFocused = m_button->ParentFocused = m_listBox->ParentFocused = ParentFocused;
			m_textbox->BaseOffset = m_button->BaseOffset = m_listBox->BaseOffset = BaseOffset;


			//m_textbox->setOwner(getOwner());
			//m_button->setOwner(getOwner());
			//m_listBox->setOwner(getOwner());

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

		bool ComboBox::getOpened() const { return m_listBox->Visible; }
		bool ComboBox::getLocked() const { return m_textbox->getLocked(); }
		void ComboBox::setLocked(bool value) const { m_textbox->setLocked(value); }

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