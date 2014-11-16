#include "Apoc3D.D3D9RenderSystem/D3D9Utils.h"

namespace APBuild
{
	class DXTex
	{
	public:
		DXTex(const String& filePath);

		/** Prepare a DXTex object for assemble build.   */
		DXTex(TextureType type, const String& name, int width, int height, int depth, PixelFormat format);

		~DXTex();


		void LoadAlphaMap(const String& filePath);
		void GenerateMipMaps();


		void Compress(D3DFORMAT fmtTo);
		void Resize(DWORD dwWidthNew, DWORD dwHeightNew);


		void AssembleCubeMap(const HashMap<uint, String>& maps, const HashMap<uint, String>* alphaMaps = nullptr);
		void AssembleVolumeMap(const HashMap<uint, String>& maps, const HashMap<uint, String>* alphaMaps = nullptr);

		void Save(const String& path, TextureCompressionType cmp);


		bool isError() const { return m_isOnError; }
		bool IsCubeMap() const { return m_dwCubeMapFlags != 0; }
		bool IsVolumeMap() const { return m_dwDepth != 0; }

		DWORD getWidth() const { return m_dwWidth; }
		DWORD getHeight() const { return m_dwHeight; }

	private:
		String m_name;
		LPDIRECT3DBASETEXTURE9 m_ptexOrig = NULL;
		LPDIRECT3DBASETEXTURE9 m_ptexNew = NULL;
		DWORD m_dwWidth = 0;
		DWORD m_dwHeight = 0;
		DWORD m_dwDepth = 0; // For volume textures
		DWORD m_numMips = 1;
		DWORD m_dwCubeMapFlags = 0;

		bool m_isOnError = false;

		void Error(const String& msg, const String& src);

		void ChangeFormat(LPDIRECT3DBASETEXTURE9 ptexCur, D3DFORMAT fmtTo, LPDIRECT3DBASETEXTURE9* pptexNew);

		D3DFORMAT GetFormat(LPDIRECT3DBASETEXTURE9 ptex);

		HRESULT BltAllLevels(D3DCUBEMAP_FACES FaceType, LPDIRECT3DBASETEXTURE9 ptexSrc, LPDIRECT3DBASETEXTURE9 ptexDest);

		// If *pptex's current format has less than 4 bits of alpha, change
		// it to a similar format that has at least 4 bits of alpha.
		HRESULT EnsureAlpha(LPDIRECT3DBASETEXTURE9* pptex);
		static HRESULT LoadAlphaIntoSurface(const String& strPath, LPDIRECT3DSURFACE9 psurf);

		HRESULT OpenCubeFace(const String& file, const String* alphaFile, D3DCUBEMAP_FACES face);
		HRESULT OpenVolumeSlice(LPDIRECT3DVOLUME9 pVolume, UINT iSlice, LPDIRECT3DSURFACE9 psurf);
		void OpenVolumeSlice(const String& file, const String* alphaFile, uint slice);

	};
}