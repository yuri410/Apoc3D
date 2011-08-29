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
#include "FileSystem.h"

#include "PathUtils.h"
#include "Utility/StringUtils.h"
#include "ResourceLocation.h"
#include "Archive.h"
#include "Apoc3DException.h"
#include "FileLocateRule.h"
#include "File.h"

using namespace Apoc3D::Utility;

SINGLETON_DECL(Apoc3D::VFS::FileSystem);

namespace Apoc3D
{
	namespace VFS
	{
		//void FileSystem::Initialize()
		//{
		//	new FileSystem();
		//}


		FileSystem::~FileSystem()
		{
			for (auto e = m_openedPack.begin(); e!=m_openedPack.end(); e++)
			{				
				Archive* arc = e->second;
				delete arc;
			}
			Singleton::~Singleton();
		}


		Archive* FileSystem::CreateArchive(FileLocation* fl)
		{
			String fileName;
			String fileExt;
			PathUtils::SplitFileNameExtension(fl->getPath(), fileName, fileExt);

			ArchiveFactory* fac = FindArchiveFactory(fileExt);
			if (fac)
				return fac->CreateInstance(fl);
			throw Apoc3DException::createException(EX_NotSupported, fileExt.c_str());
		}
		Archive* FileSystem::CreateArchive(const String& file)
		{
			String fileName;
			String fileExt;
			PathUtils::SplitFileNameExtension(file, fileName, fileExt);

			ArchiveFactory* fac = FindArchiveFactory(fileExt);
			if (fac)
				return fac->CreateInstance(file);
			throw Apoc3DException::createException(EX_NotSupported, fileExt.c_str());
		}


		void FileSystem::AddWrokingDirectory(const String& path)
		{
			String _path = path;
			
			//if (!path.EndsWith(Path.DirectorySeparatorChar.ToString()))
				//path += Path.DirectorySeparatorChar;

			m_workingDirs.push_back(_path);
		}
		void FileSystem::RegisterArchiveType(ArchiveFactory* factory)
		{
			m_factories.insert(make_pair(factory->getExtension(), factory));
		}

		bool FileSystem::UnregisterArchiveType(ArchiveFactory* factory)
		{
			PackFactoryTable::iterator iter = m_factories.find(factory->getExtension());
			if (iter == m_factories.end())
			{
				m_factories.erase(factory->getExtension());
				return true;
			}
			return false;
		}


