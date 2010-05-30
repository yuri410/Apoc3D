#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	class _Export Material
	{
	public:
		/* The maximum textures used in a material
		*/
		const static int MaxTextureLayers = 10;

	private:
		BaseTexture* m_tex[MaxTextureLayers];
		D3DMATERIAL9 m_mtrlColor;

	public:
		/* Gets the texture at texture layer idx
		*/
		BaseTexture* getTexture(int idx) { return m_tex[idx]; }
		/* Sets the texture at texture layer idx
		*/
		void setTexture(int idx, BaseTexture* value) { m_tex[idx] = value; }

		/* Gets the ambient component of this material
		*/
		const D3DCOLORVALUE& getAmbient() { return m_mtrlColor.Ambient; }
		/* Gets the diffuse component of this material
		*/
		const D3DCOLORVALUE& getDiffuse() { return m_mtrlColor.Diffuse; }
		/* Gets the emissive component of this material
		*/
		const D3DCOLORVALUE& getEmissive() { return m_mtrlColor.Emissive; }
		/* Gets the specular component of this material
		*/
		const D3DCOLORVALUE& getSpecular() { return m_mtrlColor.Specular; }
		/* Gets the specular shineness
		*/
		const float getPower() { return m_mtrlColor.Power; }

		void setAmbient(const D3DCOLORVALUE& value) { m_mtrlColor.Ambient = value; }
		void setDiffuse(const D3DCOLORVALUE& value) { m_mtrlColor.Diffuse = value; }
		void setEmissive(const D3DCOLORVALUE& value) { m_mtrlColor.Emissive = value; }
		void setSpecular(const D3DCOLORVALUE& value) { m_mtrlColor.Specular = value; }
		void setPower(const float value) { m_mtrlColor.Power = value; }
	
		Material();
		~Material(void);

	};
}
#endif