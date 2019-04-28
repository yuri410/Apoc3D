#include "Win32RenderWindow.h"
#include "Win32Window.h"
#include "Win32Clock.h"
#include "apoc3d/Core/GameTime.h"

namespace Apoc3D
{
	namespace Win32
	{
		Win32RenderWindow::Win32RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm)
			: RenderWindow(dc, rd, pm)
		{
			m_gameClock = new Win32Clock();

			m_gameWindow = new Win32Window(Win32::WindowClass, L"");
			m_gameWindow->eventApplicationActivated.Bind(this, &Win32RenderWindow::Window_ApplicationActivated);
			m_gameWindow->eventApplicationDeactivated.Bind(this, &Win32RenderWindow::Window_ApplicationDeactivated);
			m_gameWindow->eventPaint.Bind(this, &Win32RenderWindow::Window_Paint);
			m_gameWindow->eventResume.Bind(this, &Win32RenderWindow::Window_Resume);
			m_gameWindow->eventSuspend.Bind(this, &Win32RenderWindow::Window_Suspend);
		}

		Win32RenderWindow::~Win32RenderWindow()
		{
			m_gameWindow->eventApplicationActivated.Unbind(this, &Win32RenderWindow::Window_ApplicationActivated);
			m_gameWindow->eventApplicationDeactivated.Unbind(this, &Win32RenderWindow::Window_ApplicationDeactivated);
			m_gameWindow->eventPaint.Unbind(this, &Win32RenderWindow::Window_Paint);
			m_gameWindow->eventResume.Unbind(this, &Win32RenderWindow::Window_Resume);
			m_gameWindow->eventSuspend.Unbind(this, &Win32RenderWindow::Window_Suspend);

			delete m_gameWindow;

			delete m_gameClock;
		}

		void Win32RenderWindow::ChangeRenderParameters(const RenderParameters& params)
		{
			RenderWindow::ChangeRenderParameters(params);

			if (m_renderDevice == nullptr)
				ExecuteChangeDevice();
			else
				m_hasPendingDeviceChange = true;
		}

		void Win32RenderWindow::Exit()
		{
			RenderWindow::Exit();

			m_gameWindow->Close();
		}

		void Win32RenderWindow::Minimize()
		{
			if (m_visisble)
				ShowWindow(m_gameWindow->getHandle(), SW_MINIMIZE);
		}
		void Win32RenderWindow::Restore()
		{
			if (m_visisble)
				ShowWindow(m_gameWindow->getHandle(), SW_RESTORE);
		}
		void Win32RenderWindow::Maximize()
		{
			if (m_visisble)
				ShowWindow(m_gameWindow->getHandle(), SW_MAXIMIZE);
		}


		String Win32RenderWindow::getTitle() { return m_gameWindow->getWindowTitle(); }
		void Win32RenderWindow::setTitle(const String& name) { m_gameWindow->setWindowTitle(name); }

		Size Win32RenderWindow::getClientSize() { return m_gameWindow->getCurrentSize(); }

		void Win32RenderWindow::SetVisible(bool v)
		{
			ShowWindow(m_gameWindow->getHandle(), v ? SW_NORMAL : SW_HIDE);

			m_visisble = v;
		}
		
		//////////////////////////////////////////////////////////////////////////
		