		bool FileSystem::DirectoryExists(const String& path) const
		{
			for (size_t i = 0; i < m_workingDirs.size(); i++)
			{
				String fullPath = PathUtils::Combine(m_workingDirs[i], path);
				if (File::DirectoryExists(fullPath))
				{
					return true;
				}

				while (!PathUtils::ComparePath( fullPath, m_workingDirs[i]))
				{
					fullPath = PathUtils::GetDirectory(fullPath);
					if (File::FileExists(fullPath))
					{
						return true;
					}
				}

			}
			return false;
		}
		bool FileSystem::DirectoryExists(const String& path, String& result) const
		{
			for (size_t i = 0; i < m_workingDirs.size(); i++)
			{
				String fullPath = PathUtils::Combine(m_workingDirs[i], path);
				if (File::DirectoryExists(fullPath))
				{
					result = fullPath;
					return true;
				}


				while (!PathUtils::ComparePath( fullPath, m_workingDirs[i]))
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
		bool FileSystem::SplitExistingDirectories(const String& path, vector<String>& result, vector<String>& archivePath) const
		{
			for (size_t i = 0; i < m_workingDirs.size(); i++)
			{
				String fullPath = PathUtils::Combine(m_workingDirs[i], path);
				if (File::DirectoryExists(fullPath))
				{
					//archivePath = null;
					//result = fullPath;
					//return true;
					result.push_back(fullPath);
					archivePath.push_back(L"");

				}

				String rel;
				// find the root archive if the path is a archive pack
				while (!PathUtils::ComparePath( fullPath, m_workingDirs[i]))
				{
					if (File::FileExists(fullPath))
					{
						// store the rel sub dir
						result.push_back(rel);//PathUtils::Combine(m_workingDirs[i], path));

						archivePath.push_back(fullPath);
						break;
					}
					
					
					String temp;
					PathUtils::SplitFilePath(fullPath, temp, fullPath);
					rel = PathUtils::Combine(temp, rel);
				}

			}
			return !result.empty();
		}


		Archive* FileSystem::LocateArchive(const String& filePath, const FileLocateRule& rule)
		{
			Archive* res;
			if (IsOpened(filePath, &res))
			{
				return res;
			}
			FileLocation* fl = Locate(filePath, rule);
			res = CreateArchive(fl);
			m_openedPack.insert(pair<String, Archive*>(
				PathUtils::Combine(PathUtils::NormalizePath(res->getDirectory()), res->getFileName()),
				res));
			return res;
		}

		FileLocation* FileSystem::Locate(const String& filePath, const FileLocateRule& rule)
		{
			FileLocation* res = TryLocate(filePath, rule);
			if (!res)
				throw Apoc3DException::createException(EX_FileNotFound, filePath.c_str());
			return res;
		}

		FileLocation* FileSystem::TryLocate(const String& filePath, const FileLocateRule& rule)
		{
			// pass through all check points
			for (int cp = 0; cp < rule.getCount(); cp++)
			{
				LocateCheckPoint checkPt = rule.getCheckPoint(cp);
				
				for (int j = 0; j < checkPt.getCount(); j++)
				{
					//// 如果检查点要求搜索CurrectArchiveSet
					//if (checkPt.SearchesCurrectArchiveSet)
					//{
					//	for (int i = 0; i < CurrentArchiveSet.Count; i++)
					//	{
					//		Stream entStm = CurrentArchiveSet[i].GetEntryStream(filePath);

					//		if (entStm != null)
					//			return new FileLocation(CurrentArchiveSet[i], CurrentArchiveSet[i].FilePath + Path.DirectorySeparatorChar + filePath, entStm);
					//	}
					//}


					if (!checkPt.hasArchivePath(j))
					{
						String fullPath = PathUtils::Combine(checkPt.GetPath(j), filePath);
						if (File::FileExists(fullPath))
						{
							return new FileLocation(fullPath);
						}
					}
					else
					{
						String arcPath = checkPt.GetArchivePath(j);
						PathUtils::NormalizePath(arcPath);

						//try
						{
							vector<String> locs = PathUtils::Split(PathUtils::Combine(checkPt.GetPath(j), filePath));

							if (locs.size() > 1)
							{
								String sb;

								Archive* entry = 0;
								if (!IsOpened(arcPath, &entry))
								{
									entry = CreateArchive(arcPath);
									m_openedPack.insert(pair<String, Archive*>(PathUtils::NormalizePath(arcPath), entry));
								}
								Archive* parent;

								bool found = true;

								for (size_t i = 0; i < locs.size() - 1; i++)
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

									// check if the archive is never been opened
									if (!IsOpened(fPath, &entry))
									{
										// if the archive is in a parent archive
										//if (parent)
										{
											Stream* entStm = parent->GetEntryStream(locs[i]);

											if (entStm)
											{
												entry = CreateArchive(
													new FileLocation(parent, fPath, entStm));
												m_openedPack.insert(pair<String, Archive*>(fPath, entry));
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
									
									Stream* entStm = entry->GetEntryStream(locs[locs.size() - 1]);

									if (entStm)
									{
										String afp = PathUtils::Combine(entry->getDirectory(), entry->getFileName());
										return new FileLocation(entry, PathUtils::Combine(afp, filePath), entStm);
									}
								}
							}
							else
							{
								Archive* entry;
								if (!IsOpened(arcPath, &entry))
								{
									if (File::FileExists(arcPath))
									{
										entry = CreateArchive(arcPath);
										m_openedPack.insert(pair<String, Archive*>(arcPath, entry));
									}
								}
								if (entry)
								{
									Stream* entStm = entry->GetEntryStream(locs[0]);

									if (entStm)
									{
										String afp = PathUtils::Combine(entry->getDirectory(), entry->getFileName());
										return new FileLocation(entry, PathUtils::Combine(afp, filePath), entStm);
									}
								}
							}
						}
						//catch (Apoc3DException e)
						{
							//EngineConsole.Instance.Write("文件格式不对", ConsoleMessageType.Warning);
						}
					} // if archive

				}

			}
			return 0;
		}
	}
}