#pragma once
#ifndef APOC3D_APOC3DEXCEPTION_H
#define APOC3D_APOC3DEXCEPTION_H

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


#include "Common.h"

namespace Apoc3D
{

	enum ApocExceptionType
	{
		EX_Default,
		EX_InvalidOperation,
		EX_InvalidData,
		EX_NotSupported,
		EX_KeyNotFound,
		EX_FormatException,
		EX_EndOfStream,
		EX_FileNotFound,
		EX_Argument,
		EX_Duplicate,
		EX_ScriptCompileError
	};

	class APAPI ApocException
	{
	public:
		ApocException(const String& msg, ApocExceptionType type)
			: m_message(msg), m_type(type)
		{

		}
		ApocException(const ApocException &another)
			: m_message(another.m_message)
		{
		}

		const String& getMessage() const { return m_message; }
		const ApocExceptionType getType() const { return m_type; }

		static ApocException createException(ApocExceptionType type, const String& msg, const wchar_t* file, int line);

	private:
		String m_message;
		ApocExceptionType m_type;
	};

#define AP_EXCEPTION(type, msg) (ApocException::createException(type, msg, _CRT_WIDE(__FILE__), __LINE__))
};


#endif