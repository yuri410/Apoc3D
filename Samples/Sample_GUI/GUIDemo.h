
#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "DemoCommon.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "apoc3d/Math/Point.h"
#include "Apoc3D.Essentials/Game.h"

using namespace Apoc3DEx;
using namespace Apoc3D;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::UI;

namespace Sample_GUI
{
    class GUIDemo : public Game
	{
	public:
		static const int DMCount = 8;
		GUIDemo(RenderWindow* wnd);
		~GUIDemo();


		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

	private:

		void ShowAbout(MenuItem* ctrl);


		Form* m_aboutDlg;

		Menu* m_mainMenu;

		Sprite* m_sprite;
    };
}
#endif
