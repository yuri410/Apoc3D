#pragma once
#ifndef APOC3D_EFFECT_H
#define APOC3D_EFFECT_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "apoc3d/ApocCommon.h"
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
			/** Defines the interface for graphics effect. */
			class APAPI Effect
			{
				RTTI_BASE;
			public:
				Effect();
				virtual ~Effect();


				virtual void Update(const AppTime* time) { }

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

				/** Check if the effect supports instancing. */
				virtual bool SupportsInstancing() { return false; }

				bool IsUnsupported() const { return m_isUnsupported; }
				const String& getName() const { return m_name; }

			protected:				
				virtual int begin() = 0;
				virtual void end() = 0;

				//static Shader* LoadVertexShader(RenderDevice* rs, const ResourceLocation& vs);
				//static Shader* LoadPixelShader(RenderDevice* rs, const ResourceLocation& ps);

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

				virtual void Setup(Material* mtrl, const RenderOperation* rop, int32 count);

				virtual void BeginPass(int passId);
				virtual void EndPass();

				/**
				 *  Check if the effect supports instancing.
				 *  The effect is regarded instancing once at least one parameter with usage
				 *  "tr_instanceworld" is used.
				 */
				virtual bool SupportsInstancing();

				void Reload(const ResourceLocation& rl);

				int32 FindParameterIndex(const String& name);

				template<typename T>
				void SetParameterValue(int32 index, const T* value, int32 count)
				{
					ResolvedEffectParameter& param = m_parameters[index];
					param.RS_TargetShader->SetValue(param.RegisterIndex, value, count);
				}


				void SetParameterValue(int32 index, const Vector2* value, int32 count);
				void SetParameterValue(int32 index, const Vector3* value, int32 count);
				void SetParameterValue(int32 index, const Vector4* value, int32 count);
				void SetParameterValue(int32 index, const Matrix* value, int32 count);

				void SetParameterValue(int32 index, bool value) { SetParameterValue(index, &value, 1); }
				void SetParameterValue(int32 index, float value) { SetParameterValue(index, &value, 1); }
				void SetParameterValue(int32 index, const Vector2& value) { SetParameterValue(index, &value, 1); }
				void SetParameterValue(int32 index, const Vector3& value) { SetParameterValue(index, &value, 1); }
				void SetParameterValue(int32 index, const Vector4& value) { SetParameterValue(index, &value, 1); }
				void SetParameterValue(int32 index, const Matrix& value) { SetParameterValue(index, &value, 1); }


				template<typename T>
				void SetParameterValueByName(const String& name, const T* value, int32 count);

				template<typename T>
				void SetParameterValueByName(const String& name, const T& value);


				void SetParameterTexture(int32 index, ResourceHandle<Texture>* value);
				void SetParameterTexture(int32 index, Texture* value);
				void SetParameterTextureByName(const String& name, ResourceHandle<Texture>* value);
				void SetParameterTextureByName(const String& name, Texture* value);

				virtual void Update(const AppTime* time);

				RenderDevice* getDevice() const { return m_device; }
			protected:
				virtual int32 begin();
				virtual void end();

			private:
				struct ResolvedEffectParameter 
				{
					ResolvedEffectParameter() { }
					explicit ResolvedEffectParameter(RenderDevice* device, const String& effectName,
						const EffectParameter* src, Shader* targetShader, bool& hasShaderIssues);
					
					void Free();

					bool RS_SetupAtBegining = false;
					bool RS_SetupAtBeginingOnly = false;		/** if a parameter has RS_SetupAtBegining==true, this means if setup can be fully done at beginning. If yes nothing else is done in instance setup. */
					Shader* RS_TargetShader = nullptr;
					EffectParamUsage Usage = EPUSAGE_Unknown;

					int32 InstanceBlobIndex = -1;
					int32 RegisterIndex = -1;
					int32 SamplerIndex = -1;

					ResourceHandle<Texture>* DefaultTexture = nullptr;

					// for rare data, access through this reference
					const EffectParameter* ReferenceSource = nullptr;

					void SetSamplerState() const;

					void SetVector2(const Vector2& value) const;
					void SetVector3(const Vector3& value) const;
					void SetVector4(const Vector4& value) const;
					void SetVector4(const Vector4* value, int32 count) const;
					
					void SetColor4(const Color4& value) const;

					void Set4X3Matrix(const Matrix* transfroms, int32 count) const;

					void SetMatrix(const Matrix& m) const;
					void SetMatrix(const Matrix* transfroms, int32 count) const;

					void SetFloat(const float values) const;
					void SetFloat(const float* values, int32 count) const;

					void SetInt(const int32 values) const;
					void SetInt(const int32* values, int32 count) const;

					void SetBool(const bool values) const;
					void SetBool(const bool* values, int32 count) const;

				};

				void SetTexture(const ResolvedEffectParameter& param, ResourceHandle<Texture>* value);
				void SetTexture(const ResolvedEffectParameter& param, Texture* value);
				
				void SetInstanceBlobParameter(const ResolvedEffectParameter& param, const InstanceInfoBlobValue& v);
				void SetMaterialCustomParameter(const ResolvedEffectParameter& param, Material* mtrl);
				void SetSingleCustomParameter(const ResolvedEffectParameter& param, CustomEffectParameterType type, const void* data);

				Shader* m_vertexShader = nullptr;
				Shader* m_pixelShader = nullptr;

				List<EffectParameter> m_parametersSrc;
				List<ResolvedEffectParameter> m_parameters;

				RenderDevice* m_device;
				Texture* m_whiteTexture;

				bool m_supportsInstancing = false;

				float m_unifiedTime = 0;
				float m_lastTime = 0;

				// for duplicated material parameter setup check only
				void* m_previousMaterialPointer = nullptr;
			};

			template<typename T>
			void AutomaticEffect::SetParameterValueByName(const String& name, const T* value, int32 count)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterValue(idx, value, count);
			}

			template<typename T>
			void AutomaticEffect::SetParameterValueByName(const String& name, const T& value)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterValue(idx, value);
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
				CustomShaderEffect(RenderDevice* device, const String& name, const byte* vsCode, const byte* psCode);

				virtual ~CustomShaderEffect();

			protected:
				Shader* m_vertexShader;
				Shader* m_pixelShader;
			};

		};
	};
};
#endif