#include "ChooseColor.h"

#include <Windows.h>

namespace APDesigner
{
	namespace CommonDialog
	{

		DialogResult ChooseColorDialog::ShowDialog()
		{
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 

			static DWORD rgbCurrent;        // initial color selection

			// Initialize CHOOSECOLOR 
			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = GetForegroundWindow();
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc)==TRUE) 
			{
				rgbCurrent = cc.rgbResult; 
				m_selectedColor = PACK_COLOR(
					GetRValue(rgbCurrent)&0xff, 
					GetGValue(rgbCurrent)&0xff,
					GetBValue(rgbCurrent)&0xff,
					0xff);
				return DLGRES_OK;
			}
			m_selectedColor = 0;
			return DLGRES_Cancel;
		}
	}
}