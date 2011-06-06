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
#include "Collections/FastMap.h"
#include "Apoc3DException.h"

#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/RenderTarget.h"
#include "tinyxml/tinyxml.h"
#include "IO/Streams.h"
#include "VFS/ResourceLocation.h"



using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;


namespace Apoc3D
{
	namespace Scene
	{
		static const int NUM = 256;
		static const char LBracket = '(';


		static const char OpAnd = '&';
		static const char OpOr = '|';
		static const char OpNot = '!';
		static const char OpAt = '@';

		static const char RBracket = ')';

		static const char EndSym = '$';

		struct PriorInfo
		{
			char OperatorChar;
			int P1;
			int P2;
		};
		static const PriorInfo pList[7] = {
			{ OpAnd, 2, 1 },
			{ OpOr, 2, 1 },
			{ OpNot, 4, 3 },
			{ OpAt, 4, 5 },
			{ LBracket, 0, 5 },
			{ RBracket, 6, 0 },
			{ EndSym, 0, 0 }
		};

		class ExpressionCompiler
		{
		private:


			enum NodeType
			{
				NT_Operator,
				NT_Operand
			};

			class ExpressionNode
			{
			public:
				char m_optr;
				int m_opnd;
				NodeType m_type;
				ExpressionNode* m_left;
				ExpressionNode* m_right;

				/** ������������������(�ڽ��)
				*/
				ExpressionNode(char optr, ExpressionNode* left, ExpressionNode* right)
					: m_optr(optr), m_left(left), m_right(right), m_type(NT_Operator)
				{
				}

				/** ���������������(Ҷ���) 
				*/
				ExpressionNode(int operand)
					: m_opnd(operand), m_type(NT_Operand)
				{
				}
			};

			Stack<char> optrStack;
			Stack<ExpressionNode*> exprStack;

			/** ��GetNextSymbol()ΪNUMʱ�����ֶ�Ϊ����ֵ
			*/
			int tokenval;
			FastList<string> expStruct;

			int position;
			FastMap<string, int> parameters;

			/** �Ƚ�ջ�����������һ������������ȹ�ϵ
			*/
			char Precede(char opf, char opg)
			{
				int op1 = -1, op2 = -1;
				for (int i = 0; i < sizeof(pList)/sizeof(PriorInfo); i++)
				{
					if (pList[i].OperatorChar == opf)
						op1 = pList[i].P1;
					if (pList[i].OperatorChar == opg)
						op2 = pList[i].P2;
				}
				if (op1 == -1 || op2 == -1)
				{
					throw Apoc3DException::createException(EX_InvalidOperation, L"operator error!");
					//cout << "operator   error!" << endl;
					//exit(1);
				}
				if (op1 > op2)
					return '>';
				else if (op1 == op2)
					return '=';
				else
					return '<';
			}

			/** ��ñ���ʽ����һ����ǡ�����Ǳ����򷵻�NUM����������tokenval
			*/
			int GetNextSymbol()
			{
				string str = expStruct[position++];

				if (str.length() == 1 &&
					(str[0] == LBracket || str[0] == RBracket || str[0] == OpAnd || 
					str[0] == OpOr || str[0] == OpNot || str[0] == EndSym))
				{
					return str[0];
				}
				else
				{
					tokenval = parameters[str];
					return NUM;
				}
			}

			ExpressionNode* CreateBinaryTree()
			{
				int lookahead;
				char op;
				ExpressionNode* opnd1, *opnd2;
				optrStack.Push(EndSym);
				lookahead = GetNextSymbol();
				while (lookahead != EndSym || optrStack.Peek() != EndSym)
				{
					if (lookahead == NUM)
					{
						exprStack.Push(new ExpressionNode(tokenval)); // mkleaf(tokenval)
						lookahead = GetNextSymbol();
					}
					else
					{
						switch (Precede(optrStack.Peek(), lookahead))
						{
						case '<':
							optrStack.Push(lookahead);
							lookahead = GetNextSymbol();
							break;
						case '=':
							optrStack.Pop();
							lookahead = GetNextSymbol();
							break;
						case '>':
							op = optrStack.Pop();

							if (op == OpNot)
							{
								opnd2 = exprStack.Pop();
								exprStack.Push(new ExpressionNode(op, 0, opnd2));//mknode(op, null, opnd2)
							}
							else
							{
								opnd2 = exprStack.Pop();

								opnd1 = exprStack.Pop();
								exprStack.Push(new ExpressionNode(op, opnd1, opnd2));//mknode(op, opnd1, opnd2)
							}

							break;
						}
					}
				}
				return exprStack.Peek();
			}

