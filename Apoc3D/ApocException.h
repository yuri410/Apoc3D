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
	enum struct ApocExceptionType
	{
		Default,
		InvalidOperation,
		InvalidData,
		NotSupported,
		KeyNotFound,
		FormatException,
		EndOfStream,
		FileNotFound,
		Argument,
		Duplicate,
		ScriptCompileError
	};

	class APAPI ApocException
	{
	public:
		ApocException(const String& msg, ApocExceptionType type)
			: m_message(msg), m_type(type) { }

		explicit ApocException(ApocExceptionType type)
			: m_type(type) { }

		ApocException(const ApocException &another)
			: m_message(another.m_message), m_type(another.m_type) { }

		const String& getMessage() const { return m_message; }
		const ApocExceptionType getType() const { return m_type; }

		static ApocException CreateException(ApocExceptionType type, const String& msg, const wchar_t* file, int line);
		static ApocException CreateException(ApocExceptionType type, const wchar_t* file, int line);

	private:
		String m_message;
		ApocExceptionType m_type;
	};

#define AP_EXCEPTION(type, msg) (ApocException::CreateException(type, msg, _CRT_WIDE(__FILE__), __LINE__))
//#define AP_EXCEPTION(type) (ApocException::CreateException(type, _CRT_WIDE(__FILE__), __LINE__))

};


#endif