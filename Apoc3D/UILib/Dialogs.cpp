#include "Dialogs.h"

#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/UILib/PictureBox.h"
#include "apoc3d/UILib/Text.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
		static const int singleFixedButtonWidth = 80;

		MessageDialogBox::MessageDialogBox(Apoc3D::Graphics::RenderSystem::RenderDevice* device, const StyleSkin* skin,
			const String& title, const String& text, int buttonsFlags)
			: m_renderDevice(device)
		{
			m_form = new Form(skin, device, FBS_Fixed, title);

			Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(device);

			if (buttonsFlags & DLGRES_OK)
				AddButton(L"OK", skin, DLGRES_OK);

			if (buttonsFlags & DLGRES_Yes)
				AddButton(L"Yes", skin, DLGRES_Yes);

			if (buttonsFlags & DLGRES_Abort)
				AddButton(L"Abort", skin, DLGRES_Abort);

			if (buttonsFlags & DLGRES_Retry)
				AddButton(L"Retry", skin, DLGRES_Retry);

			if (buttonsFlags & DLGRES_Ignore)
				AddButton(L"Ignore", skin, DLGRES_Ignore);

			if (buttonsFlags & DLGRES_No)
				AddButton(L"No", skin, DLGRES_No);

			if (buttonsFlags & DLGRES_Cancel)
				AddButton(L"Cancel", skin, DLGRES_Cancel);


			Font* controlFont = skin->ContentTextFont;
			
			int buttonSpacing = 15;
			int buttonsWidth = 0;
			for (int i = 0; i<m_controlButtons.getCount(); i++)
			{
				buttonsWidth += singleFixedButtonWidth;
				if (i != m_controlButtons.getCount() - 1)
					buttonsWidth += buttonSpacing;
			}

			int minimumWidth = buttonsWidth;
			Point orginalContentSize = controlFont->MeasureString(text);

			bool useContentWidthCap = false;
			if (orginalContentSize.X > (rect.Width * 3 / 4))
			{
				int contentWidthLimit = rect.Width * 3 / 4;
				float lines = (float)orginalContentSize.X / contentWidthLimit;
				orginalContentSize.Y *= (int)ceil(lines);
				useContentWidthCap = true;
				orginalContentSize.X = contentWidthLimit;
			}

			if (minimumWidth < orginalContentSize.X)
				minimumWidth = orginalContentSize.X;

			int estButtonHeight = controlFont->getLineHeightInt() + skin->ButtonPadding.getVerticalSum();

			int vertPadding = 15;
			int btnHozPadding = 50;
			m_form->setWidth(minimumWidth + btnHozPadding * 2);
			m_form->setHeight(orginalContentSize.Y + estButtonHeight + vertPadding * 5);

			int contentHozPadding = 25;
			m_content = new Label(skin, Point(contentHozPadding, 14 + vertPadding), text, m_form->getWidth() - contentHozPadding * 2);
			m_form->getControls().Add(m_content);

			int buttonPosY = orginalContentSize.Y + vertPadding * 4;
			for (Button* btn : m_controlButtons)
			{
				btn->Position = Point(0, buttonPosY);
				m_form->getControls().Add(btn);
			}

			m_form->Position = (rect.getSize() - m_form->getSize()) / 2;


			int buttonPosX = (m_form->getWidth() - buttonsWidth - btnHozPadding * 2) / 2;
			for (int i = 0; i < m_controlButtons.getCount(); i++)
			{
				buttonPosX += i * buttonsWidth / m_controlButtons.getCount();
				const Point& btnSize = m_controlButtons[i]->getSize();
				m_controlButtons[i]->Position.X = btnSize.X / 2 + buttonPosX;
			}
			SystemUI::Add(m_form);
		}

		MessageDialogBox::~MessageDialogBox()
		{
			SystemUI::Remove(m_form);
			delete m_form;

			m_controlButtons.DeleteAndClear();
			delete m_content;
		}

		void MessageDialogBox::ShowModal()
		{
			m_result = DLGRES_Cancel;
			m_form->ShowModal();
		}

		void MessageDialogBox::AddButton(const String& caption, const StyleSkin* skin, DialogResult dr)
		{
			Button* btn = new Button(skin, Point(0,0), singleFixedButtonWidth, caption);
			btn->eventRelease.Bind(this, &MessageDialogBox::Button_Release);

			m_controlButtons.Add(btn);
			m_buttonMapping.Add(btn, dr);
		}

		void MessageDialogBox::Button_Release(Button* ctrl)
		{
			m_result = m_buttonMapping[ctrl];
			m_form->Close();
			eventInputResponsed.Invoke(m_result);
		}

		bool MessageDialogBox::isActive() const { return m_form->Visible; }

		//////////////////////////////////////////////////////////////////////////


		InputDialogBox::InputDialogBox(Apoc3D::Graphics::RenderSystem::RenderDevice* device, const StyleSkin* skin,
			const String& title, const String& text, bool multiline)
			: m_renderDevice(device)
		{
			m_form = new Form(skin, device, FBS_Fixed, title);

			m_form->setSize(Point(500, !multiline ? 150 : 400));

			m_content = new Label(skin, Point(15, skin->FormTitle->Height + 10), text, m_form->getWidth() - 30);
			m_form->getControls().Add(m_content);

			m_btnOk = new Button(skin, Point(315, m_form->getHeight() - 50), singleFixedButtonWidth, L"OK");
			m_btnOk->eventRelease.Bind(this, &InputDialogBox::Button_OkRelease);
			m_form->getControls().Add(m_btnOk);

			m_btnCancel = new Button(skin, Point(400, m_form->getHeight() - 50), singleFixedButtonWidth, L"Cancel");
			m_btnCancel->eventRelease.Bind(this, &InputDialogBox::Button_CancelRelease);
			m_form->getControls().Add(m_btnCancel);

			if (multiline)
				m_inputField = new TextBox(skin, Point(15, 60), m_form->getWidth() - 30, m_form->getHeight() - 140, L"");
			else
				m_inputField = new TextBox(skin, Point(15, 60), m_form->getWidth() - 30, L"");
			m_inputField->EnableAllScrollBars();

			m_form->getControls().Add(m_inputField);

			Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(device);
			m_form->Position = (rect.getSize() - m_form->getSize()) / 2;
			
			SystemUI::Add(m_form);
		}

		InputDialogBox::~InputDialogBox()
		{
			SystemUI::Remove(m_form);
			delete m_form;

			delete m_btnCancel;
			delete m_btnOk;
			delete m_content;
			delete m_inputField;
		}

		void InputDialogBox::ShowModal()
		{
			m_result = DLGRES_Cancel;
			m_form->ShowModal();
		}

		void InputDialogBox::Button_CancelRelease(Button* ctrl)
		{
			m_result = DLGRES_Cancel;
			m_form->Close();
			eventInputConfirmed.Invoke(this);
		}
		void InputDialogBox::Button_OkRelease(Button* ctrl)
		{
			m_result = DLGRES_OK;
			m_form->Close();
			eventInputConfirmed.Invoke(this);
		}

		const String& InputDialogBox::getTextInput() const { return m_inputField->getText(); }

		bool InputDialogBox::isActive() const { return m_form->Visible; }

	}
}
