#pragma once
#ifndef APOC3D_EFFECT_H
#define APOC3D_EFFECT_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "apoc3d/Common.h"
#include "EffectParameter.h"
#include "apoc3d/Collections/List.h"

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
				RTTI_BASE;
			public:
				Effect();
				virtual ~Effect();


				virtual void Update(const GameTime* time) { }

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

				static Shader* LoadVertexShader(RenderDevice* rs, const ResourceLocation& vs);
				static Shader* LoadPixelShader(RenderDevice* rs, const ResourceLocation& ps);
				static void LoadEffect(RenderDevice* rs, const ResourceLocation& rl, Shader*& vs, Shader*& ps);

				String m_name;
				bool m_isUnsupported = false;
			private:
				bool m_begun = false;
				
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
				RTTI_DERIVED(AutomaticEffect, Effect);
			public:
				AutomaticEffect(RenderDevice* device, const ResourceLocation& rl);
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

				void Reload(const ResourceLocation& rl);

				int FindParameterIndex(const String& name);

				template<typename T>
				void SetParameterValue(int index, const T* value, int count);
				void SetParameterTexture(int index, ResourceHandle<Texture>* value);
				void SetParameterTexture(int index, Texture* value);

				template<typename T>
				void SetParameterValueByName(const String& name, const T* value, int count);
				void SetParameterTextureByName(const String& name, ResourceHandle<Texture>* value);
				void SetParameterTextureByName(const String& name, Texture* value);

				virtual void Update(const GameTime* time);

			protected:
				virtual int begin();
				virtual void end();
			private:
				struct ResolvedEffectParameter : public EffectParameter
				{
					ResolvedEffectParameter() : EffectParameter() { }
					explicit ResolvedEffectParameter(const EffectParameter& base)
						: EffectParameter(base) { }

					bool RS_SetupAtBegining = false;
					bool RS_SetupAtBeginingOnly = false;
					Shader* RS_TargetShader = nullptr;

					void SetSamplerState() const;

					void SetVector2(const Vector2& value) const;
					void SetVector3(const Vector3& value) const;
					void SetVector4(const Vector4& value) const;
					void SetVector4(const Vector4* value, int count) const;
					
					void SetColor4(const Color4& value) const;

					void Set4X3Matrix(const Matrix* transfroms, int count) const;

					void SetMatrix(const Matrix& m) const;
					void SetMatrix(const Matrix* transfroms, int count) const;

					void SetFloat(const float values) const;
					void SetFloat(const float* values, int count) const;

					void SetInt(const int values) const;
					void SetInt(const int* values, int count) const;

					void SetBool(const bool values) const;
					void SetBool(const bool* values, int count) const;

				};

				void SetTexture(ResolvedEffectParameter& param, ResourceHandle<Texture>* value);
				void SetTexture(ResolvedEffectParameter& param, Texture* value);
				
				void SetInstanceBlobParameter(ResolvedEffectParameter& param, const InstanceInfoBlobValue& v);
				void SetMaterialCustomParameter(ResolvedEffectParameter& param, Material* mtrl);
				void SetSingleCustomParameter(ResolvedEffectParameter& param, CustomEffectParameterType type, const void* data);

				Shader* m_vertexShader = nullptr;
				Shader* m_pixelShader = nullptr;

				List<ResolvedEffectParameter> m_parameters;

				RenderDevice* m_device;
				Texture* m_texture;

				bool m_supportsInstancing = false;

				float m_unifiedTime = 0;
				float m_lastTime = 0;

				// for duplicated material parameter setup check only
				void* m_previousMaterialPointer = nullptr;
				//Matrix m_mtrxBuffer[InstancingData::MaxOneTimeInstances];
			};

			template<typename T>
			void AutomaticEffect::SetParameterValueByName(const String& name, const T* value, int count)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterValue(idx, value, count);
			}

			/** 
			 *  Represents a kind of shader effect which needs to be set up in client code by inheriting form this class.
			 *  This type of effect should be loaded from CFX files generated by APBuild.
			 */
			class APAPI CustomShaderEffect : public Effect
			{
				RTTI_DERIVED(CustomShaderEffect, Effect);
			public:
				CustomShaderEffect(RenderDevice* device, const ResourceLocation& rl);
				virtual ~CustomShaderEffect();

			protected:
				Shader* m_vertexShader;
				Shader* m_pixelShader;
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