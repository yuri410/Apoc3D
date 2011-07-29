
#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include "APDCommon.h"
#include "DialogCommon.h"

using namespace Apoc3D;

namespace APDesigner
{
	namespace CommonDialog
	{
		class FileDialog
		{
		public:
			String Filter;
			uint FilterIndex;
			String InitialDirectory;
			String Title;
		protected:
			wchar_t szFile[10240]; 

			FileDialog()
			{
				Filter = L"All\0*.*\0";
			}
		};
		/** Prompts the user to open a file.
		*/
		class OpenFileDialog : public FileDialog
		{
		private:
			std::vector<String> m_filePath;
		public:
			bool MultiSelect;

			const std::vector<String>& getFilePath() const { return m_filePath; }


			OpenFileDialog() : MultiSelect(false) 
			{
				Title = L"Open";
			}
			DialogResult ShowDialog();
		};

		class SaveFileDialog : public FileDialog
		{
		private:
			std::vector<String> m_filePath;
		public:

			const std::vector<String>& getFilePath() const { return m_filePath; }
			

			SaveFileDialog()
			{
				Title = L"Save";
			}
			DialogResult ShowDialog();
		};
		 
	}
}
#endif