#include "MeshProcessing.h"

#include <dxsdk/d3d9.h>
#include <dxsdk/d3dx9mesh.h>

namespace APBuild
{
	namespace Utils
	{
		VertexElementFormat ConvertVEFormat(BYTE s)
		{
			switch(s)
			{
			case D3DDECLTYPE_FLOAT1:
				return VEF_Single;
			case D3DDECLTYPE_FLOAT2:
				return VEF_Vector2;
			case D3DDECLTYPE_FLOAT3:
				return VEF_Vector3;
			case D3DDECLTYPE_FLOAT4:
				return VEF_Vector4;
			case D3DDECLTYPE_D3DCOLOR:
				return VEF_Color;
			case D3DDECLTYPE_UBYTE4:
				return VEF_Byte4;
			case D3DDECLTYPE_SHORT2:
				return VEF_Short2;
			case D3DDECLTYPE_SHORT4:
				return VEF_Short4;
			case D3DDECLTYPE_UBYTE4N:
				return VEF_NormalizedByte4;
			case D3DDECLTYPE_SHORT2N:
				return VEF_NormalizedShort2;
			case D3DDECLTYPE_SHORT4N:
				return VEF_NormalizedShort4;
			case D3DDECLTYPE_UDEC3:
				return VEF_Normalized101010;
			case D3DDECLTYPE_DEC3N:
				return VEF_UInt101010;
			case D3DDECLTYPE_FLOAT16_2:
				return VEF_HalfVector2;
			case D3DDECLTYPE_FLOAT16_4:
				return VEF_HalfVector4;
			}
			AP_EXCEPTION(ErrorID::NotSupported, L"ConvertVEFormat");
			return VEF_Vector4;
			//D3DDECLTYPE_USHORT2N    = 11,
			//D3DDECLTYPE_USHORT4N    = 12,
		}
		VertexElementUsage ConvertVEUsage(BYTE s)
		{
			switch (s)
			{
			case D3DDECLUSAGE_POSITION:
				return VEU_Position;
			case D3DDECLUSAGE_BLENDWEIGHT:
				return VEU_BlendWeight;
			case D3DDECLUSAGE_BLENDINDICES:
				return VEU_BlendIndices;
			case D3DDECLUSAGE_NORMAL:
				return VEU_Normal;
			case D3DDECLUSAGE_PSIZE:
				return VEU_PointSize;
			case D3DDECLUSAGE_TEXCOORD:
				return VEU_TextureCoordinate;
			case D3DDECLUSAGE_TANGENT:
				return VEU_Tangent;
			case D3DDECLUSAGE_BINORMAL:
				return VEU_Binormal;
			case D3DDECLUSAGE_TESSFACTOR:
				return VEU_TessellateFactor;
			case D3DDECLUSAGE_POSITIONT:
				return VEU_PositionTransformed;
			case D3DDECLUSAGE_COLOR:
				return VEU_Color;
			case D3DDECLUSAGE_FOG:
				return VEU_Fog;
			case D3DDECLUSAGE_DEPTH:
				return VEU_Depth;
			case D3DDECLUSAGE_SAMPLE:
				return VEU_Sample;
			}
			AP_EXCEPTION(ErrorID::NotSupported, L"ConvertVEUsage");
			return VEU_Position;
		}

		void meshGenerateVertexElements(uint32 fvf, List<VertexElement>& elements)
		{
			D3DVERTEXELEMENT9 d3delements[MAX_FVF_DECL_SIZE];
			D3DXDeclaratorFromFVF(fvf, d3delements);

			const D3DVERTEXELEMENT9 declEnd = D3DDECL_END();
			for (int i=0;i<MAX_FVF_DECL_SIZE;i++)
			{
				const D3DVERTEXELEMENT9& ver = d3delements[i];
				if (memcmp(&ver,&declEnd,sizeof(D3DVERTEXELEMENT9)))
				{
					VertexElement e(ver.Offset, ConvertVEFormat(ver.Type), ConvertVEUsage(ver.Usage), ver.UsageIndex );
					elements.Add(e);
				}
				else break;
			}
		}
	}

}