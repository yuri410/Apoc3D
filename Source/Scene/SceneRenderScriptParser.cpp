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

#include "SceneRenderScriptParser.h"

#include "Core/Logging.h"
#include "Utility/StringUtils.h"
#include "Collections/Stack.h"
#include "Collections/FastList.h"
#include "Apoc3DException.h"

#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "tinyxml/tinyxml.h"
#include "IO/Streams.h"
#include "VFS/ResourceLocation.h"


using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;


namespace Apoc3D
{
	namespace Scene
	{
		String toString(const string& str)
		{
			wchar_t* buffer = new wchar_t[str.length()];
			mbstowcs(buffer, str.c_str(), str.length());
			String result = buffer;
			delete[] buffer;
			return result;
		}
		String getElementName(const TiXmlElement* elem)
		{
			toString(elem->ValueStr());
		}
		String getNodeText(const TiXmlText* text)
		{
			string str = text->ValueStr();
			wchar_t* buffer = new wchar_t[str.length()];
			mbstowcs(buffer, str.c_str(), str.length());
			String result = buffer;
			delete[] buffer;
			return result;
		}
		String getAttribName(const TiXmlAttribute* attrib)
		{
			string str = attrib->NameTStr();
			wchar_t* buffer = new wchar_t[str.length()];
			mbstowcs(buffer, str.c_str(), str.length());
			String result = buffer;
			delete[] buffer;
			return result;
		}
		String getAttribValue(const TiXmlAttribute* attrib)
		{
			string str = attrib->ValueStr();
			wchar_t* buffer = new wchar_t[str.length()];
			mbstowcs(buffer, str.c_str(), str.length());
			String result = buffer;
			delete[] buffer;
			return result;
		}

		PixelFormat ConvertFormat(const string& fmt)
		{
			if (fmt == string("L8"))
			{
				return FMT_Luminance8;
			}
			else if (fmt == string("L16"))
			{
				return FMT_Luminance16;
			}
			else if (fmt == string("A8"))
			{
				return FMT_Alpha8;
			}
			else if (fmt == string("A8L8"))
			{
				return FMT_A8L8;
			}
			else if (fmt == string("R5G6B5"))
			{
				return FMT_R5G6B5;
			}
			else if (fmt == string("B5G6R5"))
			{
				return FMT_B5G6R5;
			}
			else if (fmt == string("A4R4G4B4"))
			{
				return FMT_A4R4G4B4;
			}
			else if (fmt == string("A1R5G5B5"))
			{
				return FMT_A1R5G5B5;
			}
			else if (fmt == string("R8G8B8"))
			{
				return FMT_R8G8B8;
			}
			else if (fmt == string("B8G8R8"))
			{
				return FMT_B8G8R8;
			}
			else if (fmt == string("A8R8G8B8"))
			{
				return FMT_A8R8G8B8;
			}
			else if (fmt == string("A8B8G8R8"))
			{
				return FMT_A8B8G8R8;
			}
			else if (fmt == string("B8G8R8A8"))
			{
				return FMT_B8G8R8A8;
			}
			else if (fmt == string("A2R10G10B10"))
			{
				return FMT_A2R10G10B10;
			}
			else if (fmt == string("A2B10G10R10"))
			{
				return FMT_A2B10G10R10;
			}
			else if (fmt == string("A16B16G16R16F"))
			{
				return FMT_A16B16G16R16F;
			}
			else if (fmt == string("A32B32G32R32F"))
			{
				return FMT_A32B32G32R32F;
			}
			else if (fmt == string("X8R8G8B8"))
			{
				return FMT_X8R8G8B8;
			}
			else if (fmt == string("X8B8G8R8"))
			{
				return FMT_X8B8G8R8;
			}
			else if (fmt == string("R8G8B8A8"))
			{
				return FMT_R8G8B8A8;
			}
			else if (fmt == string("A16B16G16R16"))
			{
				return FMT_A16B16G16R16;
			}
			else if (fmt == string("R3G3B2"))
			{
				return FMT_R3G3B2;
			}
			else if (fmt == string("R16F"))
			{
				return FMT_R16F;
			}
			else if (fmt == string("R32F"))
			{
				return FMT_R32F;
			}
			else if (fmt == string("G16R16"))
			{
				return FMT_G16R16;
			}
			else if (fmt == string("G16R16F"))
			{
				return FMT_G16R16F;
			}
			else if (fmt == string("G32R32F"))
			{
				return FMT_G32R32F;
			}
			else if (fmt == string("R16G16B16"))
			{
				return FMT_R16G16B16;
			}
			else if (fmt == string("B4G4R4A4"))
			{
				return FMT_B4G4R4A4;
			}
			return FMT_Unknown;
		}
		DepthFormat ConvertDepthFormat(const string& fmt)
		{
			if (fmt == string("D15S1"))
			{
				return DEPFMT_Depth15Stencil1;
			}
			else if (fmt == string("D16"))
			{
				return DEPFMT_Depth16;
			}
			else if (fmt == string("D24"))
			{
				return DEPFMT_Depth24X8;
			}
			else if (fmt == string("D24S4"))
			{
				return DEPFMT_Depth24Stencil4;
			}
			else if (fmt == string("D24S8"))
			{
				return DEPFMT_Depth24Stencil8;
			}
			else if (fmt == string("D24S8F"))
			{
				return DEPFMT_Depth24Stencil8Single;
			}
			else if (fmt == string("D32"))
			{
				return DEPFMT_Depth32;
			}
			return DEPFMT_Depth16;
		}

