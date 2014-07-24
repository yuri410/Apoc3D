#include "UIResources.h"

namespace APDesigner
{
	HashMap<String, Texture*> m_maps;

	void UIResources::Initialize(RenderDevice* device)
	{
		FileLocateRule rule;
		LocateCheckPoint pt;
		pt.AddPath(L"system.pak");
		rule.AddCheckPoint(pt);

		Archive* arc = FileSystem::getSingleton().LocateArchive(L"apdgui.pak", rule);
		int count = arc->getFileCount();
		
		m_maps.Resize(count);

		for (int i=0;i<count;i++)
		{
			String name = arc->GetEntryName(i);
			//Stream* strm = arc->GetEntryStream(name);
			
			FileLocation fl(arc, PathUtils::Combine(arc->getDirectory(), name), name);

			m_maps.Add(PathUtils::GetFileNameNoExt(name), TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false));
		}
	}
	void UIResources::Finalize()
	{
		m_maps.DeleteValuesAndClear();
	}

	Texture* UIResources::GetTexture(const String& name)
	{
		return m_maps[name];
	}
}