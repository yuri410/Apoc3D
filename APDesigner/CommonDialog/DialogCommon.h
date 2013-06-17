#ifndef DIALOGCOMMON_H
#define DIALOGCOMMON_H

#include "APDesigner/APDCommon.h"
#include "apoc3d/UILib/Dialogs.h"

using namespace Apoc3D;
using namespace Apoc3D::UI;

namespace APDesigner
{
	namespace CommonDialog
	{
		DialogResult ShowMessageBox(const String& message, const String& title, bool isError = false, bool isEx = false, bool isInfo = false);
	}
}

#endif