#include "MainWindow.h"

#include "Core/GameTime.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"

#include "Input/InputAPI.h"

using namespace Apoc3D::Input;

namespace APDesigner
{
	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd)
	{

	}

	void MainWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		m_window->setTitle(L"Apoc3D Designer");

		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);
	}
	void MainWindow::Finalize()
	{

	}

	void MainWindow::Load()
	{

	}
	void MainWindow::Unload()
	{

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