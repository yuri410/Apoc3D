#include "..\Common.h"

namespace Apoc3D
{
	namespace EffectSystem
	{
		/* Represents a small effect that cannot be divided into sub effects.
		   This is usually a piece of shader fragement.
		   eg. position transform, normal mapping
		*/
		class _Export EffectAtom
		{
		private:

		public:
			EffectAtom(void);
			~EffectAtom(void);

		};
	};
};