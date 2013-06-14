#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"

namespace rex
{
	class Win32AppHost
	{
	public:
		/** This method will ask the GraphicsDeviceManager to release resources. And 
		 *  will cause the Game::UnloadContent to be called.
		 */
		virtual void Release();

		/** When derived, this method should be overrided to initialize the graphics device,
		 *  with the specific parameters and settings provided.
		 *  As GraphicsDeviceManager has creates the device, Game::LoadContent and Game::Initialize
		 *  will be called.
		 */
		virtual void Create(const RenderParameters& params);

		GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_graphicsDeviceManager; }
		Win32AppWindow* getWindow() const { return m_gameWindow; }
		ID3D11Device* getDevice() const;
		bool getIsExiting() const { return m_exiting; }
		bool getIsActive() const { return m_active; }

		virtual void Initialize() = 0;
		virtual void LoadContent() = 0;
		virtual void UnloadContent() = 0;

		/** This should be overrided to draw a frame
			*/
		virtual void Render(const GameTime* const time) = 0;
		/** This should be overrided to allow the game to run logic such as updating the world,
			*  checking for collisions, gathering input, playing audio and etc.
			*/
		virtual void Update(const GameTime* const time) = 0;
		/** Enters the main loop. 
		*/
		void Run();
		void Tick();
		void Exit();

		CancellableEventHandler eventFrameStart;
		EventHandler eventFrameEnd;
	private:
		int m_maxSkipFrameCount;
		GameClock* m_gameClock;
		float m_maxElapsedTime;
		float m_totalGameTime;
		float m_accumulatedElapsedGameTime;
		float m_lastFrameElapsedGameTime;
		float m_lastFrameElapsedRealTime;
		float m_targetElapsedTime;
		float m_inactiveSleepTime;
		int32 m_updatesSinceRunningSlowly1;
		int32 m_updatesSinceRunningSlowly2;
		bool m_forceElapsedTimeToZero;
		bool m_drawRunningSlowly;
		int64 m_lastUpdateFrame;
		float m_lastUpdateTime;
		float m_fps;

		bool m_exiting;
		bool m_active;

		GraphicsDeviceManager* m_graphicsDeviceManager;
		Win32AppWindow* m_gameWindow;



		void DrawFrame(const GameTime* const time);

		void Window_ApplicationActivated();
		void Window_ApplicationDeactivated();
		void Window_Suspend();
		void Window_Resume();
		void Window_Paint();

	protected:

		Win32AppHost(const String& name);
		virtual ~Win32AppHost(void);
		virtual bool OnFrameStart();
		virtual void OnFrameEnd();


	};
}