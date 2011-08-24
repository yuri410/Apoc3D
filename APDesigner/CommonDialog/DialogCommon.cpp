#include "DialogCommon.h"

#include <Windows.h>

namespace APDesigner
{
	namespace CommonDialog
	{
		DialogResult ShowMessageBox(const String& message, const String& title, bool isError /* = false */, bool isEx /* = false */, bool isInfo /* = false */)
		{
			UINT flag = 0;
			if (isError)
				flag |= MB_ICONSTOP;
			else if (isEx)
				flag |= MB_ICONEXCLAMATION;
			else if (isInfo)
				flag |= MB_ICONINFORMATION;

			int result = MessageBoxEx(GetForegroundWindow(), message.c_str(), title.c_str(), MB_OK | flag, 0);

			switch (result)
			{
			case IDABORT:
				return DLGRES_Abort;
			case IDCANCEL:
				return DLGRES_Cancel;
			case IDIGNORE:
				return DLGRES_Ignore;
			case IDNO:
				return DLGRES_No;
			case IDOK:
				return DLGRES_OK;
			case IDRETRY:
				return DLGRES_Retry;
			case IDYES:
				return DLGRES_Yes;
			}
			return DLGRES_Cancel;
		}
	}
}