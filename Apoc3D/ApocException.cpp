
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
			AddPair(L"Exception", EX_Default);
			AddPair(L"InvalidData", EX_InvalidData);
			AddPair(L"InvalidOperation", EX_InvalidOperation);
			AddPair(L"NotSupported", EX_NotSupported);
			AddPair(L"KeyNotFound", EX_KeyNotFound);
			AddPair(L"FormatException", EX_FormatException);
			AddPair(L"EndOfStream", EX_EndOfStream);
			AddPair(L"FileNotFound", EX_FileNotFound);
			AddPair(L"Argument", EX_Argument);
			AddPair(L"Duplicate", EX_Duplicate);
			AddPair(L"ScriptCompileError", EX_ScriptCompileError);

		}
	} exceptTypeConverter;

	ApocException ApocException::createException(ApocExceptionType type, const String& msg, const wchar_t* file, int line)
	{
		String text = exceptTypeConverter.ToString(type);

		text.append(msg);
		
		text.append(L" @ Line ");

		text.append(StringUtils::ToString(line));
		text.append(L", ");
		text.append(file);

		LogManager::getSingleton().Write(LOG_System, text, LOGLVL_Fatal);

		return ApocException(msg, type);	
	}
}