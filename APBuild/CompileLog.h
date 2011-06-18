#ifndef COMPILELOG_H
#define COMPILELOG_H

#include "APBCommon.h"
#include "Core/Singleton.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;

using namespace std;

namespace APBuild
{
	enum CompileLogType
	{
		COMPILE_Warning,
		COMPILE_Information,
		COMPILE_Error
	};
	struct CompileLogEntry
	{
		CompileLogType Type;
		String Location;
		String Description;
	};
	class CompileLog : public Singleton<CompileLog>
	{
	private:
		vector<CompileLogEntry> m_logs;


	public:
		CompileLog()
		{

		}
		~CompileLog()
		{

		}

		void Write(CompileLogType type, const String& message, const String& location)
		{
			CompileLogEntry ent = {type, location, message};
			m_logs.push_back(ent);
		}
		void WriteInformation(const String& message, const String& location)
		{
			Write(COMPILE_Information, message, location);
		}
		void WriteError(const String& message, const String& location)
		{
			Write(COMPILE_Error, message, location);
		}
		void WriteWarning(const String& message, const String& location)
		{
			Write(COMPILE_Warning, message, location);
		}

		SINGLETON_DECL_HEARDER(CompileLog);
	};
}

#endif