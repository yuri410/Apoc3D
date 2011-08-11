#ifndef DIALOGCOMMON_H
#define DIALOGCOMMON_H

#include "APDCommon.h"

using namespace Apoc3D;

namespace APDesigner
{
	namespace CommonDialog
	{
		enum DialogResult
		{
			DLGRES_None,
			DLGRES_OK,
			DLGRES_Cancel,
			DLGRES_Abort,
			DLGRES_Retry,
			DLGRES_Ignore,
			DLGRES_Yes,
			DLGRES_No

		};

		DialogResult ShowMessageBox(const String& message, const String& title, bool isError = false, bool isEx = false, bool isInfo = false);
	}
}

#endif