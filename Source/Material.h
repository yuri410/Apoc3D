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

#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	/* Defines colors, textures and effect info for a geomentry

	   Material have some Effects. Each Effect is a set of Effect Atoms. 
	   It is an indicator that tells the batching system all passes 
	   this Material supports.

	   The other parameters are auto mapped with parameters in atom effects.
	*/
	class _Export Material
	{
	public:
		/* The maximum textures can used in a material
		*/
		const static int MaxTextureLayers = 10;
		const static int MaxEffects = 4;
	private:
		Effect* m_eff[MaxEffects];
		BaseTexture* m_tex[MaxTextureLayers];
		D3DMATERIAL9 m_mtrlColor;

	public:
		/* Gets the texture at texture layer idx
		*/
		BaseTexture* getTexture(int idx) const { return m_tex[idx]; }
		/* Sets the texture at texture layer idx
		*/
		void setTexture(int idx, BaseTexture* value) { m_tex[idx] = value; }

		/* Gets the ambient component of this material
		*/
		const Color4& getAmbient() const { return m_mtrlColor.Ambient; }
		/* Gets the diffuse component of this material
		*/
		const Color4& getDiffuse() const { return m_mtrlColor.Diffuse; }
		/* Gets the emissive component of this material
		*/
		const Color4& getEmissive() const { return m_mtrlColor.Emissive; }
		/* Gets the specular component of this material
		*/
		const Color4& getSpecular() const { return m_mtrlColor.Specular; }
		/* Gets the specular shineness
		*/
		const float getPower() const { return m_mtrlColor.Power; }

		void setAmbient(const Color4& value) { m_mtrlColor.Ambient = value; }
		void setDiffuse(const Color4& value) { m_mtrlColor.Diffuse = value; }
		void setEmissive(const Color4& value) { m_mtrlColor.Emissive = value; }
		void setSpecular(const Color4& value) { m_mtrlColor.Specular = value; }
		void setPower(const float value) { m_mtrlColor.Power = value; }
	
		Material();
		~Material(void);

	};
}
#endif