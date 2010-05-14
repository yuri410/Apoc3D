#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#include "Common.h"

class Material
{
public:
	const static int MaxTextureLayers = 10;

private:
	IDirect3DBaseTexture9* m_tex[MaxTextureLayers];
	D3DMATERIAL9 m_mtrlColor;

public:
	IDirect3DBaseTexture9* getTexture(int idx) { return m_tex[idx]; }
	void setTexture(int idx, IDirect3DBaseTexture9* value) { m_tex[idx] = value; }

	const D3DCOLORVALUE& getAmbient() { return m_mtrlColor.Ambient; }
	const D3DCOLORVALUE& getDiffuse() { return m_mtrlColor.Diffuse; }
	const D3DCOLORVALUE& getEmissive() { return m_mtrlColor.Emissive; }
	const D3DCOLORVALUE& getSpecular() { return m_mtrlColor.Specular; }
	const float getPower() { return m_mtrlColor.Power; }

	void setAmbient(const D3DCOLORVALUE& value) { m_mtrlColor.Ambient = value; }
	void setDiffuse(const D3DCOLORVALUE& value) { m_mtrlColor.Diffuse = value; }
	void setEmissive(const D3DCOLORVALUE& value) { m_mtrlColor.Emissive = value; }
	void setSpecular(const D3DCOLORVALUE& value) { m_mtrlColor.Specular = value; }
	void setPower(const float value) { m_mtrlColor.Power = value; }
	
	Material();
	~Material(void);

};

#endif