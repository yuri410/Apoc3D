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
			static const String SettingsFileName = L"apd_colordialog.bin";
			static const int32 CustomColorCount = 16;
			static const int32 FileID = 'ADCD';

			static COLORREF acrCustClr[CustomColorCount]; // array of custom colors 
			CHOOSECOLOR cc;                 // common dialog box structure 
			
			if (File::FileExists(SettingsFileName))
			{
				FileStream* fs = new FileStream(SettingsFileName);
				BinaryReader* br = new BinaryReader(fs);

				if (br->ReadInt32() == FileID)
				{
					for (int32 i=0;i<CustomColorCount;i++)
					{
						acrCustClr[i] = br->ReadUInt32();
					}
				}

				br->Close();
				delete br;
			}

			static DWORD rgbCurrent;        // initial color selection
			rgbCurrent = RGB(CV_GetColorR(m_selectedColor), CV_GetColorG(m_selectedColor), CV_GetColorB(m_selectedColor));

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
				m_selectedColor = CV_PackColor(
					GetRValue(cc.rgbResult)&0xff, 
					GetGValue(cc.rgbResult)&0xff,
					GetBValue(cc.rgbResult)&0xff,
					0xff);


				FileOutStream* fs = new FileOutStream(SettingsFileName);
				BinaryWriter* bw = new BinaryWriter(fs);

				bw->WriteInt32(FileID);
				for (int32 i=0;i<CustomColorCount;i++)
				{
					bw->WriteUInt32(static_cast<uint32>(acrCustClr[i]));
				}

				bw->Close();
				delete bw;

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