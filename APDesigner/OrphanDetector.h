#pragma once

#include "APDCommon.h"

namespace APDesigner
{
	class OrphanDetector
	{
	public:
		OrphanDetector(MainWindow* window);
		~OrphanDetector();

		void Initialize(RenderDevice* device);

		void UpdateToNewProject(Project* prj);

		void Show();
		void Hide();

	private:
		void UpdateListView();
		void PassProject(const List<ProjectItem*>& items, HashSet<String, StringEqualityComparerNoCase>& allOutputFiles);

		void BtnRemove_Release(Button* ctrl);

		MainWindow* m_mainWindow;
		const StyleSkin* m_skin;

		Form* m_form;

		ListView* m_orphanList;
		Button* m_removeItem;

		Project* m_currentProject = nullptr;

	};
}