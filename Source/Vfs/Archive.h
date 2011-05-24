
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
#ifndef ARCHIVE_H
#define ARCHIVE_H


#include "Common.h"
#include "File.h"

namespace Apoc3D
{
	namespace VFS
	{
		/** Abstract factory that create archive objects.
			One archive object keeps the archive file open during its lifetime.
		*/
		class APAPI ArchiveFactory
		{
		public:
			virtual Archive* CreateInstance(const String& file) = 0;
			virtual Archive* CreateInstance(const FileLocation* fl) = 0;

			virtual String getExtension() const = 0;
		};

		/** Define interface for accessing pack files, like .zip or custom packs.
		*/
		class APAPI Archive : public File
		{
		protected:
			Archive(const String& file, int64 size, bool isInArchive);
		public:
			virtual int getFileCount() const = 0;
			virtual Stream* GetEntryStream(const String& file) = 0;

		};
	}
}

#endif