/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

Copyright (c) 2009+ Tao Xin

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

#include "PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Math.h"

#include <algorithm>

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace VFS
	{

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		const wchar_t PathUtils::AltDirectorySeparator = '/';
		const wchar_t PathUtils::DirectorySeparator = '\\';

		const wchar_t VolumeSeparatorChar = ':';
#else
		const wchar_t PathUtils::AltDirectorySeparator = '\\';
		const wchar_t PathUtils::DirectorySeparator = '/';
#endif

		const wchar_t AllDirectorySeparators[] = { PathUtils::DirectorySeparator, PathUtils::AltDirectorySeparator, 0 };

		bool isDirectorySeparatorChar(int ch)
		{
			return ch == PathUtils::DirectorySeparator || ch == PathUtils::AltDirectorySeparator;
		}

		bool isDirectorySeparatorNeededAfter(int ch)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			if (ch == VolumeSeparatorChar)
				return true;
#endif
			return !isDirectorySeparatorChar(ch);
		}

		FORCE_INLINE void _SplitFileNameExtension(const String& fileName, String* noext, String* ext)
		{
			size_t i = fileName.find_last_of('.');
			if (i == String::npos)
			{
				if (ext)
					ext->clear();
				if (noext)
					*noext = fileName;
			}
			else
			{
				if (ext)
					*ext = fileName.substr(i + 1);
				if (noext)
					*noext = fileName.substr(0, i);
			}
		}
		FORCE_INLINE void _SplitFilePath(const String& filePath, String* fileName, String* parentDir, bool normalize)
		{
			String path = normalize ? PathUtils::NormalizePath(filePath) : filePath;

			// split based on final DirectorySeparator
			size_t i = path.find_last_of(PathUtils::DirectorySeparator);

			if (i == String::npos)
			{
				if (parentDir)
					parentDir->clear();
				if (fileName)
					*fileName = path;
			}
			else
			{
				if (i == path.size() - 1)
				{
					size_t pos = path.find_last_not_of(PathUtils::DirectorySeparator);
					path = path.substr(0, pos + 1);
					i = path.find_last_of(PathUtils::DirectorySeparator);
					if (i == String::npos)
					{
						if (parentDir)
							parentDir->clear();
						if (fileName)
							*fileName = path;
						return;
					}
				}

				if (fileName)
					*fileName = path.substr(i + 1, path.size() - i - 1);
				if (parentDir)
					*parentDir = path.substr(0, i);
			}

		}
		

		String PathUtils::GetDirectory(const String& filePath)
		{
			String result;
			_SplitFilePath(filePath, nullptr, &result, true);
			
			assert((!result.size() && !filePath.size()) || result != filePath);
			return result;
		}
		String PathUtils::GetFileNameNoExt(const String& filePath)
		{
			String fileName;
			_SplitFilePath(filePath, &fileName, nullptr, true);
			_SplitFileNameExtension(fileName, &fileName, nullptr);

			return fileName;
		}
		String PathUtils::GetFileName(const String& filePath)
		{
			String fn;
			_SplitFilePath(filePath, &fn, nullptr, true);
			return fn;
		}
		String PathUtils::GetFileExtension(const String& filePath)
		{
			String ext;
			_SplitFileNameExtension(filePath, nullptr, &ext);
			return ext;
		}

		void PathUtils::SplitFileNameExtension(const String& fileName, String& noext, String& ext) { _SplitFileNameExtension(fileName, &noext, &ext); }
		void PathUtils::SplitFilePath(const String& filePath, String& fileName, String& parentDir) { _SplitFilePath(filePath, &fileName, &parentDir, true); }

		void PathUtils::SplitFilePathNameExtension(const String& path, String& parentDir, String& noext, String& ext)
		{
			String fullName;
			SplitFilePath(path, fullName, parentDir);
			SplitFileNameExtension(fullName, noext, ext);
		}

		
		String PathUtils::NormalizePath(const String& filePath)
		{
			String result;
			result.reserve(filePath.size());

			bool isLastCharSep = false;
			for (size_t i = 0; i < filePath.size(); i++)
			{
				wchar_t ch = filePath[i];

				if (isDirectorySeparatorChar(ch))
				{
					if (!isLastCharSep)
					{
						isLastCharSep = true;
						// add to result, only once here
						result.append(1, DirectorySeparator);
					}
				}
				else
				{
					isLastCharSep = false;

					result.append(1, ch);
				}
			}

			return result;
		}
		void PathUtils::NormalizePathInPlace(String& filePath)
		{
			filePath = NormalizePath(filePath);
		}


		List<String> PathUtils::Split(const String& path)
		{
			List<String> ret = StringUtils::Split(path, AllDirectorySeparators);
		
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			if (ret.getCount()>1 && ret[0].find(VolumeSeparatorChar,0) != String::npos)
			{
				ret[1] = Combine(ret[0],ret[1]);
				ret.RemoveAt(0);
			}
#endif

			return ret;
		}

		bool PathUtils::GetRelativePath(const String& source, const String& target, String& result)
		{
			// Find common base path
			int commonLevelIndex = 0;
			ComparePath(source, target, commonLevelIndex);

			// No common base path
			if (commonLevelIndex == 0)
				return false;


			// Build up relative path
			// 1. Add .. from 'source' to the common base path
			int32 baseToSourceLevelCount = CountPathLevels(source, commonLevelIndex);
			for (int i = 0; i < baseToSourceLevelCount; i++)
			{
				result.append(L"..");
				result.append(1, DirectorySeparator);
			}

			// 2. Add sub folders to the 'target'

			if (commonLevelIndex == source.size() - 1)
			{
				// target is in sub dir of source
				// trim dir separators at the beginning
				while (commonLevelIndex+1 < (int32)target.size() &&
					isDirectorySeparatorChar(target[commonLevelIndex+1]))
					commonLevelIndex++;
			}

			result.append(NormalizePath(target.substr(commonLevelIndex + 1)));

			return true;
		}
		bool PathUtils::GetCommonBasePath(const String& path1, const String& path2, String& result)
		{
			int commonLevelIndex = 0;
			ComparePath(path1, path2, commonLevelIndex);

			// No common base path
			if (commonLevelIndex == 0)
				return false;

			result = path1.substr(0, commonLevelIndex);

			return true;
		}

		void PathUtils::Append(String& path1, const String& path2)
		{
			if (path1.empty())
			{
				path1.append(path2);
				return;
			}
			size_t len1 = path1.length();
			wchar_t ch = path1[len1 - 1];
			if (isDirectorySeparatorNeededAfter(ch))
			{
				path1.append(1, DirectorySeparator);
				path1.append(path2);
				return;
			}
			path1.append(path2);
		}
		String PathUtils::Combine(const String &path1, const String &path2)
		{
			size_t len1 = path1.length();
			if (len1 == 0) return path2;
			
			size_t len2 = path2.length();
			if (len2 == 0)
				return path1;

			wchar_t ch = path1[len1 - 1];
			if (isDirectorySeparatorNeededAfter(ch))
			{
				return path1 + DirectorySeparator + path2;
			}
			return path1 + path2;
		}
		std::string PathUtils::Combine(const std::string &path1, const std::string &path2)
		{
			size_t len1 = path1.length();
			if (len1 == 0) return path2;

			size_t len2 = path2.length();
			if (len2 == 0)
				return path1;

			char ch = path1[len1 - 1];
			if (isDirectorySeparatorNeededAfter(ch))
			{
				return path1 + (char)DirectorySeparator + path2;
			}
			return path1 + path2;
		}

		bool PathUtils::Match(const String& str, const String& pattern)
		{
			String tmpStr = NormalizePath(str);
			String tmpPattern = NormalizePath(pattern);

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			{
				StringUtils::ToLowerCase(tmpStr);
				StringUtils::ToLowerCase(tmpPattern);
			}
#endif

			return StringUtils::Match(tmpStr, tmpPattern);
		}

		bool PathUtils::IsAbsolute(const String& str)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			if (str.size() > 1 && str[1] == ':')
				return true;