			bool FollowOrderTraverse(ExpressionNode* T)
			{
				if (!T)
					return true;

				if (T->m_type == NT_Operator)
				{
					if (FollowOrderTraverse(T->m_left))
					{
						if (FollowOrderTraverse(T->m_right))
						{
							switch (T->m_optr)
							{
							case OpAnd:
								//Console.WriteLine("and");
								//codeGen.Emit(OpCodes.And);
								break;
							case OpOr:
								//Console.WriteLine("or");
								//codeGen.Emit(OpCodes.Or);
								break;
							case OpNot:
								//Console.WriteLine("not");
								//codeGen.Emit(OpCodes.Not);
								break;
							case OpAt:

								break;
							}
							return true;
						}
					}
					return false;

				}
				else
				{
					//Console.WriteLine("ldarg.0");
					//Console.WriteLine("ldc.i4   " + T.opnd.ToString());
					//Console.WriteLine("ldelem.i1");

					//codeGen.Emit(OpCodes.Ldarg_0);
					//codeGen.Emit(OpCodes.Ldc_I4, T.opnd);
					//codeGen.Emit(OpCodes.Ldelem_I1);
					return true;
				}
			}

			void Parse(string expression)
			{
				string sb;

				bool ended = false;

				int paramIndex = 0;

				for (size_t i = 0; i < expression.length(); i++)
				{
					char cc = expression[i];
					switch (cc)
					{
					case LBracket:
						expStruct.Add(string(1, LBracket));

						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}
						ended = true;

						sb = string();
						break;
					case RBracket:
						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(string(1, RBracket));

						ended = true;

						sb = string();
						break;
					case OpAnd:
						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(string(1, OpAnd));

						ended = true;

						sb = string();
						break;
					case OpOr:
						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}
						expStruct.Add(string(1, OpOr));

						ended = true;

						sb = string();
						break;
					case OpNot:
						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(string(1, OpNot));

						ended = true;

						sb = string();
						break;
					case OpAt:
						if (sb.length() > 0)
						{
							string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(string(1, OpAt));

						ended = true;

						sb = string();
						break;
					case ' ':
						break;
					default:
						sb.append(1, cc);
						ended = false;

						break;

					}
				}

				if (!ended)
				{
					string typeName = sb;
					expStruct.Add(typeName);
					if (!parameters.Contains(typeName))
					{
						parameters.Add(typeName, paramIndex++);
					}
				}
			}

		public:

		};


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
			return toString(elem->ValueStr());
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
							float v = StringUtils::ParseSingle(elems[i]);
							var.DefaultValue[i] = reinterpret_cast<const uint&>(v);
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
							float v = StringUtils::ParseSingle(elems[i]);
							var.DefaultValue[i] = reinterpret_cast<const uint&>(v);
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
							float v = StringUtils::ParseSingle(elems[i]);
							var.DefaultValue[i] = reinterpret_cast<const uint&>(v);
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
							float v = StringUtils::ParseSingle(elems[i]);
							var.DefaultValue[i] = reinterpret_cast<const uint&>(v);
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

		void FillInstructions(const string& cmd, FastList<SceneInstruction>& instructions)
		{

		}

		void SceneRenderScriptParser::BuildInstructions(const TiXmlElement* node, ScenePassData* data)
		{
			for (const TiXmlNode* i = node->FirstChild(); i!=0; i=i->NextSibling())
			{
				int type = i->Type();
				switch (type)
				{
				case TiXmlNode::TINYXML_ELEMENT:
					const TiXmlElement* elem = i->ToElement();

					String strName = getElementName(elem);

					String lowStrName = strName;
					StringUtils::ToLowerCase(lowStrName);

					if (lowStrName == String(L"if"))
					{
						FillInstructions(elem->Attribute("Cond"), data->Instructions);
						data->Instructions.Add(SceneInstruction(SOP_JZ));
						
						SceneInstruction& inst = data->Instructions[data->Instructions.getCount()-1];

						BuildInstructions(elem, data);

						// back fill
						inst.Next = data->Instructions.getCount();
					}
					else if (lowStrName == String(L"e"))
					{
						FillInstructions(elem->Attribute("S"), data->Instructions);
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