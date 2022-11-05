#pragma once

#include "UICommon.h"
#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		class AnimatedIcon;

		class PresentationManager
		{
			SINGLETON_DECL(PresentationManager);

		public:
			PresentationManager();
			~PresentationManager();

			void LoadAnimatedIcon(RenderDevice* device, int id, const String& name, const FileLocateRule& locateRule, const String& displayText, float rate, float textOffset = -1);
			void LoadIcon(RenderDevice* device, int id, const String& fileName, const FileLocateRule& locateRule, Apoc3D::Math::Rectangle iconArea);

			void Unload();

			AnimatedIcon* GetAnimatedIcon(int id) const;
			const UIGraphicSimple* GetIcon(int id) const;
		private:
			HashMap<int, AnimatedIcon*> m_animatedIcons;
			HashMap<int, UIGraphicSimple> m_icons;

			HashMap<FileLocation, Texture*> m_iconPacks;
		};

		class AnimatedIcon
		{
		public:
			AnimatedIcon(RenderDevice* device, const String& name, const FileLocateRule& locateRule, float rate, float textOffset = -1);
			~AnimatedIcon();

			AnimatedIconInstance CreateInstance() const { return AnimatedIconInstance(m_frames.getCount(), m_playbackRate); }

			void Draw(Sprite* spr, AnimatedIconInstance& instance, Rectangle dstRect, float progress = 0);
			void Draw(Sprite* spr, AnimatedIconInstance& instance, Point centerPos, int32 width, float progress = 0);

			float getAspectRatio() const { return m_aspectRatio; }

			void setDisplayText(const String& txt) { m_displayText = txt; }

		private:

			List<Texture*> m_frames;
			Font* m_txtFont;
			float m_textOffset = -1;
			float m_aspectRatio = 1;
			float m_playbackRate = 1;

			String m_displayText;
		};

	}
}
