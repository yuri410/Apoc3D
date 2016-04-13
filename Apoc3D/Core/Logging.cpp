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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Collections/CollectionsCommon.h"
#include "apoc3d/Library/tinythread.h"

#include <ctime>
#include <iostream>

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Core
	{
		SINGLETON_IMPL(LogManager);

		static uint64 LogItemSerialCounter = 1;

		LogManager::LogManager()
		{
			m_lock = new tthread::mutex();

			for (size_t i = 0; i < LOG_Count; i++)
			{
				m_logs[i] = new LogSet(static_cast<LogType>(i));
			}
		}
		LogManager::~LogManager()
		{
			for (size_t i = 0; i < LOG_Count; i++)
			{
				delete m_logs[i];
				m_logs[i] = nullptr;
			}

			delete m_lock;
			m_lock = nullptr;
		}

		void LogManager::Write(LogType type, const String& message, LogMessageLevel level)
		{
			bool ret = m_logs[static_cast<int32>(type)]->Write(message, level, 
				type != LOG_CommandResponse && type != LOG_Command && type != LOG_Game);

			if (ret)
			{
				m_lock->lock();

				const LogEntry& lastest = *m_logs[(int32)type]->LastEntry();

				eventNewLogWritten.Invoke(lastest);
				
				if (WriteLogToStd)
				{
					String msg = lastest.ToString();
					if (!StringUtils::EndsWith(msg, L"\n"))
						msg.append(L"\n");

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#if _DEBUG
					OutputDebugString(msg.c_str());
#endif
					
#endif

					std::wcout << ( msg );
				}

				m_lock->unlock();
			}

		}

		void LogManager::DumpLogs(String& result)
		{
			int32 totalEntryCount = 0;
			for (size_t i = 0; i < LOG_Count; i++)
			{
				totalEntryCount += m_logs[i]->getCount();
			}

			List<const LogEntry*> allEntries(totalEntryCount);
			for (size_t i = 0; i < LOG_Count; i++)
			{
				LogSet* ls = m_logs[i];
				for (LogSet::Iterator iter = ls->begin(); iter != ls->end(); ++iter)
				{
					const LogEntry& e = *iter;
					allEntries.Add(&e);
				}
			}

			allEntries.Sort([](const LogEntry* a, const LogEntry* b)->int
			{
				return Apoc3D::Collections::OrderComparer(a->SerialIndex, b->SerialIndex);
			});
			
			result.reserve(10240);
			for (int32 i = 0; i < allEntries.getCount(); i++)
			{
				String str = allEntries[i]->ToString();
				result.append(str);
				result.append(L"\n");
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		LogSet::LogSet(LogType type)
			: m_type(type)
		{
			m_lock = new tthread::mutex();
		}

		LogSet::~LogSet()
		{
			delete m_lock;
			m_lock = nullptr;
		}

		bool LogSet::Write(const String& message, LogMessageLevel level, bool checkDuplicate)
		{
			bool result = false;
			m_lock->lock();

			bool discard = false;
			if (checkDuplicate && m_entries.getCount())
			{
				if (m_entries.Back().Content == message)
				{
					discard = true;
				}
			}

			if (!discard)
			{
				time_t t = time(0);

				while (m_entries.getCount() > MaxEntries)
				{
					m_entries.PopFront();
				}
				m_entries.PushBack(LogEntry(LogItemSerialCounter++, t, message, level, m_type));
				result = true;
			}

			m_lock->unlock();
			return result;
		}

		int32 LogSet::getCount()
		{
			m_lock->lock();
			int result = m_entries.getCount();
			m_lock->unlock();
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		String LogEntry::ToString() const
		{
			tm* t = localtime(&Time);

			//wstringstream wss;
			String result;

			bool isCommandRelated = Type == LOG_Command || Type == LOG_CommandResponse;
			if (!isCommandRelated)
			{
				result.append(StringUtils::IntToString(t->tm_hour * 10000 + t->tm_min * 100 + t->tm_sec, StrFmt::a<6, '0'>::val));
				result.append(1, ' ');

				if (Type != LOG_Game)
				{
					result += L" [";
					switch (Type)
					{
					case LOG_System:	result += L"SYS"; break;
					case LOG_Graphics:	result += L"GRP"; break;
					case LOG_Audio:		result += L"AUD"; break;
					case LOG_Scene:		result += L"SCE"; break;
					case LOG_Network:	result += L"NET"; break;
					}
					result += L"] ";
				}
				else
				{
					result += L" ";
				}
			}
			else if (Type != LOG_CommandResponse)
			{
				result += L" >";
			}
			

			if (!isCommandRelated)
			{
				switch (Level)
				{
				case LOGLVL_Error:		result += L"[Err] "; break;
				case LOGLVL_Warning:	result += L"[Warn] "; break;
				case LOGLVL_Fatal:		result += L"[Critical] "; break;
				case LOGLVL_Infomation:	result += L" "; break;
				}
			}

			result += Content;

			return result;
		}

		bool LogEntry::operator ==(const LogEntry& b) const
		{
			return SerialIndex == b.SerialIndex && Type == b.Type && 
				Time == b.Time && Level == b.Level && Content == b.Content;
		}

	}
}