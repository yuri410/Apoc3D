
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NRSPlatform.h"
#include "Apoc3D/Core/Logging.h"
#include "Apoc3D/Utility/StringUtils.h"

#include <csignal>
#include <numeric>

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			void GameClock::Reset()
			{
				m_baseTimePoint = std::chrono::high_resolution_clock::now();
				m_curTimePoint = m_lastTimePoint = m_baseTimePoint;

				m_currentTime = 0;
				m_elapsedTime = 0;
			}

			void GameClock::Step()
			{
				m_curTimePoint = std::chrono::high_resolution_clock::now();

				m_currentTime = std::chrono::duration<double>(m_curTimePoint - m_baseTimePoint).count();
				m_elapsedTime = std::chrono::duration<double>(m_curTimePoint - m_lastTimePoint).count();

				m_lastTimePoint = m_curTimePoint;
			}

			//////////////////////////////////////////////////////////////////////////

			GameWindow* GameWindow::ms_Window = nullptr;

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			FILE* g_customConsoleOutput = NULL;
			static void LogMessageToConsole(LogEntry e)
			{
				if (g_customConsoleOutput)
				{
					HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
					if (hConsole != INVALID_HANDLE_VALUE)
					{
						WORD conTextAtt = 0;
						switch (e.Level)
						{
							case LOGLVL_Fatal:
								conTextAtt = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
								break;
							case LOGLVL_Error:
								conTextAtt = FOREGROUND_INTENSITY | FOREGROUND_RED;
								break;
							case LOGLVL_Warning:
								conTextAtt = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
								break;
							case LOGLVL_Infomation:
								conTextAtt = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
								break;
							case LOGLVL_Default:
								conTextAtt = FOREGROUND_INTENSITY;
								break;
						}
						if (conTextAtt)
							SetConsoleTextAttribute(hConsole, conTextAtt);
					}

					std::string txt = StringUtils::toPlatformNarrowString(e.ToString());

					fprintf(g_customConsoleOutput, "%s\n", txt.c_str());
				}
			}
#endif

			GameWindow::GameWindow(const String& title)
				: m_title(title)
			{
				ms_Window = this;

				signal(SIGINT, SigIntHandler);
			}

			GameWindow::~GameWindow()
			{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				FreeConsole();
#endif

				if (ms_Window == this)
					ms_Window = NULL;
			}

			void GameWindow::Load(int32 width, int32 height, bool fixed)
			{
				m_size = { width, height };

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				if (AllocConsole())
				{
					SetConsoleTitle(m_title.c_str());

					HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);

					// set the screen buffer to be big enough to let us scroll text
					const WORD MAX_CONSOLE_LINES = 999;
					CONSOLE_SCREEN_BUFFER_INFO coninfo;
					GetConsoleScreenBufferInfo(handle_out, &coninfo);
					coninfo.dwSize.X = 160;
					coninfo.dwSize.Y = MAX_CONSOLE_LINES;
					
					SetConsoleScreenBufferSize(handle_out, coninfo.dwSize);
					
					HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
					int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
					FILE* fp = _fdopen(hConHandle, "w");
					setvbuf(fp, NULL, _IONBF, 0);

					LogManager::getSingleton().eventNewLogWritten.Bind(&LogMessageToConsole);
					g_customConsoleOutput = fp;
				}
#endif
			}

			void GameWindow::SigIntHandler(int s)
			{
				if (GameWindow::ms_Window)
				{
					GameWindow::ms_Window->Close();
				}
			}

			void GameWindow::setWindowTitle(const String& txt)
			{
				m_title = txt;
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				SetConsoleTitle(m_title.c_str());
#endif
			}

			void GameWindow::Close()
			{
				m_isClosed = true;
				
			}
			void GameWindow::SetVisible(bool v)
			{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				ShowWindow(GetConsoleWindow(), v ? SW_NORMAL : SW_HIDE);
#endif
			}
			void GameWindow::Minimize()
			{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
#endif
			}
			void GameWindow::Restore()
			{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				ShowWindow(GetConsoleWindow(), SW_RESTORE);
#endif
			}
			void GameWindow::Maximize()
			{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
#endif
			}

		}
	}
}