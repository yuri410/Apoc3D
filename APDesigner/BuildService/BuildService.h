#pragma once
#ifndef BUILDSERVICE_H
#define BUILDSERVICE_H

#include "APDCommon.h"

namespace APDesigner
{
	class BuildInterface
	{
		SINGLETON_DECL(BuildInterface);
	public:
		struct BuildResult
		{
			String Message;
			bool HasWarning;
			bool HasError;
		};

		BuildInterface();
		~BuildInterface();

		void AddSingleBuildItem(ProjectItem* item);
		void AddBuild(Project* project);
		void Execute();

		bool MainThreadUpdate(const GameTime* time, BuildResult* result);

		bool PopLastResult(String& res);

		void BlockedWait();

		bool IsRunning();

	private:
		int ExecuteBuildOperation();
		void ReadPipe(void* stdoutRead, List<char>& readBuffer, bool finalRead);
		void ReadBuildOutputs(List<char>& readBuffer);

		void Thread_Main();
		static void ThreadEntry(void* arg);
		
		tthread::thread* m_processThread;

		tthread::mutex m_resultLock;
		Queue<String> m_lastResult;
		bool m_hasLastError;

		tthread::mutex m_taskLock;
		Queue<Configuration*> m_taskList;

		tthread::mutex m_flagLock;
		volatile bool m_finished;
		
		tthread::mutex m_processLock;


		String m_buildWarningMessages;
		String m_buildErrorMessages;
		bool m_isBuildPending;
	};


}

#endif