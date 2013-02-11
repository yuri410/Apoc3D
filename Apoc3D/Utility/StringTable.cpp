#include "StringTable.h"

#include "VFS\ResourceLocation.h"

namespace Apoc3D
{
	namespace Utility
	{
		StringTable* StringTableLoader::Load(const ResourceLocation* rl)
		{
			StringTable* st = new StringTable();
			Read(st, rl->GetReadStream());
			return st;
		}

		StringTable* StringTableLoader::Load(const String &file)
		{
			const FileLocation fl(file);
			return Load(&fl);
		}
	}
}