#include "BuildService.h"

#include "Project/Project.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Utility/StringUtils.h"

#include <Windows.h>

using namespace Apoc3D;
using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;

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
		GetCurrentDirectory(260, workingDir);

		PROCESS_INFORMATION procInfo;

		wchar_t cmdLine[] = L"apbuild.exe build.xml\0";
		result = 
			CreateProcess(0, cmdLine, 0,0,TRUE, CREATE_NO_WINDOW, 0, workingDir, &startUpInfo, &procInfo);
		if (!result)
		{
			DWORD ecode = GetLastError();
			assert(result);
		}
		

		LastResult.clear();

		do
		{
			char buffer[128];
			memset(buffer,0,sizeof(buffer));
			DWORD actul;
			ReadFile(writePipe, buffer,128, &actul,0);

			if (actul)
			{
				LastResult.append(StringUtils::toWString(buffer));
			}
		}
		while (WaitForSingleObject(procInfo.hProcess, 10) == WAIT_TIMEOUT);
		//GetFileSize(writePipe, );


		//OpenFile()
		//ReadFile(writePipe)


		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(startUpInfo.hStdError);
		CloseHandle(startUpInfo.hStdInput);
		CloseHandle(startUpInfo.hStdOutput);
	}
	void BuildInterface::BuildAll(Project* project)
	{
		//CreatePipe()
	}
}