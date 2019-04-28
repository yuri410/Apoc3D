#pragma once

#include "Win32Common.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Win32
	{
		class Win32RenderWindow : public RenderWindow
		{
		public:
			~Win32RenderWindow();

			virtual void ChangeRenderParameters(const RenderParameters& params) override;
			virtual void Exit() override;

			virtual void Minimize() override;
			virtual void Restore() override;
			virtual void Maximize() override;

			virtual String getTitle() override;
			virtual void setTitle(const String& name) override;

			virtual void SetVisible(bool v) override;
			virtual Size getClientSize() override;

			virtual bool getIsActive() const { return m_active; }

			Win32Window* getWindow() const { return m_gameWindow; }

		protected:
			Win32RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm);

			virtual void ExecuteChangeDevice() = 0;
			virtual bool IsDeviceReady() = 0;
			virtual void OnRenderFrame(const GameTime* time) = 0;

			void MainLoop();
			void Tick();

			void Window_ApplicationActivated();
			void Window_ApplicationDeactivated();
			void Window_Suspend();
			void Window_Resume();
			void Window_Paint();


			Win32Window* m_gameWindow;
			Win32Clock* m_gameClock;

			int32 m_maxSkipFrameCount = 10;
			int32 m_slowRenderFrameHits = 0;

			bool m_active = false;
			bool m_hasPendingDeviceChange = false;
		};
	}
}
