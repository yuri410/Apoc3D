#include "..\Common.h"

namespace Apoc3D
{
	namespace EffectSystem
	{
		/* Implements a atom effect code parser. 
		   It parses the code and generate useful information
		   as the following described:

		   Code: main computational code
		   Input: input 
		   Output: output 
		   Parameter: EffectParameter

		   EffectAtomManager uses this to create new EffectAtom from
		   source code.
		*/
		class _Export EffectAtomCodeParser
		{
		public:
			EffectAtomCodeParser(void);
			~EffectAtomCodeParser(void);
		};
	};
};