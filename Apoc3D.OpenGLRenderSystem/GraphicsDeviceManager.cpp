/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "GraphicsDeviceManager.h"
#include "Game.h"
#include "Win32GameWindow.h"

#include "Core/Logging.h"

#include "GL1DeviceContent.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			GraphicsDeviceManager::GraphicsDeviceManager(Game* game, GL1DeviceContent* devCont)
				: m_currentSetting(0), m_ignoreSizeChanges(false), m_doNotStoreBufferSize(false), m_renderingOccluded(false),
				m_deviceCreated(false),
				m_devCont(devCont)
			{
				assert(game);

				m_game = game;

				m_game->eventFrameStart()->bind(this, &GraphicsDeviceManager::game_FrameStart);
				m_game->eventFrameEnd()->bind(this, &GraphicsDeviceManager::game_FrameEnd);
				m_game->getWindow()->eventUserResized()->bind(this, &GraphicsDeviceManager::Window_UserResized);
				
				
			}

			GraphicsDeviceManager::~GraphicsDeviceManager(void)
			{
				if (m_currentSetting)
					delete m_currentSetting;
			}

	


			void GraphicsDeviceManager::ReleaseDevice()
			{
				if (!m_deviceCreated)
					return;

				if (m_game)
				{
					m_game->UnloadContent();
				}

				wglMakeCurrent(NULL,NULL);
				wglDeleteContext(m_hRC);

				HWND hWnd = m_game->getWindow()->getHandle();

				ReleaseDC(hWnd,m_hDC);
				DestroyWindow(hWnd);
				
				m_hDC = NULL;
				m_hRC = NULL;
			}



			void GraphicsDeviceManager::game_FrameStart(bool* cancel)
			{
				if (!m_deviceCreated)
					return;

				if (!m_game->getIsActive())
					Sleep(50);

			}
			void GraphicsDeviceManager::game_FrameEnd()
			{
				SwapBuffers(m_hDC);
			}
			void GraphicsDeviceManager::Window_UserResized()
			{
				// TBD
				if (m_ignoreSizeChanges || !EnsureDevice()) // || (m_currentSetting->Windowed)
					return;

				RenderParameters newSettings = *m_currentSetting;

				RECT rect;
				GetClientRect(m_game->getWindow()->getHandle(), &rect);

				int32 width = rect.right - rect.left;
				int32 height = rect.bottom - rect.top;

				if (width != newSettings.BackBufferWidth || height != newSettings.BackBufferHeight)
				{
					newSettings.BackBufferWidth = 0;
					newSettings.BackBufferHeight = 0;
					CreateDevice(newSettings);
				}
			}


			void GraphicsDeviceManager::InitializeDevice(const RenderParameters &settings)
			{
				HWND sss = m_game->getWindow()->getHandle();

				m_hDC = GetDC(sss);

				PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
				{
					sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
					1,											// Version Number
					PFD_DRAW_TO_WINDOW |						// Format Must Support Window
					PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
					PFD_DOUBLEBUFFER,							// Must Support Double Buffering
					PFD_TYPE_RGBA,								// Request An RGBA Format
					PixelFormatUtils::GetBPP(settings.ColorBufferFormat)*8,	// Select Our Color Depth
					0, 0, 0, 0, 0, 0,							// Color Bits Ignored. TODO: specify them
					0,											// No Alpha Buffer
					0,											// Shift Bit Ignored
					0,											// No Accumulation Buffer
					0, 0, 0, 0,									// Accumulation Bits Ignored
					PixelFormatUtils::GetBPP(settings.DepthBufferFormat)*8,	//  Z-Buffer (Depth Buffer)  
					PixelFormatUtils::GetStencilDepth(settings.DepthBufferFormat),	// Stencil Buffer
					0,											// No Auxiliary Buffer
					PFD_MAIN_PLANE,								// Main Drawing Layer
					0,											// Reserved
					0, 0, 0										// Layer Masks Ignored
				};
				
				int pixFmt = ChoosePixelFormat(m_hDC, &pfd);
				SetPixelFormat(m_hDC, pixFmt, &pfd);
				m_hRC = wglCreateContext(m_hDC);
				wglMakeCurrent(m_hDC, m_hRC);

 				m_game->Initialize();
				m_game->LoadContent();
			}
			void GraphicsDeviceManager::CreateDevice(const RenderParameters& settings, const DEVMODE* mode = 0)
			{
				RenderParameters* oldSettings = m_currentSetting;
				m_currentSetting = new RenderParameters(settings);

				m_ignoreSizeChanges = true;

				bool keepCurrentWindowSize = false;
				if (settings.BackBufferWidth == 0 &&
					settings.BackBufferHeight == 0)
					keepCurrentWindowSize = true;

				Win32GameWindow* wnd = m_game->getWindow();


				// check if we are going to windowed or fullscreen mode
				if (settings.IsWindowd)
				{
					if (oldSettings && !oldSettings->IsWindowd)
						SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)m_windowedStyle);
				}
				else
				{
					if (!oldSettings || oldSettings->IsWindowd)
					{
						//m_savedTopmost = wnd->getTopMost();
						long style = GetWindowLong(wnd->getHandle(), GWL_STYLE);
						style &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
						m_windowedStyle = style;

						m_windowedPlacement = WINDOWPLACEMENT();
						m_windowedPlacement.length = sizeof(WINDOWPLACEMENT);
						GetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
					}

					// hide the window until we are done messing with it
					ShowWindow(wnd->getHandle(), SW_HIDE);

					SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)(WS_POPUP | WS_SYSMENU));

					WINDOWPLACEMENT placement = WINDOWPLACEMENT();
					placement.length = sizeof(WINDOWPLACEMENT);
					GetWindowPlacement(wnd->getHandle(), &placement);

					// check if we are in the middle of a restore
					if ((placement.flags & WPF_RESTORETOMAXIMIZED) != 0)
					{
						// update the flags to avoid sizing issues
						placement.flags &= ~WPF_RESTORETOMAXIMIZED;
						placement.showCmd = SW_RESTORE;
						SetWindowPlacement(wnd->getHandle(), &placement);
					}
				}


				if (settings.IsWindowd)
				{
					if (oldSettings && !oldSettings->IsWindowd)
					{
						m_fullscreenWindowWidth = oldSettings->BackBufferWidth;
						m_fullscreenWindowHeight = oldSettings->BackBufferHeight;
					}
				}
				else
				{
					if (oldSettings && oldSettings->IsWindowd)
					{
						m_windowedWindowWidth = oldSettings->BackBufferWidth;
						m_windowedWindowHeight = oldSettings->BackBufferHeight;
					}
				}

				// check if the device can be reset, or if we need to completely recreate it
				bool canReset = true;
				if (oldSettings && oldSettings->FSAASampleCount != settings.FSAASampleCount)
				{
					canReset = false;
				}
				if (!m_deviceCreated)
					canReset = false;

				if (!canReset)
				{
					ReleaseDevice();
					LogManager::getSingleton().Write(LOG_Graphics, 
							L"[GL1]Recreating Device. ", 
							LOGLVL_Default);
					
					if (!settings.IsWindowd)
					{
						assert(mode);
						ChangeResolution(*mode);
					}

					InitializeDevice(settings);
				}
				else
				{
					if (!settings.IsWindowd)
					{
						assert(mode);
						ChangeResolution(*mode);
					}
				}

				if (GLEE_WGL_EXT_swap_control)
				{
					wglSwapIntervalEXT(settings.EnableVSync ? 1 : 0);
				}


				// check if we changed from fullscreen to windowed mode
				if (oldSettings && !oldSettings->IsWindowd && settings.IsWindowd)
				{
					SetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
					//wnd->setTopMost(m_savedTopmost);
				}

				// check if we need to resize
				if (settings.IsWindowd && !keepCurrentWindowSize)
				{
					int width;
					int height;
					if (IsIconic(wnd->getHandle()))
					{
						WINDOWPLACEMENT placement = WINDOWPLACEMENT();
						placement.length = sizeof(WINDOWPLACEMENT);
						GetWindowPlacement(wnd->getHandle(), &placement);

						// check if we are being restored
						if ((placement.flags & WPF_RESTORETOMAXIMIZED) != 0 && placement.showCmd == SW_SHOWMINIMIZED)
						{
							ShowWindow(wnd->getHandle(), SW_RESTORE);

							RECT rect;
							GetClientRect(wnd->getHandle(), &rect);

							width = rect.right - rect.left;
							height = rect.bottom - rect.top;
							ShowWindow(wnd->getHandle(), SW_MINIMIZE);
						}
						else
						{
							RECT frame;
							AdjustWindowRect(&frame, (uint32)m_windowedStyle, false);
							int frameWidth = frame.right - frame.left;
							int frameHeight = frame.bottom - frame.top;

							width = placement.rcNormalPosition.right - placement.rcNormalPosition.left - frameWidth;
							height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top - frameHeight;
						}
					}
					else
					{
						RECT rect;
						GetClientRect(wnd->getHandle(), &rect);

						width = rect.right - rect.left;
						height = rect.bottom - rect.top;
					}

					// check if we have a different desired size
					if (width != settings.BackBufferWidth ||
						height != settings.BackBufferHeight)
					{
						if (IsIconic(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);
						if (IsZoomed(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);

						RECT rect = RECT();
						rect.right = settings.BackBufferWidth;
						rect.bottom = settings.BackBufferHeight;
						AdjustWindowRect(& rect,
							GetWindowLong(wnd->getHandle(), GWL_STYLE), false);



						int clientWidth = rect.right - rect.left;
						int clientHeight = rect.bottom - rect.top;

						int scrnWidth = GetSystemMetrics(SM_CXFULLSCREEN);   
						int scrnHeight = GetSystemMetrics(SM_CYFULLSCREEN);



						SetWindowPos(wnd->getHandle(), 0, 
							(scrnWidth - clientWidth)>>1, (scrnHeight - clientHeight)>>1, 
							clientWidth, clientHeight, SWP_NOZORDER);

						RECT r;
						GetClientRect(wnd->getHandle(), &r);

						clientWidth = r.right - r.left;
						clientHeight = r.bottom - r.top;

						// check if the size was modified by Windows
						if (clientWidth != settings.BackBufferWidth ||
							clientHeight != settings.BackBufferHeight)
						{
							RenderParameters newSettings = settings;
							newSettings.BackBufferWidth = 0;
							newSettings.BackBufferHeight = 0;

							CreateDevice(newSettings);
						}
					}
				}

				// if the window is still hidden, make sure it is shown
				if (!IsWindowVisible(wnd->getHandle()))
					ShowWindow(wnd->getHandle(), SW_SHOW);

				// set the execution state of the thread
				if (!m_currentSetting->IsWindowd)
					SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
				else
					SetThreadExecutionState(ES_CONTINUOUS);

				m_ignoreSizeChanges = false;


				if (oldSettings)
					delete oldSettings;
			}
			void GraphicsDeviceManager::ChangeDevice(const RenderParameters& settings, const RenderParameters* minimumSettings)
			{
				// FSAA is currently not supported and will not be checked.

				// enumeration to find the optimal best settings

				RenderParameters validSettings = settings;
				DEVMODE dmode;
				float bestModeScore = 0;
				if (!settings.IsWindowd)
				{
					int index = 0;
					DEVMODE dm;
					// initialize the DEVMODE structure
					ZeroMemory(&dm, sizeof(dm));
					dm.dmSize = sizeof(dm);

					while (0 != EnumDisplaySettings(NULL, index++, &dm))
					{
						// inspect the DEVMODE structure to obtain details
						// about the display settings such as
						//  - Orientation
						//  - Width and Height
						//  - Frequency
						//  - etc.
						
						int bpp = PixelFormatUtils::GetBPP(settings.ColorBufferFormat);
						if (bpp * 8 != (int)dm.dmBitsPerPel)
						{
							continue;
						}

						float score = abs(settings.BackBufferWidth- (int)dm.dmPelsWidth) + abs((int)dm.dmPelsHeight-settings.BackBufferHeight);
						if (score > bestModeScore)
						{
							bestModeScore = score;
							dmode = dm;
						}
					}

					validSettings.BackBufferWidth = (int)dmode.dmPelsWidth;
					validSettings.BackBufferHeight = (int)dmode.dmPelsHeight;
				}
				

				CreateDevice(validSettings, &dmode);
				
			}
			void GraphicsDeviceManager::ChangeDevice(bool windowed, int desiredWidth, int desiredHeight)
			{
				RenderParameters desiredSettings = RenderParameters();
				desiredSettings.IsWindowd = (windowed);
				desiredSettings.BackBufferWidth = (desiredWidth);
				desiredSettings.BackBufferHeight = (desiredHeight);

				ChangeDevice(desiredSettings, 0);
			}
			void GraphicsDeviceManager::ChangeDevice(const RenderParameters &prefer)
			{
				ChangeDevice(prefer, 0);
			}
			void GraphicsDeviceManager::ToggleFullScreen()
			{
				assert(EnsureDevice());

				RenderParameters newSettings = *m_currentSetting;
				newSettings.IsWindowd =  !newSettings.IsWindowd;

				int width = newSettings.IsWindowd ? m_windowedWindowWidth :  m_fullscreenWindowWidth;
				int height = newSettings.IsWindowd ?  m_windowedWindowHeight :  m_fullscreenWindowHeight;

				newSettings.BackBufferWidth =  width;
				newSettings.BackBufferHeight =  height;

				ChangeDevice(newSettings);
			}
		}
	}
}