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

using namespace std;

namespace Apoc3D
{
	namespace Core
	{
		enum APAPI LogMessageLevel
		{
			LOGLVL_Infomation,
			LOGLVL_Warning
		};
		enum APAPI LogType
		{
			LOG_System = 0,
			LOG_Graphics = 1,
			LOG_Audio = 2,
			LOG_Scene = 3,
			LOG_Game = 4,
			LOG_Network = 5,
			LOG_Count = 6
		};


		struct APAPI LogEntry
		{
			time_t Time;
			LogMessageLevel Level;
			String Content;

			LogEntry(){}
			LogEntry(time_t time, String content, LogMessageLevel level)
				: Time(time), Level(level), Content(content)
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
		};

		template class APAPI list<LogEntry>;

		class APAPI Log
		{
		private:			
			LogType m_type;
			list<LogEntry> m_entries;

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

		public:
			LogManager();
			~LogManager();

			Log* getDefaultLog() const;
			Log* getLogSet(const String& name);

			void Write(LogType type, const String& message, LogMessageLevel level = LOGLVL_Infomation) const;

			SINGLETON_DECL_HEARDER(LogManager);
		};
	}
}

#endif