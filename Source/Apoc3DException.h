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

#ifndef APOC3DEXCEPTION_H
#define APOC3DEXCEPTION_H


#include "Common.h"

namespace Apoc3D
{

	enum Apoc3DExceptionType
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

	class APAPI Apoc3DException
	{
	public:
		Apoc3DException(const String& msg, Apoc3DExceptionType type)
			: m_message(msg), m_type(type)
		{

		}
		Apoc3DException(const Apoc3DException &another)
			: m_message(another.m_message)
		{
		}

		const String& getMessage() const { return m_message; }
		const Apoc3DExceptionType getType() const { return m_type; }

		static Apoc3DException createException(Apoc3DExceptionType type, const String& msg);

	private:
		String m_message;
		Apoc3DExceptionType m_type;
	};
};


#endif