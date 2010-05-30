
#include "Material.h"

namespace Apoc3D
{
	Material::Material()
	{	
		std::memset(m_tex, 0, sizeof(m_tex));

		std::memset(&m_mtrlColor, 0,  sizeof(m_mtrlColor));	
	}


	Material::~Material(void)
	{

	}
};