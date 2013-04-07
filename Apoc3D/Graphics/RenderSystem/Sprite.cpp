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
#include "Sprite.h"

#include "apoc3d/Apoc3DException.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Sprite::Sprite(RenderDevice* rd)
				: m_renderDevice(rd), m_transform(Matrix::Identity), m_stack(10), 
				m_currentSettings((SpriteSettings)(SPR_ChangeState | SPR_AlphaBlended))
			{
				
			}

			Sprite::~Sprite()
			{

			}

			void Sprite::PopTransform()
			{
				if (m_currentSettings & SPR_UsePostTransformStack)
				{
					m_stack.PopMatrix();
				}
				else
				{
					throw Apoc3DException::createException(EX_InvalidOperation, L"The sprite is not begun with SPR_UsePostTransformStack.");
				}
			}
		}
	}
}