
#include "Exception.h"
#include "Core/Logging.h"
#include "Collections/EnumConverterHelper.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	class Apoc3DExceptionTypeConverter : public EnumDualConversionHelper<ExceptID>
	{
	public:
		Apoc3DExceptionTypeConverter()
			: EnumDualConversionHelper(25)
		{
			AddPair(L"Default", ExceptID::Default);
			AddPair(L"InvalidData", ExceptID::InvalidData);
			AddPair(L"InvalidOperation", ExceptID::InvalidOperation);
			AddPair(L"NotSupported", ExceptID::NotSupported);
			AddPair(L"KeyNotFound", ExceptID::KeyNotFound);
			AddPair(L"FormatException", ExceptID::FormatException);
			AddPair(L"EndOfStream", ExceptID::EndOfStream);
			AddPair(L"FileNotFound", ExceptID::FileNotFound);
			AddPair(L"Argument", ExceptID::Argument);
			AddPair(L"Duplicate", ExceptID::Duplicate);
			AddPair(L"ScriptCompileError", ExceptID::ScriptCompileError);
		}
	} exceptTypeConverter;

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