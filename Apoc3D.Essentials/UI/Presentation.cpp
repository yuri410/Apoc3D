#include "Presentation.h"
#include "apoc3d.Essentials/Utils/IOUtils.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
#pragma region PresentationManager

		SINGLETON_IMPL(PresentationManager);

		PresentationManager::PresentationManager()
		{

		}

		PresentationManager::~PresentationManager()
		{

		}

		void PresentationManager::LoadAnimatedIcon(RenderDevice* device, int id, const String& name, const FileLocateRule& locateRule, const String& displayText, float rate, float textOffset)
		{
			if (m_animatedIcons.Contains(id))
				return;

			AnimatedIcon* ico = new AnimatedIcon(device, name, locateRule, rate, textOffset);
			ico->setDisplayText(displayText);

			m_animatedIcons.Add(id, ico);
		}

		void PresentationManager::LoadIcon(RenderDevice* device, int id, const String& fileName, const FileLocateRule& locateRule, Apoc3D::Math::Rectangle iconArea)
		{
			FileLocation fl = FileSystem::getSingleton().Locate(fileName, locateRule);

			Texture* texturePack;
			if (!m_iconPacks.TryGetValue(fl, texturePack))
			{
				texturePack = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
				m_iconPacks.Add(fl, texturePack);
			}

			m_icons.AddOrReplace(id, UIGraphicSimple(texturePack, iconArea));
		}

		void PresentationManager::Unload()
		{
			m_animatedIcons.DeleteValuesAndClear();
			m_iconPacks.DeleteValuesAndClear();
			m_icons.Clear();
		}

		AnimatedIcon* PresentationManager::GetAnimatedIcon(int id) const
		{
			AnimatedIcon** icoPtr = m_animatedIcons.TryGetValue(id);
			if (icoPtr)
				return *icoPtr;
			return nullptr;
		}

		const UIGraphicSimple* PresentationManager::GetIcon(int id) const
		{
			return m_icons.TryGetValue(id);
		}
#pragma endregion


#pragma region AnimatedIcon
		//////////////////////////////////////////////////////////////////////////

		AnimatedIcon::AnimatedIcon(RenderDevice* device, const String& name, const FileLocateRule& locateRule, float rate, float textOffset)
			: m_playbackRate(rate), m_textOffset(textOffset)
		{
			FileLocation fl;

			for (int32 i = 0; i <= 999; i++)
			{
				String fn = name + L"_" + StringUtils::IntToString(i + 1, StrFmt::a<3, '0'>::val) + L".tex";
				if (FileSystem::getSingleton().TryLocate(fn, locateRule, fl))
				{
					Texture* tex = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl);
					m_frames.Add(tex);
				}
				else break;
			}

			if (m_frames.getCount())
			{
				m_aspectRatio = m_frames[0]->getWidth() / (float)m_frames[0]->getHeight();
			}

			m_txtFont = FontManager::getSingleton().getFont(L"english");
		}

		AnimatedIcon::~AnimatedIcon()
		{
			m_frames.DeleteAndClear();
		}

		void AnimatedIcon::Draw(Sprite* spr, AnimatedIconInstance& instance, Rectangle dstRect, float progress)
		{
			if (dstRect.Width == 0 || dstRect.Height == 0)
				return;

			const int32 frameIdx = instance.getFrameCount();

			if (!m_frames.isIndexInRange(frameIdx))
				return;

			Texture* frame = m_frames[frameIdx];
			spr->Draw(frame, dstRect, nullptr, CV_White);

			if (m_textOffset >= 0)
			{
				Rectangle barRect = dstRect;
				barRect.Y = dstRect.getBottom() + Math::Round(m_textOffset * dstRect.Height);
				barRect.Height = m_txtFont->getLineHeightInt();

				const String& txt = m_displayText;
				Point txtSize = m_txtFont->MeasureString(txt);
				Point labelPos = barRect.GetCenterRegion(txtSize).getPosition();

				if (progress > 0)
				{
					barRect.Y += barRect.Height + 10;
					barRect.Height = 6;

					Rectangle barRectClipped = barRect;
					barRectClipped.Width = Math::Round(progress*barRectClipped.Width);

					if (barRectClipped.Width > 0)
					{
						spr->Flush();
						spr->getRenderDevice()->getRenderState()->setScissorTest(true, &barRectClipped);
						spr->DrawRoundedRect(SystemUI::GetWhitePixel(), barRect, nullptr, 3, 2, CV_White);
						spr->Flush();
						spr->getRenderDevice()->getRenderState()->setScissorTest(false, nullptr);
					}

					spr->DrawLine(SystemUI::GetWhitePixel(), barRect.getCenterLeft(), barRect.getCenterRight(), CV_White, 1, LineCapOptions::Butt);
				}

				m_txtFont->DrawString(spr, txt, labelPos, CV_White);
			}

			instance.Advance();
		}

		void AnimatedIcon::Draw(Sprite* spr, AnimatedIconInstance& instance, Point centerPos, int32 width, float progress)
		{
			int32 height = Math::Round(width / m_aspectRatio);
			Draw(spr, instance, { centerPos - Point(width / 2,height / 2), {width, height} }, progress);
		}

		/*
		int32 AnimatedIcon::CalculateHeight(int32 width) const
		{
			int32 height = Math::Round(width / m_aspectRatio);

			return height + Math::Max(0, m_textOffset) + m_txtFont->getLineHeightInt();
		}*/
#pragma endregion

		//////////////////////////////////////////////////////////////////////////

		void AnimatedIconInstance::Advance()
		{
			m_frameIndex += m_playbackRate;

			if (m_frameIndex >= m_frameCount)
				m_frameIndex = 0;
		}

	}
}