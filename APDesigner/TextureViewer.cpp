/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "TextureViewer.h"

#include "MainWindow.h"
#include "UIResources.h"

#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/ObjectFactory.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/PictureBox.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Utility/StringUtils.h"


using namespace Apoc3D::Utility;

namespace APDesigner
{
	static Point MaxSize(300, 300);
	static Point MinSize(64, 64);

	TextureThumbViewer::TextureThumbViewer(RenderDevice* device)
		: m_texture(0), m_device(device)
	{
		m_form = new Form(FBS_None, L"Texture thumbnail");

		m_pictureBox = new PictureBox(Point::Zero, 1);
		m_pictureBox->eventPictureDraw().Bind(this, &TextureThumbViewer::PictureBox_Draw);
		m_form->getControls().Add(m_pictureBox);
		m_form->Initialize(device);
	}
	void TextureThumbViewer::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* rect)
	{
		switch (m_texture->getType())
		{
		case TT_Texture1D:
		case TT_Texture2D:
			sprite->Draw(m_texture, *rect, 0, CV_White);
			break;
		case TT_Texture3D:

			break;
		case TT_CubeTexture:
			
			break;
		}
	}

	Texture* ExtractCubemapFace(ObjectFactory* objFac, Texture* cubemap, CubeMapFace face)
	{
		Texture* result = objFac->CreateTexture(cubemap->getWidth(), cubemap->getWidth(),1, TU_Static,cubemap->getFormat());

		DataRectangle src = cubemap->Lock(0,LOCK_ReadOnly, face);
		DataRectangle dest = result->Lock(0, LOCK_None);

		const char* srcPtr = reinterpret_cast<const char*>( src.getDataPointer());
		char* dstPtr = reinterpret_cast<char*>( dest.getDataPointer());

		if (src.isConsecutive() && dest.isConsecutive())
		{
			memcpy(dstPtr, srcPtr, dest.getMemorySize());
		}
		else
		{
			for (int i=0;i<src.getHeight();i++)
			{
				memcpy(dstPtr, srcPtr, dest.getPitch());
				dstPtr+=dest.getPitch();
				srcPtr+=src.getPitch();
			}
		}

		result->Unlock(0);
		cubemap->Unlock(face,0);

		return result;
	}
	Texture* ExtractVolumeSlice(ObjectFactory* objFac, Texture* map, int slice)
	{
		if (slice>map->getDepth())
			slice = map->getDepth();

		Texture* result = objFac->CreateTexture(map->getWidth(),map->getHeight(),1, TU_Static,map->getFormat());
		
		DataBox src = map->Lock(0,LOCK_ReadOnly, Box(0,0,0,map->getWidth(),map->getWidth(), map->getDepth()));
		DataRectangle dest = result->Lock(0, LOCK_None);

		const char* srcPtr = reinterpret_cast<const char*>( src.getDataPointer());
		char* dstPtr = reinterpret_cast<char*>( dest.getDataPointer());

		srcPtr+=slice * src.getSlicePitch();

		{
			for (int i=0;i<src.getHeight();i++)
			{
				memcpy(dstPtr, srcPtr, dest.getPitch());
				dstPtr+=dest.getPitch();
				srcPtr+=src.getSlicePitch();
			}
		}

		result->Unlock(0);
		map->Unlock(0);

		return result;
	}
	void TextureThumbViewer::Show(const Point& position, Texture* texture)
	{
		m_texture = texture;

		Point newSize = MaxSize;
		switch(m_texture->getType())
		{
		case TT_CubeTexture:
			{
				ObjectFactory* objFac = m_device->getObjectFactory();
				m_previewCubeFaces[0] = ExtractCubemapFace(objFac, texture, CUBE_NegativeX);
				m_previewCubeFaces[1] = ExtractCubemapFace(objFac, texture, CUBE_NegativeY);
				m_previewCubeFaces[2] = ExtractCubemapFace(objFac, texture, CUBE_NegativeZ);
				m_previewCubeFaces[3] = ExtractCubemapFace(objFac, texture, CUBE_PositiveX);
				m_previewCubeFaces[4] = ExtractCubemapFace(objFac, texture, CUBE_PositiveY);
				m_previewCubeFaces[5] = ExtractCubemapFace(objFac, texture, CUBE_PositiveZ);

			}
			break;
		case TT_Texture3D:
			{
				ObjectFactory* objFac = m_device->getObjectFactory();
				m_previewCubeFaces[0] = ExtractVolumeSlice(objFac, texture, 0);
				m_previewCubeFaces[1] = ExtractVolumeSlice(objFac, texture, texture->getDepth()/3);
				m_previewCubeFaces[2] = ExtractVolumeSlice(objFac, texture, 2*texture->getDepth()/3);
				m_previewCubeFaces[3] = ExtractVolumeSlice(objFac, texture, texture->getDepth()-1);

			}
		case TT_Texture2D:
			if (texture->getWidth() < MaxSize.X && texture->getHeight() < MaxSize.Y)
			{
				newSize.X = texture->getWidth();
				newSize.Y = texture->getHeight();		

			}
			else
			{
				if (texture->getWidth()>texture->getHeight())
				{
					newSize.X = MaxSize.X;
					newSize.Y = static_cast<int>(texture->getHeight() * MaxSize.X / (float)texture->getWidth());
				}
				else
				{
					newSize.X = static_cast<int>(texture->getWidth() * MaxSize.Y / (float)texture->getHeight());
					newSize.Y = MaxSize.Y;
				}
			}
			
			break;
		case TT_Texture1D:
			if (texture->getWidth() >1)
			{
				newSize.X = MaxSize.X;
				newSize.Y = MinSize.Y;
			}
			else
			{
				newSize.X = MinSize.X;
				newSize.Y = MaxSize.Y;
			}
			break;
		}

		if (newSize.Y < MinSize.Y)
			newSize.Y = MinSize.Y;
		if (newSize.X < MinSize.X)
			newSize.X = MinSize.X;

		m_pictureBox->Size = newSize;
		m_form->Position = position;
		m_form->Size = newSize;
		m_form->Size.X += 2;
		m_form->Size.Y += 2;
		m_form->Show();
	}

	void TextureThumbViewer::Hide()
	{
		m_form->Close();
		for (int i=0;i<4;i++)
		{
			if (m_previewSlices[i])
			{
				delete m_previewSlices[i];
				m_previewSlices[i] = 0;
			}
		}
		for (int i=0;i<6;i++)
		{
			if (m_previewCubeFaces[i])
			{
				delete m_previewCubeFaces[i];
				m_previewCubeFaces[i] = 0;
			}
		}
		m_texture = 0;
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	TextureViewer::TextureViewer(MainWindow* window, const String& name, const String& filePath)
		: Document(window, nullptr), m_pictureBox(0), m_filePath(filePath), m_texture(0), m_scale(0), m_name(name)
	{
		m_pictureBox = new PictureBox(Point(5,5 + 17), 1);
		m_pictureBox->SetSkin(window->getUISkin());
		m_pictureBox->eventPictureDraw().Bind(this, &TextureViewer::PixtureBox_Draw);
		m_btnZoomIn = new Button(Point(100,17+5), L"+");
		m_btnZoomIn->SetSkin(window->getUISkin());
		m_btnZoomIn->eventPress().Bind(this, &TextureViewer::BtnZoomIn_Pressed);
		m_btnZoomOut = new Button(Point(140,17+5), L"-");
		m_btnZoomOut->SetSkin(window->getUISkin());
		m_btnZoomOut->eventPress().Bind(this, &TextureViewer::BtnZoomOut_Pressed);


		getDocumentForm()->setTitle(name + String(L"(100%)"));
		getDocumentForm()->setMinimumSize(Point(300,300));
	}

	TextureViewer::~TextureViewer()
	{
		delete m_pictureBox;
		delete m_btnZoomIn;
		delete m_btnZoomOut;

		if (m_texture)
			delete m_texture;
	}

	void TextureViewer::PixtureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		{
			Texture* alphaGrid = UIResources::GetTexture(L"alphagrid");
			int x ,y;
			for (x =0;x<dstRect->Width;x+=alphaGrid->getWidth())
			{
				for (y =0;y<dstRect->Height;y+=alphaGrid->getHeight())
				{
					Apoc3D::Math::Rectangle destRect(dstRect->X+x,dstRect->Y+y,dstRect->Width-x, dstRect->Height-y);
					if (destRect.Width>alphaGrid->getWidth())
						destRect.Width = alphaGrid->getWidth();
					if (destRect.Height>alphaGrid->getHeight())
						destRect.Height = alphaGrid->getHeight();

					Apoc3D::Math::Rectangle srcRect(0,0,destRect.Width, destRect.Height);
					sprite->Draw(alphaGrid, destRect,&srcRect, CV_White);
				}
			}


		}

		if (m_texture)
		{
			
			float scale = powf(2, (float)m_scale);


			//Point newSize = m_pictureBox->Size;
			Apoc3D::Math::Rectangle dr(*dstRect);
			if (dr.Width > (int)(m_texture->getWidth()*scale))
			{
				dr.Width = (int)(m_texture->getWidth()*scale);
			}
			if (dr.Height > (int)(m_texture->getHeight()*scale))
			{
				dr.Height = (int)(m_texture->getHeight()*scale);
			}

			Apoc3D::Math::Rectangle srcRect(0,0,(int)(dr.Width/scale),(int)(dr.Height/scale));
			if (srcRect.Width > m_texture->getWidth())
				srcRect.Width = m_texture->getWidth();
			if (srcRect.Height > m_texture->getHeight())
				srcRect.Height = m_texture->getHeight();

			sprite->Draw(m_texture,dr,&srcRect,CV_White);
			switch(m_texture->getType())
			{
			case TT_Texture1D:
			case TT_Texture2D:
				{

					String msg = L"Type: 2D.\nFormat: ";
					msg.append(PixelFormatUtils::ToString(m_texture->getFormat()));
					msg.append(L"\nDimension:");
					msg.append(StringUtils::ToString(m_texture->getWidth()));
					msg.append(1,'x');
					msg.append(StringUtils::ToString(m_texture->getHeight()));
					msg.append(L"\nMip Levels:");
					msg.append(StringUtils::ToString(m_texture->getLevelCount()));	
					
					m_pictureBox->getFontRef()->DrawString(sprite, msg, Point(5+dr.X, 6+dr.Y), CV_Black);
					m_pictureBox->getFontRef()->DrawString(sprite, msg, Point(5+dr.X, 5+dr.Y), CV_White);
				}
				break;
			}

			//sprite->Flush();
			//if (restoreScissor)
			//{
			//	manager->setScissorTest(true, &oldScissorRect);
			//}
			//else
			//{
			//	manager->setScissorTest(false,0);
			//}
			//switch(m_texture->getType())
			//{
			//case TT_Texture2D:
			//	if (m_texture->getWidth() < MaxSize.X && m_texture->getHeight() < MaxSize.Y)
			//	{
			//		newSize.X = m_texture->getWidth();
			//		newSize.Y = m_texture->getHeight();		

			//	}
			//	else
			//	{
			//		if (m_texture->getWidth()>m_texture->getHeight())
			//		{
			//			newSize.X = MaxSize.X;
			//			newSize.Y = static_cast<int>(m_texture->getHeight() * MaxSize.X / (float)m_texture->getWidth());
			//		}
			//		else
			//		{
			//			newSize.X = static_cast<int>(m_texture->getWidth() * MaxSize.Y / (float)m_texture->getHeight());
			//			newSize.Y = MaxSize.Y;
			//		}
			//	}

			//	break;
			//case TT_Texture1D:
			//	if (m_texture->getWidth() >1)
			//	{
			//		newSize.X = MaxSize.X;
			//		newSize.Y = MinSize.Y;
			//	}
			//	else
			//	{
			//		newSize.X = MinSize.X;
			//		newSize.Y = MaxSize.Y;
			//	}
			//	break;
			//}
			
		}
	}
	void TextureViewer::BtnZoomIn_Pressed(Control* ctrl)
	{
		m_scale++;
		if (m_scale>8)
			m_scale = 8;

		float scale = powf(2, (float)m_scale);

		String scaleRatio = String(L" (") +StringUtils::ToString(scale*100,2,0,' ', std::ios::fixed);
		scaleRatio.append(L"%)");

		getDocumentForm()->setTitle(m_name + scaleRatio);
	}
	void TextureViewer::BtnZoomOut_Pressed(Control* ctrl)
	{
		m_scale--;
		if (m_scale<-8)
			m_scale = -8;

		float scale = powf(2, (float)m_scale);

		String scaleRatio = String(L" (") +StringUtils::ToString(scale*100,2,0,' ', std::ios::fixed);
		scaleRatio.append(L"%)");

		getDocumentForm()->setTitle(m_name + scaleRatio);
	}
	void TextureViewer::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_pictureBox);
		getDocumentForm()->getControls().Add(m_btnZoomIn);
		getDocumentForm()->getControls().Add(m_btnZoomOut);

		Document::Initialize(device);

	}

	void TextureViewer::LoadRes()
	{
		if (m_texture)
			delete m_texture;

		FileLocation* fl = new FileLocation(m_filePath);
		m_texture = TextureManager::getSingleton().CreateUnmanagedInstance(getMainWindow()->getDevice(), fl, false);
	}
	void TextureViewer::SaveRes()
	{

	}

	void TextureViewer::Update(const GameTime* const time)
	{
		m_pictureBox->Size = getDocumentForm()->Size;
		m_pictureBox->Size.X -= m_pictureBox->Position.X*2;
		m_pictureBox->Size.Y -= m_pictureBox->Position.Y*2;

		m_btnZoomIn->Position.X = m_pictureBox->Size.X-65;
		m_btnZoomOut->Position.X = m_pictureBox->Size.X-30;
		Document::Update(time);
	}
}