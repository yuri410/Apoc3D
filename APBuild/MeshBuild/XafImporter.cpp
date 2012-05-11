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

#include "XafImporter.h"

#include "Collections/FastList.h"
#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "BuildConfig.h"
#include "Graphics/Animation/AnimationTypes.h"
#include "IOLib/MaterialData.h"
#include "IOLib/ModelData.h"
#include "Graphics/Animation/AnimationData.h"
#include "IOLib/Streams.h"
#include "Utility/StringUtils.h"

#include "tinyxml/tinyxml.h"

using namespace Apoc3D::Utility;

namespace APBuild
{
	struct Frame
	{
		float Time;
		Matrix Transformation;
	};

	void ParseMatrixList(const TiXmlElement* element, std::vector<Frame>& nodes, double invTickTime);

	AnimationData* XafImporter::Import(const TransformAnimBuildConfig& config)
	{
		String xafFile = config.SrcFile;

		char* content = NULL;

		// tinyXml have bug handling non-english path
		// thus, here we load the content on our own
		{
			std::ifstream stm(xafFile.c_str(), std::ios::binary | std::ios::in);
			if (stm)
			{
				stm.seekg(0,ios::end);
				std::streampos length = stm.tellg();
				stm.seekg(0,ios::beg);

				content = new char[(uint32)length];
				stm.read(content, length);
			}
		}

		if (!content)
		{
#if _DEBUG
			_wassert(L"Warning: failed to load animation track", _CRT_WIDE(__FILE__), __LINE__);
#endif
			return 0;
		}

		TiXmlDocument doc;
		doc.Parse(content);
		//doc.LoadFile(filePath);
		delete[] content;

		double frameRate = 0;
		double tickpf = 0;

		//string objName = Apoc3D::Utility::StringUtils::toString(objectName);
		AnimationData* animData = new AnimationData();
		AnimationData::ClipTable rigidAnim;

		const TiXmlNode* i = doc.FirstChildElement();
		if (i)
		{
			for (const TiXmlElement* j = i->FirstChildElement(); j!=0; j=j->NextSiblingElement())
			{
				const TiXmlElement* elem = j;
				
				if (elem->ValueStr() == "SceneInfo")
				{
					elem->Attribute("frameRate", &frameRate);
					elem->Attribute("ticksPerFrame", &tickpf);
				}
				else if (elem->ValueStr() == "Node")
				{
					string objName = elem->Attribute("name");
					
					String wobjName = StringUtils::toWString(objName);

					int index = -1;
					if (config.ObjectIndexMapping.TryGetValue(wobjName, index))
					{
						std::vector<Frame> track;
						ParseMatrixList(elem->FirstChildElement("Samples"), track, 1.0/(frameRate*tickpf));
						
						float duration = 0;
						FastList<ModelKeyframe> keyFrames((int)track.size());

						for (size_t i=0;i<track.size();i++)
						{
							if (track[i].Time>duration)
								duration = track[i].Time;

							ModelKeyframe f(index,track[i].Time,track[i].Transformation);
							keyFrames.Add(f);
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, keyFrames);
						rigidAnim.insert(std::make_pair(wobjName, clip));
					}

				}
			}
		}

		animData->setRigidAnimationClips(rigidAnim);
		return animData;
	}


	void ParseMatrixList(const TiXmlElement* element, std::vector<Frame>& nodes, double invTickTime)
	{
		if (!element)
			return;

		uint index = 0;
		for (const TiXmlElement* j = element->FirstChildElement(); j!=0; j=j->NextSiblingElement())
		{
			if (j->ValueStr() == "S")
			{
				double t;
				j->Attribute("t",&t);

				String v = Apoc3D::Utility::StringUtils::toWString(j->Attribute("v"));
				std::vector<String> values = Apoc3D::Utility::StringUtils::Split(v);
				assert(values.size() == 12);

				Matrix trans;
				trans.LoadIdentity();
				
				trans.M11 = Apoc3D::Utility::StringUtils::ParseSingle(values[0]);
				trans.M12 = Apoc3D::Utility::StringUtils::ParseSingle(values[1]);
				trans.M13 = Apoc3D::Utility::StringUtils::ParseSingle(values[2]);

				trans.M21 = Apoc3D::Utility::StringUtils::ParseSingle(values[3]);
				trans.M22 = Apoc3D::Utility::StringUtils::ParseSingle(values[4]);
				trans.M23 = Apoc3D::Utility::StringUtils::ParseSingle(values[5]);

				trans.M31 = Apoc3D::Utility::StringUtils::ParseSingle(values[6]);
				trans.M32 = Apoc3D::Utility::StringUtils::ParseSingle(values[7]);
				trans.M33 = Apoc3D::Utility::StringUtils::ParseSingle(values[8]);

				trans.M41 = Apoc3D::Utility::StringUtils::ParseSingle(values[9]);
				trans.M42 = Apoc3D::Utility::StringUtils::ParseSingle(values[10]);
				trans.M43 = Apoc3D::Utility::StringUtils::ParseSingle(values[11]);


				Matrix spaceSwitcher(
					1,0,0,0,
					0,0,1,0,
					0,1,0,0,
					0,0,0,1);

				Matrix temp;

				Matrix::Multiply(temp, spaceSwitcher, trans);
				Matrix::Multiply(trans, temp, spaceSwitcher);
				//D3DXMatrixMultiply(&trans, &spaceSwitcher, &trans);
				//D3DXMatrixMultiply(&trans, &trans, &spaceSwitcher);

				if (index<nodes.size())
				{
					nodes[index].Transformation = trans;
				}
				else
				{
					Frame node;
					memset(&node, 0, sizeof(node));
					node.Time = (float)(t*invTickTime);

					node.Transformation = trans;

					nodes.push_back(node);
				}

				index++;
			}
		}
	}
}