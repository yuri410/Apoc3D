#pragma once
#ifndef APOC3D_LOGGING_H
#define APOC3D_LOGGING_H

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
#include "apoc3d/Meta/EventDelegate.h"
#include "apoc3d/Collections/LinkedList.h"

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
		enum LogMessageLevel
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
		enum LogType
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
			uint64 SerialIndex;

			time_t Time;
			LogMessageLevel Level;
			String Content;
			LogType Type;

			LogEntry() { }
			LogEntry(uint64 serIdx, time_t time, const String& content, LogMessageLevel level, LogType type);

			bool operator ==(const LogEntry& b) const;
			bool operator !=(const LogEntry& b) const { return !(this->operator==(b)); }

			String ToString() const;
		};

		typedef EventDelegate<LogEntry> NewLogWrittenHandler;

		/** 
		 *  A singleton providing possibilities to log messages anywhere in the code.
		 */
		class APAPI LogManager
		{
			SINGLETON_DECL(LogManager);
		public:
			LogManager();
			~LogManager();

			LogSet* getLogSet(LogType type) const { return m_logs[static_cast<int32>(type)]; }

			void Write(LogType type, const String& message, LogMessageLevel level = LOGLVL_Infomation);

			void DumpLogs(String& result, bool lastFirst);
			
			bool WriteLogToStd;
			NewLogWrittenHandler eventNewLogWritten;
		private:
			LogSet* m_logs[LOG_Count];

			tthread::mutex* m_lock;
		};

		inline void ApocLog(LogType type, const String& message, LogMessageLevel level = LOGLVL_Infomation);
		inline void ApocLog(LogType type, const String& message, LogMessageLevel level) { LogManager::getSingleton().Write(type, message, level); }

		/** 
		 *  A LogSet is a set of LogEntries with the same LogType. 
		 *  LogSet only keep the most recent 200 messages, the earlier ones are 
		 *  deleted once it reaches the limit as new messages came in.
		 */
		class APAPI LogSet
		{
		public:
			typedef LinkedList<LogEntry>::Iterator Iterator;

			static const int32 MaxEntries = 200;

			Iterator begin() const { return m_entries.begin(); }
			Iterator end() const { return m_entries.end(); }

			LogEntry* LastEntry() { return &m_entries.Back(); }

			LogSet(LogType type);
			~LogSet();

			LogType getType() const { return m_type; }

			int getCount();

			bool Write(const String& message, LogMessageLevel level = LOGLVL_Infomation, bool checkDuplicate = true);

		private:			
			LogType m_type;
			LinkedList<LogEntry> m_entries;

			tthread::mutex* m_lock;
		};

	}
}

#endif