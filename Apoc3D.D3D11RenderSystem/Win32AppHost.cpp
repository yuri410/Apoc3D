#include "Win32AppHost.h"
#include "Win32AppWindow.h"
#include "GraphicsDeviceManager.h"
#include "GameClock.h"
#include "GameTime.h"

namespace rex
{
	Win32AppHost::Win32AppHost(const String& name)
		: m_maxElapsedTime(0.5f), m_targetElapsedTime(1.0f / 60.0f), m_inactiveSleepTime(20),
		m_updatesSinceRunningSlowly1(MAXINT32), m_updatesSinceRunningSlowly2(MAXINT32),
		m_exiting(false),
		m_accumulatedElapsedGameTime(0), m_lastFrameElapsedGameTime(0), m_lastFrameElapsedRealTime(0),
		m_totalGameTime(0), m_forceElapsedTimeToZero(false), m_drawRunningSlowly(false), m_lastUpdateFrame(0),
		m_lastUpdateTime(0), m_fps(0),
		m_maxSkipFrameCount(10)
	{
		m_gameClock = new GameClock();

		m_gameWindow = new Win32AppWindow(name, name);
		m_gameWindow->eventApplicationActivated.Bind(this, &Win32AppHost::Window_ApplicationActivated);
		m_gameWindow->eventApplicationDeactivated.Bind(this, &Win32AppHost::Window_ApplicationDeactivated);
		m_gameWindow->eventPaint.Bind(this, &Win32AppHost::Window_Paint);
		m_gameWindow->eventResume.Bind(this, &Win32AppHost::Window_Resume);
		m_gameWindow->eventSuspend.Bind(this, &Win32AppHost::Window_Suspend);

		m_graphicsDeviceManager = new GraphicsDeviceManager(this);
	}
	void Win32AppHost::Release()
	{
		m_graphicsDeviceManager->ReleaseDevice();
	}
	void Win32AppHost::Create(const RenderParameters& params)
	{
		m_gameWindow->Load(params.BackBufferWidth, params.BackBufferHeight, params.IsFixedWindow);
	}
	Win32AppHost::~Win32AppHost()
	{
		m_gameWindow->eventApplicationActivated.Unbind(this, &Win32AppHost::Window_ApplicationActivated);
		m_gameWindow->eventApplicationDeactivated.Unbind(this, &Win32AppHost::Window_ApplicationDeactivated);
		m_gameWindow->eventPaint.Unbind(this, &Win32AppHost::Window_Paint);
		m_gameWindow->eventResume.Unbind(this, &Win32AppHost::Window_Resume);
		m_gameWindow->eventSuspend.Unbind(this, &Win32AppHost::Window_Suspend);

		delete m_graphicsDeviceManager;
		delete m_gameWindow;

		delete m_gameClock;
	}

	ID3D11Device* Win32AppHost::getDevice() const
	{
		return m_graphicsDeviceManager->getD3D11Device();
	}

	bool Win32AppHost::OnFrameStart()
	{
		bool re = false;
		eventFrameStart.Invoke(&re);
		return re;
	}

	void Win32AppHost::OnFrameEnd()
	{
		eventFrameEnd.Invoke();
	}

	void Win32AppHost::DrawFrame(const GameTime* const time)
	{
		//try
		{
			if (!m_gameWindow->getIsMinimized())
			{
				if (!OnFrameStart())
				{
					Render(time);
					OnFrameEnd();
				}
			}
		}
		//catch (Apoc3DException &e)
		//{
		//m_lastFrameElapsedGameTime = 0;
		//m_lastFrameElapsedRealTime = 0;
		//}
		m_lastFrameElapsedGameTime = 0;
		m_lastFrameElapsedRealTime = 0;
	}
	void Win32AppHost::Exit()
	{
		m_exiting = true;
		m_gameWindow->Close();
	}

