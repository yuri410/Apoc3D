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
#include "TextureViewer.h"

#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Math/ColorValue.h"
#include "Math/Box.h"
#include "UILib/Form.h"
#include "UILib/PictureBox.h"

namespace APDesigner
{
	static Point MaxSize(300, 300);
	static Point MinSize(64, 64);

	TextureThumbViewer::TextureThumbViewer(RenderDevice* device)
		: m_texture(0), m_device(device)
	{
		m_form = new Form(Form::FBS_None, L"Texture thumbnail");

		m_pictureBox = new PictureBox(Point::Zero, 0, 1);
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
			break;
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
}