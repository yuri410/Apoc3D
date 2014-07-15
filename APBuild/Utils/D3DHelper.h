#ifndef D3D9HELPER_H
#define D3D9HELPER_H

#include "APBCommon.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace APBuild
{
	namespace Utils
	{
		/** Provide a nullref device for using D3DX functions
		*/
		class D3DHelper
		{
		public:
			static IDirect3DDevice9* getDevice() { return m_device; }
			static void Initalize();
			static void Finalize();

		private:
			//static IDirect3D9* m_d3d;
			static IDirect3DDevice9* m_device;
		};
	}

}

#endif