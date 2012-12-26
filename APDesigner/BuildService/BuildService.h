#ifndef BUILDSERVICE_H
#define BUILDSERVICE_H

#include "APDCommon.h"
#include "Collections/FastList.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;

namespace APDesigner
{
	class BuildInterface
	{
	private:
		static int ExecuteBuildOperation();
	public:
		static std::vector<String> LastResult;
		static void BuildSingleItem(ProjectItem* item);
		static void BuildAll(Project* project);
	};


}

#endif