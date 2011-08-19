#include "BuildService.h"

#include "Project/Project.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"

#include <Windows.h>

using namespace Apoc3D;
using namespace Apoc3D::Config;

namespace APDesigner
{
	String BuildInterface::LastResult;

	void BuildInterface::BuildSingleItem(ProjectItem* item)
	{
		ConfigurationSection* sect = item->Save(true);

		XMLConfiguration* xml = new XMLConfiguration(L"Build");
		xml->Add(sect);


		xml->Save(L"build.xml");



		STARTUPINFO startUpInfo;
		ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
		//CreatePipe()
		startUpInfo.cb = sizeof(STARTUPINFO);
		startUpInfo.dwFlags |= STARTF_USESTDHANDLES;
		

		SECURITY_ATTRIBUTES pipeAttr;
		ZeroMemory(&pipeAttr, sizeof(SECURITY_ATTRIBUTES));
		pipeAttr.bInheritHandle = TRUE;

		HANDLE writePipe;
		BOOL result = CreatePipe(&writePipe, &startUpInfo.hStdOutput, &pipeAttr,0);
		startUpInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startUpInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		wchar_t workingDir[260];
		//String workingDir = buffer; 

		PROCESS_INFORMATION procInfo;

		wchar_t cmdLine[] = L"build.xml\0";
		result = 
			CreateProcess(L"apbuild.exe", cmdLine, 0,0,TRUE, CREATE_NO_WINDOW, 0, workingDir, &startUpInfo, &procInfo);
		assert(result);

		LastResult.clear();

		do
		{
			wchar_t buffer[128];
			DWORD actul;
			ReadFile(writePipe, buffer,128, &actul,0);

			if (actul)
			{
				LastResult.append(buffer);
			}
		}
		while (WaitForSingleObject(procInfo.hProcess, 10) == WAIT_TIMEOUT);
		//GetFileSize(writePipe, );


		//OpenFile()
		//ReadFile(writePipe)

		CloseHandle(writePipe);

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
	}
	void BuildInterface::BuildAll(Project* project)
	{
		//CreatePipe()
	}
}