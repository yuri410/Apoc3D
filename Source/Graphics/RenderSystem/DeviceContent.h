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
#ifndef DEVICECONTENT_H
#define DEVICECONTENT_H


#include "Common.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"

using namespace Apoc3D::Graphics;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			enum PresentInterval
			{
				/** The device will present immediately without waiting for the refresh.
				*/
				Immediate = 1<<32,
				/** The device will wait for the vertical retrace period.
				*/
				Default = 0, 
				/** The device will wait for the vertical retrace period.
				*/
				One = 1,
				/**  Present operations will not be affected more than twice every screen refresh.
				*/
				Two = 2,
				/** Present operations will not be affected more than three times every screen refresh
				*/
				Three = 4,
				/** Present operations will not be affected more than four times every screen refresh.
				*/
				Four = 8,
			};			

			struct APAPI PresentParameters
			{
				bool IsWindowd;
				bool IsFullForm;

				int BackBufferWidth;
				int BackBufferHeight;

				DepthFormat DepthBufferFormat;
				PixelFormat ColorBufferFormat;

				uint32 FSAASampleCount;
				PresentInterval PresentInterval;


				uint64 TargetHandle;

				void* UserData;

			};

			/* Represent a graphics device of a graphics API. It can create one or more RenderViews.
			*/
			class APAPI DeviceContent
			{
			private:
				vector<RenderView*> m_renderPorts;
				bool m_supportsRenderControl;
			public:
				virtual bool SupportsRenderControl() { return m_supportsRenderControl; }

				RenderView* Create(const PresentParameters &pm);
				void Destroy(RenderView* rc);

				virtual RenderDevice* getRenderDevice() = 0;
			protected:
				virtual RenderView* create(const PresentParameters &pm) = 0;

				DeviceContent(bool supportsRenderCtrl)
					: m_supportsRenderControl(supportsRenderCtrl)
				{
				}
			};


		}
	}
}

#endif