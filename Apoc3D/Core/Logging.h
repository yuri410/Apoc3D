#pragma once
#ifndef APOC3D_LOGGING_H
#define APOC3D_LOGGING_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3D/Collections/LinkedList.h"
#include "Singleton.h"

using namespace Apoc3D::Collections;

namespace tthread
{
	class mutex;
}

namespace Apoc3D
{
	namespace Core
	{
		/**
		 *  Defines the level of importance for the messages.
		 */
		enum APAPI LogMessageLevel
		{
			LOGLVL_Default,
			LOGLVL_Infomation,
			LOGLVL_Warning,
			LOGLVL_Error,
			LOGLVL_Fatal
		};
		/**
		 *  Defines the field that the messages are related to.
		 */
		enum APAPI LogType
		{
			LOG_System = 0,
			LOG_Graphics = 1,
			LOG_Audio = 2,
			LOG_Scene = 3,
			LOG_Game = 4,
			LOG_Network = 5,
			LOG_Command = 6,
			LOG_CommandResponse = 7,
			LOG_Count = 8
		};

		
		/** 
		 *  Defines a piece of message in the log system. 
		 */
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

		/** 
		 *  A log is a set of LogEntries with the same LogType. 
		 *  Log only keep the most recent 200 messages, the earlier ones are 
		 *  deleted once the the limit has been reached and new messages come in.
		 */
		class APAPI Log
		{
		private:			
			LogType m_type;
			LinkedList<LogEntry> m_entries;

			tthread::mutex* m_lock;

		public:
			typedef LinkedList<LogEntry>::Iterator Iterator;

			static const int32 MaxEntries = 200;

			Iterator begin() const { return m_entries.Begin(); }
			Iterator end() const { return m_entries.End(); }

			LogEntry* LastEntry() { return &m_entries.Back(); }

			Log(LogType type);
			~Log();

			LogType getType() const { return m_type; }

			int getCount();

			bool Write(const String& message, LogMessageLevel level = LOGLVL_Infomation, bool checkDuplicate = true);
		};

		/** 
		 *  A singleton providing possibilities to log messages anywhere in the code.
		 */
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