/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#ifndef PATHUTILS_H
#define PATHUTILS_H
#pragma once

#include "Common.h"

namespace Apoc3D
{	
	namespace VFS
	{
		class PathUtils
		{
		public:
			static const wchar_t DirectorySeparator;
			static const wchar_t AltDirectorySeparator;
			static const wchar_t VolumeSeparatorChar;

			static void SplitFileNameExtension(const String& fileName, String& noext, String& ext);
			static void SplitFilePath(const String& path, String& fileName, String& parentDir);
			static void SplitFilePathNameExtension(const String& path, String& parentDir, String& noext, String& ext);
			static String Combine(const String& left, const String& right);
			static void Append(String& str, const String& app);
			static bool Match(const String& str, const String& pattern, bool caseSensitive);

		private:
			PathUtils(void) {}
			~PathUtils(void) {}
		};
	}
}
#endif