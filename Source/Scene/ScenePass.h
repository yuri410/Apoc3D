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
#ifndef SCENEPASS_H
#define SCENEPASS_H

#include "Common.h"

namespace Apoc3D
{
	namespace Scene
	{
		/* Define a sequence of operation for a rendering result.

		   A scene pass is a scene rendering operation that usually 
		   renders objects to a RenderTarget (or the like) which can 
		   be used for further scene rendering passes.
		   
		   The ScenePass is a part of render script that controls 
		   a pass of scene rendering. It selects objects for rendering
		   by check the objects' Material::PassFlag against the pass sequence 
		   in the script.
		*/
		class APAPI ScenePass
		{
		private:
			int32 m_sequence;
			String m_name;
		public:
			/** Gets the sequence of this pass in a entire scene rendering process.
			*/
			int32 getSequence() const { return m_sequence; }

			/** Gets the name of this pass.
			*/
			String getName() const { return m_name; }

			ScenePass(void);
			~ScenePass(void);
		};
	};
};
#endif