#pragma once
#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include "APDesigner/APDCommon.h"

#include "DialogCommon.h"

using namespace Apoc3D;

namespace APDesigner
{
	namespace CommonDialog
	{
		class FileDialog
		{
		public:
			wchar_t Filter[1024];
			uint FilterIndex;
			String InitialDirectory;
			String Title;

			void SetFilter(const wchar_t* str)
			{
				int len = 0;
				while (len<1024)
				{
					if (!str[len] && !str[len+1])
					{
						len += 2;
						break;
					}
					len++;
				}
				memcpy(Filter, str, len * sizeof(wchar_t));
			}
			void SetFilter(const String& fileTypeName, const List<String>& exts);
			void SetAllFilesFilter();
		protected:
			wchar_t szFile[10240]; 

			FileDialog()
			{
				memset(szFile, 0, sizeof(szFile));

				SetFilter(L"All\0*.*\0\0");
			}
		};
		/** Prompts the user to open a file.
		*/
		class OpenFileDialog : public FileDialog
		{
		private:
			List<String> m_filePath;
		public:
			bool MultiSelect;

			const List<String>& getFilePath() const { return m_filePath; }


			OpenFileDialog() : MultiSelect(false) 
			{
				Title = L"Open";
			}
			DialogResult ShowDialog();
		};

		class SaveFileDialog : public FileDialog
		{
		private:
			List<String> m_filePath;
		public:

			const List<String>& getFilePath() const { return m_filePath; }
			

			SaveFileDialog()
			{
				Title = L"Save";
			}
			DialogResult ShowDialog();
		};
		 
	}
}
#endif