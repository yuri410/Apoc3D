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
#ifndef LOGGING_H
#define LOGGING_H

#include "Common.h"
#include "Singleton.h"
#include "tthread/tinythread.h"
#include "tthread/fast_mutex.h"

using namespace std;

namespace Apoc3D
{
	namespace Core
	{
		enum APAPI LogMessageLevel
		{
			LOGLVL_Default,
			LOGLVL_Infomation,
			LOGLVL_Warning,
			LOGLVL_Error,
			LOGLVL_Fatal
		};
		enum APAPI LogType
		{
			LOG_System = 0,
			LOG_Graphics = 1,
			LOG_Audio = 2,
			LOG_Scene = 3,
			LOG_Game = 4,
			LOG_Network = 5,
			LOG_Command = 6,
			LOG_Count = 7
		};

		

		struct APAPI LogEntry
		{
			time_t Time;
			LogMessageLevel Level;
			String Content;
			LogType Type;

			LogEntry(){}
			LogEntry(time_t time, const String& content, LogMessageLevel level, LogType type)
				: Time(time), Level(level), Content(content), Type(type)
			{
			}

			friend bool operator <(const LogEntry& a, const LogEntry& b)
			{
				return a.Time < b.Time || a.Level < b.Level || a.Content < b.Content;
			}

			friend bool operator ==(const LogEntry& a, const LogEntry& b)
			{
				return a.Time == b.Time && a.Level == b.Level && a.Content == b.Content;
			}

			String ToString() const;
		};
		
		typedef fastdelegate::FastDelegate1<LogEntry, void> NewLogWrittenHandler;

		//template class APAPI list<LogEntry>;

		class APAPI Log
		{
		private:			
			LogType m_type;
			list<LogEntry> m_entries;

			tthread::fast_mutex m_lock;

		public:
			typedef list<LogEntry>::const_iterator Iterator;

			static const int32 MaxEntries = 200;

			Iterator begin() const { return m_entries.begin(); }
			Iterator end() const { return m_entries.end(); }

			Log(LogType type);
			~Log();

			LogType getType() const { return m_type; }

			void Write(const String& message, LogMessageLevel level = LOGLVL_Infomation);
		};


		class APAPI LogManager : public Singleton<LogManager>
		{
		private:
			Log* m_logs[LOG_Count];
			NewLogWrittenHandler m_eNewLog;
		public:
			bool WriteLogToStd;

			NewLogWrittenHandler& eventNewLogWritten() { return m_eNewLog; }

			LogManager();
			~LogManager();

			Log* getDefaultLog() const;
			Log* getLogSet(LogType type) { return m_logs[static_cast<int32>(type)]; }

			void Write(LogType type, const String& message, LogMessageLevel level = LOGLVL_Infomation) const;

			SINGLETON_DECL_HEARDER(LogManager);
		};
	}
}

#endif