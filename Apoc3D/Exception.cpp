
#include "Exception.h"
#include "Core/Logging.h"
#include "Utility/StringUtils.h"
#include "Utility/TypeConverter.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	const TypeDualConverter<ExceptID> exceptTypeConverter = 
	{
		{ L"Default", ExceptID::Default },
		{ L"InvalidData", ExceptID::InvalidData },
		{ L"InvalidOperation", ExceptID::InvalidOperation },
		{ L"NotSupported", ExceptID::NotSupported },
		{ L"KeyNotFound", ExceptID::KeyNotFound },
		{ L"FormatException", ExceptID::FormatException },
		{ L"EndOfStream", ExceptID::EndOfStream },
		{ L"FileNotFound", ExceptID::FileNotFound },
		{ L"Argument", ExceptID::Argument },
		{ L"Duplicate", ExceptID::Duplicate },
		{ L"ScriptCompileError", ExceptID::ScriptCompileError },
	};


	Exception Exception::CreateException(ExceptID type, const String& msg, const wchar_t* file, int line)
	{
		String text = exceptTypeConverter.ToString(type);

		text.append(msg);
		
		text.append(L" @ Line ");

		text.append(StringUtils::IntToString(line));
		text.append(L", ");
		text.append(file);

		LogManager::getSingleton().Write(LOG_System, text, LOGLVL_Fatal);

		return Exception(msg, type);
	}
	Exception Exception::CreateException(ExceptID type, const wchar_t* file, int line)
	{
		return CreateException(type, L"Location: ", file, line);
	}
}