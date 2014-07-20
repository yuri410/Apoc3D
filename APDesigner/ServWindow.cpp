#include "ServWindow.h"

#include "BuildService/BuildService.h"
#include "CommonDialog/DialogCommon.h"

#include <Windows.h>

namespace APDesigner
{
	ServWindow::ServWindow(RenderWindow* wnd, const String& projectFilePath)
		: m_window(wnd), m_UIskin(0), m_projectFilePath(projectFilePath), 
		m_lastHideKeyPressed(false), m_lastBuildKeyPressed(false),
		m_changeBuffer(8192), m_currentProject(nullptr)
	{
		UpdateProject();
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
			pt.AddPath(L"system.pak\\skin.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);
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

			m_lblLoadedProject = new Label(Point(10, 22+25), L"Project: ", m_baseForm->Size.X-10);
			m_lblLoadedProject->SetSkin(m_UIskin);

			m_lblStatus = new Label(Point(10, 22+25*2), L"Status: ", m_baseForm->Size.X-10);
			m_lblStatus->SetSkin(m_UIskin);


			m_lblHotKeys = new Label(Point(10, m_baseForm->Size.Y - 90), L"Toggle Hide: Ctrl + Alt + H\nForce Build: Ctrl + Alt + B", m_baseForm->Size.X-10);
			m_lblHotKeys->SetSkin(m_UIskin);

			int32 btnWidth = (m_baseForm->Size.X-40)/3;
			m_btnHide = new Button(Point(10, m_baseForm->Size.Y - 35), btnWidth, L"Hide");
			m_btnHide->SetSkin(m_UIskin);
			m_btnHide->eventRelease.Bind(this, &ServWindow::BtnHide_Release);

			m_btnBuild = new Button(Point(m_btnHide->Position.X + btnWidth + 10, m_baseForm->Size.Y - 35), btnWidth, L"Build");
			m_btnBuild->SetSkin(m_UIskin);
			m_btnBuild->eventRelease.Bind(this, &ServWindow::BtnBuild_Release);

			m_btnExit = new Button(Point(m_btnHide->Position.X + btnWidth * 2 + 20, m_baseForm->Size.Y - 35), btnWidth, L"Exit");
			m_btnExit->SetSkin(m_UIskin);
			m_btnExit->eventRelease.Bind(this, &ServWindow::BtnExit_Release);


			m_baseForm->getControls().Add(m_lblDescription);
			m_baseForm->getControls().Add(m_lblLoadedProject);
			m_baseForm->getControls().Add(m_lblStatus);
			m_baseForm->getControls().Add(m_lblHotKeys);
			m_baseForm->getControls().Add(m_btnExit);
			m_baseForm->getControls().Add(m_btnHide);
			m_baseForm->getControls().Add(m_btnBuild);

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
		delete m_lblHotKeys;
		delete m_btnExit;
		delete m_btnHide;
	}

	void ServWindow::Update(const GameTime* const time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		EffectManager::getSingleton().Update(time);

		m_baseForm->Size = Point(m_window->getClientSize().Width, m_window->getClientSize().Height);


		m_lblLoadedProject->Text = L"Project: " + m_currentProject->getName();
		m_lblLoadedProject->Text.append(L" (");
		m_lblLoadedProject->Text.append(PathUtils::GetFileName(m_projectFilePath));
		m_lblLoadedProject->Text.append(L")");

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

		BuildInterface::BuildResult result;
		if (BuildInterface::getSingleton().MainThreadUpdate(time, &result))
		{
			if (result.Message.size() || result.HasError || result.HasWarning)
			{
				Notify(L"Build Failed - " + m_currentProject->getName(), result.Message);
				m_queuedMessage = result.Message;
			}
			else
			{
				Notify(L"Build Success - " + m_currentProject->getName(), L" - Apoc3D Build Service");
			}
		}

		bool pressed = (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('H'));
		if (!m_lastHideKeyPressed && pressed)
		{
			m_window->SetVisible(!m_window->getVisisble());
		}
		m_lastHideKeyPressed = pressed;

		pressed = (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('B'));
		if (!m_lastBuildKeyPressed && pressed)
		{
			BtnBuild_Release(nullptr);
		}
		m_lastBuildKeyPressed = pressed;


		if (m_window->getVisisble() && m_queuedMessage.size())
		{
			CommonDialog::ShowMessageBox(m_queuedMessage, L"Build Issues", false, true);
			m_queuedMessage.clear();
		}

		{
			//ReadDirectoryChangesW();
		}
	}
	void ServWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1, 0);

		m_device->BeginFrame();

		UIRoot::Draw();

		m_device->EndFrame();

	}

	void ServWindow::UpdateProject()
	{
		FileLocation fl(m_projectFilePath);
		Configuration* conf = XMLConfigurationFormat::Instance.Load(fl);

		Project* prj = new Project();
		prj->Parse(conf->get(L"Project"));
		prj->setBasePath(PathUtils::GetDirectory(m_projectFilePath));

		delete conf;

		if (m_currentProject)
			delete m_currentProject;
		m_currentProject = prj;
	}
	void ServWindow::Notify(const String& title, const String& message)
	{
		if (!m_window->getVisisble())
		{
			String cmdLine = L"notifu.exe /i parent";
			cmdLine.append(L" /p \"");
			cmdLine.append(title);
			cmdLine.append(L"\"");

			cmdLine.append(L" /m \"");
			cmdLine.append(message);
			cmdLine.append(L"\"");

			STARTUPINFO startUpInfo;
			ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
			startUpInfo.cb = sizeof(STARTUPINFO);
			
			size_t len = cmdLine.size() + 1;
			wchar_t* chars = new wchar_t[cmdLine.size() + 1];
			memcpy(chars, cmdLine.c_str(), sizeof(wchar_t) * len);

			PROCESS_INFORMATION procInfo;

			BOOL res = CreateProcess(nullptr, chars, nullptr, nullptr, FALSE, 0 ,nullptr, nullptr, &startUpInfo, &procInfo);

			CloseHandle(procInfo.hProcess);
			CloseHandle(procInfo.hThread);

			delete[] chars;
			//ShellExecute(GetForegroundWindow(), nullptr, cmdLine.c_str(), nullptr, nullptr, SW_SHOWNA);
		}
	}

	void ServWindow::BtnHide_Release(Control* ctrl)
	{
		m_window->SetVisible(false);
	}
	void ServWindow::BtnBuild_Release(Control* ctrl)
	{
		if (!BuildInterface::getSingleton().IsRunning())
		{
			Notify(L"Build Started - " + m_currentProject->getName(), L" - Apoc3D Build Service");

			UpdateProject();
			BuildInterface::getSingleton().AddBuild(m_currentProject);
			BuildInterface::getSingleton().Execute();
		}
	}
	void ServWindow::BtnExit_Release(Control* ctrl)
	{
		m_window->Exit();
	}
}