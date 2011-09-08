#include "ChooseColor.h"

#include <Windows.h>
#undef GetGValue
#define GetGValue(rgb) (LOBYTE((rgb) >> 8))

namespace APDesigner
{
	namespace CommonDialog
	{

		DialogResult ChooseColorDialog::ShowDialog()
		{
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 

			static DWORD rgbCurrent;        // initial color selection
			rgbCurrent = RGB(GetColorR(m_selectedColor), GetColorG(m_selectedColor), GetColorB(m_selectedColor));

			// Initialize CHOOSECOLOR 
			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = GetForegroundWindow();
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbCurrent; 
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc)==TRUE) 
			{
				//rgbCurrent = cc.rgbResult; 
				m_selectedColor = PACK_COLOR(
					GetRValue(cc.rgbResult)&0xff, 
					GetGValue(cc.rgbResult)&0xff,
					GetBValue(cc.rgbResult)&0xff,
					0xff);
				return DLGRES_OK;
			}
			m_selectedColor = 0;
			return DLGRES_Cancel;
		}

		void ChooseColorDialog::setSelectedColor(ColorValue cv)
		{
			m_selectedColor = cv;
		}
	}
}