	void Win32AppHost::Run()
	{
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT)
		{			
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				Tick();
			}
		}


	}

	void Win32AppHost::Tick()
	{
		if (m_exiting)
			return;

		if (!m_active)
			Sleep(static_cast<int>(m_inactiveSleepTime));

		m_gameClock->Step();

		float elapsedRealTime = (float)m_gameClock->getElapsedTime();
		float totalRealTime = (float)m_gameClock->getCurrentTime();

		m_lastFrameElapsedRealTime += (float)m_gameClock->getElapsedTime();

		float elapsedAdjustedTime = m_gameClock->getElapsedAdjustedTime();
		if (elapsedAdjustedTime < 0)
			elapsedAdjustedTime = 0;

		if (m_forceElapsedTimeToZero)
		{
			elapsedRealTime = 0;
			m_lastFrameElapsedRealTime = elapsedAdjustedTime = 0;
			m_forceElapsedTimeToZero = false;
		}

		m_accumulatedElapsedGameTime += elapsedAdjustedTime;

		float targetElapsedTime = m_targetElapsedTime;
		float ratio = m_accumulatedElapsedGameTime / m_targetElapsedTime;


		m_accumulatedElapsedGameTime = fmod(m_accumulatedElapsedGameTime, targetElapsedTime);
		m_lastFrameElapsedGameTime = 0;

		if (ratio == 0)
			return;


		if (ratio > 1)
		{
			m_updatesSinceRunningSlowly2 = m_updatesSinceRunningSlowly1;
			m_updatesSinceRunningSlowly1 = 0;
		}
		else
		{
			if (m_updatesSinceRunningSlowly1 < MAXINT32)
				m_updatesSinceRunningSlowly1++;
			if (m_updatesSinceRunningSlowly2 < MAXINT32)
				m_updatesSinceRunningSlowly2++;
		}

		m_drawRunningSlowly = m_updatesSinceRunningSlowly2 < 20;

#if _DEBUG
		if (ratio>m_maxSkipFrameCount)
			ratio=0;
#else
		if (ratio>m_maxSkipFrameCount)
			ratio= static_cast<float>(m_maxSkipFrameCount);
#endif

		// keep up update calls:
		// update until it's time to draw the next frame
		while (ratio > 1)
		{
			ratio -= 1;

			//try
			{
				GameTime gt(m_targetElapsedTime, m_totalGameTime,
					elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
				Update(&gt);
			}
			//catch (Apoc3DException &e)
			//{
			//m_lastFrameElapsedGameTime += targetElapsedTime;
			//m_totalGameTime += targetElapsedTime;
			//}

			m_lastFrameElapsedGameTime += targetElapsedTime;
			m_totalGameTime += targetElapsedTime;

		}

		{
			GameTime gt(m_targetElapsedTime, m_totalGameTime,
				elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
			DrawFrame(&gt);
		}


		// refresh the FPS counter once per second
		m_lastUpdateFrame++;
		if (totalRealTime - m_lastUpdateTime > 1.0f)
		{
			m_fps = (float)m_lastUpdateFrame / (float)(totalRealTime - m_lastUpdateTime);
			m_lastUpdateTime = totalRealTime;
			m_lastUpdateFrame = 0;
		}
	}

	void Win32AppHost::Window_ApplicationActivated()
	{
		m_active = true;
	}
	void Win32AppHost::Window_ApplicationDeactivated()
	{
		m_active = false;
	}
	void Win32AppHost::Window_Suspend()
	{
		m_gameClock->Suspend();
	}
	void Win32AppHost::Window_Resume()
	{
		m_gameClock->Resume();
	}
	void Win32AppHost::Window_Paint()
	{
		//#if _DEBUG
		// If WM_PAINT frame have errors when debugging in windows, the error dlgs 
		// can not be displayed. 

		// The frame fails and another WM_PAINT will arrive in attempt to render the window.
		// And if the error can not be recovered,
		// WM_PAINT frames will arrive and fail again and again. Causing normal loop render unable
		// to start. And in windows error dialogs cannot display.
		return;
		//#endif
		//// the paint event may be raised before device init -- just created wnd class
		//if (getDevice())
		//{

		//	const float elapsedRealTime = (float)m_gameClock->getElapsedTime();
		//	const float totalRealTime = (float)m_gameClock->getCurrentTime();

		//	GameTime gt(m_targetElapsedTime, m_totalGameTime,
		//		elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
		//	DrawFrame(&gt);
		//}		
	}
}