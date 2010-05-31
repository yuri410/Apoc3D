#pragma once

#include "..\Common.h"
#include "Frustum.h"

namespace Apoc3D
{
	namespace Core
	{
		/* Represents a view into a 3D scene. 

		*/
		class _Export Camera
		{
		private:
	
			D3DMATRIX m_view;
			D3DMATRIX m_proj;

			Frustum frustum;
		public:
	
			/* Gets the view transform matrix
			*/
			const D3DMATRIX& getViewMatrix() { return m_view; }
			/* Gets the projection matrix
			*/
			const D3DMATRIX& getProjMatrix() { return m_proj; }

			/* Sets the view transform matrix
			*/
			void setViewMatrix(const D3DMATRIX& value) { m_view = value; }
			/* Sets the projection transform matrix
			*/
			void setProjMatrix(const D3DMATRIX& value) { m_proj = value; }

			/* Update the camera's state. 
			*/
			void Update(GameTime* time);


			Camera(void);
			~Camera(void);
		};

	}
}