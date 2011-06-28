#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "APDCommon.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace APDesigner
{
	class MainWindow : public RenderWindowHandler
	{
	private:
		RenderWindow* m_window;
		RenderDevice* m_device;

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