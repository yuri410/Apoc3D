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

#include "XafImporter.h"

#include "../BuildConfig.h"

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Graphics/Animation/AnimationTypes.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"

#include "apoc3d/Library/tinyxml.h"

#include <fstream>
#include <vector>

using namespace Apoc3D::Utility;

namespace APBuild
{
	struct Frame
	{
		float Time;
		Matrix Transformation;
	};

	void ParseMatrixList(const TiXmlElement* element, List<Frame>& nodes, double invTickTime);

	AnimationData* XafImporter::Import(const TransformAnimBuildConfig& config)
	{
		String xafFile = config.SrcFile;

		TiXmlDocument doc;
		doc.Load(new FileStream(xafFile), TIXML_ENCODING_UNKNOWN);
		
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
					std::string objName = elem->Attribute("name");
					
					String wobjName = StringUtils::toPlatformWideString(objName);

					int index = -1;
					if (config.ObjectIndexMapping.TryGetValue(wobjName, index))
					{
						List<Frame> track;
						ParseMatrixList(elem->FirstChildElement("Samples"), track, 1.0/(frameRate*tickpf));
						
						float duration = 0;
						FastList<ModelKeyframe> keyFrames((int)track.getCount());

						for (int32 i=0;i<track.getCount();i++)
							if (track[i].Time>duration)
								duration = track[i].Time;
						

						for (int32 i=0;i<track.getCount();i++)
						{
							if (config.Reverse)
							{
								ModelKeyframe f(index,duration - track[i].Time,track[i].Transformation);
								keyFrames.Add(f);
							}
							else
							{
								ModelKeyframe f(index,track[i].Time,track[i].Transformation);
								keyFrames.Add(f);
							}
						}

						if (config.Reverse)
						{
							keyFrames.Reverse();
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, keyFrames);
						rigidAnim.Add(wobjName, clip);
					}

				}
			}
		}

		animData->setRigidAnimationClips(rigidAnim);
		return animData;
	}


	void ParseMatrixList(const TiXmlElement* element, List<Frame>& nodes, double invTickTime)
	{
		if (!element)
			return;

		int index = 0;
		for (const TiXmlElement* j = element->FirstChildElement(); j!=0; j=j->NextSiblingElement())
		{
			if (j->ValueStr() == "S")
			{
				double t;
				j->Attribute("t",&t);

				String v = Apoc3D::Utility::StringUtils::toPlatformWideString(j->Attribute("v"));
				List<String> values(12);
				Apoc3D::Utility::StringUtils::Split(v, values);
				assert(values.getCount() == 12);

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

				if (index<nodes.getCount())
				{
					nodes[index].Transformation = trans;
				}
				else
				{
					Frame node;
					memset(&node, 0, sizeof(node));
					node.Time = (float)(t*invTickTime);

					node.Transformation = trans;

					nodes.Add(node);
				}

				index++;
			}
		}
	}
}
