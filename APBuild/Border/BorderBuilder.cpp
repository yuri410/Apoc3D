#include "BorderBuilder.h"

#include "BuildSystem.h"
#include "BuildConfig.h"
#include "MeshBuild/XImporter.h"
#include "MeshBuild/MeshBuild.h"
#include "Utils/MeshProcessing.h"

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


	struct FaceEdgeEqualityComparer
	{
		static bool Equals(const FaceEdge& x, const FaceEdge& y) 
		{
			return x.SmallerIDVertex == y.SmallerIDVertex && x.LargerIDVertex == y.LargerIDVertex;
		}

		static int64 GetHashCode(const FaceEdge& obj) 
		{
			return obj.SmallerIDVertex + (obj.LargerIDVertex << 16);
		}
	};

	void BorderBuilder::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		String srcFile = sect->getAttribute(L"SourceFile");
		String dstFile = sect->getAttribute(L"DestinationFile");
		String outputFormat = sect->getAttribute(L"OutputFormat");
		StringUtils::ToLowerCase(outputFormat);

		if (!File::FileExists(srcFile))
		{
			BuildSystem::LogError(srcFile, L"Could not find source file.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(dstFile));


		MeshBuildConfig config;
		config.CollapseMeshs = false;
		config.UseVertexFormatConversion = true;
		config.VertexElements.Add(VertexElement(0,VEF_Count,VEU_Position,0));
		config.Method = MeshBuildMethod::D3D;
		config.SrcFile = srcFile;
		config.DstFile = dstFile;
		

		ModelData* data = XImporter::Import(config);

		MeshBuild::ConvertVertexData(data, config);

		List<BorderData> borderData;

		for (MeshData* mesh : data->Entities)
		{
			if (mesh->VertexCount == 0 || mesh->Faces.getCount() == 0)
				continue;

			//const Vector3* vertices = reinterpret_cast<const Vector3*>(mesh->VertexData);
			assert(mesh->VertexSize == 12);

			uint totalVertexCount = mesh->VertexCount;

			HashMap<Vector3, int> vtxHashTable(totalVertexCount);
			List<Vector3> newVertexList;

			Utils::meshWeldVertices(vtxHashTable, mesh, newVertexList);

			// =================== do the unique edge detection ============================== 
			HashMap<FaceEdge, int, FaceEdgeEqualityComparer> edgeUsageCounter;

			for (const MeshFace& face : mesh->Faces)
			{
				FaceEdge edge1 = { face.IndexA, face.IndexB };
				FaceEdge edge2 = { face.IndexB, face.IndexC };
				FaceEdge edge3 = { face.IndexA, face.IndexC };

				if (edge1.SmallerIDVertex > edge1.LargerIDVertex)
					std::swap(edge1.SmallerIDVertex, edge1.LargerIDVertex);
				if (edge2.SmallerIDVertex > edge2.LargerIDVertex)
					std::swap(edge2.SmallerIDVertex, edge2.LargerIDVertex);
				if (edge3.SmallerIDVertex > edge3.LargerIDVertex)
					std::swap(edge3.SmallerIDVertex, edge3.LargerIDVertex);

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

			List<FaceEdge> border; // the border edges, unsorted
			// dump out from the map
			for (HashMap<FaceEdge, int, FaceEdgeEqualityComparer>::Enumerator e = edgeUsageCounter.GetEnumerator();e.MoveNext();)
			{
				if (e.getCurrentValue() == 1)
					border.Add(e.getCurrentKey());
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
				List<Vector3> flattenVertices;

				List<FaceEdge*> sortedEdge;
				HashMap<void*, int> passedEdge;

				Queue<FaceEdge*> bfsQueue;
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
			Configuration* xml = new Configuration(L"Root");

			const uint64 idFmtFlags = StrFmt::a<4, '0'>::val;

			for (int32 i = 0; i < borderData.getCount();i++)
			{
				const BorderData& bdrData = borderData[i];
				ConfigurationSection* sect = new ConfigurationSection(L"Section" + StringUtils::IntToString(i));	
				ConfigurationSection* sectV = new ConfigurationSection(L"Vertex");	
				ConfigurationSection* sectB = new ConfigurationSection(L"Border");	
				ConfigurationSection* sectFV = new ConfigurationSection(L"FlattenVertex");	

				for (int j=0;j<bdrData.vertexCount;j++)
				{
					String value = StringUtils::SingleToString(bdrData.Vertices[j].X);
					value.append(L",");
					value.append(StringUtils::SingleToString(bdrData.Vertices[j].Y));
					value.append(L",");
					value.append(StringUtils::SingleToString(bdrData.Vertices[j].Z));

					sectV->AddStringValue(L"Vertex" + StringUtils::IntToString(j, idFmtFlags), value);
				}
					
				for (int j=0;j<bdrData.borderCount;j++)
				{
					String value = StringUtils::IntToString(bdrData.BorderDef[j].SmallerIDVertex, StrFmt::a<3>::val);
					value.append(L",");
					value.append(StringUtils::IntToString(bdrData.BorderDef[j].LargerIDVertex, StrFmt::a<3>::val));
					
					sectB->AddStringValue(L"Border" + StringUtils::IntToString(j, idFmtFlags), value);
				}

				for (int j=0;j<bdrData.flattenVertexCount;j++)
				{
					String value = StringUtils::SingleToString(bdrData.FlattenVertices[j].X);
					value.append(L",");
					value.append(StringUtils::SingleToString(bdrData.FlattenVertices[j].Y));
					value.append(L",");
					value.append(StringUtils::SingleToString(bdrData.FlattenVertices[j].Z));

					sectFV->AddStringValue(L"Vertex" + StringUtils::IntToString(j, idFmtFlags), value);
				}

				delete[] bdrData.Vertices;
				delete[] bdrData.BorderDef;
				delete[] bdrData.FlattenVertices;

				sect->AddSection(sectB);
				sect->AddSection(sectV);
				sect->AddSection(sectFV);
				xml->Add(sect);
			}

			//xml->Save(dstFile);
			XMLConfigurationFormat::Instance.Save(xml, FileOutStream(dstFile));

			delete xml;
			
		}

		BuildSystem::LogEntryProcessed(dstFile, hierarchyPath);
	}
}