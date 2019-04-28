/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
 *
 * This content of this file is subject to the terms of the Mozilla Public
 * License v2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This program is distributed in the hope that it will be useful,
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the
 * Mozilla Public License for more details.
 *
 * ------------------------------------------------------------------------
 */

#include "GraphicsDeviceManager.h"
#include "GL3RenderWindow.h"

#include "apoc3d.Win32/Win32Window.h"
#include "apoc3d/Core/Logging.h"

#include "GL3DeviceContext.h"
#include "GL/wgl.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GraphicsDeviceManager::GraphicsDeviceManager(GL3RenderWindow* game)
				: m_game(game)
			{
				assert(game);

				m_game->getWindow()->eventUserResized.Bind(this, &GraphicsDeviceManager::Window_UserResized);

				m_windowedStyle = GetWindowLong(m_game->getWindow()->getHandle(), GWL_STYLE);
			}

			GraphicsDeviceManager::~GraphicsDeviceManager(void)
			{
				m_game->getWindow()->eventUserResized.Unbind(this, &GraphicsDeviceManager::Window_UserResized);

				if (m_currentSetting)
					delete m_currentSetting;
			}

			void GraphicsDeviceManager::ReleaseDevice()
			{
				if (!m_deviceCreated)
					return;

				if (m_game)
				{
					m_game->GL_UnloadContent();
				}

				wglMakeCurrent(NULL,NULL);
				wglDeleteContext(m_hRC);

				HWND hWnd = m_game->getWindow()->getHandle();

				ReleaseDC(hWnd,m_hDC);
				DestroyWindow(hWnd);

				m_hDC = NULL;
				m_hRC = NULL;
			}

			void GraphicsDeviceManager::Present()
			{
				if (wglGetCurrentContext() != m_hRC)
					wglMakeCurrent(m_hDC, m_hRC);

				SwapBuffers(m_hDC);
			}
			void GraphicsDeviceManager::Window_UserResized()
			{
				// TBD
				if (m_ignoreSizeChanges || !m_deviceCreated)
					return;

				RenderParameters newSettings = *m_currentSetting;

				RECT rect;
				GetClientRect(m_game->getWindow()->getHandle(), &rect);

				int32 width = rect.right - rect.left;
				int32 height = rect.bottom - rect.top;

				if (width != newSettings.BackBufferWidth || height != newSettings.BackBufferHeight)
				{
					newSettings.BackBufferWidth = width;
					newSettings.BackBufferHeight = height;
					CreateDevice(newSettings);
				}
			}

			void GraphicsDeviceManager::InitializeDevice(const RenderParameters &settings)
			{
				int colorBitDepth = PixelFormatUtils::GetBPP(settings.ColorBufferFormat) * 8;
				int depthBitDepth = PixelFormatUtils::GetDepthBitDepth(settings.DepthBufferFormat);
				int stencilBitDepth = PixelFormatUtils::GetStencilBitDepth(settings.DepthBufferFormat);

				PIXELFORMATDESCRIPTOR pfd =
				{
					sizeof(PIXELFORMATDESCRIPTOR), 1,
					PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
					PFD_TYPE_RGBA, (BYTE)colorBitDepth,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					(BYTE)depthBitDepth, (BYTE)stencilBitDepth,
					0, PFD_MAIN_PLANE, 0, 0, 0, 0
				};

				// Create temp window and context
				HWND tempHwnd = CreateWindowA("STATIC", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
				HDC  tempHdc = GetDC(tempHwnd);
				SetPixelFormat(tempHdc, ChoosePixelFormat(tempHdc, &pfd), &pfd);

				HGLRC tempHrc = wglCreateContext(tempHdc);
				wglMakeCurrent(tempHdc, tempHrc);

				int major = 0;
				int minor = 0;
				glGetIntegerv(GL_MAJOR_VERSION, &major);
				glGetIntegerv(GL_MINOR_VERSION, &minor);
				
				if (major < 3 || (major == 3 && minor < 1))
				{
					AP_EXCEPTION(ErrorID::NotSupported, L"OpenGL 3.1 not supported.");
					return;
				}

				// Choose final pixel format
				const int pixelAttribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, colorBitDepth,
					WGL_DEPTH_BITS_ARB, depthBitDepth,
					WGL_STENCIL_BITS_ARB, stencilBitDepth,
					WGL_SAMPLE_BUFFERS_ARB, settings.FSAASampleCount > 1 ? GL_TRUE : GL_FALSE,
					WGL_SAMPLES_ARB, settings.FSAASampleCount > 1 ? (int)settings.FSAASampleCount : 0,
					0
				};

				m_hDC = GetDC(m_game->getWindow()->getHandle());

				int pixelFormat;
				uint formatCount;
				wglChoosePixelFormatARB(m_hDC, pixelAttribs, NULL, 1, &pixelFormat, &formatCount);
				if (formatCount == 0)
				{
					AP_EXCEPTION(ErrorID::NotSupported, L"RenderParams not supported.");
					return;
				}
				SetPixelFormat(m_hDC, pixelFormat, &pfd);

				// Create OpenGL 3.1 context		
				int ctxAttribs[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
					WGL_CONTEXT_MINOR_VERSION_ARB, 1,
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
					0
				};

				m_hRC = wglCreateContextAttribsARB(m_hDC, NULL, ctxAttribs);
				
				// Clean up
				wglMakeCurrent(m_hDC, NULL);
				wglDeleteContext(tempHrc);
				wglMakeCurrent(m_hDC, m_hRC);
				
				ReleaseDC(tempHwnd, tempHdc);
				DestroyWindow(tempHwnd);

				if (settings.FSAASampleCount > 1)
				{
					glEnable(GL_MULTISAMPLE);
				}

				m_deviceCreated = true;

				m_game->GL_Initialize();
				m_game->GL_LoadContent();
			}
			void GraphicsDeviceManager::CreateDevice(const RenderParameters& settings, const DEVMODE* mode)
			{
				RenderParameters* oldSettings = m_currentSetting;
				m_currentSetting = new RenderParameters(settings);

				m_ignoreSizeChanges = true;

				bool keepCurrentWindowSize = false;
				if (settings.BackBufferWidth == 0 ||
					settings.BackBufferHeight == 0)
					keepCurrentWindowSize = true;

				Win32Window* wnd = m_game->getWindow();

				// check if we are going to windowed or fullscreen mode
				if (settings.IsWindowed)
				{
					if (oldSettings && !oldSettings->IsWindowed)
						SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)m_windowedStyle);
				}
				else
				{
					if (!oldSettings || oldSettings->IsWindowed)
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


				if (settings.IsWindowed)
				{
					if (oldSettings && !oldSettings->IsWindowed)
					{
						m_fullscreenWindowWidth = oldSettings->BackBufferWidth;
						m_fullscreenWindowHeight = oldSettings->BackBufferHeight;
					}
				}
				else
				{
					if (oldSettings && oldSettings->IsWindowed)
					{
						m_windowedWindowWidth = oldSettings->BackBufferWidth;
						m_windowedWindowHeight = oldSettings->BackBufferHeight;
					}
				}

				if (!m_deviceCreated)
				{
					ChangeResolution(mode);

					InitializeDevice(settings);
				}
				else
				{
					ChangeResolution(mode);
				}
				
				if (wglSwapIntervalEXT)
				{
					wglSwapIntervalEXT(settings.EnableVSync ? 1 : 0);
				}

				glViewport(0,0, settings.BackBufferWidth,settings.BackBufferHeight);


				// check if we changed from fullscreen to windowed mode
				if (oldSettings && !oldSettings->IsWindowed && settings.IsWindowed)
				{
					SetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
					//wnd->setTopMost(m_savedTopmost);
				}

				// check if we need to resize
				if (settings.IsWindowed && !keepCurrentWindowSize)
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

							CreateDevice(newSettings, mode);
						}
					}
				}

				// if the window is still hidden, make sure it is shown
				if (!IsWindowVisible(wnd->getHandle()))
					ShowWindow(wnd->getHandle(), SW_SHOW);

				// set the execution state of the thread
				if (!m_currentSetting->IsWindowed)
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
				if (!settings.IsWindowed)
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

						float score = abs(settings.BackBufferWidth - (int)dm.dmPelsWidth) + abs((int)dm.dmPelsHeight - settings.BackBufferHeight);
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
				desiredSettings.IsWindowed = (windowed);
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
				RenderParameters newSettings = *m_currentSetting;
				newSettings.IsWindowed = !newSettings.IsWindowed;

				int width = newSettings.IsWindowed ? m_windowedWindowWidth : m_fullscreenWindowWidth;
				int height = newSettings.IsWindowed ? m_windowedWindowHeight : m_fullscreenWindowHeight;

				newSettings.BackBufferWidth =  width;
				newSettings.BackBufferHeight =  height;

				ChangeDevice(newSettings);
			}
			void GraphicsDeviceManager::ChangeResolution(const DEVMODE* mode)
			{
				Win32Window* wnd = m_game->getWindow();
				HWND hwnd = wnd->getHandle();

				long style = GetWindowLong(hwnd, GWL_STYLE);
				long styleEx = GetWindowLong(hwnd, GWL_EXSTYLE);

				if (mode)
				{
					ChangeDisplaySettings((DEVMODE*)mode, CDS_FULLSCREEN);

					SetWindowLong(hwnd, GWL_STYLE, style | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
					SetWindowLong(hwnd, GWL_EXSTYLE, styleEx | WS_EX_APPWINDOW);

					SetWindowPos(hwnd, HWND_TOP, 0, 0, mode->dmPelsWidth, mode->dmPelsHeight, SWP_FRAMECHANGED);
				}
				else
				{
					SetWindowLong(hwnd, GWL_STYLE, style & ~(WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS));
					SetWindowLong(hwnd, GWL_EXSTYLE, styleEx & ~(WS_EX_APPWINDOW));

					ChangeDisplaySettings(NULL, 0);
				}
			}
		}
	}
}