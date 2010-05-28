#include "Common.h"
#include "Material.h"


Material::Material()
{	
	memset(m_tex, 0, sizeof(m_tex));

	
	ZeroMemory(&m_mtrlColor, sizeof(m_mtrlColor));
}


Material::~Material(void)
{

}
