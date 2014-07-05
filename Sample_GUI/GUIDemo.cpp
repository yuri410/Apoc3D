
#include "GUIDemo.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/Label.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/UILib/PictureBox.h"
#include "apoc3d/UILib/ComboBox.h"
#include "apoc3d/UILib/CheckBox.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Platform/Thread.h"

#include <cstddef>

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


namespace Sample_GUI
{
    GUIDemo::GUIDemo(RenderWindow* wnd)
        : Game(wnd)
    {
		
    }

	GUIDemo::~GUIDemo()
	{		

	}

	void GUIDemo::OnFrameStart() { }
	void GUIDemo::OnFrameEnd() { }
	void GUIDemo::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();
		
		m_sprite->Begin(Sprite::SPRMix_RestoreStateAlphaBlended);
		
		const StyleSkin* skin = getSystemUISkin();

		for (int32 i=0;i<15;i++)
		{
			//Apoc3D::Math::Rectangle drect(20, i*40+40, i, 40);
			Point pos(i*40+40,40);
			guiDrawRegion3Vert(m_sprite, pos, i, CV_White, skin->SkinTexture, skin->VScrollBarCursor);
		}

		for (int32 i=0;i<15;i++)
		{
			//Apoc3D::Math::Rectangle drect(20, i*40+40, i, 40);
			Point pos(40,i*40+100);
			guiDrawRegion3(m_sprite, pos, i, CV_White, skin->SkinTexture, skin->HScrollBarCursor);
		}

		m_sprite->End();

		Game::Draw(time);



		m_device->EndFrame();

	}
	void GUIDemo::Initialize()
	{
		m_window->setTitle(L"GUI Demo");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
		//m_audioEngine = new Sounds(this);
	}
	void GUIDemo::Finalize()
	{
		Game::Finalize();
	}
	void GUIDemo::Load()
	{
		Game::Load();



		m_console->Minimize();


		m_mainMenu = new Menu();
		m_mainMenu->SetSkin(m_UIskin);


		{
			MenuItem* pojMenu = new MenuItem(L"Demos");
			SubMenu* pojSubMenu = new SubMenu(0);
			pojSubMenu->SetSkin(m_UIskin);

			MenuItem* mi=new MenuItem(L"Histogram Processing");
			//mi->event().Bind(this, &GUIDemo::Work11);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Noise Generation and Reduction");
			//mi->event().Bind(this, &GUIDemo::Work12);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Transformation");
			//mi->event().Bind(this, &GUIDemo::Work13);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"-");
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Edge Filters");
			//mi->event().Bind(this, &GUIDemo::Work21);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Freq Domain Filters");
			//mi->event().Bind(this, &GUIDemo::Work22);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Huffman");
			//mi->event().Bind(this, &GUIDemo::Work23);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Clustering");
			//mi->event().Bind(this, &GUIDemo::WorkDM);
			pojSubMenu->Add(mi,0);


			m_mainMenu->Add(pojMenu,pojSubMenu);
		}

		{
			MenuItem* aboutMenu = new MenuItem(L"Help");
			
			SubMenu* aboutSubMenu = new SubMenu(0);
			aboutSubMenu->SetSkin(m_UIskin);

			MenuItem* mi=new MenuItem(L"About...");
			mi->event().Bind(this, &GUIDemo::ShowAbout);
			aboutSubMenu->Add(mi,0);

			m_mainMenu->Add(aboutMenu,aboutSubMenu);
		}


		m_mainMenu->Initialize(m_device);
		UIRoot::setMainMenu(m_mainMenu);

		{
			m_aboutDlg = new Form(FBS_Fixed, L"About");
			m_aboutDlg->SetSkin(m_UIskin);
			m_aboutDlg->Size.X = 460;
			m_aboutDlg->Size.Y = 175;

			Label* lbl = new Label(Point(30+128,30),
				L"Test. \n"
				L"Powered by Apoc3D.", 
				m_aboutDlg->Size.X-40-128, Label::ALIGN_Left);
			lbl->SetSkin(m_UIskin);

			PictureBox* photo = new PictureBox(Point(15,30),1);
			photo->Size = Point(128, 128);
			photo->SetSkin(m_UIskin);

			m_aboutDlg->getControls().Add(photo);
			m_aboutDlg->getControls().Add(lbl);

			m_aboutDlg->Initialize(m_device);
			UIRoot::Add(m_aboutDlg);
		}
	}

	void GUIDemo::Unload()
	{
		Game::Unload();


		delete m_sprite;
	}

	void GUIDemo::Update(const GameTime* const time)
	{
		Game::Update(time);
	}

	void GUIDemo::ShowAbout(MenuItem* ctrl)
	{
		m_aboutDlg->ShowModal();
	}
}
