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
#include "MeshBuild.h"

#include "../BuildEngine.h"
#include "../BuildConfig.h"
#include "../CompileLog.h"

#include "AIImporter.h"
#include "XImporter.h"
#include "FbxImporter.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	void MeshBuild::BuildByFBX(const MeshBuildConfig& config)
	{
		FbxImporter::Import(config);

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
	void MeshBuild::BuildByASS(const MeshBuildConfig& config)
	{
		AIImporter importer;
		ModelData* data = importer.Import(config);

		ConvertVertexData(data, config);
		CollapseMeshs(data, config);

		FileOutStream* fs = new FileOutStream(config.DstFile);
		data->Save(fs);
		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
	void MeshBuild::BuildByD3D(const MeshBuildConfig& config)
	{
		XImporter importer;
		ModelData* data = importer.Import(config);

		ConvertVertexData(data, config);
		CollapseMeshs(data, config);

		FileOutStream* fs = new FileOutStream(config.DstFile);
		data->Save(fs);
		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}

	void MeshBuild::Build(const ConfigurationSection* sect)
	{
		MeshBuildConfig config;
		config.Parse(sect);
		
		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find source file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DstFile));
		EnsureDirectory(PathUtils::GetDirectory(config.DstAnimationFile));

		switch (config.Method)
		{
		case MESHBUILD_ASS:
			BuildByASS(config);
			break;
		case MESHBUILD_FBX:
			BuildByFBX(config);
			break;
		case MESHBUILD_D3D:
			BuildByD3D(config);
			break;
		}
	}

	void MeshBuild::ConvertVertexData(ModelData* data, const MeshBuildConfig& config)
	{
		if (config.UseVertexFormatConversion)
		{
			for (int i=0;i<data->Entities.getCount();i++)
			{
				FastList<VertexElement> newElements; // to new data buffer
				FastList<VertexElement> extractingElements; // from source

				MeshData* md = data->Entities[i];
				
				for (int j=0;j<md->VertexElements.getCount();j++)
				{
					const VertexElement& srcElement = md->VertexElements[j];

					for (int k=0;k<config.VertexElements.getCount();k++)
					{
						const VertexElement& targetElement = config.VertexElements[k];

						if (srcElement.getUsage()==targetElement.getUsage())
						{
							if (targetElement.getUsage() == VEU_TextureCoordinate && 
								targetElement.getIndex() == srcElement.getIndex())
							{
								extractingElements.Add(srcElement);
							}
							else if (targetElement.getUsage() != VEU_TextureCoordinate)
							{
								extractingElements.Add(srcElement);
							}
						}
					}
				}

				// this will also keep newElements is one to one to extractingElements
				int newOffset = 0;
				for (int j=0;j<extractingElements.getCount();j++)
				{
					const VertexElement& ve = extractingElements[j];
					
					VertexElement newVe(newOffset, ve.getType(), ve.getUsage(), ve.getIndex());
					newElements.Add(newVe);

					newOffset += ve.getSize();
				}
				int newVertexSize = newOffset;

				char* newVertexData = new char[newVertexSize*md->VertexCount];

				for (uint j=0;j<md->VertexCount;j++)
				{
					int dstOffset = newVertexSize * j;
					int srcOffset = md->VertexSize*j;

					for (int k=0;k<newElements.getCount();k++)
					{
						const VertexElement& vextr = extractingElements[k];
						const VertexElement& vstore = newElements[k];

						memcpy(newVertexData+dstOffset + vstore.getOffset(), 
							md->VertexData+srcOffset + vextr.getOffset(), 
							vextr.getSize());
					}
				}

				delete[] md->VertexData;
				md->VertexData = newVertexData;
				md->VertexElements = newElements;
				md->VertexSize = newVertexSize;
			}
		}
	}
	void MeshBuild::CollapseMeshs(ModelData* data, const MeshBuildConfig& config)
	{
		if (config.CollapseMeshs && data->Entities.getCount()>1)
		{
			MeshData* firstMd=data->Entities[0];

			uint totalVertexCount = 0;
			uint totalMaterialCount = 0;
			uint refVertexSize = firstMd->VertexSize;
			int totalFaceCount = 0; // for preserving container space only

			int* indexShifters = new int[data->Entities.getCount()];
			int* materialShifters = new int[data->Entities.getCount()];

			for (int i=0;i<data->Entities.getCount();i++)
			{
				MeshData* md=data->Entities[i];


				indexShifters[i] = totalVertexCount;
				materialShifters[i] = totalMaterialCount;

				totalVertexCount += md->VertexCount;
				totalMaterialCount +=  md->Materials.getMaterialCount();
				totalFaceCount += md->Faces.getCount();

				if (refVertexSize!=md->VertexSize)
				{
					CompileLog::WriteError(config.SrcFile, 
						L"The mesh collapse only works with meshes that have the same vertex format.");
					delete[] indexShifters;
					delete[] materialShifters;
					return;
				}
			}

			MeshData* newMD = new MeshData();
			newMD->VertexData = new char[totalVertexCount*firstMd->VertexSize];
			newMD->VertexCount = totalVertexCount;
			newMD->VertexElements = firstMd->VertexElements;
			newMD->VertexSize = refVertexSize;
			newMD->Faces.ResizeDiscard(totalFaceCount);
			newMD->Materials.Reserve((int32)totalMaterialCount);
			newMD->BoundingSphere.Center = Vector3Utils::Zero;
			newMD->BoundingSphere.Radius = 0;


			int vertexBufferOffset = 0;
			for (int i=0;i<data->Entities.getCount();i++)
			{
				MeshData* md=data->Entities[i];

				memcpy(newMD->VertexData+vertexBufferOffset, md->VertexData, refVertexSize*md->VertexCount);
				vertexBufferOffset += refVertexSize*md->VertexCount;

				for (int j=0;j<md->Faces.getCount();j++)
				{
					MeshFace f=md->Faces[j];

					f.IndexA += indexShifters[i];
					f.IndexB += indexShifters[i];
					f.IndexC += indexShifters[i];

					f.MaterialID += materialShifters[i];

					newMD->Faces.Add(f);
				}

				for (int32 j=0;j<md->Materials.getMaterialCount();j++)
				{
					Apoc3D::IO::MaterialData* mtrlData = md->Materials.getMaterial(j);
					newMD->Materials.Add(mtrlData);

					for (int32 k=1;k<md->Materials.getFrameCount(j);k++)
					{
						Apoc3D::IO::MaterialData* mtrlData = md->Materials.getMaterial(j,k);
						newMD->Materials.AddFrame(mtrlData,j);
					}
				}

				BoundingSphere result;
				BoundingSphere::Merge(result, newMD->BoundingSphere, md->BoundingSphere);
				newMD->BoundingSphere = result;
			}

			delete[] indexShifters;
			delete[] materialShifters;

			for (int i=0;i<data->Entities.getCount();i++)
			{
				delete data->Entities[i];
			}
			data->Entities.Clear();
			data->Entities.Add(newMD);
		}
	}

}