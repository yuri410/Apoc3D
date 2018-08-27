#pragma once
#ifndef APOC3D_BATCHMODELBUILDER_H
#define APOC3D_BATCHMODELBUILDER_H

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

#include "Renderable.h"
#include "RenderOperationBuffer.h"
#include "ModelTypes.h"
#include "apoc3d/Vfs/ResourceLocation.h"

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
		/**
		 *  A static model batching combiner.
		 *  This class joins several small models into a big one with each transformed by a matrix.
		 *  All input models are expected to have the same vertex format.
		 */
		class APAPI BatchModelBuilder
		{
		public:
			/** Add an instance of a given source, with a given transform matrix. */
			void AddEntry(int source, const Matrix& transform);

			/** Add the sources of models represented by FileLocation */
			void AddSource(const FileLocation& fl);

			/** Build the model data, when finished adding sources and entries. */
			ModelData* BuildData();

			BatchModelBuilder();
			~BatchModelBuilder();
		private:
			struct Entry
			{
				int SourceId;
				Matrix Transform;
			};

			List<FileLocation> m_modelTable;

			List<Entry> m_entires;
		};

	}
}
#endif