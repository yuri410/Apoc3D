#include "BorderBuilder.h"

#include "Config/XmlConfiguration.h"
#include "Config/ConfigurationSection.h"
#include "Collections/FastQueue.h"
#include "Graphics/PixelFormat.h"
#include "Utility/StringUtils.h"
#include "Graphics/GraphicsCommon.h"
#include "MeshBuild/XImporter.h"
#include "MeshBuild/MeshBuild.h"
#include "IOLib/Streams.h"
#include "IOLib/ModelData.h"
#include "CompileLog.h"
#include "Vfs/File.h"
#include "Vfs/PathUtils.h"
#include "BuildEngine.h"
#include "BuildConfig.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	struct FaceEdge 
	{
		int SmallerIDVertex;
		int LargerIDVertex;
	};
	struct BorderData
	{
		FaceEdge* BorderDef;
		Vector3* Vertices;
		int borderCount;
		int vertexCount;

		Vector3* FlattenVertices;
		int flattenVertexCount;
	};

	class Vector3EqualityComparer : public IEqualityComparer<Vector3>
	{
	public:
		virtual bool Equals(const Vector3& x, const Vector3& y) const
		{
			return v3x(x)==v3x(y) && v3y(x)==v3y(y) && v3z(x)==v3z(y);
		}

		virtual int64 GetHashCode(const Vector3& obj) const
		{
			float x = v3x(obj);
			float y = v3y(obj);
			float z = v3z(obj);

			return reinterpret_cast<const int64&>(x) ^ reinterpret_cast<const int64&>(y) ^ reinterpret_cast<const int64&>(z);
		}
	};

	class FaceEdgeEqualityComparer : public IEqualityComparer<FaceEdge>
	{
	public:
		virtual bool Equals(const FaceEdge& x, const FaceEdge& y) const
		{
			return x.SmallerIDVertex == y.SmallerIDVertex && x.LargerIDVertex == y.LargerIDVertex;
		}

		virtual int64 GetHashCode(const FaceEdge& obj) const
		{
			return obj.SmallerIDVertex + (obj.LargerIDVertex << 16);
		}
	};

	void BorderBuilder::Build(const ConfigurationSection* sect)
	{
		Vector3EqualityComparer vec3Comparer;
		FaceEdgeEqualityComparer faceEdgeComparer;

		String srcFile = sect->getAttribute(L"SourceFile");
		String dstFile = sect->getAttribute(L"DestinationFile");
		String outputFormat = sect->getAttribute(L"OutputFormat");
		StringUtils::ToLowerCase(outputFormat);

		if (!File::FileExists(srcFile))
		{
			CompileLog::WriteError(srcFile, L"Could not find source file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(dstFile));


		MeshBuildConfig config;
		config.CollapseMeshs = false;
		config.UseVertexFormatConversion = true;
		config.VertexElements.Add(VertexElement(0,VEF_Count,VEU_Position,0));
		config.Method = MESHBUILD_D3D;
		config.SrcFile = srcFile;
		config.DstFile = dstFile;
		

		XImporter importer;
		ModelData* data = importer.Import(config);

		MeshBuild::ConvertVertexData(data, config);

		FastList<BorderData> borderData;

		for (int i=0;i<data->Entities.getCount();i++)
		{
			MeshData* mesh = data->Entities[i];

			if (mesh->VertexCount == 0 || mesh->Faces.getCount() == 0)
				continue;

			const Vector3* vertices = reinterpret_cast<const Vector3*>(mesh->VertexData);
			assert(mesh->VertexSize == 12);

			uint totalVertexCount = mesh->VertexCount;

			FastMap<Vector3, int> vtxHashTable(totalVertexCount, &vec3Comparer);
			FastList<Vector3> newVertexList(totalVertexCount);

			// vertex reduction - welding same vertices
			for (int j=0;j<mesh->Faces.getCount();j++)
			{
				MeshFace& face = mesh->Faces[j];

				Vector3 va = vertices[face.IndexA];
				int existA;
				if (!vtxHashTable.TryGetValue(va, existA))
				{
					existA = newVertexList.getCount();
					newVertexList.Add(va);
					vtxHashTable.Add(va, existA);
				}
				face.IndexA = existA;

				Vector3 vb = vertices[face.IndexB];
				int existB;
				if (!vtxHashTable.TryGetValue(vb, existB))
				{
					existB = newVertexList.getCount();
					newVertexList.Add(vb);
					vtxHashTable.Add(vb, existB);
				}
				face.IndexB = existB;

				Vector3 vc = vertices[face.IndexC];
				int existC;
				if (!vtxHashTable.TryGetValue(vc, existC))
				{
					existC = newVertexList.getCount();
					newVertexList.Add(vc);
					vtxHashTable.Add(vc, existC);
				}
				face.IndexC = existC;
			}

			// =================== do the unique edge detection ============================== 
			FastMap<FaceEdge, int> edgeUsageCounter(&faceEdgeComparer);

			for (int j=0;j<mesh->Faces.getCount();j++)
			{
				const MeshFace& face = mesh->Faces[j];
				FaceEdge edge1 = { face.IndexA, face.IndexB };
				FaceEdge edge2 = { face.IndexB, face.IndexC };
				FaceEdge edge3 = { face.IndexA, face.IndexC };

				if (edge1.SmallerIDVertex > edge1.LargerIDVertex)
					swap(edge1.SmallerIDVertex, edge1.LargerIDVertex);
				if (edge2.SmallerIDVertex > edge2.LargerIDVertex)
					swap(edge2.SmallerIDVertex, edge2.LargerIDVertex);
				if (edge3.SmallerIDVertex > edge3.LargerIDVertex)
					swap(edge3.SmallerIDVertex, edge3.LargerIDVertex);

				int count;
				if (edgeUsageCounter.TryGetValue(edge1, count))
				{
					edgeUsageCounter[edge1] = count + 1;
				}
				else
					edgeUsageCounter.Add(edge1, 1);
				
				if (edgeUsageCounter.TryGetValue(edge2, count))
				{
					edgeUsageCounter[edge2] = count + 1;
				}
				else
					edgeUsageCounter.Add(edge2, 1);

				if (edgeUsageCounter.TryGetValue(edge3, count))
				{
					edgeUsageCounter[edge3] = count + 1;
				}
				else
					edgeUsageCounter.Add(edge3, 1);
			}

			// ================ sorting edges =============

			FastList<FaceEdge> border; // the border edges, unsorted
			// dump out from the map
			for (FastMap<FaceEdge, int>::Enumerator e = edgeUsageCounter.GetEnumerator();e.MoveNext();)
			{
				if ((*e.getCurrentValue()) == 1)
					border.Add(*e.getCurrentKey());
			}

			if (border.getCount()>0)
			{
				// a table of FaceEdge*[newVertexCount][2];
				FaceEdge*** vertexIndexToEdgeList = new FaceEdge**[newVertexList.getCount()];
				for (int j=0;j<newVertexList.getCount();j++)
				{
					vertexIndexToEdgeList[j] = new FaceEdge*[2];
					memset(vertexIndexToEdgeList[j], 0, sizeof(FaceEdge*)*2);
				}
				for (int j=0;j<border.getCount();j++)
				{
					int a = border[j].SmallerIDVertex;
					if (!vertexIndexToEdgeList[a][0])
						vertexIndexToEdgeList[a][0] = &border[j];
					else if (!vertexIndexToEdgeList[a][1])
						vertexIndexToEdgeList[a][1] = &border[j];
					else
						assert(0);

					int b = border[j].LargerIDVertex;
					if (!vertexIndexToEdgeList[b][0])
						vertexIndexToEdgeList[b][0] = &border[j];
					else if (!vertexIndexToEdgeList[b][1])
						vertexIndexToEdgeList[b][1] = &border[j];
					else
						assert(0);
				}


				// bfs pass
				FastList<Vector3> flattenVertices;

				FastList<FaceEdge*> sortedEdge;
				FastMap<void*, int> passedEdge;

				FastQueue<FaceEdge*> bfsQueue;
				bfsQueue.Enqueue(&border[0]);
				passedEdge.Add(&border[0], 0);
				bool firstPass = true;

				while (bfsQueue.getCount()>0)
				{
					FaceEdge* current = bfsQueue.Dequeue();

					sortedEdge.Add(current);
					
					bool expansionSucceed = false;
					FaceEdge** newDirections1 = vertexIndexToEdgeList[current->SmallerIDVertex];
					for (int j=0;j<2;j++)
					{
						if (newDirections1[j] != current && !passedEdge.Contains(newDirections1[j]))
						{
							passedEdge.Add(newDirections1[j], 0);
							bfsQueue.Enqueue(newDirections1[j]);
							expansionSucceed = true;

							if (firstPass)
							{
								flattenVertices.Add(newVertexList[current->LargerIDVertex]);
								firstPass = false;
							}
							
							flattenVertices.Add(newVertexList[current->SmallerIDVertex]);

							break;
						}
					}
					FaceEdge** newDirections2 = vertexIndexToEdgeList[current->LargerIDVertex];
					for (int j=0;j<2 && !expansionSucceed;j++)
					{
						if (newDirections2[j] != current && !passedEdge.Contains(newDirections2[j]))
						{
							passedEdge.Add(newDirections2[j], 0);
							bfsQueue.Enqueue(newDirections2[j]);
							expansionSucceed = true;

							if (firstPass)
							{
								flattenVertices.Add(newVertexList[current->SmallerIDVertex]);
								firstPass = false;
							}

							flattenVertices.Add(newVertexList[current->LargerIDVertex]);
							break;
						}
					}
				}

				for (int j=0;j<newVertexList.getCount();j++)
					delete[] vertexIndexToEdgeList[j];
				delete[] vertexIndexToEdgeList;



				// ===================================
				
				BorderData bd;
				bd.BorderDef = new FaceEdge[sortedEdge.getCount()];
				for (int j=0;j<sortedEdge.getCount();j++)
					bd.BorderDef[j] = *sortedEdge[j];

				bd.Vertices = new Vector3[newVertexList.getCount()];
				memcpy(bd.Vertices, &newVertexList[0], sizeof(Vector3)* newVertexList.getCount());

				bd.vertexCount = newVertexList.getCount();
				bd.borderCount = sortedEdge.getCount();

				bd.flattenVertexCount = flattenVertices.getCount();
				bd.FlattenVertices = new Vector3[flattenVertices.getCount()];
				memcpy(bd.FlattenVertices, &flattenVertices[0], sizeof(Vector3)* flattenVertices.getCount());

				borderData.Add(bd);
			}
		}

		delete data;

		if (outputFormat == L"xml")
		{
			XMLConfiguration* xml = new XMLConfiguration(L"Root");

			
			for (int i=0;i<borderData.getCount();i++)
			{
				ConfigurationSection* sect = new ConfigurationSection(L"Section" + StringUtils::ToString(i));	
				ConfigurationSection* sectV = new ConfigurationSection(L"Vertex");	
				ConfigurationSection* sectB = new ConfigurationSection(L"Border");	
				ConfigurationSection* sectFV = new ConfigurationSection(L"FlattenVertex");	

				for (int j=0;j<borderData[i].vertexCount;j++)
				{
					String value = StringUtils::ToString(v3x(borderData[i].Vertices[j]), 4, 10, ' ', ios::right | ios::fixed);
					value.append(L",");
					value.append(StringUtils::ToString(v3y(borderData[i].Vertices[j]), 4, 10, ' ', ios::right | ios::fixed));
					value.append(L",");
					value.append(StringUtils::ToString(v3z(borderData[i].Vertices[j]), 4, 10, ' ', ios::right | ios::fixed));

					sectV->SetValue(L"Vertex" + StringUtils::ToString(j, 4, '0', ios::right),
						value);
				}
					
				for (int j=0;j<borderData[i].borderCount;j++)
				{
					String value = StringUtils::ToString(borderData[i].BorderDef[j].SmallerIDVertex, 3, ' ', ios::right);
					value.append(L",");
					value.append(StringUtils::ToString(borderData[i].BorderDef[j].LargerIDVertex, 3, ' ', ios::right));
					
					sectB->SetValue(L"Border" + StringUtils::ToString(j, 4, '0', ios::right),
						value);
				}

				for (int j=0;j<borderData[i].flattenVertexCount;j++)
				{
					String value = StringUtils::ToString(v3x(borderData[i].FlattenVertices[j]), 4, 10, ' ', ios::right | ios::fixed);
					value.append(L",");
					value.append(StringUtils::ToString(v3y(borderData[i].FlattenVertices[j]), 4, 10, ' ', ios::right | ios::fixed));
					value.append(L",");
					value.append(StringUtils::ToString(v3z(borderData[i].FlattenVertices[j]), 4, 10, ' ', ios::right | ios::fixed));

					sectFV->SetValue(L"Vertex" + StringUtils::ToString(j, 4, '0', ios::right),
						value);
				}

				delete[] borderData[i].Vertices;
				delete[] borderData[i].BorderDef;
				delete[] borderData[i].FlattenVertices;

				sect->AddSection(sectB);
				sect->AddSection(sectV);
				sect->AddSection(sectFV);
				xml->Add(sect);
			}

			xml->Save(dstFile);

			delete xml;
			
		}

	}
}