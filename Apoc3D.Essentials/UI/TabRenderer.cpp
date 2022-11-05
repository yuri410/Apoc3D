#include "TabRenderer.h"
#include "apoc3d.Essentials/Utils/IOUtils.h"
#include "Layout.h"
#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		TabRenderer::TabRenderer(RenderDevice* dev, Tab* tab, ColorValue colorValue, const String& exportPath)
			: m_tab(tab)
			, m_clearColor(colorValue)
		{
			Point tabSize = tab->getLayoutManager()->GetTabSize();
			
			m_bufWidth = tabSize.X;
			m_bufHeight = tabSize.Y;
			m_contentHeight = Math::Max(m_bufHeight, tab->CalculateMaxContentSize().Y);

			m_passCount = (m_contentHeight + m_bufHeight - 1) / m_bufHeight;

			m_fileOutStream = new FileOutStream(exportPath);

			m_pngSave = BeginStreamPng(m_bufWidth, m_contentHeight, *m_fileOutStream);

			ObjectFactory* objFac = dev->getObjectFactory();
			m_exportBuffer = objFac->CreateRenderTarget(m_bufWidth, m_bufHeight, FMT_A8R8G8B8);
		}

		TabRenderer::~TabRenderer()
		{
			EndStreamPng(m_pngSave);

			DELETE_AND_NULL(m_fileOutStream);
			DELETE_AND_NULL(m_exportBuffer);
		}


		void TabRenderer::DoStep(Sprite* spr)
		{
			int32 yPos = m_bufHeight * m_currentPass;

			if (m_currentPassStage == 0)
			{
				AppTime time;

				m_tab->HideScroll = true;
				m_tab->SetScroll(yPos);
				m_tab->Update(&time);
				
				RenderDevice* dev = spr->getRenderDevice();

				spr->Flush();
				{
					ScopeRenderTargetChange srtc(dev, 0, m_exportBuffer);
					dev->Clear(CLEAR_ColorBuffer, m_clearColor, 1, 0);
					
					m_tab->Draw(spr);
					spr->Flush();
				}

				m_currentPassStage++;
			}
			else
			{
				m_tab->HideScroll = false;

				int32 passHeight = Math::Min(m_contentHeight - yPos, m_bufHeight);
				int32 startY = m_bufHeight - passHeight;

				if (m_currentSaveRow < passHeight)
				{
					int32 saveY = startY + m_currentSaveRow;
					int32 saveHeight = Math::Min(passHeight - m_currentSaveRow, 60);

					StreamInPng(m_pngSave, m_exportBuffer, saveY, saveHeight, true);
					m_currentSaveRow += 60;
				}
				else
				{
					m_currentSaveRow = 0;
					m_currentPassStage = 0;
					m_currentPass++;
				}
			}
		}

		float TabRenderer::GetProgress() const
		{
			float baseProgress = (float)m_currentPass / m_passCount;
			float subProgress = m_currentPassStage ? 0.1f : 0.0f;

			int32 yPos = m_bufHeight * (m_passCount - m_currentPass - 1);
			int32 passHeight = Math::Min(m_contentHeight - yPos, m_bufHeight);

			subProgress += 0.9f * Math::Saturate((float)m_currentSaveRow / passHeight);

			return baseProgress + subProgress / m_passCount;
		}
	}
}