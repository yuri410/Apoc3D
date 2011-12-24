#include "BuildService.h"

#include "Project/Project.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Utility/StringUtils.h"
#include "Core/Logging.h"
#include <Windows.h>

using namespace Apoc3D;
using namespace Apoc3D::Core;
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

		ExecuteBuildOperation();
	}
	void BuildInterface::BuildAll(Project* project)
	{
		//project->Save(L"build.xml", true);
		FastList<ConfigurationSection*> scripts;
		project->GenerateBuildScripts(scripts);

		String allresult;
		for (int i=0;i<scripts.getCount();i++)
		{
			XMLConfiguration* xc = new XMLConfiguration(L"Root");
			xc->Add(scripts[i]);
			xc->Save(L"build.xml");
			delete xc;
			if (ExecuteBuildOperation())
			{
				allresult.append(LastResult);
				LogManager::getSingleton().Write(LOG_System, L"Build failed.", LOGLVL_Error);
				break;
			}
			allresult.append(LastResult);
		}

		LastResult = allresult;
	}

	int BuildInterface::ExecuteBuildOperation()
	{
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

		while (WaitForSingleObject(procInfo.hProcess, 10) == WAIT_TIMEOUT)
		{
			char buffer[1024];
			//memset(buffer,0,sizeof(buffer));
			DWORD actul;
			ReadFile(writePipe, buffer, sizeof(buffer), &actul,0);

			if (actul<sizeof(buffer))
			{
				memset(buffer+actul,0, sizeof(buffer)-actul);
			}

			if (actul)
			{
				LastResult.append(StringUtils::toWString(buffer));
			}
		}

		//GetFileSize(writePipe, );

		DWORD code;
		if (!GetExitCodeProcess(procInfo.hProcess, &code))
		{
			code = 1;
		}
		//OpenFile()
		//ReadFile(writePipe)


		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(startUpInfo.hStdError);
		CloseHandle(startUpInfo.hStdInput);
		CloseHandle(startUpInfo.hStdOutput);
		return code;
	}
}