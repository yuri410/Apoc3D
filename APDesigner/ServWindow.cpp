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
		: m_window(wnd), m_UIskin(0), m_projectFilePath(projectFilePath)
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
	}
	void ServWindow::Finalize()
	{
		InputAPIManager::getSingleton().FinalizeInput();
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
		UIRoot::Initialize(m_device);

		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();

	}
	void ServWindow::Unload()
	{
		UIRoot::Finalize();
		delete m_UIskin;
		delete m_sprite;

	}

	void ServWindow::Update(const GameTime* const time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		UIRoot::Update(time);

		BuildInterface::getSingleton().MainThreadUpdate(time);
	}
	void ServWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1, 0);

		m_device->BeginFrame();

		UIRoot::Draw();

		m_device->EndFrame();

	}
}