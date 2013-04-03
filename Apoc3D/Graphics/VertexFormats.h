#pragma once
#ifndef APOC3D_VERTEXFORMATS_H
#define APOC3D_VERTEXFORMATS_H

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

#include "RenderSystem/VertexElement.h"

#include "apoc3d/Collections/FastList.h"
#include "apoc3d/Core/IHashed.h"
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		struct APAPI VertexPIBNT1 : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector4 BlendIndices;
			Vector4 BlendWeight;
			Vector3 Normal;
			Vector2 TexCoord1;

			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector3Utils::GetHashCode(Normal) ^ Vector2Utils::GetHashCode(TexCoord1) ^
					Vector4Utils::GetHashCode(BlendWeight) ^ Vector4Utils::GetHashCode(BlendIndices);
			}
			virtual String GetHashString() const;
		};

		struct APAPI VertexPN : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;

			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector3Utils::GetHashCode(Normal);
			}
			virtual String GetHashString() const;
		};

		struct APAPI VertexPNT1 : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;
			Vector2 TexCoord1;


			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector3Utils::GetHashCode(Normal) ^ Vector2Utils::GetHashCode(TexCoord1);
			}
			virtual String GetHashString() const;
		};

		struct APAPI VertexPNT2 : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;
			Vector2 TexCoord1;
			Vector2 TexCoord2;

			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector3Utils::GetHashCode(Normal) ^ 
					Vector2Utils::GetHashCode(TexCoord1) ^ Vector2Utils::GetHashCode(TexCoord2);
			}
			virtual String GetHashString() const;
		};

		struct APAPI VertexPT1 : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector2 TexCoord1;

			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector2Utils::GetHashCode(TexCoord1);
			}
			virtual String GetHashString() const;
		};
		struct APAPI VertexPT2 : public IHashed
		{
		private:
			static FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector2 TexCoord1;
			Vector2 TexCoord2;

			virtual HashHandle GetHashCode() const
			{
				return Vector3Utils::GetHashCode(Position) ^ Vector2Utils::GetHashCode(TexCoord1) ^ Vector2Utils::GetHashCode(TexCoord2);
			}
			virtual String GetHashString() const;
		};

	}
}

#endif