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
#ifndef APOC3D_SCENE_PROCEDURE_H
#define APOC3D_SCENE_PROCEDURE_H

#include "Common.h"
#include "Collections/FastList.h"
#include "ScenePassTypes.h"

#include "Math/Vector.h"
//#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

using namespace std;

namespace Apoc3D
{
	namespace Scene
	{
		/* 
			pass config <Pass></Pass>
				attributes:
				Name
				SelectorID  -- use -1 to select all objects
				CameraID  --  index of camera registered in SceneRenderer object, try not to change camera too often


			Commands (aka. function call)
				<C S = "func" params/>

		*/

		/** Represent a sequence of scene passes that can finally 
		 *  generate the end result.
		 *
		 *  A SceneProcedure can be either normal passes (like shadow
		 *  mapping) or post effect passes (like bloom & HDR).
		 *
		 *  This class also manages resources and vars used in rendering.
		 * 
		 * @remarks
		 *   For the structure or the use of the script, please refer
		 *   to the sample file.
		 *   
		 *   Commands(aka. function call):
		 *    Clear
		 *    UseRT
		 *    Render
		 *    RenderQuad
		 *    VisibleTo
		 *    State
		 *
		 *   Variable Types
		 *    GaussBlurFilter - see VARTYPE_GaussBlurFilter
		 *    Texture - see VARTYPE_Texture
		 *    RenderTarget - see VARTYPE_RenderTarget
		 *    Matrix - see VARTYPE_Matrix
		 *
		 *   Immediate Value Types
		 *
		 *
		 */
		class APAPI SceneProcedure
		{
		public:
			/** Find a variable define in the procedure script by name; 
			 *  then returns as a RenderTarget.
			 */
			RenderTarget* FindRenderTargetVar(const String& name) const
			{
				if (!m_isAvailable)
					return 0;
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name==name)
					{
						return m_vars[i]->RTValue;
					}
				}
				return 0;
			}
			void SetTextureVar(const String& name, ResourceHandle<Texture>* tex)
			{
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name == name)
					{
						assert(m_vars[i]->Type == VARTYPE_Texture);
						m_vars[i]->TextureValue = tex;
						break;
					}
				}
			}
			void SetBooleanVar(const String& name, bool val)
			{
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name == name)
					{
						assert(m_vars[i]->Type == VARTYPE_Boolean);
						m_vars[i]->Value[0] = val ? 1:0;
						break;
					}
				}
			}
			void SetVector4Var(const String& name, const Vector4& val)
			{
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name == name)
					{
						assert(m_vars[i]->Type == VARTYPE_Vector4);
						memcpy(m_vars[i]->Value, &val, sizeof(float)*4);
						break;
					}
				}
			}
			void SetVector2Var(const String& name, const Vector2& val)
			{
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name == name)
					{
						assert(m_vars[i]->Type == VARTYPE_Vector2);
						memcpy(m_vars[i]->Value, &val, sizeof(float)*2);
						break;
					}
				}
			}
			void SetFloatVar(const String& name, const float val)
			{
				for (int i=0;i<m_varCount;i++)
				{
					if (m_vars[i]->Name == name)
					{
						assert(m_vars[i]->Type == VARTYPE_Single);
						*reinterpret_cast<float*>(m_vars[i]->Value) = val;
						break;
					}
				}
			}

			SceneProcedure(RenderDevice* device);
			~SceneProcedure(void);

			/** Check if the SceneProcedure is available. A SceneProcedure is unavailable if the techniques
			 *  used is not supported. (eg. A type of render target format is not supported)
			 */
			bool IsAvailable();

			void Load(SceneRenderer* renderer, const ResourceLocation* rl);

			/** Execute the procedure, respectively invoking a series of Scene Passes.
			*/
			void Invoke(const FastList<Camera*> cameras, SceneManager* sceMgr, BatchData* batchData);
			
			/** Gets the current camera in the rendering process.
			 *  If this method is call when not rendering, eg. updating, the last camera use will be returned.
			 */
			const Camera* getLastCamera() const { return m_lastCamera; }

		private:
			RenderDevice* m_renderDevice;

			FastList<ScenePass*> m_passes;
			SceneVariable** m_vars;
			int m_varCount;

			FastList<RenderTarget*> m_createdRenderTarget;
			FastList<ResourceHandle<Texture>*> m_createdTextures;
			FastList<GaussBlurFilter*> m_createdGaussFilters;

			bool m_isAvailable;
			String m_name;

			const Camera* m_lastCamera;
		};
	};
};
#endif