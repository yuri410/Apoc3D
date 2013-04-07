#ifndef COMPILELOG_H
#define COMPILELOG_H

#include "APBCommon.h"
#include "apoc3D/Collections/List.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;

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
		static List<CompileLogEntry> Logs;

		static void Write(CompileLogType type, const String& message, const String& location)
		{
			CompileLogEntry ent = {type, location, message};
			Logs.Add(ent);
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
			Logs.Clear();
		}

	};
}

#endif