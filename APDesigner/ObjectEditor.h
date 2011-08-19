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
	class ProjectFolderItemEditor : public ObjectPropertyEditor
	{

	};

	class ProjectTextureItemEditor : public ObjectPropertyEditor
	{

	};
	class ProjectModelItemEditor : public ObjectPropertyEditor
	{

	};
	class ProjectFontItemEditor : public ObjectPropertyEditor
	{

	};

	class ObjectTools
	{
	private:
		
		
	public:
		ObjectTools(MainWindow* window);
		~ObjectTools();
	};


}

#endif