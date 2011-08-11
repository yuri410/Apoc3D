#include "DevResourceLocation.h"

#include "Vfs/File.h"
#include "IOLib/Streams.h"

namespace APDesigner
{
	DevFileLocation::DevFileLocation(const String& filePath)
		: ResourceLocation(filePath, File::GetFileSize(filePath)), m_filePath(filePath)
	{			

	}

	Stream* DevFileLocation::GetReadStream() const
	{
		return new FileStream(m_filePath);
	}

	Stream* DevFileLocation::GetWriteStream() const
	{
		return new FileOutStream(m_filePath);
	}
}
