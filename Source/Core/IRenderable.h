#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/*
		  Represents drawable object in the scene.
		*/
		class _Export IRenderable
		{
		public:
			/*
			  Gets the render operation of this renderable object at a ceratin LOD level
			*/
			virtual RenderOperationBuffer* GetRenderOperation(int level);
			/*
			  Gets the render operation of this renderable object at the default LOD level
			*/
			virtual RenderOperationBuffer* GetRenderOperation() { return GetRenderOperation(0); }

		protected:
			IRenderable(void)
			{
			}

			~IRenderable(void)
			{
			}
		};
	}
}