#endif

			if (str.size() > 0 && isDirectorySeparatorChar(str[0]))
				return true;
			return false;
		}

		bool PathUtils::ComparePath(const String& left, const String& right, int32& commonBaseIndex)
		{
			commonBaseIndex = 0;

			if (left.empty() || right.empty())
			{
				return left == right;
			}

			// remove the last DirectorySeparators
			int32 leftLength = left.size();
			int32 rightLength = right.size();
			while (leftLength > 0 && isDirectorySeparatorChar(left[leftLength - 1]))
				leftLength--;

			while (rightLength > 0 && isDirectorySeparatorChar(right[rightLength - 1]))
				rightLength--;

			// these are needed to check against consecutive DirectorySeparators
			bool isLastSepL = false;
			bool isLastSepR = false;

			//bool finished = false;

			int lvll = 0;
			int lvlr = 0;
			int i = 0;
			int j = 0;

			for (;;)
			{
				wchar_t lch = left[i];
				wchar_t rch = right[j];

				if (isDirectorySeparatorChar(lch))
				{
					i++;
					if (!isLastSepL)
					{
						lvll++;
						isLastSepL = true;
					}
					continue;
				}
				else
				{
					isLastSepL = false;
				}

				if (isDirectorySeparatorChar(rch))
				{
					j++;
					if (!isLastSepR)
					{
						lvlr++;
						isLastSepR = true;
					}
					continue;
				}
				else
				{
					isLastSepR = false;
				}

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				if (tolower(lch) != tolower(rch))
					return false;
#else
				if (lch != rch)
					return false;
#endif

				i++;
				j++;

				if (i >= leftLength && j >= rightLength)
					return lvll == lvlr;


				if (i >= leftLength)
					return false;
				if (j >= rightLength)
					return false;

				commonBaseIndex = Math::Min(i, j);
			}
		}

		bool PathUtils::ComparePath(const String& left, const String& right) { int32 cbi; return ComparePath(left, right, cbi); }



		
		int32 PathUtils::CountPathLevels(const String& path, int32 startIndex)
		{
			bool isLastSep = false;
			int32 lvl = 0;

			for (size_t i = startIndex; i < path.size(); i++)
			{
				wchar_t ch = path[i];

				if (isDirectorySeparatorChar(ch))
				{
					if (!isLastSep)
					{
						isLastSep = true;

						lvl++;
					}
				}
				else
				{
					isLastSep = false;
				}
			}
			return lvl;
		}

		int32 PathUtils::CountPathLevels(const String& path) { return CountPathLevels(path, 0); }

	}
}