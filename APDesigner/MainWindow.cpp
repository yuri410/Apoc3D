#include "MainWindow.h"

#include "Core/GameTime.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "UILib/StyleSkin.h"
#include "Input/InputAPI.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;

namespace APDesigner
{
	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd), m_UIskin(0)
	{

	}

	void MainWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		m_window->setTitle(L"Apoc3D Designer");

		FileSystem::getSingletonPtr()->RegisterArchiveType(new PakArchiveFactory());
		
	}
	void MainWindow::Finalize()
	{
		
	}

	void MainWindow::Load()
	{
		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);

		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"classic_ui.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);
		}
		
	}
	void MainWindow::Unload()
	{
		delete m_UIskin;
	}
	void MainWindow::Update(const GameTime* const time)
	{

	}
	void MainWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_Target, 0, 1, 0);



	}

	void MainWindow::OnFrameStart()
	{

	}
	void MainWindow::OnFrameEnd()
	{

	}
}