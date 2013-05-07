/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "Logging.h"

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "apoc3d/Utility/StringUtils.h"

#include "tthread/tinythread.h"
#include <ctime>

#include <strstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Apoc3D::Utility;

SINGLETON_DECL(Apoc3D::Core::LogManager);

namespace Apoc3D
{
	namespace Core
	{
//#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		//HANDLE g_StandardOutput;
//#endif
		String LogEntry::ToString() const
		{
			tm* t = localtime(&Time);

			wstringstream wss;

			bool isCommandRelated = Type == LOG_Command || Type == LOG_CommandResponse;
			if (!isCommandRelated)
			{
				wss.width(2);
				wss.fill('0');
				wss.imbue(locale::classic());
				wss.setf(std::ios::right);
				wss << t->tm_hour;

				wss.width(2);
				wss.fill('0');
				wss.imbue(locale::classic());
				wss.setf(std::ios::right);
				wss << t->tm_min;

				wss.width(2);
				wss.fill('0');
				wss.imbue(locale::classic());
				wss.setf(std::ios::right);
				wss << t->tm_sec;

				wss.width(0);
				wss.fill(' ');
				wss.setf(0);

				if (Type != LOG_Game)
				{
					wss << L" [";
					switch (Type)
					{
					case LOG_System:
						wss << L"SYS";
						break;
					case LOG_Graphics:
						wss << L"GRP";
						break;
					case LOG_Audio:
						wss << L"AUD";
						break;
					case LOG_Scene:
						wss << L"SCE";
						break;
					case LOG_Network:
						wss << L"NET";
						break;
					}
					wss << L"] ";
				}
				else
				{
					wss << L" ";
				}
			}
			else if (Type != LOG_CommandResponse)
			{
				wss << L" >";
			}
			

			if (!isCommandRelated)
			{
				switch (Level)
				{
				case LOGLVL_Error:
					wss << L"[Err] ";
					break;
				case LOGLVL_Warning:
					wss << L"[Warn] ";
					break;
				case LOGLVL_Fatal:
					wss << L"[Critical] ";
					break;
				case LOGLVL_Infomation:
					wss << L" ";
					break;
				}
			}

			wss << Content;
			//wss << L"\n";

			return wss.str();
		}


		Log::Log(LogType type)
			: m_type(type)
		{
			m_lock = new tthread::mutex();
		}

		Log::~Log()
		{
			delete m_lock;
			m_lock = nullptr;
		}

		bool Log::Write(const String& message, LogMessageLevel level, bool checkDuplicate)
		{
			bool result = false;
			m_lock->lock();

			bool discard = false;
			if (checkDuplicate && m_entries.Size())
			{
				if (m_entries.Back().Content == message)
				{
					discard = true;
				}
			}
			
			if (!discard)
			{
				time_t t = time(0);

				while (m_entries.Size()>MaxEntries)
				{
					m_entries.PopFront();
				}
				m_entries.PushBack(LogEntry(t, message, level, m_type));
				result = true;
			}

			m_lock->unlock();
			return result;
		}

		LogManager::LogManager()
		{
			for (size_t i=0;i<LOG_Count;i++)
			{
				m_logs[i] = new Log(reinterpret_cast<LogType&>(i));
			}
			//g_StandardOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		}
		LogManager::~LogManager()
		{
			for (size_t i=0;i<LOG_Count;i++)
			{
				delete m_logs[i];	
			}
////#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			//if (g_StandardOutput!= INVALID_HANDLE_VALUE)
			//{
				//CloseHandle(g_StandardOutput);
			//}
//#endif
		}

		void LogManager::Write(LogType type, const String& message, LogMessageLevel level) const
		{
			bool ret = m_logs[static_cast<int32>(type)]->Write(message, level, 
				type != LOG_CommandResponse && type != LOG_Command && type != LOG_Game);

			if (ret)
			{
				const LogEntry& lastest = *m_logs[(int32)type]->LastEntry();// *(--m_logs[static_cast<int32>(type)]->end());

				if (!m_eNewLog.empty())
				{
					m_eNewLog(lastest);
				}
				if (WriteLogToStd)
				{
					String msg = lastest.ToString();
					if (!StringUtils::EndsWidth(msg, L"\n"))
						msg.append(L"\n");

					cout << ( msg.c_str() );
				
	#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
					//if (g_StandardOutput!= INVALID_HANDLE_VALUE)
					//{
	#if _DEBUG
					OutputDebugString(msg.c_str());
						//WriteFile(g_StandardOutput, msg.c_str(), msg.size() * sizeof(wchar_t), NULL, NULL );
					//}
	#endif
	#endif
				}
			}

		}

		int32 Log::getCount()
		{
			m_lock->lock();
			int result = (int)m_entries.Size();
			m_lock->unlock();
			return result;
		}
	}
}