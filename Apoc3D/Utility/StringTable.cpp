#include "StringTable.h"

#include "apoc3d/VFS/ResourceLocation.h"
#include "apoc3d/Utility/StringUtils.h"

namespace Apoc3D
{
	namespace Utility
	{
		String StringTable::GetString(const std::string& name) const
		{
			const Entry* e = m_entryTable.TryGetValue(name);
			if (e)
			{
				return e->Text;
			}
			return L"MISSING: " + StringUtils::toASCIIWideString(name);
		}
	}
}