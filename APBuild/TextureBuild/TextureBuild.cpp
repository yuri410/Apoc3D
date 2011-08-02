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
#include "TextureBuild.h"
#include "Config/ConfigurationSection.h"

#include "IOLib/TextureData.h"
#include "IOLib/Streams.h"
#include "Vfs/File.h"
#include "Apoc3DException.h"
#include "CompileLog.h"
#include "D3DHelper.h"
#include "Graphics/LockData.h"
#include "dds.h"

#include <IL/il.h>
#include <IL/ilu.h>

using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

#ifndef ReleasePpo
#define ReleasePpo(ppo) \
	if (*(ppo) != NULL) \
{ \
	(*(ppo))->Release(); \
	*(ppo) = NULL; \
} \
		else (VOID)0
#endif

namespace APBuild
{

	class DXTex
	{
	private:
		String m_sourceFile;
		LPDIRECT3DBASETEXTURE9 m_ptexOrig;
		LPDIRECT3DBASETEXTURE9 m_ptexNew;
		DWORD m_dwWidth;
		DWORD m_dwHeight;
		DWORD m_dwDepth; // For volume textures
		DWORD m_numMips;
		DWORD m_dwCubeMapFlags;

		bool m_isOnError;

		void Error(const String& msg, const String& src)
		{
			CompileLog::WriteError(msg, src);
			m_isOnError = true;
		}



		D3DFORMAT GetFormat(LPDIRECT3DBASETEXTURE9 ptex)
		{
			LPDIRECT3DTEXTURE9 pmiptex = NULL;
			LPDIRECT3DCUBETEXTURE9 pcubetex = NULL;
			LPDIRECT3DVOLUMETEXTURE9 pvoltex = NULL;
			D3DFORMAT fmt = D3DFMT_UNKNOWN;

			if (IsVolumeMap())
				pvoltex = (LPDIRECT3DVOLUMETEXTURE9)ptex;
			else if (IsCubeMap())
				pcubetex = (LPDIRECT3DCUBETEXTURE9)ptex;
			else
				pmiptex = (LPDIRECT3DTEXTURE9)ptex;

			if (pvoltex != NULL)
			{
				D3DVOLUME_DESC vd;
				pvoltex->GetLevelDesc(0, &vd);
				fmt = vd.Format;
			}
			else if (pcubetex != NULL)
			{
				D3DSURFACE_DESC sd;
				pcubetex->GetLevelDesc(0, &sd);
				fmt = sd.Format;
			}
			else if( pmiptex != NULL )
			{
				D3DSURFACE_DESC sd;
				pmiptex->GetLevelDesc(0, &sd);
				fmt = sd.Format;
			}
			return fmt;
		}
		HRESULT BltAllLevels(D3DCUBEMAP_FACES FaceType, 
			LPDIRECT3DBASETEXTURE9 ptexSrc, LPDIRECT3DBASETEXTURE9 ptexDest)
		{
			HRESULT hr;
			LPDIRECT3DTEXTURE9 pmiptexSrc;
			LPDIRECT3DTEXTURE9 pmiptexDest;
			LPDIRECT3DCUBETEXTURE9 pcubetexSrc;
			LPDIRECT3DCUBETEXTURE9 pcubetexDest;
			LPDIRECT3DVOLUMETEXTURE9 pvoltexSrc;
			LPDIRECT3DVOLUMETEXTURE9 pvoltexDest;
			DWORD iLevel;

			if (IsVolumeMap())
			{
				pvoltexSrc = (LPDIRECT3DVOLUMETEXTURE9)ptexSrc;
				pvoltexDest = (LPDIRECT3DVOLUMETEXTURE9)ptexDest;
			}
			else if (IsCubeMap())
			{
				pcubetexSrc = (LPDIRECT3DCUBETEXTURE9)ptexSrc;
				pcubetexDest = (LPDIRECT3DCUBETEXTURE9)ptexDest;
			}
			else
			{
				pmiptexSrc = (LPDIRECT3DTEXTURE9)ptexSrc;
				pmiptexDest = (LPDIRECT3DTEXTURE9)ptexDest;
			}

			for (iLevel = 0; iLevel < m_numMips; iLevel++)
			{
				if (IsVolumeMap())
				{
					LPDIRECT3DVOLUME9 pvolSrc = NULL;
					LPDIRECT3DVOLUME9 pvolDest = NULL;
					hr = pvoltexSrc->GetVolumeLevel(iLevel, &pvolSrc);
					hr = pvoltexDest->GetVolumeLevel(iLevel, &pvolDest);
					hr = D3DXLoadVolumeFromVolume(pvolDest, NULL, NULL, 
						pvolSrc, NULL, NULL, D3DX_DEFAULT, 0);
					ReleasePpo(&pvolSrc);
					ReleasePpo(&pvolDest);
				}
				else if (IsCubeMap())
				{
					LPDIRECT3DSURFACE9 psurfSrc = NULL;
					LPDIRECT3DSURFACE9 psurfDest = NULL;
					hr = pcubetexSrc->GetCubeMapSurface(FaceType, iLevel, &psurfSrc);
					hr = pcubetexDest->GetCubeMapSurface(FaceType, iLevel, &psurfDest);
					hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, 
						psurfSrc, NULL, NULL, D3DX_DEFAULT, 0);
					ReleasePpo(&psurfSrc);
					ReleasePpo(&psurfDest);
				}
				else
				{
					LPDIRECT3DSURFACE9 psurfSrc = NULL;
					LPDIRECT3DSURFACE9 psurfDest = NULL;
					hr = pmiptexSrc->GetSurfaceLevel(iLevel, &psurfSrc);
					hr = pmiptexDest->GetSurfaceLevel(iLevel, &psurfDest);
					hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, 
						psurfSrc, NULL, NULL, D3DX_DEFAULT, 0);
					ReleasePpo(&psurfSrc);
					ReleasePpo(&psurfDest);
				}
			}

