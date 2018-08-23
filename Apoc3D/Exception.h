#pragma once
#ifndef APOC3D_APOC3DEXCEPTION_H
#define APOC3D_APOC3DEXCEPTION_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2014-2017 Tao Xin
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

#include "ApocCommon.h"

namespace Apoc3D
{
	enum struct ExceptID
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

	class APAPI Exception
	{
	public:
		Exception(const String& msg, ExceptID type)
			: m_message(msg), m_type(type) { }

		explicit Exception(ExceptID type)
			: m_type(type) { }

		Exception(const Exception &another)
			: m_message(another.m_message), m_type(another.m_type) { }

		const String& getMessage() const { return m_message; }
		const ExceptID getType() const { return m_type; }

		static Exception CreateException(ExceptID type, const String& msg, const wchar_t* file, int line);
		static Exception CreateException(ExceptID type, const wchar_t* file, int line);

	private:
		String m_message;
		ExceptID m_type;
	};

#define AP_EXCEPTION(type, msg) (Exception::CreateException(type, msg, _CRT_WIDE(__FILE__), __LINE__))
//#define AP_EXCEPTION(type) (Exception::CreateException(type, _CRT_WIDE(__FILE__), __LINE__))

};


#endif