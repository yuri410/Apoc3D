#ifndef BUILDSERVICE_H
#define BUILDSERVICE_H

#include "APDCommon.h"
#include "apoc3d/Core/Singleton.h"
#include "apoc3d/Collections/FastList.h"
#include "apoc3d/Collections/FastQueue.h"

#include "tthread/fast_mutex.h"
#include "tthread/tinythread.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Config;
using namespace Apoc3D::Collections;

namespace APDesigner
{
	class BuildInterface : public Singleton<BuildInterface>
	{
	public:
		SINGLETON_DECL_HEARDER(BuildInterface);

		BuildInterface();

		void AddSingleBuildItem(ProjectItem* item);
		void AddBuild(Project* project);
		void Execute();

		void MainThreadUpdate(const GameTime* const time);

		bool PopLastResult(String& res);

		void BlockedWait();

		bool IsRunning();

	private:
		int ExecuteBuildOperation();
		void ReadPipe(void* stdoutRead);

		void Thread_Main();
		static void ThreadEntry(void* arg);
		
		tthread::thread* m_processThread;

		tthread::fast_mutex m_resultLock;
		FastQueue<String> m_lastResult;
		bool m_hasLastError;

		tthread::fast_mutex m_taskLock;
		FastQueue<Configuration*> m_taskList;

		tthread::fast_mutex m_flagLock;
		bool m_finished;
		
		tthread::fast_mutex m_processLock;


		String m_buildWarningMessages;
		String m_buildErrorMessages;
		bool m_isBuildPending;
	};


}

#endif