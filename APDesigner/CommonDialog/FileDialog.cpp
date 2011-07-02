#include "FileDialog.h"
#include "Utility/StringUtils.h"

#include <Windows.h>

using namespace Apoc3D::Utility;

namespace APDesigner
{
	namespace CommonDialog
	{
		//wchar_t FileDialog::szFile[10240];

		DialogResult OpenFileDialog::ShowDialog()
		{
			OPENFILENAME ofn;       // common dialog box structure
			
			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = 0;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrInitialDir = InitialDirectory.c_str();
			ofn.lpstrFilter = Filter.c_str();
			ofn.nFilterIndex = FilterIndex;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = Title.c_str();

			if (MultiSelect)
			{
				ofn.Flags |= OFN_ALLOWMULTISELECT;
			}
			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn)==TRUE) 
			{
				m_filePath = StringUtils::Split(ofn.lpstrFile, L"\0");
				//hf = CreateFile(ofn.lpstrFile, 
				//GENERIC_READ,
				//0,
				//(LPSECURITY_ATTRIBUTES) NULL,
				//OPEN_EXISTING,
				//FILE_ATTRIBUTE_NORMAL,
				//(HANDLE) NULL);
				return DLGRES_OK;
			}
			return DLGRES_Cancel;

		}

		DialogResult SaveFileDialog::ShowDialog()
		{
			OPENFILENAME ofn;       // common dialog box structure

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = 0;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrInitialDir = InitialDirectory.c_str();
			ofn.lpstrFilter = Filter.c_str();
			ofn.nFilterIndex = FilterIndex;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = Title.c_str();

			m_filePath.clear();
			if (GetSaveFileName(&ofn)==TRUE) 
			{
				m_filePath.push_back(ofn.lpstrFile);
				//hf = CreateFile(ofn.lpstrFile, 
				//GENERIC_READ,
				//0,
				//(LPSECURITY_ATTRIBUTES) NULL,
				//OPEN_EXISTING,
				//FILE_ATTRIBUTE_NORMAL,
				//(HANDLE) NULL);
				return DLGRES_OK;
			}
			return DLGRES_Cancel;

		}
	}
}
