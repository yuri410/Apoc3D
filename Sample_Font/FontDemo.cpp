#include "FontDemo.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/CommandInterpreter.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/ModelManager.h"
#include "apoc3d/Graphics/Model.h"
#include "apoc3d/Graphics/Mesh.h"
#include "apoc3d/Scene/OctreeSceneManager.h"
#include "apoc3d/Scene/SceneRenderer.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Keyboard.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/PerlinNoise.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/Platform/Thread.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


namespace SampleFont
{
	static const String mlText = 
		L"Language Learning and Teaching\n"
		L"外国語の学習と教授\n"
		L"Изучение и обучение иностранных языков\n"
		L"Tere Daaheng Aneng Karimah\n"
		L"語文教學・语文教学\n"
		L"Enseñanza y estudio de idiomas\n"
		L"Изучаване и Преподаване на Чужди Езици\n"
		L"Lus kawm thaib qhia\n"
		L"Ngôn Ngữ, Sự học,\n"
		L"L'enseignement et l'étude des langues\n"
		L"Nauka języków obcych\n"
		L"\n"
		L"あいうえおかがきぎくぐけこさざしじすせそただちぢつてとなにぬねのはばぱひふへほまみむ\n"
		L"안녕하세요(This font does not support Korean)";

	FontDemo::FontDemo(RenderWindow* wnd)
		: Game(wnd)
	{
	}

	FontDemo::~FontDemo()
	{		

	}

	void FontDemo::Initialize()
	{
		m_window->setTitle(L"Font Demo");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void FontDemo::Finalize()
	{
		Game::Finalize();
	}

	void FontDemo::Load()
	{
		Game::Load();

		{
			FileLocation* fl = FileSystem::getSingleton().Locate(L"uming14.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming14", fl);

			fl = FileSystem::getSingleton().Locate(L"uming22.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming22", fl);
		}

		m_console->Minimize();

	}
	void FontDemo::Unload()
	{
		delete m_sprite;

		Game::Unload();
	}
	void FontDemo::Update(const GameTime* const time)
	{
		Game::Update(time);
	}
	void FontDemo::Draw(const GameTime* const time)
	{
		m_device->BeginFrame();
		m_device->Clear(CLEAR_ColorBuffer, 0, 1, 0);

		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_AlphaBlended | Sprite::SPR_RestoreState));
		
		Font* uming14 = FontManager::getSingleton().getFont(L"uming14");
		Font* uming22 = FontManager::getSingleton().getFont(L"uming22");

		uming22->DrawString(m_sprite, mlText, 0,0, m_window->getClientSize().Width, CV_White);



		Font* english = FontManager::getSingleton().getFont(L"english");
		english->DrawString(m_sprite, L"FPS: " + StringUtils::ToString(time->getFPS(),1), Point(0, m_window->getClientSize().Height-50), CV_White);

		m_sprite->End();

		Game::Draw(time);

		m_device->EndFrame();
	}

	void FontDemo::OnFrameStart() { }
	void FontDemo::OnFrameEnd() { }

}