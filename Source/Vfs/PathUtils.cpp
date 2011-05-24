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

#include "PathUtils.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace VFS
	{
		const wchar_t PathUtils::DirectorySeparator = '\\';
		const wchar_t PathUtils::AltDirectorySeparator = '/';
		const wchar_t PathUtils::VolumeSeparatorChar = ':';

		void PathUtils::SplitFileNameExtension(const String& fileName, String& noext, String& ext)
		{
			size_t i = fileName.find_last_of('.');
			if (i == String::npos)
			{
				ext.clear();
				noext = fileName;
			}
			else
			{
				ext = fileName.substr(i+1);
				noext = fileName.substr(0, i);
			}
		}
		void PathUtils::SplitFilePath(const String& filePath, String& fileName, String& parentDir)
		{
			String path = filePath;
			// Replace \ with / first
			std::replace( path.begin(), path.end(), DirectorySeparator, AltDirectorySeparator );
			// split based on final /
			size_t i = path.find_last_of(AltDirectorySeparator);

			if (i == String::npos)
			{
				parentDir.clear();
				fileName = path;
			}
			else
			{
				fileName = path.substr(i+1, path.size() - i - 1);
				parentDir = path.substr(0, i+1);
			}

		}
		void PathUtils::SplitFilePathNameExtension(const String& path, String& parentDir, String& noext, String& ext)
		{
			String fullName;
			SplitFilePath( path, fullName, parentDir );
			SplitFileNameExtension( fullName, noext, ext );
		}
		void PathUtils::Append(String& path1, const String& path2)
		{
			size_t len1 = path1.length();
			wchar_t ch = path1[len1 - 1];
			if (((ch != DirectorySeparator) && (ch != AltDirectorySeparator)) && (ch != VolumeSeparatorChar))
			{
				path1.append(&AltDirectorySeparator, 1);
				path1.append(path2);
				return;
			}
			path1.append(path2);
		}
		String PathUtils::Combine(const String &path1, const String &path2)
		{
			size_t len1 = path1.length();
			if (!len1)
			{
				return path2;
			}
			
			size_t len2 = path2.length();
			if (!len2)
			{
				return path1;
			}

			wchar_t ch = path1[len1 - 1];
			if (((ch != DirectorySeparator) && (ch != AltDirectorySeparator)) && (ch != VolumeSeparatorChar))
			{
				return (path1 + DirectorySeparator + path2);
			}
			return (path1 + path2);
		}
		bool PathUtils::Match(const String& str, const String& pattern, bool caseSensitive)
		{
			String tmpStr = str;
			String tmpPattern = pattern;
			if (!caseSensitive)
			{
				StringUtils::ToLowerCase(tmpStr);
				StringUtils::ToLowerCase(tmpPattern);
			}

			String::const_iterator strIt = tmpStr.begin();
			String::const_iterator patIt = tmpPattern.begin();
			String::const_iterator lastWildCardIt = tmpPattern.end();
			while (strIt != tmpStr.end() && patIt != tmpPattern.end())
			{
				if (*patIt == '*')
				{
					lastWildCardIt = patIt;
					// Skip over looking for next character
					++patIt;
					if (patIt == tmpPattern.end())
					{
						// Skip right to the end since * matches the entire rest of the string
						strIt = tmpStr.end();
					}
					else
					{
						// scan until we find next pattern character
						while(strIt != tmpStr.end() && *strIt != *patIt)
							++strIt;
					}
				}
				else
				{
					if (*patIt != *strIt)
					{
						if (lastWildCardIt != tmpPattern.end())
						{
							// The last wildcard can match this incorrect sequence
							// rewind pattern to wildcard and keep searching
							patIt = lastWildCardIt;
							lastWildCardIt = tmpPattern.end();
						}
						else
						{
							// no wildwards left
							return false;
						}
					}
					else
					{
						++patIt;
						++strIt;
					}
				}

			}
			// If we reached the end of both the pattern and the string, we succeeded
			if (patIt == tmpPattern.end() && strIt == tmpStr.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		

		}

		bool PathUtils::ComparePath(const String& left, const String& right)
		{
			return false;
		}
	}
}