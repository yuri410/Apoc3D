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

#ifndef APOC3D_EFFECT_H
#define APOC3D_EFFECT_H

#include "apoc3d/Common.h"
#include "EffectParameter.h"
#include "apoc3d/Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			/**
			 *  Defines the interface for graphics effect.
			 */
			class APAPI Effect
			{
			public:
				Effect(void);
				virtual ~Effect(void);


				virtual void Update(const GameTime* const time) { }

				/**
				 *  Begins a effect.
				 *  Ordinarily this is call by the RenderDevice when drawing a list of render operations.
				 *  As the nature of batching, that is to say, this is called per material and geometry shape.
				 *  For instance, this will not be call more than one time, 
				 *  if all the render operations are from the same model.
				 *
				 * @return The number of passes needed.
				 */
				int Begin();
				/**
				 *  Ends s effect.
				 *  This must be call corresponding to Begin. 
				 */
				void End();

				/**
				 *  Prepare the parameter setting up
				 * 
				 *  A list of render operation with the same geometry data is provided here as parameter 
				 *  in case the needs of other render data, eg. instancing requires all transformation of the each instance
				 */
				virtual void Setup(Material* mtrl, const RenderOperation* rop, int count) = 0;

				/**
				 *  Some effects need to draw the mesh more than one time.
				 *  So material-shape level multi-pass is supported here.
				 *
				 *  Notice this is different form scene-level pass(aka. ScenePass).
				 */
				virtual void BeginPass(int passId) = 0;
				virtual void EndPass() = 0;

				/**
				 *  Check if the effect supports instancing. 
				 */
				virtual bool SupportsInstancing() { return false; }

				bool IsUnsupported() const { return m_isUnsupported; }
				const String& getName() const { return m_name; }
			protected:				
				virtual int begin() = 0;
				virtual void end() = 0;

				static VertexShader* LoadVertexShader(RenderDevice* rs, const ResourceLocation* vs);
				static PixelShader* LoadPixelShader(RenderDevice* rs, const ResourceLocation* ps);
				static void LoadEffect(RenderDevice* rs, const ResourceLocation* rl, VertexShader*& vs, PixelShader*& ps);

				bool m_isUnsupported;
				String m_name;
			private:
				
				bool m_begun;

			};

			/**
			 *  AutomaticEffect is a sub set of Effect which can be create from AFX while only each parameter's usage string
			 *  is required for setup, so that the engine can set up the
			 *  parameters based on the usage description all by the engine itself, without additional setup code.
			 *
			 *  The AFX, generated by APBuild, is the result of a sequence of micro effects.
			*/
			class APAPI AutomaticEffect : public Effect
			{
			public:
				AutomaticEffect(RenderDevice* device, const ResourceLocation* rl);
				~AutomaticEffect();

				virtual void Setup(Material* mtrl, const RenderOperation* rop, int count);

				virtual void BeginPass(int passId);
				virtual void EndPass();

				/**
				 *  Check if the effect supports instancing.
				 *  The effect is regarded instancing once at least one parameter with usage
				 *  "tr_instanceworld" is used.
				 */
				virtual bool SupportsInstancing();

				int FindParameterIndex(const String& name);

				template<typename T>
				void SetParameterValue(int index, T* value, int count);
				void SetParameterTexture(int index, ResourceHandle<Texture>* value);
				void SetParameterTexture(int index, Texture* value);

				virtual void Update(const GameTime* const time);
			protected:
				virtual int begin();
				virtual void end();
			private:
				void SetVector2(EffectParameter& param, Vector2 value);
				void SetVector3(EffectParameter& param, Vector3 value);
				void SetVector4(EffectParameter& param, Vector4 value);
				void SetSamplerState(EffectParameter& param);
				void SetTexture(EffectParameter& param, ResourceHandle<Texture>* value);
				void SetTexture(EffectParameter& param, Texture* value);
				template<typename T>
				void SetValue(EffectParameter& param, const T& value);
				void Set4X3Matrix(EffectParameter& param, const Matrix* transfroms, int count);
				void SetMatrix(EffectParameter& param, const Matrix* transfroms, int count);

				void SetInstanceBlobParameter(EffectParameter& param, const InstanceInfoBlob::CustomValue& v);
				void SetMaterialCustomParameter(EffectParameter& param, Material* mtrl);
				void SetSingleCustomParameter(EffectParameter& param, CustomEffectParameterType type, void* data);
				void SetSingleCustomParameter(EffectParameter& param, CustomEffectParameterType type, const uint* data);

				VertexShader* m_vertexShader;
				PixelShader* m_pixelShader;

				List<EffectParameter> m_parameters;

				RenderDevice* m_device;
				Texture* m_texture;

				bool m_supportsInstancing;

				float m_unifiedTime;
				float m_lastTime;
				//Matrix m_mtrxBuffer[InstancingData::MaxOneTimeInstances];
			};

			/** 
			 *  Represents a kind of shader effect which needs to be set up in client code by inheriting form this class.
			 *  This type of effect should be loaded from CFX files generated by APBuild.
			 */
			class APAPI CustomShaderEffect : public Effect
			{
			public:
				CustomShaderEffect(RenderDevice* device, const ResourceLocation* rl);
				virtual ~CustomShaderEffect();

			protected:
				VertexShader* m_vertexShader;
				PixelShader* m_pixelShader;
			};

			//class APAPI PostEffect
			//{
			//public:
			//	PostEffect(void);
			//	virtual ~PostEffect(void);

			//	virtual void Setup() = 0;

			//	bool IsUnsupported() const { return m_isUnsupported; }
			//	const String& getName() const { return m_name; }
			//protected:				

			//	bool m_isUnsupported;
			//	String m_name;
			//};
		};
	};
};
#endif