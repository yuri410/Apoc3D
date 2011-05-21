#include "sse_functions.h"

#if APOC3D_MATH_IMPL == APOC3D_SSE
class FieldInitializer
{
public:
	FieldInitializer()
	{
		uint Data2[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
		memcpy((void*)&_MASKSIGN_, Data2, sizeof(Data2));

	}

} Initializer;

#endif