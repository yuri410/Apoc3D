#ifndef PANES_H
#define PANES_H

#include "APDCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D::Core;

namespace APDesigner
{
	class ToolsPane
	{
	private:

	public:

	};
	class ResourcePane
	{
	private:
		static const int ItemHeight = 60;
		MainWindow* m_mainWindow;

		Form* m_form;

		TreeView* m_resourceView;
	public:
		
		ResourcePane(MainWindow* window);
		~ResourcePane();

		void Initialize(RenderDevice* device);

		void Update(const GameTime* const time);
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