/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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
		LogEntry::LogEntry(uint64 serIdx, time_t time, const String& content, LogMessageLevel level, LogType type)
			: SerialIndex(serIdx), Time(time), Level(level), Content(content), Type(type)
		{
		}

		/************************************************************************/
		/*   LogManager                                                         */
		/************************************************************************/

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

		void LogManager::DumpLogs(String& result, bool lastFirst)
		{
			int32 totalEntryCount = 0;
			for (LogSet* ls : m_logs)
			{
				totalEntryCount += ls->getCount();
			}

			List<const LogEntry*> allEntries(totalEntryCount);
			for (LogSet* ls : m_logs)
			{
				for (const LogEntry& e : *ls)
				{
					allEntries.Add(&e);
				}
			}

			allEntries.Sort([lastFirst](const LogEntry* a, const LogEntry* b)->int
			{
				return Apoc3D::Collections::OrderComparer(a->SerialIndex, b->SerialIndex) * (lastFirst ? -1 : 1);
			});
			
			result.reserve(10240);
			for (const LogEntry* ent : allEntries)
			{
				String str = ent->ToString();
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