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

#include "Logging.h"

SINGLETON_DECL(Apoc3D::Core::LogManager);

namespace Apoc3D
{
	namespace Core
	{
		Log::Log(LogType type)
			: m_type(type)
		{

		}

		Log::~Log()
		{

		}

		void Log::Write(const String& message, LogMessageLevel level)
		{
			time_t t;
			time(&t);
			m_entries.push_back(LogEntry(t, message, level));

			while (m_entries.size()>MaxEntries)
			{
				m_entries.erase(m_entries.begin());
			}
		}

		LogManager::LogManager()
		{
			for (size_t i=0;i<LOG_Count;i++)
			{
				m_logs[i] = new Log(reinterpret_cast<LogType&>(i));
			}

		}
		LogManager::~LogManager()
		{
			for (size_t i=0;i<LOG_Count;i++)
			{
				delete m_logs[i];	
			}
		}

		void LogManager::Write(LogType type, const String& message, LogMessageLevel level) const
		{
			m_logs[static_cast<int32>(type)]->Write(message, level);
		}
	}
}