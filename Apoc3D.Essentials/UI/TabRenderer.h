#pragma once

#include "UICommon.h"
#include "apoc3d/Math/ColorValue.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	struct PngSaveContext;

	namespace UI
	{
		class TabRenderer
		{
		public:
			TabRenderer(RenderDevice* dev, Tab* tab, ColorValue clearColor, const String& exportPath);
			~TabRenderer();

			void DoStep(Sprite* spr);

			bool isFinished() const { return m_currentPass >= m_passCount; }

			float GetProgress() const;
		private:
			FileOutStream* m_fileOutStream = nullptr;

			int32 m_currentPass = 0;
			int32 m_currentPassStage = 0;
			int32 m_currentSaveRow = 0;

			int32 m_passCount = 0;
			int32 m_bufWidth = 0;
			int32 m_bufHeight = 0;
			int32 m_contentHeight = 0;

			ColorValue m_clearColor;
			
			Tab* m_tab;
			RenderTarget* m_exportBuffer;

			PngSaveContext* m_pngSave = nullptr;
		};
	}
}