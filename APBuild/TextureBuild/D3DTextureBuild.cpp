#include "D3DTextureBuild.h"
#include "BuildSystem.h"
#include "Utils/D3DHelper.h"

#include "Apoc3D.D3D9RenderSystem/D3D9TextureUtils.h"
#include "dds.h"

#ifndef ReleasePpo
#define ReleasePpo(ppo) \
	if (*(ppo) != NULL) \
		{ \
	(*(ppo))->Release(); \
	*(ppo) = NULL; \
		} \
								else (VOID)0
#endif

using namespace Apoc3D::Graphics::D3D9RenderSystem;

namespace APBuild
{
	static D3DCUBEMAP_FACES ConvertCubemapFace(CubeMapFace face)
	{
		switch (face)
		{
			case CUBE_PositiveX: return D3DCUBEMAP_FACE_POSITIVE_X;
			case CUBE_NegativeX: return  D3DCUBEMAP_FACE_NEGATIVE_X;
			case CUBE_PositiveY: return  D3DCUBEMAP_FACE_POSITIVE_Y;
			case CUBE_NegativeY: return  D3DCUBEMAP_FACE_NEGATIVE_Y;
			case CUBE_PositiveZ: return  D3DCUBEMAP_FACE_POSITIVE_Z;
			case CUBE_NegativeZ: return  D3DCUBEMAP_FACE_NEGATIVE_Z;
		}
		return D3DCUBEMAP_FACE_POSITIVE_X;
	}

	DXTex::DXTex(const String& filePath)
		: m_name(filePath), m_numMips(0)
	{
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();
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
				hr = D3DXCreateTextureFromFileEx(Utils::D3DHelper::getDevice(), filePath.c_str(),
					imageInfo.Width, imageInfo.Height, imageInfo.MipLevels, 0,
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
				hr = D3DXCreateVolumeTextureFromFileEx(Utils::D3DHelper::getDevice(), filePath.c_str(),
					imageInfo.Width, imageInfo.Height, imageInfo.Depth, imageInfo.MipLevels,
					0, imageInfo.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
					0, &imageInfo2, 0, (LPDIRECT3DVOLUMETEXTURE9*)&m_ptexOrig);
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
				hr = D3DXCreateCubeTextureFromFileEx(Utils::D3DHelper::getDevice(), filePath.c_str(),
					imageInfo.Width, imageInfo.MipLevels, 0, imageInfo.Format,
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

	DXTex::DXTex(TextureType type, const String& name, int width, int height, int depth, PixelFormat format)
		: m_name(name), m_dwWidth(width), m_dwHeight(height)
	{
		HRESULT hr;
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();

		switch (type)
		{
			case TT_Texture1D:
			case TT_Texture2D:
			{
				LPDIRECT3DTEXTURE9 pmiptex;
				HRESULT hr = pd3ddev->CreateTexture(m_dwWidth, m_dwHeight, m_numMips,
					0, D3D9Utils::ConvertPixelFormat(format), D3DPOOL_MANAGED, &pmiptex, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create texture.", m_name);
					return;
				}
				m_ptexOrig = pmiptex;
			}
			break;
			case TT_CubeTexture:
			{
				// Cube Map
				LPDIRECT3DCUBETEXTURE9 pcubetex;
				m_dwCubeMapFlags = DDS_CUBEMAP_ALLFACES;
				hr = pd3ddev->CreateCubeTexture(m_dwWidth, m_numMips,
					0, D3D9Utils::ConvertPixelFormat(format), D3DPOOL_MANAGED, &pcubetex, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create cube texture.", m_name);
					return;
				}
				m_ptexOrig = pcubetex;
			}
			break;
			case TT_Texture3D:
			{
				LPDIRECT3DVOLUMETEXTURE9 pvoltex;
				m_dwDepth = depth;
				hr = pd3ddev->CreateVolumeTexture(m_dwWidth, m_dwHeight, m_dwDepth, m_numMips,
					0, D3D9Utils::ConvertPixelFormat(format), D3DPOOL_SYSTEMMEM, &pvoltex, NULL);
				if (FAILED(hr))
				{
					Error(L"Unexpected error: cannot create volume texture.",
						m_name);
					return;
				}
				m_ptexOrig = pvoltex;
			}

			break;
		}

	}

	DXTex::~DXTex()
	{
		ReleasePpo(&m_ptexOrig);
		ReleasePpo(&m_ptexNew);
	}


	void DXTex::LoadAlphaMap(const String& filePath)
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
	void DXTex::GenerateMipMaps()
	{
		LONG lwTempH;
		LONG lwTempW;
		LONG lwPowsW;
		LONG lwPowsH;
		LPDIRECT3DTEXTURE9 pddsNew = NULL;
		D3DFORMAT fmt;
		HRESULT hr;
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();
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
			Compress(fmt);
		}

		return;

	LFail:
		ReleasePpo(&pddsNew);
	}


	void DXTex::Compress(D3DFORMAT fmtTo)
	{
		LPDIRECT3DBASETEXTURE9 ptexNew = NULL;
		ChangeFormat(m_ptexOrig, fmtTo, &ptexNew);
		if (!isError())
		{
			ReleasePpo(&m_ptexNew);
			m_ptexNew = ptexNew;
		}
	}
	void DXTex::Resize(DWORD dwWidthNew, DWORD dwHeightNew)
	{
		HRESULT hr;
		LPDIRECT3DTEXTURE9 pmiptexNew;
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();

		hr = pd3ddev->CreateTexture(dwWidthNew, dwHeightNew, m_numMips,
			0, GetFormat(m_ptexOrig), D3DPOOL_MANAGED, &pmiptexNew, NULL);
		if (FAILED(hr))
		{
			Error(L"Unexpected error: cannot create texture.",
				m_name);
			return;
		}
		if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, m_ptexOrig, pmiptexNew)))
		{
			Error(L"Unexpected error.", m_name);
			return;
		}
		ReleasePpo(&m_ptexOrig);
		m_ptexOrig = pmiptexNew;

