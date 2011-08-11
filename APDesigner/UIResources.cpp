#include "UIResources.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

#include "Vfs/Archive.h"
#include "Vfs/FileSystem.h"
#include "Vfs/PathUtils.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/ResourceLocation.h"
#include "Graphics/TextureManager.h"

namespace APDesigner
{
	FastMap<String, Texture*> UIResources::m_maps(10, IBuiltInEqualityComparer<String>::Default);

	void UIResources::Initialize(RenderDevice* device)
	{
		Archive* arc = FileSystem::getSingleton().LocateArchive(L"apdui_icons.pak", FileLocateRule::Default);
		int count = arc->getFileCount();
		for (int i=0;i<count;i++)
		{
			String name = arc->GetEntryName(i);
			Stream* strm = arc->GetEntryStream(name);
			
			FileLocation* fl = new FileLocation(arc, PathUtils::Combine(arc->getFilePath(), name), strm);

			m_maps.Add(PathUtils::GetFileNameNoExt(name), TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false));
		}
	}
	void UIResources::Finalize()
	{
	
	}
}