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

#include "FontManager.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Vfs/ResourceLocation.h"
#include "IOLib/BinaryReader.h"
#include "IOLib/Streams.h"
#include "Math/Point.h"

using namespace Apoc3D::IO;

SINGLETON_DECL(Apoc3D::UI::FontManager);

namespace Apoc3D
{
	namespace UI
	{
		Font::Font(RenderDevice* device, const ResourceLocation* fl)
			: m_charTable(255, WCharEqualityComparer::BuiltIn::Default)
		{
			ObjectFactory* fac = device->getObjectFactory();
			m_font = fac->CreateTexture(FontManager::TextureSize, FontManager::TextureSize, 1, TU_DynamicWriteOnly, FMT_Alpha8);

			Stream* strm = fl->GetReadStream();
			BinaryReader* br = new BinaryReader(strm);

			int charCount = br->ReadInt32();
			


			br->Close();
			delete br;
			delete strm;
		}
		Font::~Font()
		{
			delete m_font;
		}

		void Font::DrawString(Sprite* sprite, const String& text, const Point& pt, uint color)
		{

		}
		void Font::DrawString(Sprite* sprite, const String& text, int x, int y, uint color)
		{

		}

		int FontManager::TextureSize = 512;

		FontManager::FontManager()
			: m_fontTable()
		{

		}
		FontManager::~FontManager()
		{

		}

		Font* FontManager::getFont(const String& fontName)
		{
			return m_fontTable[fontName];
		}

		
	}
}