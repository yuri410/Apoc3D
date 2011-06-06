
#include "Apoc3DException.h"
#include "Core/Logging.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	Apoc3DException Apoc3DException::createException(Apoc3DExceptionType type, const String& msg)
	{
		String perfix;
		switch (type)
		{
		case EX_Default:
			break;
		case EX_InvalidData:
			break;
		case EX_InvalidOperation:
			break;
		}
		perfix.append(msg);

		LogManager::getSingleton().Write(LOG_System, perfix, LOGLVL_Fatal);

		return Apoc3DException(perfix.c_str());	
	}
}