		if (m_ptexNew != NULL)
		{
			hr = pd3ddev->CreateTexture(dwWidthNew, dwHeightNew, m_numMips,
				0, GetFormat(m_ptexOrig), D3DPOOL_MANAGED, &pmiptexNew, NULL);
			if (FAILED(hr))
			{
				Error(L"Unexpected error: cannot create texture.",
					m_name);
				return;
			}
			if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, m_ptexNew, pmiptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			ReleasePpo(&m_ptexNew);
			m_ptexNew = pmiptexNew;
		}

		m_dwWidth = dwWidthNew;
		m_dwHeight = dwHeightNew;

	}


	void DXTex::AssembleCubeMap(const HashMap<uint, String>& maps, const HashMap<uint, String>* alphaMaps)
	{
		for (auto e : maps)
		{
			String alpha;
			bool hasAlpha = alphaMaps->TryGetValue(e.Key, alpha);

			OpenCubeFace(e.Value, hasAlpha ? &alpha : NULL, ConvertCubemapFace((CubeMapFace)e.Key));
		}
	}
	void DXTex::AssembleVolumeMap(const HashMap<uint, String>& maps, const HashMap<uint, String>* alphaMaps)
	{
		for (auto e : maps)
		{
			String alpha;
			bool hasAlpha = alphaMaps->TryGetValue(e.Key, alpha);

			OpenVolumeSlice(e.Value, hasAlpha ? &alpha : NULL, e.Key);
		}
	}

	void DXTex::Save(const String& path, TextureCompressionType cmp)
	{
		LPDIRECT3DBASETEXTURE9 ptex = (m_ptexNew == NULL ? m_ptexOrig : m_ptexNew);

		TextureData data;
		data.LevelCount = (int32)m_numMips;
		data.Flags = TextureData::TDF_None;

		D3DFORMAT fmt;
		if (IsCubeMap())
		{
			D3DSURFACE_DESC sd;
			((LPDIRECT3DCUBETEXTURE9)ptex)->GetLevelDesc(0, &sd);

			fmt = sd.Format;

			data.Type = TT_CubeTexture;
		}
		else if (IsVolumeMap())
		{
			D3DVOLUME_DESC vd;
			((LPDIRECT3DVOLUMETEXTURE9)ptex)->GetLevelDesc(0, &vd);

			fmt = vd.Format;

			data.Type = TT_Texture3D;
		}
		else
		{
			D3DSURFACE_DESC sd;
			((LPDIRECT3DTEXTURE9)ptex)->GetLevelDesc(0, &sd);
			fmt = sd.Format;

			if (m_dwWidth == 1 || m_dwHeight == 1)
			{
				data.Type = TT_Texture1D;
			}
			else
			{
				data.Type = TT_Texture2D;
			}
		}

		data.Format = D3D9Utils::ConvertBackPixelFormat(fmt);
		data.ContentSize = 0;
		switch (data.Type)
		{
			case (int)TT_Texture1D:
			case (int)TT_Texture2D:
				getData(data, (LPDIRECT3DTEXTURE9)ptex);
				break;
			case (int)TT_CubeTexture:
				getData(data, (LPDIRECT3DCUBETEXTURE9)ptex);
				break;
			case (int)TT_Texture3D:
				getData(data, (LPDIRECT3DVOLUMETEXTURE9)ptex);
				break;
		}

		if (cmp == TextureCompressionType::RLE)
			data.Flags = TextureData::TDF_RLECompressed;
		else if (cmp == TextureCompressionType::LZ4)
			data.Flags = TextureData::TDF_LZ4Compressed;
		//else if (cmp == TDCT_Auto)
		//{
		//	int32 compressedSize = 0;
		//	for (int32 i=0;i<data.Levels.getCount();i++)
		//	{
		//		compressedSize += rleEvalCompressedSize(data.Levels[i].ContentData, data.Levels[i].LevelSize);
		//	}
		//	float ratio = (float)compressedSize / data.ContentSize;
		//	if (ratio < RLECompressRatioThreshold)
		//	{
		//		data.Flags = TextureData::TDF_RLECompressed;
		//	}
		//}

		data.Save(FileOutStream(path));
	}

	void DXTex::Error(const String& msg, const String& src)
	{
		BuildSystem::LogError(msg, src);
		m_isOnError = true;
	}

	void DXTex::ChangeFormat(LPDIRECT3DBASETEXTURE9 ptexCur, D3DFORMAT fmtTo,
		LPDIRECT3DBASETEXTURE9* pptexNew)
	{
		HRESULT hr;
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();
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
				BuildSystem::LogWarning(
					L"The source image contains premultiplied alpha, and the RGB values will be copied to \
					 the destination without \"unpremultiplying\" them so the resulting colors may be affected.", m_name);
				//AfxMessageBox(ID_ERROR_PREMULTTODXT1);
			}
			else if (fmtTo != D3DFMT_DXT2 && fmtTo != D3DFMT_DXT4)
			{
				Error(L"The conversion is impossible. The source image uses premultiplied alpha.",
					m_name);
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
					m_name);
				return;
			}
			*pptexNew = pvoltexNew;
			if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
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
					m_name);
				return;
			}
			*pptexNew = pcubetexNew;
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_X, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_X, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_Y, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_Y, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_NEGATIVE_Z, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
			if (FAILED(hr = BltAllLevels(D3DCUBEMAP_FACE_POSITIVE_Z, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
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
					m_name);
				return;
			}

			hr = pd3ddev->CreateTexture(m_dwWidth, m_dwHeight, m_numMips,
				0, fmtTo, D3DPOOL_MANAGED, &pmiptexNew, NULL);
			if (FAILED(hr))
			{
				Error(L"Unexpected error: cannot create texture.",
					m_name);
				return;
			}
			*pptexNew = pmiptexNew;
			if (FAILED(BltAllLevels(D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew)))
			{
				Error(L"Unexpected error.", m_name);
				return;
			}
		}
	}

	D3DFORMAT DXTex::GetFormat(LPDIRECT3DBASETEXTURE9 ptex)
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
		else if (pmiptex != NULL)
		{
			D3DSURFACE_DESC sd;
			pmiptex->GetLevelDesc(0, &sd);
			fmt = sd.Format;
		}
		return fmt;
	}
	
	HRESULT DXTex::BltAllLevels(D3DCUBEMAP_FACES FaceType,
		LPDIRECT3DBASETEXTURE9 ptexSrc, LPDIRECT3DBASETEXTURE9 ptexDest)
	{
		HRESULT hr;
		LPDIRECT3DTEXTURE9 pmiptexSrc;
		LPDIRECT3DTEXTURE9 pmiptexDest;
		LPDIRECT3DCUBETEXTURE9 pcubetexSrc;
		LPDIRECT3DCUBETEXTURE9 pcubetexDest;
		LPDIRECT3DVOLUMETEXTURE9 pvoltexSrc;
		LPDIRECT3DVOLUMETEXTURE9 pvoltexDest;

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

		for (DWORD iLevel = 0; iLevel < m_numMips; iLevel++)
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
	HRESULT DXTex::EnsureAlpha(LPDIRECT3DBASETEXTURE9* pptex)
	{
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

		if (fmtNew != D3DFMT_UNKNOWN)
		{
			ChangeFormat(m_ptexOrig, fmtNew, &ptex);

			ReleasePpo(&m_ptexOrig);
			m_ptexOrig = ptex;
		}

		return S_OK;
	}
	
	HRESULT DXTex::LoadAlphaIntoSurface(const String& strPath, LPDIRECT3DSURFACE9 psurf)
	{
		HRESULT hr;
		D3DSURFACE_DESC sd;
		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();
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

	HRESULT DXTex::OpenCubeFace(const String& file, const String* alphaFile, D3DCUBEMAP_FACES face)
	{
		HRESULT hr;
		LPDIRECT3DSURFACE9 psurfOrig = NULL;
		LPDIRECT3DSURFACE9 psurfNew = NULL;

		if (!IsCubeMap())
		{
			return D3DERR_INVALIDCALL;
		}

		String fileName = file;
		hr = ((LPDIRECT3DCUBETEXTURE9)m_ptexOrig)->GetCubeMapSurface(face, 0, &psurfOrig);
		if (m_ptexNew != NULL)
			hr = ((LPDIRECT3DCUBETEXTURE9)m_ptexNew)->GetCubeMapSurface(face, 0, &psurfNew);

		hr = D3DXLoadSurfaceFromFile(psurfOrig, NULL, NULL, fileName.c_str(), NULL, D3DX_DEFAULT, 0, NULL);

		if (alphaFile)
		{
			if (FAILED(hr = LoadAlphaIntoSurface(*alphaFile, psurfOrig)))
			{
				return hr;
			}
		}

		if (psurfNew != NULL)
		{
			hr = D3DXLoadSurfaceFromSurface(psurfNew, NULL, NULL, psurfOrig, NULL, NULL, D3DX_DEFAULT, 0);
		}


		ReleasePpo(&psurfOrig);
		ReleasePpo(&psurfNew);
		return S_OK;
	}
	
	HRESULT DXTex::OpenVolumeSlice(LPDIRECT3DVOLUME9 pVolume, UINT iSlice, LPDIRECT3DSURFACE9 psurf)
	{
		HRESULT hr;
		D3DSURFACE_DESC sd;
		D3DVOLUME_DESC vd;
		D3DLOCKED_RECT lr;
		D3DBOX boxSrc;
		D3DBOX boxDest;

		psurf->GetDesc(&sd);
		pVolume->GetDesc(&vd);

		boxSrc.Left = 0;
		boxSrc.Right = sd.Width;
		boxSrc.Top = 0;
		boxSrc.Bottom = sd.Height;
		boxSrc.Front = 0;
		boxSrc.Back = 1;

		boxDest.Left = 0;
		boxDest.Right = vd.Width;
		boxDest.Top = 0;
		boxDest.Bottom = vd.Height;
		boxDest.Front = iSlice;
		boxDest.Back = iSlice + 1;

		hr = psurf->LockRect(&lr, NULL, 0);
		if (FAILED(hr))
			return hr;

		hr = D3DXLoadVolumeFromMemory(pVolume, NULL, &boxDest, lr.pBits, sd.Format, lr.Pitch,
			0, NULL, &boxSrc, D3DX_DEFAULT, 0);

		psurf->UnlockRect();

		return hr;
	}
	
	void DXTex::OpenVolumeSlice(const String& file, const String* alphaFile, uint slice)
	{
		HRESULT hr;

		LPDIRECT3DDEVICE9 pd3ddev = Utils::D3DHelper::getDevice();
		LPDIRECT3DTEXTURE9 ptex = NULL;
		LPDIRECT3DSURFACE9 psurfOrig = NULL;
		LPDIRECT3DSURFACE9 psurfNew = NULL;

		hr = D3DXCreateTextureFromFile(pd3ddev, file.c_str(), &ptex);
		hr = ptex->GetSurfaceLevel(0, &psurfOrig);


		if (alphaFile)
		{
			if (FAILED(hr = LoadAlphaIntoSurface(*alphaFile, psurfOrig)))
			{
				Error(L"Unexpected Error. ", m_name);
				return;
			}
		}
		LPDIRECT3DVOLUME9 pvol;
		hr = ((LPDIRECT3DVOLUMETEXTURE9)m_ptexOrig)->GetVolumeLevel(0, &pvol);
		OpenVolumeSlice(pvol, slice, psurfOrig);
		ReleasePpo(&pvol);
		if (m_ptexNew)
		{
			hr = ((LPDIRECT3DVOLUMETEXTURE9)m_ptexNew)->GetVolumeLevel(0, &pvol);
			OpenVolumeSlice(pvol, slice, psurfOrig);
			ReleasePpo(&pvol);
		}

		ReleasePpo(&psurfOrig);
		ReleasePpo(&psurfNew);
		ReleasePpo(&ptex);

	}


}