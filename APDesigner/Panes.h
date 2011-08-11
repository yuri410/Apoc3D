#ifndef PANES_H
#define PANES_H

#include "APDCommon.h"

#include "Collections/FastList.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D::Core;

namespace APDesigner
{
	class ToolsPane
	{
	private:
		MainWindow* m_mainWindow;

		Form* m_pane;
		Button* m_btSave;

		ObjectTools* m_currentTools;
	public:
		ToolsPane(MainWindow* window);
		~ToolsPane();


		void Initialize(RenderDevice* device);

		void Update(const GameTime* const time);
	};


	class ResourcePane
	{
	private:
		//static const int ItemHeight = 60;
		MainWindow* m_mainWindow;

		Form* m_form;

		TreeView* m_resourceView;

		Project* m_currentProject;

		void NukeTreeViewNodes(FastList<TreeViewNode*>& nodes);
		void NukeTreeView();
	public:
		
		ResourcePane(MainWindow* window);
		~ResourcePane();

		void Initialize(RenderDevice* device);

		void Update(const GameTime* const time);

		void UpdateToNewProject(Project* prj);
	};

	class PropertyPane
	{
	private:
		ObjectPropertyEditor* m_currentEditor;

		
	public:

		PropertyPane(MainWindow* window);

		void Initialize(RenderDevice* device);
		void Update(const GameTime* const time);
	};
}

#endif