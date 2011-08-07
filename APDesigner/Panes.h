#ifndef PANES_H
#define PANES_H

#include "APDCommon.h"

using namespace Apoc3D::UI;

namespace APDesigner
{
	class ResourcePane
	{
	private:
		static const int ItemHeight = 60;

		Form* m_form;
	public:
		ResourcePane();
		~ResourcePane();
	};

	class PropertyPane
	{

	};
}

#endif