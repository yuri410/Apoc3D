#ifndef COMPILELOG_H
#define COMPILELOG_H

#include "APBCommon.h"

using namespace Apoc3D;
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
	class CompileLog
	{
	public:
		static vector<CompileLogEntry> Logs;

		static void Write(CompileLogType type, const String& message, const String& location)
		{
			CompileLogEntry ent = {type, location, message};
			Logs.push_back(ent);
		}
		static  void WriteInformation(const String& message, const String& location)
		{
			Write(COMPILE_Information, message, location);
		}
		static  void WriteError(const String& message, const String& location)
		{
			Write(COMPILE_Error, message, location);
		}
		static  void WriteWarning(const String& message, const String& location)
		{
			Write(COMPILE_Warning, message, location);
		}

		static void Clear()
		{
			Logs.clear();
		}

	};
}

#endif