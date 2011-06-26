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

#include "Common.h"

using namespace std;

namespace Apoc3D
{	
	namespace VFS
	{
		class APAPI PathUtils
		{
		public:
			static const wchar_t DirectorySeparator;
			static const wchar_t AltDirectorySeparator;
			static const wchar_t VolumeSeparatorChar;

			static String GetDirectory(const String& fileName);
			static void SplitFileNameExtension(const String& fileName, String& noext, String& ext);
			static void SplitFilePath(const String& path, String& fileName, String& parentDir);
			static void SplitFilePathNameExtension(const String& path, String& parentDir, String& noext, String& ext);
			static vector<String> Split(const String& path);
			/** Combines two path
			*/
			static String Combine(const String& left, const String& right);
			/** Append sub dir to a path
			*/
			static void Append(String& str, const String& app);
			static bool Match(const String& str, const String& pattern, bool caseSensitive);

			static bool ComparePath(const String& left, const String& right);

		private:
			PathUtils(void) {}
			~PathUtils(void) {}
		};
	}
}
#endif