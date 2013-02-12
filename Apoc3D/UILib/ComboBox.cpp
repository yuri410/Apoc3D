#include "ComboBox.h"
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
		ComboBox::ComboBox(const Point& position, int width, const List<String>& items)
			: Control(position), m_items(items), m_textbox(0), m_listBox(0), m_button(0)
		{
			Size.X = width;
		}
		ComboBox::~ComboBox()
		{
			if (m_textbox)
				delete m_textbox;
			if (m_button)
				delete m_button;
			if (m_listBox)
				delete m_listBox;
		}

		void ComboBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			m_textbox = new TextBox(Position, Size.X);
			m_textbox->SetSkin(m_skin);
			m_textbox->setOwner(getOwner());
			m_textbox->setLocked(true);
			m_textbox->Initialize(device);

			m_button = new Button(Position + Point(Size.X-16,0), L"");
			m_button->setOwner(getOwner());
			m_button->setNormalTexture(m_skin->ComboButton);
			m_button->eventPress().bind(this, &ComboBox::Button_OnPress);
			m_button->Initialize(device);
			

			m_listBox = new ListBox(Position + Point(0,19), Size.X, 8*m_fontRef->getLineHeightInt(), m_items);
			m_listBox->SetSkin(m_skin);
			m_listBox->setOwner(getOwner());
			m_listBox->Visible = false;
			m_listBox->eventSelectionChanged().bind(this, &ComboBox::ListBox_SelectionChanged);
			m_listBox->eventPress().bind(this, &ComboBox::ListBox_OnPress);
			m_listBox->Initialize(device);
		}
		void ComboBox::Update(const GameTime* const time)
		{
			m_textbox->Position = Position;
			m_button->Position = Position + Point(Size.X-16,0);
			m_listBox->Position = Position + Point(0,19);
			m_textbox->setOwner(getOwner());
			m_button->setOwner(getOwner());
			m_listBox->setOwner(getOwner());

			m_textbox->Update(time);
			m_button->Update(time);

			if (m_listBox->Visible)
			{
				m_listBox->Update(time);

				Mouse* mouse = InputAPIManager::getSingleton().getMouse();
				if (!m_justOpened && mouse->IsLeftPressed() &&
					!m_listBox->getAbsoluteArea().Contains(mouse->GetCurrentPosition()))
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
		const String& ComboBox::getText() const { return m_textbox->Text; }
		void ComboBox::setText(const String& value) { m_textbox->Text = value; }
		int ComboBox::getSelectedIndex() const { return m_listBox->getSelectedIndex(); }
		void ComboBox::setSelectedIndex(int v) const
		{
			m_listBox->setSelectedIndex(v);
			if (m_listBox->getSelectedIndex()!=-1)
			{
				m_textbox->Text = m_listBox->getItems()[m_listBox->getSelectedIndex()];
			}
		}
		void ComboBox::SetSelectedByName(const String& name)
		{
			m_listBox->SetSelectedByName(name);
			if (m_listBox->getSelectedIndex()!=-1)
			{
				m_textbox->Text = m_listBox->getItems()[m_listBox->getSelectedIndex()];
			}
		}
		void ComboBox::ListBox_OnPress(Control* ctrl) {  }
		void ComboBox::ListBox_SelectionChanged(Control* ctrl)
		{
			//String previousItem = m_textbox->Text;
			//int lastIndex = m_listBox->getSelectedIndex();
			if (m_listBox->getSelectedIndex()!=-1)
			{
				m_textbox->Text = m_listBox->getItems()[m_listBox->getSelectedIndex()];

				if (!m_eSelectionChanged.empty())
				{
					m_eSelectionChanged(this);
				}

				Close();
			}
			
		}
		void ComboBox::Button_OnPress(Control* ctrl)
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