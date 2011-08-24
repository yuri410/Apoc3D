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
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrInitialDir = InitialDirectory.c_str();
			//Filter = L"All Files\0*.*\0";
			ofn.lpstrFilter = Filter;// Filter.c_str();//"All Files\0*.*\0\0"
			
			ofn.nFilterIndex = FilterIndex;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			ofn.lpstrTitle = Title.c_str();

			if (MultiSelect)
			{
				ofn.Flags |= OFN_ALLOWMULTISELECT;
			}
			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn)==TRUE) 
			{
				if (MultiSelect)
				{
					m_filePath.clear();

					
					String name;
					int pos = 0;
					while (1)
					{
						if (!ofn.lpstrFile[pos])
						{
							if (!name.length())
								break;
							m_filePath.push_back(name);
							name.clear();
						}
						else
						{
							name.append(1,ofn.lpstrFile[pos]);
						}
						
						pos++;
					}
				}
				else
				{
					m_filePath.clear();
					m_filePath.push_back(ofn.lpstrFile);//StringUtils::Split(ofn.lpstrFile, L"\0");
				}
				
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
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrInitialDir = InitialDirectory.c_str();
			ofn.lpstrFilter = Filter;
			ofn.nFilterIndex = FilterIndex;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_NOCHANGEDIR;
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
