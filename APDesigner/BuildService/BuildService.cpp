#include "BuildService.h"

#include "Project/Project.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfigurationFormat.h"
#include "Utility/StringUtils.h"
#include "IOLib/Streams.h"
#include "Core/Logging.h"
#include <Windows.h>

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;

namespace APDesigner
{
	std::vector<String> BuildInterface::LastResult;

	void BuildInterface::BuildSingleItem(ProjectItem* item)
	{
		ConfigurationSection* sect = item->Save(true);

		Configuration* xml = new Configuration(L"Build");
		xml->Add(sect);

		
		XMLConfigurationFormat::Instance.Save(xml, new FileOutStream(L"build.xml"));
		//xml->Save(L"build.xml");

		ExecuteBuildOperation();
	}
	void BuildInterface::BuildAll(Project* project)
	{
		//project->Save(L"build.xml", true);
		FastList<ConfigurationSection*> scripts;
		project->GenerateBuildScripts(scripts);

		std::vector<String> allresult;
		for (int i=0;i<scripts.getCount();i++)
		{
			Configuration* xc = new Configuration(L"Root");
			xc->Add(scripts[i]);
			XMLConfigurationFormat::Instance.Save(xc, new FileOutStream(L"build.xml"));
			//xc->Save(L"build.xml");
			delete xc;
			if (ExecuteBuildOperation())
			{
				for (size_t i=0;i<LastResult.size();i++)
					allresult.push_back(LastResult[i]);
				LogManager::getSingleton().Write(LOG_System, L"Build failed.", LOGLVL_Error);
				break;
			}
			for (size_t i=0;i<LastResult.size();i++)
				allresult.push_back(LastResult[i]);
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

		WaitForSingleObject(procInfo.hProcess, INFINITE);
		
		String buildOutput;
		{
			char buffer[1024];

			int readSize = sizeof(buffer) - 1;
			DWORD actul;
			do 
			{

				memset(buffer, 0, sizeof(buffer));


				ReadFile(writePipe, buffer, readSize, &actul,0);

				if (actul)
				{
					buildOutput.append(StringUtils::toWString(buffer));
					//LastResult.append(StringUtils::toWString(buffer));
				}
			} while (actul == readSize);
		}

		LastResult = StringUtils::Split(buildOutput, L"\n\r");

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