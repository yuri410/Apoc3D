/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#ifndef D3D9DEVICESETTINGS_H
#define D3D9DEVICESETTINGS_H

#include "D3D9Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			struct Direct3D9Settings
			{
				uint32 AdapterOrdinal;
				D3DDEVTYPE DeviceType;
				D3DFORMAT AdapterFormat;
				uint32 CreationFlags;
				D3DPRESENT_PARAMETERS PresentParameters;

				Direct3D9Settings()
					: AdapterFormat(D3DFMT_UNKNOWN), DeviceType(D3DDEVTYPE_HAL), AdapterOrdinal(0), CreationFlags(0)
				{
					memset(&PresentParameters, 0,  sizeof(PresentParameters));
				}
			};

			struct DeviceSettings
			{
				/** The adapter ordinal.
				*/
				uint32 AdapterOrdinal;

				/** The type of the device.
				*/
				D3DDEVTYPE DeviceType;

				/** The refresh rate.
				*/
				int32 RefreshRate;
				/** The width of the back buffer.
				*/
				int32 BackBufferWidth;
				/** The height of the back buffer.
				*/
				int32 BackBufferHeight;
				/** The back buffer format
				*/
				D3DFORMAT BackBufferFormat;

				/** The back buffer count.
				*/
				int32 BackBufferCount;
				
				/** The depth stencil format.
				*/
				D3DFORMAT DepthStencilFormat;

				/** true if windowed; otherwise, false.
				*/
				bool Windowed;

				/** true if VSync is enabled; otherwise, false.
				*/
				bool EnableVSync;

				/** true if VSync is multithreaded; otherwise, false.
				*/
				bool Multithreaded;

				/** The multisample type.
				*/
				D3DMULTISAMPLE_TYPE MultiSampleType;

				/** The multisample quality.
				*/
				int MultiSampleQuality;


				Direct3D9Settings D3D9;

				DeviceSettings()
					: AdapterOrdinal(0), DeviceType(D3DDEVTYPE_HAL), 
					RefreshRate(0), BackBufferWidth(0), BackBufferHeight(0),
					BackBufferFormat(D3DFMT_UNKNOWN), BackBufferCount(0),
					Windowed(false), EnableVSync(false), Multithreaded(false),
					MultiSampleType(D3DMULTISAMPLE_NONE), MultiSampleQuality(0),
					DepthStencilFormat(D3DFMT_UNKNOWN)
				{

				}
			};
		}
	};
};
#endif