			return S_OK;
		}
		// If *pptex's current format has less than 4 bits of alpha, change
		// it to a similar format that has at least 4 bits of alpha.
		HRESULT EnsureAlpha(LPDIRECT3DBASETEXTURE9* pptex)
		{
			HRESULT hr;
			D3DFORMAT fmtCur = GetFormat(*pptex);
			D3DFORMAT fmtNew = D3DFMT_UNKNOWN;
			LPDIRECT3DBASETEXTURE9 ptex = NULL;

			switch (fmtCur)
			{
			case D3DFMT_X8R8G8B8:
			case D3DFMT_R8G8B8:
				fmtNew = D3DFMT_A8R8G8B8;
				break;

			case D3DFMT_X1R5G5B5:
			case D3DFMT_R5G6B5:
				fmtNew = D3DFMT_A1R5G5B5;
				break;

			case D3DFMT_X8B8G8R8:
				fmtNew = D3DFMT_A8B8G8R8;
				break;

			case D3DFMT_L8:
				fmtNew = D3DFMT_A8L8;
				break;

			default:
				break;
			}

			if( fmtNew != D3DFMT_UNKNOWN )
			{
				if (FAILED(hr = ChangeFormat(m_ptexOrig, fmtNew, &ptex)))
					return hr;
				ReleasePpo(&m_ptexOrig);
				m_ptexOrig = ptex;
			}

			return S_OK;
		}
		static HRESULT LoadAlphaIntoSurface(const String& strPath, LPDIRECT3DSURFACE9 psurf)
		{
			HRESULT hr;
			D3DSURFACE_DESC sd;
			LPDIRECT3DDEVICE9 pd3ddev = D3DHelper::getDevice();
			LPDIRECT3DTEXTURE9 ptexAlpha;
			LPDIRECT3DSURFACE9 psurfAlpha;
			LPDIRECT3DSURFACE9 psurfTarget;

			psurf->GetDesc(&sd);

			// Load the alpha BMP into psurfAlpha, a new A8R8G8B8 surface
			hr = D3DXCreateTextureFromFileEx(pd3ddev, strPath.c_str(), sd.Width, sd.Height, 1, 0, 
				D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, 
				D3DX_DEFAULT, 0, NULL, NULL, &ptexAlpha);
			hr = ptexAlpha->GetSurfaceLevel(0, &psurfAlpha);

			// Copy the target surface into an A8R8G8B8 surface
			hr = pd3ddev->CreateOffscreenPlainSurface(sd.Width, sd.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &psurfTarget, NULL);
			hr = D3DXLoadSurfaceFromSurface(psurfTarget, NULL, NULL, psurf, NULL, NULL, 
				D3DX_DEFAULT, 0);

			// Fill in the alpha channels of psurfTarget based on the blue channel of psurfAlpha
			D3DLOCKED_RECT lrSrc;
			D3DLOCKED_RECT lrDest;

			hr = psurfAlpha->LockRect(&lrSrc, NULL, D3DLOCK_READONLY);
			hr = psurfTarget->LockRect(&lrDest, NULL, 0);

			DWORD xp;
			DWORD yp;
			DWORD* pdwRowSrc = (DWORD*)lrSrc.pBits;
			DWORD* pdwRowDest = (DWORD*)lrDest.pBits;
			DWORD* pdwSrc;
			DWORD* pdwDest;
			DWORD dwAlpha;
			LONG dataBytesPerRow = 4 * sd.Width;

			for (yp = 0; yp < sd.Height; yp++)
			{
				pdwSrc = pdwRowSrc;
				pdwDest = pdwRowDest;
				for (xp = 0; xp < sd.Width; xp++)
				{
					dwAlpha = *pdwSrc << 24;
					*pdwDest &= 0x00ffffff;
					*pdwDest |= dwAlpha;

					pdwSrc++;
					pdwDest++;
				}
				pdwRowSrc += lrSrc.Pitch / 4;
				pdwRowDest += lrDest.Pitch / 4;
			}

			psurfAlpha->UnlockRect();
			psurfTarget->UnlockRect();

			// Copy psurfTarget back into real surface
			hr = D3DXLoadSurfaceFromSurface(psurf, NULL, NULL, psurfTarget, NULL, NULL, 
				D3DX_DEFAULT, 0);

			// Release allocated interfaces
			ReleasePpo(&psurfTarget);
			ReleasePpo(&psurfAlpha);
			ReleasePpo(&ptexAlpha);

			return S_OK;
		}
	public:
		bool isError() const { return m_isOnError; }
		bool IsCubeMap() const { return !!m_dwCubeMapFlags; }
		bool IsVolumeMap() const { return !!m_dwDepth; }

		DXTex(const String& filePath)
			: m_isOnError(false), m_sourceFile(filePath)
		{
			LPDIRECT3DDEVICE9 pd3ddev = D3DHelper::getDevice();
			D3DXIMAGE_INFO imageInfo;
			D3DXIMAGE_INFO imageInfo2;

			HRESULT hr = D3DXGetImageInfoFromFile(filePath.c_str(), &imageInfo);
			if (FAILED(hr))
			{
				Error(L"Cannot not load file.", filePath);
				return;
			}

			switch (imageInfo.ResourceType)
			{
			case D3DRTYPE_TEXTURE:
				{
					hr = D3DXCreateTextureFromFileEx(D3DHelper::getDevice(), filePath.c_str(),
						imageInfo.Width, imageInfo.Height, imageInfo.MipLevels,0,
						imageInfo.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0,
						&imageInfo2, NULL, (LPDIRECT3DTEXTURE9*)&m_ptexOrig);
					if (FAILED(hr))
					{
						Error(L"Cannot not load file.", filePath);
						return;
					}
					m_dwWidth = imageInfo2.Width;
					m_dwHeight = imageInfo2.Height;
					m_dwDepth = 0;
					m_numMips = imageInfo2.MipLevels;
				}
				break;
			case D3DRTYPE_VOLUMETEXTURE:
				{
					hr = D3DXCreateVolumeTextureFromFileEx(D3DHelper::getDevice(), filePath.c_str(),
						imageInfo.Width, imageInfo.Height,imageInfo.Depth,imageInfo.MipLevels,
						0, imageInfo.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
						0,&imageInfo2, 0, (LPDIRECT3DVOLUMETEXTURE9*)&m_ptexOrig);
					if (FAILED(hr))
					{
						Error(L"Cannot not load file.", filePath);
						return;
					}
					m_dwWidth = imageInfo2.Width;
					m_dwHeight = imageInfo2.Height;
					m_dwDepth = imageInfo2.Depth;
					m_numMips = imageInfo2.MipLevels;
				}
				break;
			case D3DRTYPE_CUBETEXTURE:
				{
					hr = D3DXCreateCubeTextureFromFileEx(D3DHelper::getDevice(), filePath.c_str(),
						imageInfo.Width, imageInfo.MipLevels, 0,imageInfo.Format,
						D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
						0, &imageInfo2, NULL, (LPDIRECT3DCUBETEXTURE9*)&m_ptexOrig);
					if (FAILED(hr))
					{
						Error(L"Cannot not load file.", filePath);
						return;
					}
					m_dwWidth = imageInfo2.Width;
					m_dwHeight = imageInfo2.Height;
					m_dwDepth = 0;
					m_numMips = imageInfo2.MipLevels;
					m_dwCubeMapFlags = DDS_CUBEMAP_ALLFACES;
				}
				break;
			default:
				Error(L"Cannot not load file.", filePath);
				return;
			}
			

		}

		void LoadAlpha(const String& filePath)
		{
			HRESULT hr;
			LPDIRECT3DTEXTURE9 pmiptex;
			LPDIRECT3DSURFACE9 psurf;

			if (IsCubeMap())
			{
				Error(L"Cubemap is not supported for loading alpha.", filePath);
				return;
			}
			pmiptex = (LPDIRECT3DTEXTURE9)m_ptexOrig;
			hr = pmiptex->GetSurfaceLevel(0, &psurf);
			if (FAILED(hr))
			{
				Error(L"TEXTURE::GetSurfaceLevel. Unexpected error.", filePath);
				return;
			}

			hr = LoadAlphaIntoSurface(filePath, psurf);
			ReleasePpo(&psurf);
			if (FAILED(hr))
			{
				Error(L"Unexpected error.", filePath);
				return;
			}

		}
		void GenerateMipMaps()
		{
			LONG lwTempH;
			LONG lwTempW;
			LONG lwPowsW;
			LONG lwPowsH;
			LPDIRECT3DTEXTURE9 pddsNew = NULL;
			D3DFORMAT fmt;
			HRESULT hr;
			LPDIRECT3DDEVICE9 pd3ddev = D3DHelper::getDevice();
			LPDIRECT3DTEXTURE9 pmiptex = NULL;
			LPDIRECT3DCUBETEXTURE9 pcubetex = NULL;
			LPDIRECT3DVOLUMETEXTURE9 pvoltex = NULL;
			LPDIRECT3DTEXTURE9 pmiptexNew = NULL;
			LPDIRECT3DCUBETEXTURE9 pcubetexNew = NULL;
			LPDIRECT3DVOLUMETEXTURE9 pvoltexNew = NULL;
			LPDIRECT3DSURFACE9 psurfSrc;
			LPDIRECT3DSURFACE9 psurfDest;
			LPDIRECT3DVOLUME9 pvolSrc;
			LPDIRECT3DVOLUME9 pvolDest;

			if (IsVolumeMap())
				pvoltex = (LPDIRECT3DVOLUMETEXTURE9)m_ptexOrig;
			else if (IsCubeMap())
				pcubetex = (LPDIRECT3DCUBETEXTURE9)m_ptexOrig;
			else
				pmiptex = (LPDIRECT3DTEXTURE9)m_ptexOrig;

			if (pvoltex != NULL)
			{
				D3DVOLUME_DESC vd;
				pvoltex->GetLevelDesc(0, &vd);
				fmt = vd.Format;
			}
			else if (pcubetex != NULL)
			{
				D3DSURFACE_DESC sd;
				pcubetex->GetLevelDesc(0, &sd);
				fmt = sd.Format;
			}
			else
			{
				D3DSURFACE_DESC sd;
				pmiptex->GetLevelDesc(0, &sd);
				fmt = sd.Format;
			}

			lwTempW = m_dwWidth;
			lwTempH = m_dwHeight;
			lwPowsW = 0;
			lwPowsH = 0;
			while (lwTempW > 0)
			{
				lwPowsW++;
				lwTempW = lwTempW / 2;
			}
			while (lwTempH > 0)
			{
				lwPowsH++;
				lwTempH = lwTempH / 2;
			}
			m_numMips = lwPowsW > lwPowsH ? lwPowsW : lwPowsH;

			// Create destination mipmap surface - same format as source
			if (pvoltex != NULL)
			{
				if (FAILED(hr = pd3ddev->CreateVolumeTexture(m_dwWidth, m_dwHeight, m_dwDepth, 
					m_numMips, 0, fmt, D3DPOOL_SYSTEMMEM, &pvoltexNew, NULL)))
				{
					goto LFail;
				}
				hr = pvoltex->GetVolumeLevel(0, &pvolSrc);
				hr = pvoltexNew->GetVolumeLevel(0, &pvolDest);
				hr = D3DXLoadVolumeFromVolume(pvolDest, NULL, NULL, pvolSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&pvolSrc);
				ReleasePpo(&pvolDest);
				hr = D3DXFilterVolumeTexture(pvoltexNew, NULL, 0, D3DX_DEFAULT);
			}
			else if (pmiptex != NULL)
			{
				if (FAILED(hr = pd3ddev->CreateTexture(m_dwWidth, m_dwHeight, m_numMips, 
					0, fmt, D3DPOOL_MANAGED, &pmiptexNew, NULL)))
				{
					goto LFail;
				}
				hr = pmiptex->GetSurfaceLevel(0, &psurfSrc);
				hr = pmiptexNew->GetSurfaceLevel(0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = D3DXFilterTexture(pmiptexNew, NULL, 0, D3DX_DEFAULT);
			}
			else
			{
				if (FAILED(hr = pd3ddev->CreateCubeTexture(m_dwWidth, m_numMips, 
					0, fmt, D3DPOOL_MANAGED, &pcubetexNew, NULL)))
				{
					goto LFail;
				}
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_X, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_X, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Y, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Y, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Y, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Y, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = pcubetex->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &psurfSrc);
				hr = pcubetexNew->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &psurfDest);
				hr = D3DXLoadSurfaceFromSurface(psurfDest, NULL, NULL, psurfSrc, NULL, NULL, 
					D3DX_DEFAULT, 0);
				ReleasePpo(&psurfSrc);
				ReleasePpo(&psurfDest);
				hr = D3DXFilterCubeTexture(pcubetexNew, NULL, 0, D3DX_DEFAULT);
			}

			ReleasePpo(&m_ptexOrig);
			if (pvoltexNew != NULL)
				m_ptexOrig = pvoltexNew;
			else if (pcubetexNew != NULL)
				m_ptexOrig = pcubetexNew;
			else
				m_ptexOrig = pmiptexNew;

			if (m_ptexNew != NULL)
			{
				// Rather than filtering down the (probably-compressed) m_ptexNew 
				// top level, compress each mip level from the (probably-uncompressed)
				// m_ptexOrig levels.
				if (pvoltexNew != NULL)
				{
					D3DVOLUME_DESC vd;
					((LPDIRECT3DVOLUMETEXTURE9)m_ptexNew)->GetLevelDesc(0, &vd);
					fmt = vd.Format;
				}
				else if (pcubetexNew != NULL)
				{
					D3DSURFACE_DESC sd;
					((LPDIRECT3DTEXTURE9)m_ptexNew)->GetLevelDesc(0, &sd);
					fmt = sd.Format;
				}
				else
				{
					D3DSURFACE_DESC sd;
					((LPDIRECT3DCUBETEXTURE9)m_ptexNew)->GetLevelDesc(0, &sd);
					fmt = sd.Format;
				}
				Compress(fmt, FALSE);
			}

			return;

