#ifndef D3D9HELPER
#define D3D9HELPER

#include "APBCommon.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace APBuild
{
	class D3DHelper
	{
	private:
		//static IDirect3D9* m_d3d;
		static IDirect3DDevice9* m_device = 0;

	public:
		static void Initalize();
		static void Finalize();

		
	};
}

#endif