#include "BuildService.h"

#include "APDesigner/CommonDialog/DialogCommon.h"

#include <Windows.h>

namespace APDesigner
{
	SINGLETON_IMPL(BuildInterface);

	BuildInterface::BuildInterface()
		: m_hasLastError(false), m_finished(true), m_isBuildPending(false)
	{

	}

	BuildInterface::~BuildInterface()
	{

	}

	void BuildInterface::AddSingleBuildItem(ProjectItem* item)
	{
		ConfigurationSection* sect = item->Save(true);

		Configuration* xml = new Configuration(L"Build");
		xml->Add(sect);

		m_taskLock.lock();
		m_taskList.Enqueue(xml);
		m_taskLock.unlock();
	}

	void BuildInterface::AddBuild(Project* project)
	{
		//project->Save(L"build.xml", true);
		List<ProjectBuildScript> scripts;
		project->GenerateBuildScripts(scripts);

		for (const ProjectBuildScript& pbs : scripts)
		{
			Configuration* xc = new Configuration(L"Root");
			xc->Add(pbs.ItemTree);

			// attachment section
			{
				ConfigurationSection* attachmentSect = new ConfigurationSection(ProjectUtils::BuildAttachmentSectionGUID);
				attachmentSect->AddStringValue(L"BaseOutputPath", pbs.BaseOutputDir);
				xc->Add(attachmentSect);
			}

			m_taskLock.lock();
			m_taskList.Enqueue(xc);
			m_taskLock.unlock();
		}
	}

	bool BuildInterface::PopLastResult(String& res)
	{
		volatile bool result = false;
		m_resultLock.lock();

		result = m_lastResult.getCount()>0;

		if (result)
		{
			res = m_lastResult.Dequeue();
		}

		m_resultLock.unlock();

		return result;
	}

	void BuildInterface::Execute()
	{
		assert(!m_isBuildPending);

		m_finished = false;
		m_hasLastError = false;
		m_isBuildPending = true;

		m_processLock.lock();
		m_processThread = new tthread::thread(&BuildInterface::ThreadEntry, this);
		Apoc3D::Platform::SetThreadName(m_processThread, L"Apoc3D Designer Build Service");

		LogManager::getSingleton().Write(LOG_System, L"Build Started", LOGLVL_Default);

		m_processLock.unlock();
	}


	bool BuildInterface::MainThreadUpdate(const GameTime* time, BuildResult* result)
	{
		bool justEnded = false;

		String msg;
		while (BuildInterface::getSingleton().PopLastResult(msg))
		{
			bool isError = msg.find(L"[Error]") != String::npos;
			bool isWarning = msg.find(L"[Warning]") != String::npos;

			LogMessageLevel level = LOGLVL_Infomation;
			if (isWarning)
			{
				level = LOGLVL_Warning;
				m_buildWarningMessages.append(msg);
				m_buildWarningMessages.append(L"\n");
			}
			if (isError)
			{
				level = LOGLVL_Error;
				m_buildErrorMessages.append(msg);
				m_buildErrorMessages.append(L"\n");
			}
			LogManager::getSingleton().Write(LOG_System, msg, level);

		}


		if (m_isBuildPending)
		{
			if (!IsRunning())
			{
				m_isBuildPending = false;

				if (m_processThread)
				{
					delete m_processThread;
					m_processThread = nullptr;
				}
				
				LogManager::getSingleton().Write(LOG_System, L"Build Ended", LOGLVL_Default);
				justEnded = true;

				if (result)
				{
					result->HasError = false;
					result->HasWarning = false;
				}

				if (m_hasLastError || m_buildErrorMessages.size() || m_buildWarningMessages.size())
				{
					LogManager::getSingleton().Write(LOG_System, L"Build has some issues.", LOGLVL_Error);

					if (m_buildErrorMessages.size() || m_buildWarningMessages.size())
					{
						String msg = L"There are some potential build issues.";
						msg.append(L"\n");
						msg.append(L"\n");
						msg.append(m_buildErrorMessages);
						msg.append(L"\n");
						msg.append(m_buildWarningMessages);

						if (result)
						{
							result->Message = msg;
							result->HasError = !!m_buildErrorMessages.size();
							result->HasWarning = !!m_buildWarningMessages.size();
						}
						else
						{
							APDesigner::CommonDialog::ShowMessageBox(msg, L"Build Result", !m_buildErrorMessages.empty(), !m_buildWarningMessages.empty(), false);
						}
					}
				}
				m_buildErrorMessages.clear();
				m_buildWarningMessages.clear();
			}
		}
		return justEnded;
	}


	void BuildInterface::BlockedWait()
	{
		while (m_processThread)
		{
			Apoc3D::Platform::ApocSleep(1);

			MainThreadUpdate(nullptr, nullptr);
		}
	}

