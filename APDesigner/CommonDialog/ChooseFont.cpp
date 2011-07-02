#include "ChooseFont.h"
#include "Utility/StringUtils.h"
#include <Windows.h>

using namespace Apoc3D::Utility;

namespace APDesigner
{
	namespace CommonDialog
	{

		DialogResult ChooseFontDialog::ShowDialog()
		{
			//HWND hwnd;                // owner window
			//HDC hdc;                  // display device context of owner window

			CHOOSEFONT cf;            // common dialog box structure
			static LOGFONT lf;        // logical font structure
			static DWORD rgbCurrent;  // current text color
			HFONT hfont, hfontPrev;
			DWORD rgbPrev;

			// Initialize CHOOSEFONT
			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof (cf);
			cf.hwndOwner = 0;
			cf.lpLogFont = &lf;
			cf.rgbColors = rgbCurrent;
			cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

			if (ChooseFont(&cf)==TRUE)
			{
				//hfont = CreateFontIndirect(cf.lpLogFont);
				//hfontPrev = SelectObject(hdc, hfont);
				//rgbCurrent= cf.rgbColors;
				//rgbPrev = SetTextColor(hdc, rgbCurrent);

				m_fontFamilyName = (lf.lfFaceName);

				if (lf.lfItalic)
				{
					m_isBold = true;
				}

				return DLGRES_OK;
			}
			return DLGRES_Cancel;
		}
	}
}
