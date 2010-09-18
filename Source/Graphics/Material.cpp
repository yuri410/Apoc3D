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


#include "Material.h"

namespace Apoc3D
{
	namespace Graphics
	{
		Material::Material()
			: m_passFlags(0), m_priority(0), 
			m_blendOperation(D3DBLENDOP_ADD), m_alphaBlendEnable(false), m_srcBlend(D3DBLEND_SRCALPHA), m_dstBlend(D3DBLEND_INVSRCALPHA),
			m_alphaTestEnable(false),
			m_zWriteEnable(true), m_zTestEnable(true)
		{				
			std::memset(&m_mtrlColor, 0,  sizeof(m_mtrlColor));	


		}


		Material::~Material(void)
		{

		}

		void Material::Load(istream &strm)
		{

		}
		void Material::Save(ostream &strm)
		{

		}
	}
};