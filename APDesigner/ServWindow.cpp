#include "ServWindow.h"

#include "BuildService/BuildService.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/ObjectFactory.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Label.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/Project/Project.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/ColorValue.h"

namespace APDesigner
{
	ServWindow::ServWindow(RenderWindow* wnd, const String& projectFilePath)
		: m_window(wnd), m_UIskin(0), m_projectFilePath(projectFilePath), m_lastHotKeyPressed(false)
	{
		
	}

	ServWindow::~ServWindow()
	{

	}

	void ServWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		m_window->setTitle(L"Apoc3D Build Service");
		
		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);
		BuildInterface::Initialize();
	}
	void ServWindow::Finalize()
	{
		InputAPIManager::getSingleton().FinalizeInput();
		BuildInterface::Finalize();
	}

	void ServWindow::Load()
	{
		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"system.pak\\classic_skin.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);

			m_UIskin->ControlFontName = L"english";
			FileLocation* fl = FileSystem::getSingleton().Locate(L"english.fnt", rule);
			FontManager::getSingleton().LoadFont(m_device, L"english", fl);
		}
		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();

		UIRoot::Initialize(m_device);
		
		{
			m_baseForm = new Form(FBS_Pane, L"");
			m_baseForm->SetSkin(m_UIskin);

			m_baseForm->Position = Point(0, 0);
			m_baseForm->Size = Point(m_window->getClientSize().Width, m_window->getClientSize().Height);
			
			m_lblDescription = new Label(Point(10, 5), L"Automatic Content Build Service", m_baseForm->Size.X-10);
			m_lblDescription->SetSkin(m_UIskin);

			m_lblLoadedProject = new Label(Point(10, 22+25), L"Project: " + m_projectFilePath, m_baseForm->Size.X-10);
			m_lblLoadedProject->SetSkin(m_UIskin);

			m_lblStatus = new Label(Point(10, 22+25*2), L"Status: ", m_baseForm->Size.X-10);
			m_lblStatus->SetSkin(m_UIskin);


			m_lblHide = new Label(Point(10, m_baseForm->Size.Y - 60), L"Toggle Hide: Ctrl + Alt + H ", m_baseForm->Size.X-10);
			m_lblHide->SetSkin(m_UIskin);

			int32 btnWidth = (m_baseForm->Size.X-30)/2;
			m_btnHide = new Button(Point(10, m_baseForm->Size.Y - 35), btnWidth, L"Hide");
			m_btnHide->SetSkin(m_UIskin);
			m_btnHide->eventRelease().bind(this, &ServWindow::BtnHide_Release);

			m_btnExit = new Button(Point(m_btnHide->Position.X + btnWidth + 10, m_baseForm->Size.Y - 35), (m_baseForm->Size.X-30)/2, L"Exit");
			m_btnExit->SetSkin(m_UIskin);
			m_btnExit->eventRelease().bind(this, &ServWindow::BtnExit_Release);

			m_baseForm->getControls().Add(m_lblDescription);
			m_baseForm->getControls().Add(m_lblLoadedProject);
			m_baseForm->getControls().Add(m_lblStatus);
			m_baseForm->getControls().Add(m_lblHide);
			m_baseForm->getControls().Add(m_btnExit);
			m_baseForm->getControls().Add(m_btnHide);

			m_baseForm->Initialize(m_device);

			UIRoot::Add(m_baseForm);

			m_baseForm->Show();
		}

	}
	void ServWindow::Unload()
	{
		UIRoot::Finalize();
		delete m_UIskin;
		delete m_sprite;

		delete m_baseForm;
		delete m_lblDescription;
		delete m_lblLoadedProject;
		delete m_lblStatus;
		delete m_lblHide;
		delete m_btnExit;
		delete m_btnHide;
	}

	void ServWindow::Update(const GameTime* const time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		m_baseForm->Size = Point(m_window->getClientSize().Width, m_window->getClientSize().Height);

		m_lblStatus->Position.Y = m_lblLoadedProject->Position.Y + m_lblLoadedProject->Size.Y;
		m_lblStatus->Text = L"Status: ";
		if (!BuildInterface::getSingleton().IsRunning())
		{
			m_lblStatus->Text.append(L" Ready");
		}
		else
		{
			m_lblStatus->Text.append(L" Building...");
		}

		UIRoot::Update(time);

		BuildInterface::getSingleton().MainThreadUpdate(time);

		bool pressed = (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('H'));
		if (!m_lastHotKeyPressed && pressed)
		{
			m_window->SetVisible(!m_window->getVisisble());
		}
		m_lastHotKeyPressed = pressed;
	}
	void ServWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1, 0);

		m_device->BeginFrame();

		UIRoot::Draw();

		m_device->EndFrame();

	}

	void ServWindow::BtnHide_Release(Control* ctrl)
	{
		m_window->SetVisible(false);
	}
	void ServWindow::BtnExit_Release(Control* ctrl)
	{
		m_window->Exit();
	}
}