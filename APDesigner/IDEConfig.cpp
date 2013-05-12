#include "IDEConfig.h"

#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;

namespace APDesigner
{
	const int MaxRecentProjects = 7;

	Queue<std::pair<String, String>> RecentProjects;

	void cfInitializeConfig()
	{
		if (File::FileExists(L"ideconf.xml"))
		{
			FileLocation fl(L"ideconf.xml");
			Configuration* conf = XMLConfigurationFormat::Instance.Load(&fl);

			ConfigurationSection* sect = conf->get(L"Recents");
			for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
			{
				ConfigurationSection* ss = *e.getCurrentValue();

				String name = ss->getAttribute(L"Name");
				String value = ss->getValue();
				RecentProjects.Enqueue(std::make_pair(name, value));
			}
			
			delete conf;
		}
	}

	void cfFlushConfig()
	{
		Configuration* conf = new Configuration(L"IDE");

		ConfigurationSection* sect = new ConfigurationSection(L"Recents");
		conf->Add(sect);

		for (int i=RecentProjects.getCount()-1;i>=0;i--)
		{
			const std::pair<String, String>& p = RecentProjects.GetElement(i);

			ConfigurationSection* ss = new ConfigurationSection(L"Item" + StringUtils::ToString(i));
			ss->AddAttributeString(L"Name", p.first);
			ss->SetValue(p.second);
			sect->AddSection(ss);
		}

		FileOutStream* fs = new FileOutStream(L"ideconf.xml");
		XMLConfigurationFormat::Instance.Save(conf, fs);

		delete conf;
	}

	const Queue<std::pair<String, String>>& cfGetRecentProjects() { return RecentProjects; }
	void cfAddRecentProject(const String& name, const String& path)
	{
		Queue<std::pair<String, String>> newList;
		for (int i=RecentProjects.getCount()-1;i>=0;i--)
		{
			if (!PathUtils::ComparePath(RecentProjects.GetElement(i).second, path))
			{
				newList.Enqueue(RecentProjects.GetElement(i));
			}
		}
		RecentProjects = newList;

		while (RecentProjects.getCount()>=MaxRecentProjects)
			RecentProjects.DequeueOnly();

		RecentProjects.Enqueue(std::make_pair(name, path));
		cfFlushConfig();
	}
	void cfRemoveRecentProject(int index)
	{
		Queue<std::pair<String, String>> newList;
		for (int i=RecentProjects.getCount()-1;i>=0;i--)
		{
			if (i != index)
			{
				newList.Enqueue(RecentProjects.GetElement(i));
			}
		}
		RecentProjects = newList;
		cfFlushConfig();
	}
}