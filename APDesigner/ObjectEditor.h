#ifndef OBJECTPROPERTYEDITOR_H
#define OBJECTPROPERTYEDITOR_H

#include "APDCommon.h"

using namespace Apoc3D::UI;

namespace APDesigner
{
	class ObjectPropertyEditor
	{
	private:

	public:

	};
	class ObjectTools
	{
	private:
		Form* m_pane;
		Button* m_btSave;
		
	public:
		ObjectTools(MainWindow* window);
		~ObjectTools();
	};


}

#endif