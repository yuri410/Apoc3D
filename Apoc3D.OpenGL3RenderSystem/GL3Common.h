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

#ifndef GL3COMMON_H
#define GL3COMMON_H

#include "apoc3d/ApocCommon.h"

#include <Windows.h>

#include "GL/gl3w.h"

#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")
#pragma comment(lib, "Apoc3D.Win32.lib")
//#pragma comment(lib, "OpenGL32.lib")


/*

GL_ARB_vertex_array_object
GL_ARB_framebuffer_object
If not this, you get very similar functionality from these:
GL_EXT_framebuffer_object
GL_EXT_framebuffer_blit
GL_EXT_framebuffer_multisample
GL_EXT_packed_depth_stencil.
GL_ARB_map_buffer_range
GL_ARB_copy_buffer
GL_ARB_texture_rectangle
GL_ARB_color_buffer_float
GL_ARB_half_float_pixel
GL_ARB_sync


GL_ARB_texture_rg
GL_ARB_texture_compression_rgtc
GL_EXT_bindable_uniform (similar to Uniform Buffer Objects, but not as cleanly specified.)
GL_EXT_draw_buffers2
GL_EXT_geometry_shader4
GL_EXT_gpu_shader4
GL_EXT_framebuffer_sRGB
*/

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GraphicsDeviceManager;

			class GL3RenderView;
			class GL3RenderWindow;

			class GL3DeviceContext;
			
			class GL3ObjectFactory;
			class GL3RenderDevice;
			class GL3Capabilities;
			class GL3RenderStateManager;
			class GL3RenderTarget;

			class NativeGL3StateManager;

			class GL3Texture;
			class GL3Sprite;
			
		}
	}
}
#endif