		SceneRenderScriptParser::SceneRenderScriptParser(RenderDevice* dev)
			: m_renderDevice(dev)
		{

		}

		void SceneRenderScriptParser::ParseGlocalVarNode(const TiXmlElement* node)
		{
			string tstr = node->Attribute("Type");
			transform(tstr.begin(), tstr.end(), tstr.begin(), tolower);

			if (tstr == string("rendertarget"))
			{
				SceneVariable var;
				var.Type = VARTYPE_RenderTarget;

				const TiXmlElement* e1 = node->FirstChildElement("Width");
				const TiXmlElement* e2 = node->FirstChildElement("Height");

				if (e1 && e2)
				{
					var.DefaultValue[0] = StringUtils::ParseInt32(toString(e1->GetText()));
					var.DefaultValue[1] = StringUtils::ParseInt32(toString(e2->GetText()));
				}
				else
				{
					e1 = node->FirstChildElement("WidthP");
					e2 = node->FirstChildElement("HeightP");

					if (e1 && e2)
					{
						float r = StringUtils::ParseSingle(toString(e1->GetText()));
						var.DefaultValue[2] = reinterpret_cast<const uint&>(r);

						r = StringUtils::ParseSingle(toString(e2->GetText()));
						var.DefaultValue[3] = reinterpret_cast<const uint&>(r);
					}
					else
					{
						float r = 1;
						var.DefaultValue[2] = reinterpret_cast<const uint&>(r);
						var.DefaultValue[3] = reinterpret_cast<const uint&>(r);
					}
				}
				
				e1 = node->FirstChildElement("Format");
				if (e1)
				{
					PixelFormat fmt = ConvertFormat(e1->GetText());
					var.DefaultValue[4] = reinterpret_cast<const uint&>(fmt);
				}
				else
				{
					PixelFormat fmt = m_renderDevice->GetRenderTarget(0)->getColorFormat();
					var.DefaultValue[4] = reinterpret_cast<const uint&>(fmt);
				}

				e1 = node->FirstChildElement("Depth");
				if (e1)
				{
					DepthFormat fmt = ConvertDepthFormat(e1->GetText());
					var.DefaultValue[5] = reinterpret_cast<const uint&>(fmt);
				}
				else
				{
					DepthFormat fmt = m_renderDevice->GetRenderTarget(0)->getDepthFormat();
					var.DefaultValue[5] = reinterpret_cast<const uint&>(fmt);
				}

				e1 = node->FirstChildElement("SampleCount");
				if (e1)
				{
					var.DefaultValue[6] = StringUtils::ParseInt32(toString(e1->GetText()));
				}
				else
				{
					var.DefaultValue[6] = 0;
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("matrix"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Matrix;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					vector<String> elems = StringUtils::Split(toString(e1->GetText()), L" ,", 16);
					if (elems.size() != 16)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for matrix.", LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<16;i++)
						{
							var.DefaultValue[i] = reinterpret_cast<const uint&>(StringUtils::ParseSingle(elems[i]));
						}
					}
				}

				e1 = node->FirstChildElement("Bind");
				if (e1)
				{

				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("vector4"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Vector4;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					vector<String> elems = StringUtils::Split(toString(e1->GetText()), L" ,", 4);
					if (elems.size() != 4)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector4.", LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<4;i++)
						{
							var.DefaultValue[i] = reinterpret_cast<const uint&>(StringUtils::ParseSingle(elems[i]));
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("vector3"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Vector3;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					vector<String> elems = StringUtils::Split(toString(e1->GetText()), L" ,", 4);
					if (elems.size() != 3)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector3.", LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<3;i++)
						{
							var.DefaultValue[i] = reinterpret_cast<const uint&>(StringUtils::ParseSingle(elems[i]));
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("vector2"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Vector2;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					vector<String> elems = StringUtils::Split(toString(e1->GetText()), L" ,", 2);
					if (elems.size() != 2)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector4.", LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<2;i++)
						{
							var.DefaultValue[i] = reinterpret_cast<const uint&>(StringUtils::ParseSingle(elems[i]));
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("integer"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Integer;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					StringUtils::ParseInt32(toString(e1->GetText()));
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("boolean"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Boolean;

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					StringUtils::ParseBool(toString(e1->GetText()));
				}

				GlobalVars.Add(var);
			}
			else if (tstr == string("texture"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Texture;

				const TiXmlElement* e1 = node->FirstChildElement("Source");
				if (e1)
				{
					var.DefaultStringValue = toString(e1->GetText());
					GlobalVars.Add(var);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Scene, L"Can not find texture source.", LOGLVL_Warning);
				}
			}
			//else if (tstr == string("camera"))
			//{
			//	SceneVariable var;
			//	var.Type = VARTYPE_Camera;

			//	GlobalVars.Add(var);
			//}
			else if (tstr == string("effect"))
			{
				SceneVariable var;
				var.Type = VARTYPE_Effect;

				const TiXmlElement* e1 = node->FirstChildElement("Source");
				if (e1)
				{
					var.DefaultStringValue = toString(e1->GetText());
					GlobalVars.Add(var);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Scene, L"Can not find effect name.", LOGLVL_Warning);
				}
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Unsupported variable type " + toString(tstr), LOGLVL_Warning);
			}
		}

		void SceneRenderScriptParser::BuildNode(const TiXmlNode* node)
		{
			int type = node->Type();

			switch (type)
			{
			case TiXmlNode::TINYXML_ELEMENT:
				{
					const TiXmlElement* elem = node->ToElement();

					String strName = getElementName(elem);

					String lowStrName = strName;
					StringUtils::ToLowerCase(lowStrName);

					if (lowStrName == String(L"scene"))
					{
						m_sceneName = toString(elem->Attribute("Name"));
					}
					else if (lowStrName == String(L"pass"))
					{
						BuildPass(elem);
					}
					else if (lowStrName == String(L"declare"))
					{
						ParseGlocalVarNode(elem);
					}
					else
					{
						for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
						{
							BuildNode(i);
						}
					}
				}
				break;
			}
		}

		void SceneRenderScriptParser::BuildInstructions(const TiXmlElement* elem, ScenePassData* data)
		{
			for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
			{
				int type = i->Type();
				switch (type)
				{
				case TiXmlNode::TINYXML_ELEMENT:
					String strName = getElementName(elem);

					String lowStrName = strName;
					StringUtils::ToLowerCase(lowStrName);

					if (lowStrName == String(L"if"))
					{

					}
					
					
					break;
				}
			}
		}

		void SceneRenderScriptParser::BuildPass(const TiXmlElement* node)
		{
			ScenePassData passData;
			node->Attribute("SelectorID", &passData.SelectorID);

			BuildInstructions(node, &passData);
		}
		void SceneRenderScriptParser::Parse(const ResourceLocation* rl)
		{
			TiXmlDocument doc;

			Stream* strm = rl->GetReadStream();

			char* buffer = new char[strm->getLength()];
			strm->Read(buffer, strm->getLength());

			doc.Parse(buffer);

			strm->Close();
			delete strm;
			delete buffer;

			for (const TiXmlNode* i = doc.FirstChild(); i!=0; i=i->NextSibling())
			{
				BuildNode(i);
			}
			//vector<String> lines = StringUtils::Split(code, L"\n\r");


			//FastList<Block> stack;

			//for (size_t i=0;i<lines.size();i++)
			//{
			//	StringUtils::Trim(lines[i]);

			//	vector<String> args;

			//	{
			//		String buildingStr;
			//		bool isInStr = false;
			//		bool lower = true;
			//		for (size_t j=0;j<lines[i].length();j++)
			//		{
			//			wchar_t ch = lines[i][j];
			//			if (ch == '"')
			//			{
			//				isInStr = !isInStr;
			//				if (isInStr)
			//					lower = false;
			//			}
			//			else if (ch == ';' && !isInStr)
			//			{
			//				break;
			//			}
			//			else if (ch == ' ' && !isInStr)
			//			{
			//				StringUtils::Trim(buildingStr);
			//				if (buildingStr.length())
			//				{
			//					args.push_back(buildingStr);
			//				}
			//				buildingStr = String();
			//				lower = true;
			//			}
			//			else
			//			{
			//				if (lower)
			//				{
			//					ch = tolower(ch);
			//				}
			//				buildingStr.append(1, ch);
			//			}
			//		}
			//		StringUtils::Trim(buildingStr);
			//		if (buildingStr.length())
			//		{
			//			args.push_back(buildingStr);
			//		}
			//	}
			//	

			//	if (args.size())
			//	{
			//		const String& first = args[0];
			//		
			//		if (first == PreserveWords[PWORD_Scene])
			//		{
			//			Block block = { BLK_Scene, args };
			//			stack.Add(block);
			//		}
			//		else if (first == PreserveWords[PWORD_Pass])
			//		{
			//			Block block = { BLK_Pass, args[1] };
			//			stack.Add(block);
			//		}
			//		else if (first == PreserveWords[PWORD_IF])
			//		{
			//			Block block = { BLK_IF, args[1] };
			//			stack.Add(block);
			//		}
			//		else if (first == PreserveWords[PWORD_EndIF])
			//		{
			//			if (stack.Peek().BlockType == BLK_IF)
			//			{
			//				invStack.Push(stack.Pop());
			//			}
			//			else
			//			{
			//				throw Apoc3DException::createException(EX_ScriptCompileError, 
			//					String(L"No corresponding IF Block. Line ") + StringUtils::ToString(i));
			//			}
			//		}
			//		else if (first == PreserveWords[PWORD_EndPass])
			//		{
			//			if (stack.Peek().BlockType == BLK_Pass)
			//			{
			//				invStack.Push(stack.Pop());
			//			}
			//			else
			//			{
			//				throw Apoc3DException::createException(EX_ScriptCompileError, 
			//					String(L"No corresponding Pass Block. Line ") + StringUtils::ToString(i));
			//			}
			//		}
			//		else if (first == PreserveWords[PWORD_EndScene])
			//		{
			//			if (stack.Peek().BlockType == BLK_Scene)
			//			{
			//				invStack.Push(stack.Pop());
			//			}
			//			else
			//			{
			//				throw Apoc3DException::createException(EX_ScriptCompileError, 
			//					String(L"No corresponding Scene Block. Line ") + StringUtils::ToString(i));
			//			}
			//		}
			//		else
			//		{
			//			Block blk = 
			//		}
			//	}


			//}
		}
	}
}