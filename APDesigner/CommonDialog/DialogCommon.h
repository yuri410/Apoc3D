#pragma once
#ifndef DIALOGCOMMON_H
#define DIALOGCOMMON_H

#include "APDesigner/APDCommon.h"

namespace APDesigner
{
	namespace CommonDialog
	{
		DialogResult ShowMessageBox(const String& message, const String& title, bool isError = false, bool isEx = false, bool isInfo = false);
	}
}

#endif