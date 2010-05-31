#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/*
		  Represents an operation to render a mesh part in the scene.
		  RenderOperation is used by the engine to manage the scene rendering pipeline.
		*/
		class _Export RenderOperation
		{
		private:
			GeomentryData* m_data;
		public:
			GeomentryData* getGeomentryData() { return m_data; }
			void setGeomentryData(GeomentryData* data) { m_data = data; }

			RenderOperation(void) { }
			~RenderOperation(void) { }
		};
	}
}