		void Win32RenderWindow::MainLoop()
		{
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					Tick();
				}
			}
		}

		void Win32RenderWindow::Tick()
		{
			if (m_isExiting)
				return;

			if (!m_active && m_inactiveSleepTime > 0)
				Sleep(m_inactiveSleepTime);

			m_gameClock->Step();

			float dt = (float)m_gameClock->getElapsedTime();
			float totalRealTime = (float)m_gameClock->getCurrentTime();

			if (dt < 0)
				dt = 0;

			float fps = getFPS();

			int32 maxFrameSkip = m_maxSkipFrameCount;
#if _DEBUG
			if (maxFrameSkip > 5)
				maxFrameSkip = 5;
#endif

			bool renderingSlow = m_accumulatedDt_fixedStep > m_referenceElapsedTime * 1.2f;

			if (renderingSlow)
				m_slowRenderFrameHits++;
			else
				m_slowRenderFrameHits--;

			renderingSlow = m_slowRenderFrameHits > 5;

			if (m_hasPendingDeviceChange)
			{
				m_hasPendingDeviceChange = false;

				ExecuteChangeDevice();
			}

			if (m_timeStepMode == TimeStepMode::FixedStep)
			{
				m_accumulatedDt_fixedStep += dt;

				// keep up update calls:
				// update until it's time to draw the next frame

				int32 numUpdatesNeeded = (int32)(m_accumulatedDt_fixedStep / m_referenceElapsedTime);
				if (numUpdatesNeeded > maxFrameSkip)
					numUpdatesNeeded = maxFrameSkip;

				float iterationFullDt = m_accumulatedDt_fixedStep;
				m_accumulatedDt_fixedStep = fmod(fabs(m_accumulatedDt_fixedStep), m_referenceElapsedTime);
				iterationFullDt -= m_accumulatedDt_fixedStep;

				for (int32 i = 0; i < numUpdatesNeeded; i++)
				{
					if (IsDeviceReady())
					{
						GameTime gt(m_referenceElapsedTime, iterationFullDt / numUpdatesNeeded, m_accumulatedDt_fixedStep, numUpdatesNeeded, fps, renderingSlow);
						OnUpdate(&gt);
					}
				}

				if (dt > 0)
				{
					numUpdatesNeeded = (int32)(ceil(dt / m_referenceElapsedTime));
					if (numUpdatesNeeded > maxFrameSkip)
						numUpdatesNeeded = maxFrameSkip;
					if (numUpdatesNeeded < 1)
						numUpdatesNeeded = 1;

					float dtPerIteration = dt / numUpdatesNeeded;
					if (dtPerIteration > m_referenceElapsedTime)
						dtPerIteration = m_referenceElapsedTime;

					for (int32 i = 0; i < numUpdatesNeeded; i++)
					{
						if (IsDeviceReady())
						{
							GameTime gt(dtPerIteration, dt / numUpdatesNeeded, m_accumulatedDt_fixedStep, numUpdatesNeeded, fps, renderingSlow);
							OnUpdateConstrainedVarTimeStep(&gt);
						}
					}
				}

				GameTime gt(iterationFullDt, dt, m_accumulatedDt_fixedStep, 1, fps, renderingSlow);
				OnRenderFrame(&gt);
			}
			else if (m_timeStepMode == TimeStepMode::Constrained)
			{
				if (dt > 0)
				{
					int32 numUpdatesNeeded = (int32)(ceil(dt / m_referenceElapsedTime));
					if (numUpdatesNeeded > maxFrameSkip)
						numUpdatesNeeded = maxFrameSkip;
					if (numUpdatesNeeded < 1)
						numUpdatesNeeded = 1;

					float dtPerIteration = dt / numUpdatesNeeded;
					if (dtPerIteration > m_referenceElapsedTime)
						dtPerIteration = m_referenceElapsedTime;

					for (int32 i = 0; i < numUpdatesNeeded; i++)
					{
						if (IsDeviceReady())
						{
							GameTime gt(dtPerIteration, dt / numUpdatesNeeded, 0, numUpdatesNeeded, fps, renderingSlow);
							OnUpdate(&gt);
						}
					}
				}

				GameTime gt(dt, dt, 0, 1, fps, renderingSlow);
				OnRenderFrame(&gt);
			}
			else
			{
				GameTime gt(dt, fps);

				if (IsDeviceReady())
				{
					OnUpdate(&gt);
				}
				OnRenderFrame(&gt);
			}
		}

		void Win32RenderWindow::Window_ApplicationActivated()
		{
			m_active = true;
		}
		void Win32RenderWindow::Window_ApplicationDeactivated()
		{
			m_active = false;
		}
		void Win32RenderWindow::Window_Suspend()
		{
			//m_gameClock->Suspend();
		}
		void Win32RenderWindow::Window_Resume()
		{
			//m_gameClock->Resume();
		}
		void Win32RenderWindow::Window_Paint()
		{
			
		}
	}
}