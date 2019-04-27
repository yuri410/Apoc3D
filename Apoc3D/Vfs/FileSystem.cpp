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

#include "FileSystem.h"

#include "PathUtils.h"
#include "ResourceLocation.h"
#include "Archive.h"
#include "FileLocateRule.h"
#include "File.h"

#include "apoc3d/Utility/StringUtils.h"

#include "apoc3d/Library/tinythread.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace VFS
	{
		SINGLETON_IMPL(FileSystem);

		FileSystem::FileSystem()
			: m_openedPack(10)
		{

		}
		FileSystem::~FileSystem()
		{
			m_openedPack.DeleteValuesAndClear();
		}

		void FileSystem::AddWrokingDirectory(const String& path)
		{
			String _path = PathUtils::NormalizePath(path);
			
			m_workingDirs.Add(_path);
		}
		void FileSystem::PushWrokingDirectory(const String& path)
		{
			String _path = PathUtils::NormalizePath(path);

			m_workingDirs.Add(_path);
		}
		void FileSystem::PopWrokingDirectory()
		{
			if (m_workingDirs.getCount() > 0)
				m_workingDirs.RemoveAt(m_workingDirs.getCount() - 1);
		}

		void FileSystem::RegisterArchiveType(ArchiveFactory* factory)
		{
			m_factories.Add(factory->getExtension(), factory);
		}

		bool FileSystem::UnregisterArchiveType(ArchiveFactory* factory)
		{
			return m_factories.Remove(factory->getExtension());
		}


		bool FileSystem::DirectoryExists(const String& path) const
		{
			String dummy;
			return DirectoryExists(path, dummy);
		}
		bool FileSystem::DirectoryExists(const String& path, String& result) const
		{
			for (int32 i = 0; i < m_workingDirs.getCount(); i++)
			{
				String fullPath = PathUtils::Combine(m_workingDirs[i], path);
				if (File::DirectoryExists(fullPath))
				{
					result = fullPath;
					return true;
				}

				// goto parent until (a)found: the archive file (b)not found: workingDir
				while (!PathUtils::ComparePath(fullPath, m_workingDirs[i]))
				{
					fullPath = PathUtils::GetDirectory(fullPath);
					if (File::FileExists(fullPath))
					{
						result = PathUtils::Combine(m_workingDirs[i], path);
						return true;
					}
				}

			}
			return false;
		}
		bool FileSystem::FindAllExistingDirectoriesSplited(const String& path, List<String>& result, List<String>& archivePath) const
		{
			for (int32 i = 0; i < m_workingDirs.getCount(); i++)
			{
				String fullPath = PathUtils::Combine(m_workingDirs[i], path);
				if (File::DirectoryExists(fullPath))
				{
					result.Add(fullPath);
					archivePath.Add(L"");

				}

				String rel;
				// find the root archive if the path is a archive pack
				while (!PathUtils::ComparePath( fullPath, m_workingDirs[i]))
				{
					if (File::FileExists(fullPath))
					{
						// store the rel sub dir
						result.Add(rel);

						archivePath.Add(fullPath);
						break;
					}
					
					String fn;
					PathUtils::SplitFilePath(fullPath, fn, fullPath);
					rel = PathUtils::Combine(fn, rel);
				}

			}
			return result.getCount() != 0;
		}

		void FileSystem::ListDirectoryFiles(const String& path, List<String>& subItems, String* dirPath)
		{
			String fullPath;
			if (DirectoryExists(path, fullPath))
			{
				File::ListDirectoryFiles(fullPath, subItems);
				if (dirPath)
					*dirPath = fullPath;
			}
		}

		Archive* FileSystem::LocateArchive(const String& filePath, const FileLocateRule& rule)
		{
			Archive* res;
			if (ObtainOpenedArchive(filePath, res))
			{
				return res;
			}

			FileLocation fl = Locate(filePath, rule);
			String fullPath = PathUtils::NormalizePath(fl.getPath());

			if (ObtainOpenedArchive(fullPath, res))
			{
				return res;
			}

			res = CreateArchive(fl);
			StoreNewArchive(fullPath, res);
			
			return res;
		}

		FileLocation FileSystem::Locate(const String& filePath, const FileLocateRule& rule)
		{
			FileLocation res;
			if (!TryLocate(filePath, rule, res))
			{
				AP_EXCEPTION(ErrorID::FileNotFound, filePath);
			}
			return res;
		}

		bool FileSystem::TryLocate(const String& filePath, const FileLocateRule& rule, FileLocation& result)
		{
			// pass through all check points
			for (int cp = 0; cp < rule.getCount(); cp++)
			{
				const LocateCheckPoint& checkPt = rule.getCheckPoint(cp);
				
				for (int j = 0; j < checkPt.getCount(); j++)
				{
					if (!checkPt.hasArchivePath(j))
					{
						String fullPath = PathUtils::Combine(checkPt.GetPath(j), filePath);
						if (File::FileExists(fullPath))
						{
							result = FileLocation(fullPath);
							return true;
						}
					}
					else
					{
						String arcPath = checkPt.GetArchivePath(j);
						PathUtils::NormalizePath(arcPath);

						List<String> locs = PathUtils::Split(PathUtils::Combine(checkPt.GetPath(j), filePath));

						if (locs.getCount() > 1)
						{
							String sb;

							Archive* entry = nullptr;
							if (!ObtainOpenedArchive(arcPath, entry))
							{
								entry = CreateArchive(arcPath);
								StoreNewArchive(PathUtils::NormalizePath(arcPath), entry);
							}
							Archive* parent;

							bool found = true;

							for (int32 i = 0; i < locs.getCount() - 1; i++)
							{
								if (i > 0)
								{
									sb.append(1, PathUtils::DirectorySeparator);
									sb.append(locs[i]);
								}
								else
									sb.append(locs[i]);

								parent = entry;

								String afp = PathUtils::Combine(parent->getDirectory(), parent->getFileName());
								String fPath = PathUtils::Combine(afp, sb);
								fPath = PathUtils::NormalizePath(fPath);

								// check if the archive has never been opened
								if (!ObtainOpenedArchive(fPath, entry))
								{
									// if the archive is in a parent archive
									//if (parent)
									{
										if (parent->HasEntry(locs[i]))
										{
											entry = CreateArchive(FileLocation(parent, fPath, locs[i]));
											StoreNewArchive(fPath, entry);
										}
										else
										{
											found = false;
											break;
										}
									}
									//else
									//{
									//	String arc = sb;
									//	if (File::FileExists(arc))
									//	{
									//		entry = CreateArchive(arc);
									//		m_openedPack.insert(pair<String, Archive*>(PathUtils::NormalizePath(arc), entry));
									//	}
									//	else
									//	{
									//		found = false;
									//		break;
									//	}
									//}
								}
							}
							if (found && entry)
							{
								const String& entName = locs[locs.getCount()-1];
								if (entry->HasEntry(entName))
								{
									String afp = PathUtils::Combine(entry->getDirectory(), entry->getFileName());
									result = FileLocation(entry, PathUtils::Combine(afp, filePath), entName);
									return true;
								}
							}
						}
						else
						{
							Archive* entry;
							if (!ObtainOpenedArchive(arcPath, entry))
							{
								if (File::FileExists(arcPath))
								{
									entry = CreateArchive(arcPath);
									StoreNewArchive(arcPath, entry);
								}
							}
							if (entry)
							{
								if (entry->HasEntry(locs[0]))
								{
									String afp = PathUtils::Combine(entry->getDirectory(), entry->getFileName());
									result = FileLocation(entry, PathUtils::Combine(afp, filePath), locs[0]);
									return true;
								}
							}
						}
						
					} // if archive

				}

			}
			return false;
		}
		bool FileSystem::TryLocate(const String& filePath, const FileLocateRule& rule)
		{
			FileLocation fl;
			return TryLocate(filePath, rule, fl);
		}

		bool FileSystem::ObtainOpenedArchive(const String& filePath, Archive*& entry) 
		{
			bool result = false;

			ArchiveKey ak;
			ak.filePath = filePath;
			ak.threadID = tthread::this_thread::get_id().hash();
			
			m_openedPackMutex.lock();
			Archive* pack;
			if (m_openedPack.TryGetValue(ak, pack))
			{
				entry = pack;
				result = true;
			}
			m_openedPackMutex.unlock();

			return result;
		}

		void FileSystem::StoreNewArchive(const String& filePath, Archive* arc)
		{
			ArchiveKey ak;
			ak.filePath = filePath;
			ak.threadID = tthread::this_thread::get_id().hash();

			m_openedPackMutex.lock();

			m_openedPack.Add(ak, arc);

			m_openedPackMutex.unlock();
		}



		Archive* FileSystem::CreateArchive(const FileLocation& fl)
		{
			String fileName;
			String fileExt;
			PathUtils::SplitFileNameExtension(fl.getPath(), fileName, fileExt);

			ArchiveFactory* fac = FindArchiveFactory(fileExt);
			if (fac)
				return fac->CreateInstance(fl);
			AP_EXCEPTION(ErrorID::NotSupported, fileExt);
			return nullptr;
		}
		Archive* FileSystem::CreateArchive(const String& file)
		{
			String fileName;
			String fileExt;
			PathUtils::SplitFileNameExtension(file, fileName, fileExt);

			ArchiveFactory* fac = FindArchiveFactory(fileExt);
			if (fac)
				return fac->CreateInstance(file);
			AP_EXCEPTION(ErrorID::NotSupported, fileExt);
			return nullptr;
		}


		bool FileSystem::ArchiveKeyEqualityComparer::Equals(const FileSystem::ArchiveKey& x, const FileSystem::ArchiveKey& y) 
		{
			return x.threadID == y.threadID && x.filePath == y.filePath; 
		}
		int64 FileSystem::ArchiveKeyEqualityComparer::GetHashCode(const FileSystem::ArchiveKey& obj) 
		{
			return StringUtils::GetHashCode(obj.filePath) ^ obj.threadID;
		}

	}
}