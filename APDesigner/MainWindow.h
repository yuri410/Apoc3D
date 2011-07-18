#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "APDCommon.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;

namespace APDesigner
{
	class MainWindow : public RenderWindowHandler
	{
	private:
		StyleSkin* m_UIskin;

		RenderWindow* m_window;
		RenderDevice* m_device;

		Sprite* m_sprite;
	public:
		MainWindow(RenderWindow* wnd);
		void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();
	};
}

#endif