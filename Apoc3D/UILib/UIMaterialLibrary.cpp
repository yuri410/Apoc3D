
#include "UIMaterialLibrary.h"

SINGLETON_DECL(Apoc3D::UI::UIMaterialLibrary);

namespace Apoc3D
{
	namespace UI
	{
		UIMaterialLibrary::UIMaterialLibrary() { }
		UIMaterialLibrary::~UIMaterialLibrary() { }

		Material* UIMaterialLibrary::getMaterial(const String& name) const
		{
			return 0;
		}

	}
}