	bool BuildInterface::IsRunning()
	{
		volatile bool result;
		m_flagLock.lock();
		result = !m_finished;
		m_flagLock.unlock();

		return result;
	}

	int BuildInterface::ExecuteBuildOperation()
	{
		SECURITY_ATTRIBUTES pipeAttr;
		ZeroMemory(&pipeAttr, sizeof(SECURITY_ATTRIBUTES));
		pipeAttr.bInheritHandle = TRUE;

		
		HANDLE stdoutRead;
		HANDLE stdoutWrite;
		BOOL result = CreatePipe(&stdoutRead, &stdoutWrite, &pipeAttr, 0);
		SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);

		
		STARTUPINFO startUpInfo;
		ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
		startUpInfo.cb = sizeof(STARTUPINFO);
		startUpInfo.dwFlags |= STARTF_USESTDHANDLES;
		startUpInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startUpInfo.hStdError = stdoutWrite;
		startUpInfo.hStdOutput = stdoutWrite;
		
		
		PROCESS_INFORMATION procInfo;

		wchar_t cmdLine[] = L"apbuild.exe build.xml\0";
		result = CreateProcess(0, cmdLine, 0,0,TRUE, CREATE_NO_WINDOW, 0, 0, &startUpInfo, &procInfo);
		if (!result)
		{
			DWORD ecode = GetLastError();
			assert(result);
		}


		
		CloseHandle(stdoutWrite);
		
		List<char> readBuffer;
		while (WaitForSingleObject(procInfo.hProcess, 20) == WAIT_TIMEOUT)
		{
			ReadPipe(stdoutRead, readBuffer, false);
		}

		ReadPipe(stdoutRead, readBuffer, true);
		
		DWORD procCode = 0;
		if (!GetExitCodeProcess(procInfo.hProcess, &procCode))
			procCode = 1;

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);

		return procCode;
	}

	void BuildInterface::ThreadEntry(void* arg)
	{
		BuildInterface* obj = static_cast<BuildInterface*>(arg);

		obj->Thread_Main();
	}
	void BuildInterface::Thread_Main()
	{
		Configuration* buildScript;
		m_processLock.lock();

		for (;;) 
		{
			buildScript = nullptr;

			m_taskLock.lock();
			if (m_taskList.getCount())
			{
				buildScript = m_taskList.Dequeue();
			}
			m_taskLock.unlock();

			if (buildScript)
			{
				XMLConfigurationFormat::Instance.Save(buildScript, new FileOutStream(L"build.xml"));
				if (ExecuteBuildOperation())
				{
					m_hasLastError = true;
					break;
				}
				delete buildScript;
			}
			else break;
		}

		m_taskLock.lock();
		while (m_taskList.getCount()>0)
		{
			delete m_taskList.Dequeue();
		}
		m_taskLock.unlock();

		m_flagLock.lock();
		m_finished = true;
		m_flagLock.unlock();

		m_processLock.unlock();
	}

	

	void BuildInterface::ReadPipe(void* stdoutRead, List<char>& readBuffer, bool finalRead)
	{
		char buffer[1024];

		DWORD readSize = sizeof(buffer) - 1;

		for (;;)
		{
			memset(buffer, 0, sizeof(buffer));

			DWORD actul;
			BOOL bSuccess = ReadFile(stdoutRead, buffer, readSize, &actul, NULL);

			if( ! bSuccess || actul == 0 ) break; 

			if (actul)
			{
				readBuffer.AddArray(buffer, actul);
				//buildOutput.append(StringUtils::toPlatformWideString(buffer));
			}

			if (actul < readSize)
			{
				break;
				//Apoc3D::Platform::ApocSleep(1);
			}

			ReadBuildOutputs(readBuffer);
		}
		
		ReadBuildOutputs(readBuffer);

		if (finalRead && readBuffer.getCount()>0)
		{
			std::string line;
			for (char c : readBuffer)
			{
				if (c != '\r' && c != '\n')
					line.append(1, c);
			}

			m_resultLock.lock();
			m_lastResult.Enqueue(StringUtils::toPlatformWideString(line));
			m_resultLock.unlock();
		}
	}

	void BuildInterface::ReadBuildOutputs(List<char>& readBuffer)
	{
		bool done = false;
		do
		{
			int32 nextRet = readBuffer.IndexOf('\n');

			if (nextRet != -1)
			{
				std::string line;
				for (int32 i = 0; i <= nextRet; i++)
				{
					char c = readBuffer[i];
					if (c != '\r' && c != '\n')
						line.append(1, c);
				}

				m_resultLock.lock();
				m_lastResult.Enqueue(StringUtils::toPlatformWideString(line));
				m_resultLock.unlock();

				readBuffer.RemoveRange(0, nextRet + 1);
			}
			else
			{
				break;
			}
		} while (!done);
	}
}