LFail:
			ReleasePpo(&pddsNew);
		}

		void ChangeFormat(LPDIRECT3DBASETEXTURE9 ptexCur, D3DFORMAT fmtTo, 
			LPDIRECT3DBASETEXTURE9* pptexNew)
		{
			HRESULT hr;
			LPDIRECT3DDEVICE9 pd3ddev = D3DHelper::getDevice();
			LPDIRECT3DTEXTURE9 pmiptex;
			LPDIRECT3DCUBETEXTURE9 pcubetex;
			LPDIRECT3DVOLUMETEXTURE9 pvoltex;
			D3DFORMAT fmtFrom;
			LPDIRECT3DTEXTURE9 pmiptexNew;
			LPDIRECT3DCUBETEXTURE9 pcubetexNew;
			LPDIRECT3DVOLUMETEXTURE9 pvoltexNew;

			if (IsVolumeMap())
			{
				pvoltex = (LPDIRECT3DVOLUMETEXTURE9)ptexCur;
				D3DVOLUME_DESC vd;
				pvoltex->GetLevelDesc(0, &vd);
				fmtFrom = vd.Format;
			}
			else if (IsCubeMap())
			{
				pcubetex = (LPDIRECT3DCUBETEXTURE9)ptexCur;
				D3DSURFACE_DESC sd;
				pcubetex->GetLevelDesc(0, &sd);
				fmtFrom = sd.Format;
			}
			else
			{
				pmiptex = (LPDIRECT3DTEXTURE9)ptexCur;
				D3DSURFACE_DESC sd;
				pmiptex->GetLevelDesc(0, &sd);
				fmtFrom = sd.Format;
			}

			if (fmtFrom == D3DFMT_DXT2 || fmtFrom == D3DFMT_DXT4)
			{
				if (fmtTo == D3DFMT_DXT1)
				{
					CompileLog::WriteWarning(
						L"The source image contains premultiplied alpha, " + 
						L"and the RGB values will be copied to the destination without \"unpremultiplying\" them" + 
						L" so the resulting colors may be affected.", m_sourceFile);
					//AfxMessageBox(ID_ERROR_PREMULTTODXT1);
				}
				else if (fmtTo != D3DFMT_DXT2 && fmtTo != D3DFMT_DXT4)
				{
					Error(L"The conversion is impossible. The source image uses premultiplied alpha.",
						m_sourceFile);
					//AfxMessageBox(ID_ERROR_PREMULTALPHA);
					return;
				}
			}

			if (IsVolumeMap())
			{
				hr = pd3ddev->CreateVolumeTexture(m_dwWidth, m_dwHeight, m_dwDepth, m_numMips,
					0, fmtTo, D3DPOOL_SYSTEMMEM, &pvoltexNew, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create volume texture.",
						m_sourceFile);
					return;
				}
				*pptexNew = pvoltexNew;
				if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
			}
			else if (IsCubeMap())
			{
				hr = pd3ddev->CreateCubeTexture(m_dwWidth, m_numMips, 
					0, fmtTo, D3DPOOL_MANAGED, &pcubetexNew, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create cube texture.",
						m_sourceFile);
					return;
				}
				*pptexNew = pcubetexNew;
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_X, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_X, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_Y, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_Y, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_Z, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_Z, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
			}
			else
			{
				if ((fmtTo == D3DFMT_DXT1 || fmtTo == D3DFMT_DXT2 ||
					fmtTo == D3DFMT_DXT3 || fmtTo == D3DFMT_DXT4 ||
					fmtTo == D3DFMT_DXT5) && (m_dwWidth % 4 != 0 || m_dwHeight % 4 != 0))
				{
					Error(L"This operation requires the source textures to have dimensions that are multiples of 4.",
						m_sourceFile);
					return;
				}

				hr = pd3ddev->CreateTexture(m_dwWidth, m_dwHeight, m_numMips, 
					0, fmtTo, D3DPOOL_MANAGED, &pmiptexNew, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create texture.",
						m_sourceFile);
					return;
				}
				*pptexNew = pmiptexNew;
				if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
			}
		}
		void Compress(D3DFORMAT fmtTo, BOOL bSwitchView)
		{
			HRESULT hr;
			LPDIRECT3DBASETEXTURE9 ptexNew = NULL;
			ChangeFormat(m_ptexOrig, fmtTo, &ptexNew);
			if (!isError())
			{
				ReleasePpo(&m_ptexNew);
				m_ptexNew = ptexNew;
			}
		}
		void Resize(DWORD dwWidthNew, DWORD dwHeightNew)
		{
			HRESULT hr;
			LPDIRECT3DTEXTURE9 pmiptexNew;
			LPDIRECT3DDEVICE9 pd3ddev = D3DHelper::getDevice();

			hr = pd3ddev->CreateTexture(dwWidthNew, dwHeightNew, m_numMips, 
				0, GetFormat(m_ptexOrig), D3DPOOL_MANAGED, &pmiptexNew, NULL);
			if (FAILED(hr))
			{
				Error(L"Unexpected error: cannot create texture.",
					m_sourceFile);
				return;
			}
			if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, m_ptexOrig, pmiptexNew)))
			{
				Error(L"Unexpected error.", m_sourceFile);
				return;
			}
			ReleasePpo(&m_ptexOrig);
			m_ptexOrig = pmiptexNew;

			if( m_ptexNew != NULL )
			{
				hr = pd3ddev->CreateTexture(dwWidthNew, dwHeightNew, m_numMips, 
					0, GetFormat(m_ptexOrig), D3DPOOL_MANAGED, &pmiptexNew, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create texture.",
						m_sourceFile);
					return;
				}
				if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, m_ptexNew, pmiptexNew)))
				{
					Error(L"Unexpected error.", m_sourceFile);
					return;
				}
				ReleasePpo(&m_ptexNew);
				m_ptexNew = pmiptexNew;
			}

			m_dwWidth = dwWidthNew;
			m_dwHeight = dwHeightNew;

		}
	};


	PixelFormat ConvertBackPixelFormat(DWORD fmt)
	{
		switch (fmt)
		{
		case D3DFMT_A2R10G10B10:
			return FMT_A2R10G10B10;
		case D3DFMT_A8R8G8B8:
			return FMT_A8R8G8B8;
		case D3DFMT_X8R8G8B8:
			return FMT_X8R8G8B8;
		case D3DFMT_A1R5G5B5:
			return FMT_A1R5G5B5;
		case D3DFMT_X1R5G5B5:
			return FMT_X1R5G5B5;
		case D3DFMT_R5G6B5:
			return FMT_R5G6B5;

		case D3DFMT_DXT1:
			return FMT_DXT1;
		case D3DFMT_DXT2:
			return FMT_DXT2;
		case D3DFMT_DXT3:
			return FMT_DXT3;
		case D3DFMT_DXT4:
			return FMT_DXT4;
		case D3DFMT_DXT5:
			return FMT_DXT5;

		case D3DFMT_R16F:
			return FMT_R16F;
		case D3DFMT_G16R16F:
			return FMT_G16R16F;
		case D3DFMT_A16B16G16R16F:
			return FMT_A16B16G16R16F;

		case D3DFMT_R32F:
			return FMT_R32F;
		case D3DFMT_G32R32F:
			return FMT_G32R32F;
		case D3DFMT_A32B32G32R32F:
			return FMT_A32B32G32R32F;

		case D3DFMT_R8G8B8:
			return FMT_R8G8B8;
		case D3DFMT_A4R4G4B4:
			return FMT_A4R4G4B4;
		case D3DFMT_R3G3B2:
			return FMT_R3G3B2;
		case D3DFMT_A8:
			return FMT_Alpha8;
		case D3DFMT_A2B10G10R10:
			return FMT_A2B10G10R10;
		case D3DFMT_G16R16:
			return FMT_G16R16;
		case D3DFMT_A16B16G16R16:
			return FMT_A16B16G16R16;
		case D3DFMT_A8P8:
			return FMT_Palette8Alpha8;
		case D3DFMT_P8:
			return FMT_Palette8;
		case D3DFMT_L8:
			return FMT_Luminance8;
		case D3DFMT_L16:
			return FMT_Luminance16;
		case D3DFMT_A8L8:
			return FMT_A8L8;
		case D3DFMT_A4L4:
			return FMT_A4L4;
		case D3DFMT_A1:
			return FMT_Alpha1;

		case D3DFMT_X4R4G4B4:
		case D3DFMT_A8R3G3B2:
			return FMT_Unknown;
		}
		throw Apoc3DException::createException(EX_NotSupported, L"");
	}

	void TextureBuild::BuildByD3D(const TextureBuildConfig& config)
	{
		//DWORD usage = 0;
		//if (config.GenerateMipmaps)
		//{
		//	usage |= D3DUSAGE_AUTOGENMIPMAP;
		//}

		//IDirect3DTexture9* texture;
		//
		//if (config.Resize)
		//{
		//	if (config.GenerateMipmaps)
		//	{
		//		D3DXCreateTextureFromFileEx(D3DHelper::getDevice(),
		//			config.SourceFile.c_str(),
		//			config.NewWidth,
		//			config.NewHeight,
		//			D3DX_DEFAULT,
		//			usage,
		//			D3DFMT_FROM_FILE,
		//			D3DPOOL_SCRATCH,
		//			D3DX_FILTER_TRIANGLE,
		//			D3DX_FILTER_BOX, 0, NULL, NULL, &texture);
		//	}
		//	else
		//	{
		//		D3DXCreateTextureFromFileEx(D3DHelper::getDevice(),
		//			config.SourceFile.c_str(),
		//			config.NewWidth,
		//			config.NewHeight,
		//			D3DX_FROM_FILE,
		//			usage,
		//			D3DFMT_FROM_FILE,
		//			D3DPOOL_SCRATCH,
		//			D3DX_FILTER_TRIANGLE,
		//			D3DX_FILTER_BOX, 0, NULL, NULL, &texture);
		//	}
		//}
		//else
		//{
		//	if (config.GenerateMipmaps)
		//	{
		//		D3DXCreateTextureFromFileEx(D3DHelper::getDevice(),
		//			config.SourceFile.c_str(),
		//			D3DX_DEFAULT_NONPOW2,
		//			D3DX_DEFAULT_NONPOW2,
		//			D3DX_DEFAULT,
		//			usage,
		//			D3DFMT_FROM_FILE,
		//			D3DPOOL_SCRATCH,
		//			D3DX_FILTER_TRIANGLE,
		//			D3DX_FILTER_BOX, 0, NULL, NULL, &texture);
		//	}
		//	else
		//	{
		//		D3DXCreateTextureFromFileEx(D3DHelper::getDevice(),
		//			config.SourceFile.c_str(),
		//			D3DX_DEFAULT_NONPOW2,
		//			D3DX_DEFAULT_NONPOW2,
		//			D3DX_FROM_FILE,
		//			usage,
		//			D3DFMT_FROM_FILE,
		//			D3DPOOL_SCRATCH,
		//			D3DX_FILTER_TRIANGLE,
		//			D3DX_FILTER_BOX, 0, NULL, NULL, &texture);
		//	}
		//	
		//}

		//TextureData texData;
		//texData.LevelCount = (int32)texture->GetLevelCount();
		//texData.ContentSize = 0;

		//D3DSURFACE_DESC desc;
		//texture->GetLevelDesc(0, &desc);
		//texData.Format = ConvertBackPixelFormat(desc.Format);
		//for (int i=0;i<texData.LevelCount;i++)
		//{

		//}

	}

	PixelFormat ConvertFormat(int format, int elementType, int bpp)
	{
		switch (format)
		{
		case IL_BGR:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				if (bpp == 4)
					return FMT_X8R8G8B8;
				return FMT_R8G8B8;
			}
			break;
		case IL_BGRA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8R8G8B8;
			}
			break;
		case IL_COLOUR_INDEX:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_Palette8;
			}
			break;
		case IL_LUMINANCE:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_Luminance8;
			case IL_SHORT:
			case IL_UNSIGNED_SHORT:
				return FMT_Luminance16;
			}

			break;
		case IL_LUMINANCE_ALPHA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8L8;
			}
			break;
		case IL_RGB:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				if (bpp == 4)
					return FMT_X8B8G8R8;
				return FMT_B8G8R8;
			}
			break;
		case IL_RGBA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8B8G8R8;
			case IL_SHORT:
			case IL_UNSIGNED_SHORT:
				return FMT_A16B16G16R16;
			case IL_FLOAT:
				if (bpp == 2)
					return FMT_A16B16G16R16F;
				return FMT_A32B32G32R32F;
			}
			break;

		case IL_DXT1:
			return FMT_DXT1;
		case IL_DXT2:
			return FMT_DXT2;
		case IL_DXT3:
			return FMT_DXT3;
		case IL_DXT4:
			return FMT_DXT4;
		case IL_DXT5:
			return FMT_DXT5;
		}
		throw Apoc3DException::createException(EX_NotSupported, L"");
		//return FMT_Unknown;
	}
	int ConvertFilter(TextureFilterType flt)
	{
		switch (flt)
		{
		case TFLT_Nearest:
			return ILU_NEAREST;
		case TFLT_Box:
			return ILU_SCALE_BOX;
		case TFLT_BSpline:
			return ILU_SCALE_BSPLINE;
		}
		throw Apoc3DException::createException(EX_NotSupported, L"Not supported filter type");
	}
	void TextureBuild::BuildByDevIL(const TextureBuildConfig& config)
	{
		int image = ilGenImage();

		ilBindImage(image);
		ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);
		
		ILboolean ret = ilLoadImage(config.SourceFile.c_str());
		assert(ret);

		int ilFormat = ilGetInteger(IL_IMAGE_FORMAT);


		if (config.GenerateMipmaps)
		{
			iluBuildMipmaps();
		}
		if (config.Resize)
		{
			iluImageParameter(ILU_FILTER, ConvertFilter(config.ResizeFilterType));
			iluScale(config.NewWidth, config.NewHeight, config.NewDepth);
		}

		int mipCount = ilGetInteger(IL_NUM_MIPMAPS) + 1;

		int dataType = ilGetInteger(IL_IMAGE_TYPE);
		int bytePP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

		bool cubeFlags = ilGetInteger(IL_IMAGE_CUBEFLAGS) > 0;
		int depth0 = ilGetInteger(IL_IMAGE_DEPTH);

		TextureData texData;
		texData.LevelCount = mipCount;
		texData.ContentSize = 0;
		texData.Format = ConvertFormat(ilFormat, dataType, bytePP);

		if (cubeFlags)
		{
			texData.Type = TT_CubeTexture;
		}
		else
		{
			texData.Type = depth0 > 1 ? TT_Texture3D : TT_Texture2D;
		}

		int dxtFormat = ilGetInteger(IL_DXTC_DATA_FORMAT);
		if (dxtFormat != IL_DXT_NO_COMP)
		{
			texData.Format = ConvertFormat(dxtFormat, 0, 0);
		}

		texData.Levels.resize(mipCount);
		for (int i=0;i<mipCount;i++)
		{
			ilBindImage(image);
			ilActiveMipmap(i);

			texData.Levels[i].Width = ilGetInteger(IL_IMAGE_WIDTH);
			texData.Levels[i].Height = ilGetInteger(IL_IMAGE_HEIGHT);
			texData.Levels[i].Depth = ilGetInteger(IL_IMAGE_DEPTH);

			if (dxtFormat != IL_DXT_NO_COMP)
			{
				int numImagePasses = cubeFlags ? 6 : 1;
				int dxtSize = ilGetDXTCData(0, 0, dxtFormat);

				char* buffer = new char[numImagePasses * dxtSize];

				for (int j = 0, offset = 0; j < numImagePasses; j++, offset += dxtSize)
				{
					if (cubeFlags)
					{
						ilBindImage(image);
						ilActiveImage(j);
						ilActiveMipmap(i);
					}
					ilGetDXTCData(&buffer[offset], dxtSize, dxtFormat);
				}

				texData.Levels[i].ContentData = buffer;
				texData.Levels[i].LevelSize = numImagePasses * dxtSize;

				texData.ContentSize += texData.Levels[i].LevelSize;
			}
			else
			{
				int numImagePasses = cubeFlags ? 6 : 1;
				int imageSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

				texData.Levels[i].LevelSize = imageSize;
				char* buffer = new char[numImagePasses * imageSize];
				texData.Levels[i].ContentData = buffer;

				for (int j = 0, offset = 0; j < numImagePasses; j++, offset += imageSize)
				{
					if (cubeFlags)
					{
						ilBindImage(image);
						ilActiveImage(j);
						ilActiveMipmap(i);
					}

					//if (texData.Format == ImagePixelFormat.A8B8G8R8)
					//{
					//	fixed (byte* dst = &buffer[offset])
					//	{
					//		Il.ilCopyPixels(0, 0, 0, texData.Levels[i].Width, texData.Levels[i].Height,
					//			texData.Levels[i].Depth, Il.IL_BGRA, Il.IL_UNSIGNED_BYTE, new IntPtr(dst));
					//	}
					//}
					//else
					//{
					//IntPtr ptr = Il.ilGetData();
					//fixed (byte* dst = &buffer[offset])
					//{
					memcpy(&buffer[offset], ilGetData(), imageSize);
					//}

					//}
				}

				texData.ContentSize += imageSize;
			}

		}

		ilDeleteImage(image);


		if (config.NewFormat != FMT_Unknown &&
			texData.Format != config.NewFormat)
		{
			TextureData newdata;
			newdata.Format = config.NewFormat;
			newdata.ContentSize = 0;
			newdata.LevelCount = texData.LevelCount;
			newdata.Type = texData.Type;
			newdata.Levels.reserve(texData.LevelCount);

			
			for (int i=0;i<newdata.LevelCount;i++)
			{
				TextureLevelData& srcLvl = texData.Levels[i];

				TextureLevelData dstLvl;
				dstLvl.Depth = srcLvl.Depth;
				dstLvl.Width = srcLvl.Width;
				dstLvl.Height = srcLvl.Height;

				int lvlSize = PixelFormatUtils::GetMemorySize(
					dstLvl.Width, dstLvl.Height, dstLvl.Depth, newdata.Format);
				dstLvl.LevelSize = lvlSize;

				dstLvl.ContentData = new char[lvlSize];
				newdata.ContentSize += lvlSize;

				DataBox src = DataBox(
					srcLvl.Width, 
					srcLvl.Height, 
					srcLvl.Depth, 
					PixelFormatUtils::GetMemorySize(srcLvl.Width, 1, 1, texData.Format),
					PixelFormatUtils::GetMemorySize(srcLvl.Width, srcLvl.Height, 1, texData.Format), 
					srcLvl.ContentData,
					texData.Format);

				DataBox dst = DataBox(
					dstLvl.Width,
					dstLvl.Height, 
					dstLvl.Depth, 
					PixelFormatUtils::GetMemorySize(dstLvl.Width, 1, 1, newdata.Format),
					PixelFormatUtils::GetMemorySize(dstLvl.Width, dstLvl.Height, 1, newdata.Format), 
					dstLvl.ContentData,
					newdata.Format);

				int r = PixelFormatUtils::ConvertPixels(src, dst);
				assert(r);
				delete[] srcLvl.ContentData;

				newdata.Levels.push_back(dstLvl);
			}

			texData = newdata;
		}

		FileOutStream* fs = new FileOutStream(config.DestinationFile);
		texData.Save(fs);

		for (int i=0;i<mipCount;i++)
		{
			delete[] texData.Levels[i].ContentData;
		}
	}
	void TextureBuild::Build(const ConfigurationSection* sect)
	{
		TextureBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SourceFile))
		{
			CompileLog::WriteError(config.SourceFile, L"Can not file input file.");
			return;
		}

		if (!File::FileExists(config.DestinationFile) || 
			File::GetFileModifiyTime(config.SourceFile) >= File::GetFileModifiyTime(config.DestinationFile))
		{
			CompileLog::WriteInformation(config.SourceFile, L">");

			BuildByDevIL(config);
		}
	}
}