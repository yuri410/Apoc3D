#pragma once
#ifndef APOC3D_PATHUTILS_H
#define APOC3D_PATHUTILS_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{	
	namespace VFS
	{
		/** 
		 *  A collection of function used to process path strings
		 */
		namespace PathUtils
		{
			APAPI extern const wchar_t AltDirectorySeparator;
			APAPI extern const wchar_t DirectorySeparator;

			/** Returns the parent directory path for the specified path string. */
			APAPI String GetDirectory(const String& fileName);
			
			/** Returns the file name of the specified path string without the extension. */
			APAPI String GetFileNameNoExt(const String& filePath);
			APAPI String GetFileName(const String& filePath);
			APAPI String GetFileExtension(const String& filePath);

			APAPI String GetFilePathNoExt(const String& filePath);

			/**
			 *  Replaces AltDirectorySeparator with DirectorySeparator in a path
			 */
			APAPI String NormalizePath(const String& filePath);
			APAPI void NormalizePathInPlace(String& filePath);

			APAPI void SplitFileNameExtension(const String& fileName, String& noext, String& ext);
			APAPI void SplitFilePath(const String& path, String& fileName, String& parentDir);
			APAPI void SplitFilePathNameExtension(const String& path, String& parentDir, String& noext, String& ext);
			APAPI List<String> Split(const String& path);

			APAPI bool GetRelativePath(const String& source, const String& target, String& result);
			APAPI bool GetCommonBasePath(const String& path1, const String& path2, String& result);
			
			/** Combines two path strings */
			APAPI String Combine(const String& left, const String& right);
			APAPI std::string Combine(const std::string& left, const std::string& right);
			
			template <typename T> 
			String CombineMult(const String& left, const T& right)
			{
				return Combine(left, right); 
			}

			template <typename ... T> 
			String CombineMult(const String& left, const T&...parts) 
			{
				return Combine(left, CombineMult(parts...)); 
			}


			/** Append sub dir to a path string */
			APAPI void Append(String& str, const String& app);
			APAPI bool Match(const String& str, const String& pattern);

			APAPI bool IsAbsolute(const String& str);

			/** Compare two path string in a reliable way. Returns true if equal */
			APAPI bool ComparePath(const String& left, const String& right);
			APAPI bool ComparePath(const String& left, const String& right, int32& commonBaseIndex);

			APAPI int32 CountPathLevels(const String& path);
			APAPI int32 CountPathLevels(const String& path, int32 startIndex);

		};
	}
}
#endif