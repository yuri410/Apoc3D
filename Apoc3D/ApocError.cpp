#include "ApocCommon.h"
#include "Utility/TypeConverter.h"
#include "Core/Logging.h"

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <Windows.h>
#endif

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	const TypeDualConverter<ErrorID> ErrorIDConverter =
	{
		{ L"DefaultError", ErrorID::Default },
		{ L"InvalidData", ErrorID::InvalidData },
		{ L"InvalidOperation", ErrorID::InvalidOperation },
		{ L"NotSupported", ErrorID::NotSupported },
		{ L"KeyNotFound", ErrorID::KeyNotFound },
		{ L"FormatException", ErrorID::FormatException },
		{ L"EndOfStream", ErrorID::EndOfStream },
		{ L"FileNotFound", ErrorID::FileNotFound },
		{ L"Argument", ErrorID::Argument },
		{ L"Duplicate", ErrorID::Duplicate },
		{ L"ScriptCompileError", ErrorID::ScriptCompileError },
	};

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS

	struct EnumWindowContext
	{
		HWND m_result1 = NULL;
		HWND m_result2 = NULL;
	};

	static BOOL CALLBACK EnumWindowsProc(_In_  HWND hwnd, _In_  LPARAM lParam)
	{
		WINDOWINFO info;
		info.cbSize = sizeof(WINDOWINFO);
		if (GetWindowInfo(hwnd, &info))
		{
			EnumWindowContext* ctx = (EnumWindowContext*)lParam;

			if (info.dwWindowStatus == WS_ACTIVECAPTION)
			{
				ctx->m_result1 = hwnd;
			}

			if (ctx->m_result2 == NULL)
			{
				ctx->m_result2 = hwnd;
			}
		}
		return TRUE;
	}

	static HWND SearchOwnerWindow()
	{
		EnumWindowContext ctx;
		EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProc, (LPARAM)&ctx);
		
		if (ctx.m_result1)
			return ctx.m_result1;
		return ctx.m_result2;
	}

	static String MakeDialogText(const String& text, const String& location, bool isAssert)
	{
		wchar_t modulePath[1024] = { 0 };
		GetModuleFileName(0, modulePath, 1023);

		String result;

		if (isAssert)
			result = L"Assertion Failed!";
		else
			result = L"Error!";

		result += L"\n\nProgram: ";
		result += modulePath;
		result += L"\n\n";
		
		if (isAssert)
			result += L"Expression: ";
		result += text;
		result += L"\n\n";
		result += location;

		return result;
	}

	#define BlockingMessage(text, location, isAssert) \
	{ \
		HWND hwnd = SearchOwnerWindow(); \
		int result = MessageBox(hwnd, MakeDialogText(text, location, isAssert).c_str(), L"Apoc3D Framework", MB_ABORTRETRYIGNORE | MB_ICONERROR); \
		if (result == IDRETRY) \
			__debugbreak(); \
		else if (result == IDABORT) \
			std::terminate(); \
	}

#else
	#define BlockingMessage(text, location, isAssert) 
#endif

	static String MakeLocationText(const wchar_t * file, unsigned line)
	{
		String text;
		text.append(file);
		text.append(L"(");
		text.append(StringUtils::UIntToString(line));
		text.append(L")");
		return text;
	}

	void Assert(const wchar_t * msg, const wchar_t * file, unsigned line) 
	{
		String location = MakeLocationText(file, line);

		if (LogManager::isInitialized())
		{
			String consoleMsg = String(L"Assert: ") + msg + L"\n" + location;
			LogManager::getSingleton().Write(LOG_System, consoleMsg, LOGLVL_Error);
		}

		BlockingMessage(msg, location, true);
	}

	void Error(ErrorID eid, const String& msg, const wchar_t * file, unsigned line)
	{
		String message = ErrorIDConverter.ToString(eid) + L" - " + msg;
		String location = MakeLocationText(file, line);

		if (LogManager::isInitialized())
		{
			String consoleMsg = message + L"\n" + location;
			LogManager::getSingleton().Write(LOG_System, message + L"\n" + location, LOGLVL_Error);
		}

		BlockingMessage(message, location, false);
	}
}