
#include "Apoc3DException.h"

namespace Apoc3D
{
	Apoc3DException Apoc3DException::createException(Apoc3DExceptionType type, const wchar_t* const msg)
	{
		String perfix;
		switch (type)
		{
		case  EX_Default:
			break;
		case EX_InvalidData:
			break;
		case EX_InvalidOperation:
			break;
		}
		perfix.append(msg);

		return Apoc3DException(perfix.c_str());	
	}
}