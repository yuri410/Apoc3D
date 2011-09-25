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
#ifndef BATCHMODELBUILDER_H
#define BATCHMODELBUILDER_H

#include "Common.h"
#include "Core/Resource.h"
#include "Renderable.h"
#include "RenderOperationBuffer.h"
#include "Collections/FastList.h"
#include "ModelTypes.h"
#include "Animation/AnimationTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI BatchModelBuilder
		{
		public:

			void AddEntry(int source, const Matrix& transform)
			{
				Entry ent = { source, transform };
				m_entires.Add(ent);
			}

			void AddSource(FileLocation* fl)
			{
				m_modelTable.Add(fl);
			}

			ModelData* BuildData();

			BatchModelBuilder(){}
			~BatchModelBuilder();
		private:
			struct Entry
			{
				int SourceId;
				Matrix Transform;
			};

			FastList<FileLocation*> m_modelTable;

			FastList<Entry> m_entires;
		};

	}
}
#endif