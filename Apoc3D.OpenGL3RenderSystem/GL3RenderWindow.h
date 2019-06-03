#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
 *
 * This content of this file is subject to the terms of the Mozilla Public
 * License v2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This program is distributed in the hope that it will be useful,
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the
 * Mozilla Public License for more details.
 *
 * ------------------------------------------------------------------------
 */

#ifndef GL3RENDERWINDOW_H
#define GL3RENDERWINDOW_H

#include "GL3Common.h"
#include "GL3RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "apoc3d.Win32/Win32RenderWindow.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Win32;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3RenderView : public RenderView
			{
			public:
				GL3RenderView(GL3RenderDevice* device, const RenderParameters& pm);
				~GL3RenderView();

				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present() override;

			private:
				GL3RenderDevice* m_device;

				HANDLE m_controlHandle;
			};


			class GL3RenderWindow final : public Win32RenderWindow
			{
			public:

				GL3RenderWindow(GL3RenderDevice* device, GL3DeviceContext* dc, const RenderParameters& pm);
				~GL3RenderWindow();

				virtual void Present() override;

				virtual void Run() override;

				void GL_Initialize();
				void GL_Finalize();

				void GL_LoadContent();
				void GL_UnloadContent();

				const String& GetHardwareName() const;

			private:

				void GL_Create(const RenderParameters& params);

				void GL_Release();

				void OnRenderFrame(const AppTime* time) override;

				bool IsDeviceReady() override;
				void ExecuteChangeDevice() override;

				GL3DeviceContext* m_dc;

				GraphicsDeviceManager* m_graphicsDeviceManager;

			};
		}
	}
}

#endif