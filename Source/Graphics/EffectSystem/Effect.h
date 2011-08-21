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

#ifndef EFFECT_H
#define EFFECT_H

#include "Common.h"
#include "EffectParameter.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			/* Represents the result of a sequence of micro effects.
			*/
			class APAPI Effect
			{
			public:
				Effect(void);
				~Effect(void);

				int Begin();
				void End();

				virtual void Setup(Material* mtrl, const RenderOperation& rop) = 0;

				virtual void BeginPass(int passId) = 0;
				virtual void EndPass() = 0;

				const String& getName() const { return m_name; }
			protected:
				virtual int begin() = 0;
				virtual void end() = 0;

				static VertexShader* LoadVertexShader(RenderDevice* rs, const ResourceLocation* vs);
				static PixelShader* LoadPixelShader(RenderDevice* rs, const ResourceLocation* ps);
				static void LoadEffect(RenderDevice* rs, const ResourceLocation* rl, VertexShader*& vs, PixelShader*& ps);
			private:
				String m_name;
				bool m_begun;
			};

			/** AutomaticEffect is a sub set of Effect that create from AFX and only has know usage 
				for each of its parameters, so that the engine can set up the
				parameters all by the engine itself, without additional setup code.
			*/
			class AutomaticEffect : public Effect
			{
			public:
				AutomaticEffect();

				virtual void Setup(Material* mtrl, const RenderOperation& rop);

				virtual void BeginPass(int passId);
				virtual void EndPass();

			protected:
				virtual int begin();
				virtual void end();
			private:
				List<EffectParameter> m_parameters;
			};

		};
	};
};
#endif