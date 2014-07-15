#include "FileDialog.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/PathUtils.h"

#include <Commdlg.h>

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

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
			ofn.hwndOwner = 0;//GetForegroundWindow();
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
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
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
					m_filePath.Clear();

					// the first one is either the file path(when only one is selected) or the base path of all the files selected
					String basePath;
					String name;
					int pos = 0;
					while (1)
					{
						if (!ofn.lpstrFile[pos])
						{
							if (name.empty())
								break;

							if (basePath.empty())
							{
								basePath = name;
								name.clear();
							}
							else
							{
								if (!basePath.empty())
								{
									m_filePath.Add(PathUtils::Combine(basePath, name));
								}
								else
								{
									m_filePath.Add(name);
								}
								name.clear();
							}

						}
						else
						{
							name.append(1,ofn.lpstrFile[pos]);
						}

						pos++;
					}

					// add the only when file when only one item is selected in the dialog
					if (m_filePath.getCount()==0)
					{
						m_filePath.Add(basePath);
					}
				}
				else
				{
					m_filePath.Clear();
					m_filePath.Add(ofn.lpstrFile);//StringUtils::Split(ofn.lpstrFile, L"\0");
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
			ofn.hwndOwner = 0;//GetForegroundWindow();
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

			m_filePath.Clear();
			if (GetSaveFileName(&ofn)==TRUE) 
			{
				m_filePath.Add(ofn.lpstrFile);
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
