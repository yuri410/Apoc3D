
#include "ApocException.h"
#include "Core/Logging.h"
#include "Collections/EnumConverterHelper.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	class Apoc3DExceptionTypeConverter : public EnumDualConversionHelper<ApocExceptionType>
	{
	public:
		Apoc3DExceptionTypeConverter()
			: EnumDualConversionHelper(25)
		{
			AddPair(L"Default", ApocExceptionType::Default);
			AddPair(L"InvalidData", ApocExceptionType::InvalidData);
			AddPair(L"InvalidOperation", ApocExceptionType::InvalidOperation);
			AddPair(L"NotSupported", ApocExceptionType::NotSupported);
			AddPair(L"KeyNotFound", ApocExceptionType::KeyNotFound);
			AddPair(L"FormatException", ApocExceptionType::FormatException);
			AddPair(L"EndOfStream", ApocExceptionType::EndOfStream);
			AddPair(L"FileNotFound", ApocExceptionType::FileNotFound);
			AddPair(L"Argument", ApocExceptionType::Argument);
			AddPair(L"Duplicate", ApocExceptionType::Duplicate);
			AddPair(L"ScriptCompileError", ApocExceptionType::ScriptCompileError);
		}
	} exceptTypeConverter;

	ApocException ApocException::CreateException(ApocExceptionType type, const String& msg, const wchar_t* file, int line)
	{
		String text = exceptTypeConverter.ToString(type);

		text.append(msg);
		
		text.append(L" @ Line ");

		text.append(StringUtils::IntToString(line));
		text.append(L", ");
		text.append(file);

		LogManager::getSingleton().Write(LOG_System, text, LOGLVL_Fatal);

		return ApocException(msg, type);
	}
	ApocException ApocException::CreateException(ApocExceptionType type, const wchar_t* file, int line)
	{
		return CreateException(type, L"Location: ", file, line);
	}
}