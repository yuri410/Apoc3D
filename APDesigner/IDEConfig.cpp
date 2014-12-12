#include "IDEConfig.h"

namespace APDesigner
{
	const int MaxRecentProjects = 7;

	Queue<std::pair<String, String>> RecentProjects;

	void cfInitializeConfig()
	{
		if (File::FileExists(L"ideconf.xml"))
		{
			Configuration conf;
			XMLConfigurationFormat::Instance.Load(FileLocation(L"ideconf.xml"), &conf);

			ConfigurationSection* sect = conf[L"Recents"];
			if (sect)
			{
				for (ConfigurationSection* ss : sect->getSubSections())
				{
					String name = ss->getAttribute(L"Name");
					String value = ss->getValue();
					RecentProjects.Enqueue(std::make_pair(name, value));
				}
			}
			
		}
	}

	void cfFlushConfig()
	{
		Configuration conf(L"IDE");

		ConfigurationSection* sect = new ConfigurationSection(L"Recents");
		conf.Add(sect);

		for (int32 i = RecentProjects.getCount() - 1; i >= 0; i--)
		{
			const std::pair<String, String>& p = RecentProjects.GetElement(i);

			ConfigurationSection* ss = new ConfigurationSection(L"Item" + StringUtils::IntToString(i));
			ss->AddAttributeString(L"Name", p.first);
			ss->SetValue(p.second);
			sect->AddSection(ss);
		}

		XMLConfigurationFormat::Instance.Save(&conf, FileOutStream(L"ideconf.xml"));
	}

	const Queue<std::pair<String, String>>& cfGetRecentProjects() { return RecentProjects; }
	void cfAddRecentProject(const String& name, const String& path)
	{
		Queue<std::pair<String, String>> newList;
		for (int i = RecentProjects.getCount() - 1; i >= 0; i--)
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
		for (int i = RecentProjects.getCount() - 1; i >= 0; i--)
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