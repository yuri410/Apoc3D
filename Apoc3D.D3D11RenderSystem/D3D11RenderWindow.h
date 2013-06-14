#pragma once

#include "D3D11Common.h"
#include "Win32AppHost.h"
#include "D3D11RenderDevice.h"

namespace rex
{
	class D3D11RenderWindow : public RenderWindow
	{
	private:
		/** 
		 * This D3D11Game class redirect the events from Win32AppHost to a more clean interface - RenderWindow
		 */
		class D3D11Game : public Win32AppHost
		{
		private:
			D3D11RenderWindow* m_window;

		public:
			D3D11Game(D3D11RenderWindow* wnd)
				: Win32AppHost(L"Experimental - Direct3D11"), m_window(wnd)
			{
			}
			virtual void Create(const RenderParameters& params);
					
			virtual void Release() 
			{
				// First use this to make sure the Unload is handled first
				Win32AppHost::Release();
				// then finalize
				m_window->OnFinalize();
			}
			virtual void Initialize();
			virtual void LoadContent() { m_window->OnLoad(); }
			virtual void UnloadContent() { m_window->OnUnload(); }
			virtual void Render(const GameTime* const time) { m_window->OnDraw(time); }
			virtual void Update(const GameTime* const time) { m_window->OnUpdate(time); }
			virtual bool OnFrameStart()
			{
				if (Win32AppHost::OnFrameStart())
				{
					m_window->OnFrameStart();
					return true;
				}
				return false;
			}
			virtual void OnFrameEnd()
			{
				m_window->OnFrameEnd();
				Win32AppHost::OnFrameEnd();
			}
		};
	public:

		virtual void ChangeRenderParameters(const RenderParameters& params);

		D3D11RenderWindow(D3D11RenderDevice* device, D3D11DeviceContext* dc, const RenderParameters& pm);
		~D3D11RenderWindow();

		virtual void Exit();
		virtual void Run();

		virtual String getTitle();
		virtual void setTitle(const String& name);

		virtual Size getClientSize();

		virtual bool getIsActive() const;

		const String& getHardwareName() const { return m_hardwareName; }

		virtual void SetVisible(bool v);
	private:
		D3D11Game* m_game;
		D3D11DeviceContext* m_dc;
		String m_hardwareName;

		void setDevice(RenderDevice* device);
	};
}