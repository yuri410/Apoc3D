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

#include "SceneRenderScriptParser.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Collections/Stack.h"
#include "apoc3d/Apoc3DException.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Math/GaussBlurFilter.h"
#include "apoc3d/tinyxml.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/VFS/ResourceLocation.h"

#include <algorithm>

using namespace Apoc3D::Graphics::EffectSystem;
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
		//static const char OpAt = '@';

		static const char RBracket = ')';

		static const char EndSym = '$';

		struct PriorInfo
		{
			char OperatorChar;
			// left side priority
			int P1;
			// right side priority
			int P2;
		};
		static const PriorInfo pList[6] = {
			{ OpAnd, 2, 1 },
			{ OpOr, 2, 1 },
			{ OpNot, 4, 3 },
			//{ OpAt, 4, 5 },
			{ LBracket, 0, 5 },
			{ RBracket, 6, 0 },
			{ EndSym, 0, 0 }
		};


		bool ParseCallArgAsVar(const std::string& value, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars);

		bool ParseCallArgBool(const TiXmlElement* node,  const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, bool def);
		bool ParseCallArgFloat(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, float def);
		bool ParseCallArgUint(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, uint def);
		bool ParseCallArgInt(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, int def);
		bool ParseCallArgRef(const TiXmlElement* node, const std::string& name, SceneOpArg& arg,
			const FastList<SceneVariable*>& vars);
		bool ParseCallArgUintHexImm(const TiXmlElement* node, const std::string& name, SceneOpArg& arg);
		bool ParseCallArgVector2(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, Vector2 def);
		void ParseCallArgRenderStates(const std::string& value, List<SceneOpArg>& args);

		PixelFormat ConvertFormat(const std::string& fmt);
		DepthFormat ConvertDepthFormat(const std::string& fmt);

		String getElementName(const TiXmlElement* elem) { return StringUtils::toWString(elem->ValueStr()); }
		String getNodeText(const TiXmlText* text) { return StringUtils::toWString(text->ValueStr()); }
		String getAttribName(const TiXmlAttribute* attrib) { return StringUtils::toWString(attrib->NameTStr()); }
		String getAttribValue(const TiXmlAttribute* attrib) { return StringUtils::toWString(attrib->ValueStr()); }


		SceneVariable* FindVar(const FastList<SceneVariable*>& vars, const String& name)
		{
			for (int i=0;i<vars.getCount();i++)
			{
				if (vars[i]->Name == name)
					return vars[i];
			}
			return nullptr;
		}

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

				/** 建立二叉树运算符结点(内结点)
				*/
				ExpressionNode(char optr, ExpressionNode* left, ExpressionNode* right)
					: m_optr(optr), m_left(left), m_right(right), m_type(NT_Operator)
				{
				}

				/** 建立二叉树数结点(叶结点) 
				*/
				ExpressionNode(int operand)
					: m_opnd(operand), m_type(NT_Operand), m_left(0), m_right(0)
				{
				}

				~ExpressionNode()
				{
					if (m_left)
						delete m_left;
					if (m_right)
						delete m_right;
				}
			};

			Stack<char> optrStack;
			Stack<ExpressionNode*> exprStack;

			/** 当GetNextSymbol()为NUM时，该字段为参数值
			*/
			int tokenval;
			List<std::string> expStruct;

			int position;
			HashMap<std::string, int> parameters;
			List<std::string> paramList;

			/** 比较栈顶运算符与下一输入运算符优先关系
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

			/** 获得表达式中下一个标记。如果是变量则返回NUM，变量索引tokenval
			*/
			int GetNextSymbol()
			{
				std::string str = expStruct[position++];

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
						switch (Precede(optrStack.Peek(), static_cast<char>(lookahead)))
						{
						case '<':
							optrStack.Push(static_cast<char>(lookahead));
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

			bool FollowOrderTraverse(ExpressionNode* T, List<SceneInstruction>& insts, const FastList<SceneVariable*>& vars)
			{
				if (!T)
					return true;

				if (T->m_type == NT_Operator)
				{
					if (FollowOrderTraverse(T->m_left, insts, vars))
					{
						if (FollowOrderTraverse(T->m_right, insts, vars))
						{
							switch (T->m_optr)
							{
							case OpAnd:
								{
									SceneInstruction inst;
									inst.Operation = SOP_And;
									insts.Add(inst);
								}
								break;
							case OpOr:
								{
									SceneInstruction inst;
									inst.Operation = SOP_Or;
									insts.Add(inst);
								}
								break;
							case OpNot:
								{
									SceneInstruction inst;
									inst.Operation = SOP_Not;
									insts.Add(inst);
								}
								break;
							//case OpAt:

								//break;
							}
							return true;
						}
					}
					return false;

				}
				else
				{
					SceneInstruction inst;
					inst.Operation = SOP_Load;
					
					String token = StringUtils::toWString( paramList[T->m_opnd] );
					SceneVariable* var = FindVar(vars, token);
					if (!var)
					{
						SceneOpArg arg;
						arg.IsImmediate = true;
						bool flag = StringUtils::ParseBool(token);
						arg.DefaultValue[0] = flag ? 1 : 0;
						inst.Args.Add(arg);
					}
					else
					{
						SceneOpArg arg;
						arg.IsImmediate = false;
						arg.Var = var;
						inst.Args.Add(arg);
					}
					
					
					//SceneOpArg arg;
					//arg.IsImmediate = false;
					//inst.Args.push_back(arg);

					insts.Add(inst);
					//Console.WriteLine("ldarg.0");
					//Console.WriteLine("ldc.i4   " + T.opnd.ToString());
					//Console.WriteLine("ldelem.i1");

					//codeGen.Emit(OpCodes.Ldarg_0);
					//codeGen.Emit(OpCodes.Ldc_I4, T.opnd);
					//codeGen.Emit(OpCodes.Ldelem_I1);
					return true;
				}
			}

			void Parse(std::string expression)
			{
				std::string sb;

				bool ended = false;

				int paramIndex = 0;

				for (size_t i = 0; i < expression.length(); i++)
				{
					char cc = expression[i];
					switch (cc)
					{
					case LBracket:
						expStruct.Add(std::string(1, LBracket));

						if (sb.length() > 0)
						{
							std::string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								paramList.Add(typeName);
								parameters.Add(typeName, paramIndex++);
							}
						}
						ended = true;

						sb = std::string();
						break;
					case RBracket:
						if (sb.length() > 0)
						{
							std::string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								paramList.Add(typeName);
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(std::string(1, RBracket));

						ended = true;

						sb = std::string();
						break;
					case OpAnd:
						if (sb.length() > 0)
						{
							std::string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								paramList.Add(typeName);
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(std::string(1, OpAnd));

						ended = true;

						sb = std::string();
						break;
					case OpOr:
						if (sb.length() > 0)
						{
							std::string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								paramList.Add(typeName);
								parameters.Add(typeName, paramIndex++);
							}
						}
						expStruct.Add(std::string(1, OpOr));

						ended = true;

						sb = std::string();
						break;
					case OpNot:
						if (sb.length() > 0)
						{
							std::string typeName = sb;
							expStruct.Add(typeName);
							if (!parameters.Contains(typeName))
							{
								paramList.Add(typeName);
								parameters.Add(typeName, paramIndex++);
							}
						}

						expStruct.Add(std::string(1, OpNot));

						ended = true;

						sb = std::string();
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
					std::string typeName = sb;
					expStruct.Add(typeName);
					if (!parameters.Contains(typeName))
					{
						paramList.Add(typeName);
						parameters.Add(typeName, paramIndex++);
					}
				}
			}

		public:
			ExpressionCompiler()
				: tokenval(0), position(0)
			{
				
			}
			void FillInstrunctions(const std::string& expression, List<SceneInstruction>& insts, const FastList<SceneVariable*>& vars)
			{
				Parse(expression);
				expStruct.Add(std::string(1,EndSym));

				ExpressionNode* tree = CreateBinaryTree();

				FollowOrderTraverse(tree, insts, vars);

				delete tree;
			}
		};


		SceneRenderScriptParser::SceneRenderScriptParser(RenderDevice* dev)
			: m_renderDevice(dev)
		{

		}

		// the parsing processing is done recursively as the structure of xml
		// first the outer element, then the inside ScenePass and Commands
		void SceneRenderScriptParser::Parse(const ResourceLocation* rl)
		{
			TiXmlDocument doc;

			doc.Load(rl->GetReadStream(), TIXML_ENCODING_UNKNOWN);

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
						m_sceneName = StringUtils::toWString(elem->Attribute("Name"));
						for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
						{
							BuildNode(i);
						}
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
						LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + strName + L" Node", LOGLVL_Warning);

						for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
						{
							BuildNode(i);
						}
					}
				}
				break;
			}
		}
		void SceneRenderScriptParser::BuildPass(const TiXmlElement* node)
		{
			ScenePassData passData;
			node->Attribute("SelectorID", &passData.SelectorID);
			std::string strName = node->Attribute("Name");
			passData.Name = StringUtils::toWString(strName);

			passData.CameraID=0;
			node->Attribute("CameraID", &passData.CameraID);

			BuildInstructions(node, &passData);
			PassData.Add(passData);
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
						FillInstructions(elem->Attribute("E"), data->Instructions);

						SceneInstruction inst(SOP_JZ);
						data->Instructions.Add(inst);
						
						int32 refIdx = data->Instructions.getCount()-1;
						

						BuildInstructions(elem, data);

						// back fill
						SceneInstruction& instref = data->Instructions[refIdx];
						instref.Next = data->Instructions.getCount();
					}
					else if (lowStrName == String(L"e"))
					{
						FillInstructions(elem->Attribute("S"), data->Instructions);

						SceneOpArg arg;
						if (ParseCallArgRef(elem, "Ret", arg, GlobalVars))
						{
							SceneInstruction inst(SOP_Pop);
							inst.Args.Add(arg);
							data->Instructions.Add(inst);
						}
						else
						{
							data->Instructions.Add(SceneInstruction(SOP_Pop));
						}

					}
					else if (lowStrName == String(L"c"))
					{
						FillFunctionCall(elem, data->Instructions);
					}
					else
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + strName + L" Element", LOGLVL_Warning);
					}

					break;
				}
			}
		}

		void SceneRenderScriptParser::FillFunctionCall(const TiXmlElement* node, List<SceneInstruction>& instructions)
		{
			String stat = StringUtils::toWString(node->Attribute("S"));
			StringUtils::Trim(stat);
			StringUtils::ToLowerCase(stat);
			SceneOpArg zeroArg;
			zeroArg.Var = 0;
			zeroArg.IsImmediate = true;
			zeroArg.StrData = L"0";
			memset(zeroArg.DefaultValue, 0, sizeof(zeroArg.DefaultValue));

			if (stat ==  L"clear")
			{
				SceneInstruction inst;
				inst.Operation = SOP_Clear;

				bool passed = false;
				{
					SceneOpArg arg;
					passed |= ParseCallArgBool(node, "CL_Target", arg, GlobalVars, false);
					inst.Args.Add(arg);

					passed |= ParseCallArgBool(node, "CL_Depth", arg, GlobalVars, false);
					inst.Args.Add(arg);

					passed |= ParseCallArgBool(node, "CL_Stencil", arg, GlobalVars, false);
					inst.Args.Add(arg);


				}


				if (passed)
				{
					SceneOpArg arg;
					ParseCallArgFloat(node, "Depth", arg, GlobalVars, 1.0f);
					inst.Args.Add(arg);

					ParseCallArgInt(node, "Stencil", arg, GlobalVars, 0);
					inst.Args.Add(arg);

					if (ParseCallArgUintHexImm(node, "ClearColorHex", arg))
					{
						inst.Args.Add(arg);
					}
					else 
					{
						ParseCallArgUint(node, "ClearColor", arg, GlobalVars, 0);
						inst.Args.Add(arg);
					}
					
					instructions.Add(inst);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Scene, 
						L"Clear command don't actually clear any thing. Ignored", LOGLVL_Warning);
				}
			}
			else if (stat ==  L"usert")
			{
				SceneInstruction inst;
				inst.Operation = SOP_UseRT;

				SceneOpArg arg;
				ParseCallArgUint(node, "Index", arg, GlobalVars, 0);
				inst.Args.Add(arg);

				if (node->Attribute("RT") && ParseCallArgRef(node, "RT", arg, GlobalVars))
					inst.Args.Add(arg);
				else
					inst.Args.Add(zeroArg);
				
				if (node->Attribute("Mask") && ParseCallArgUintHexImm(node, "Mask", arg))
					inst.Args.Add(arg);
				else
				{
					arg.DefaultValue[0] = 0x1111;
					arg.Var = 0;
					arg.IsImmediate = true;
					arg.StrData = L"";
					inst.Args.Add(arg);
				}

				instructions.Add(inst);
			}
			else if (stat == L"visibleto")
			{
				SceneInstruction inst;
				inst.Operation = SOP_VisibleTo;

				SceneOpArg arg;
				ParseCallArgInt(node, "Selector", arg, GlobalVars, 0);
				inst.Args.Add(arg);

				ParseCallArgRef(node, "Ret", arg, GlobalVars);
				inst.Args.Add(arg);
				instructions.Add(inst);
			}
			else if (stat == L"render")
			{
				SceneInstruction inst;
				inst.Operation = SOP_Render;
				instructions.Add(inst);
			}
			else if (stat == L"renderquad")
			{
				FillRenderQuad(node, instructions);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + stat + L" function", LOGLVL_Warning);
			}

		}

		void SceneRenderScriptParser::FillInstructions(const std::string& cmd, List<SceneInstruction>& instructions)
		{
			ExpressionCompiler compiler;

			compiler.FillInstrunctions(cmd, instructions, GlobalVars);
		}


		void SceneRenderScriptParser::ParseGlocalVarNode(const TiXmlElement* node)
		{
			std::string tstr = node->Attribute("Type");
			std::transform(tstr.begin(), tstr.end(), tstr.begin(), tolower);
			std::string name = node->Attribute("Name");

			if (tstr == "rendertarget")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_RenderTarget;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Width");
				const TiXmlElement* e2 = node->FirstChildElement("Height");

				var->Value[0] = var->Value[1] = 0;
				if (e1 && e2)
				{
					var->Value[0] = StringUtils::ParseInt32(StringUtils::toWString(e1->GetText()));
					var->Value[1] = StringUtils::ParseInt32(StringUtils::toWString(e2->GetText()));
				}
				else
				{
					e1 = node->FirstChildElement("WidthP");
					e2 = node->FirstChildElement("HeightP");

					if (e1 && e2)
					{
						float r = StringUtils::ParseSingle(StringUtils::toWString(e1->GetText()));
						var->Value[2] = reinterpret_cast<const uint&>(r);

						r = StringUtils::ParseSingle(StringUtils::toWString(e2->GetText()));
						var->Value[3] = reinterpret_cast<const uint&>(r);
					}
					else
					{
						float r = 1;
						var->Value[2] = reinterpret_cast<const uint&>(r);
						var->Value[3] = reinterpret_cast<const uint&>(r);
					}
				}

				e1 = node->FirstChildElement("Format");
				if (e1)
				{
					const char* txt = e1->GetText();
					PixelFormat fmt = ConvertFormat(txt ? txt : "");
					var->Value[4] = reinterpret_cast<const uint&>(fmt);
				}
				else
				{
					PixelFormat fmt = m_renderDevice->GetDefaultRTFormat();
					var->Value[4] = reinterpret_cast<const uint&>(fmt);
				}

				e1 = node->FirstChildElement("Depth");
				if (e1)
				{
					const char* txt = e1->GetText();
					DepthFormat fmt = ConvertDepthFormat(txt ? txt : "");
					if (fmt == DEPFMT_Count)
						fmt = m_renderDevice->GetDefaultDepthStencilFormat();
					var->Value[5] = reinterpret_cast<const uint&>(fmt);
				}
				else
				{
					DepthFormat fmt = DEPFMT_Count;// m_renderDevice->GetRenderTarget(0)->getDepthFormat();
					var->Value[5] = reinterpret_cast<const uint&>(fmt);
				}

				e1 = node->FirstChildElement("SampleCount");
				if (e1)
				{
					var->Value[6] = StringUtils::ParseInt32(StringUtils::toWString(e1->GetText()));
				}
				else
				{
					var->Value[6] = 0;
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "matrix")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Matrix;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					List<String> elems(16);
					StringUtils::Split(StringUtils::toWString(e1->GetText()), elems, L" ,");
					if (elems.getCount() != 16)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for matrix.", LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<16;i++)
						{
							float v = StringUtils::ParseSingle(elems[i]);
							var->Value[i] = reinterpret_cast<const uint&>(v);
						}
					}
				}

				e1 = node->FirstChildElement("Bind");
				if (e1)
				{

				}

				GlobalVars.Add(var);
			}
			else if (tstr == "vector4")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Vector4;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					List<String> elems(4);
					StringUtils::Split(StringUtils::toWString(e1->GetText()), elems, L" ,");
					if (elems.getCount() != 4)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector4." + var->Name, LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<4;i++)
						{
							float v = StringUtils::ParseSingle(elems[i]);
							var->Value[i] = reinterpret_cast<const uint&>(v);
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "vector3")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Vector3;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					List<String> elems(4);
					StringUtils::Split(StringUtils::toWString(e1->GetText()), elems, L" ,");
					if (elems.getCount() != 3)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector3." + var->Name, LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<3;i++)
						{
							float v = StringUtils::ParseSingle(elems[i]);
							var->Value[i] = reinterpret_cast<const uint&>(v);
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "vector2")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Vector2;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					List<String> elems;
					StringUtils::Split(StringUtils::toWString(e1->GetText()), elems, L" ,");
					if (elems.getCount() != 2)
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for vector2." + var->Name, LOGLVL_Warning);
					}
					else
					{
						for (size_t i=0;i<2;i++)
						{
							float v = StringUtils::ParseSingle(elems[i]);
							var->Value[i] = reinterpret_cast<const uint&>(v);
						}
					}
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "float")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Single;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					float v = StringUtils::ParseSingle(StringUtils::toWString(e1->GetText()));
					var->Value[0] = reinterpret_cast<const uint&>(v);
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "integer")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Integer;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					int iv = StringUtils::ParseInt32(StringUtils::toWString(e1->GetText()));
					var->Value[0] = reinterpret_cast<const uint&>(iv);
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "boolean")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Boolean;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Value");
				if (e1)
				{
					var->Value[0] = StringUtils::ParseBool(StringUtils::toWString(e1->GetText()));
				}

				GlobalVars.Add(var);
			}
			else if (tstr == "texture")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Texture;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Source");
				if (e1)
				{
					var->DefaultStringValue = StringUtils::toWString(e1->GetText());
					GlobalVars.Add(var);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Scene, L"Texture source not defined for " + var->Name, LOGLVL_Warning);
				}
			}
			//else if (tstr == string("camera"))
			//{
			//	SceneVariable var;
			//	var.Type = VARTYPE_Camera;

			//	GlobalVars.Add(var);
			//}
			else if (tstr == "effect")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_Effect;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("Source");
				if (e1)
				{
					var->DefaultStringValue =  StringUtils::toWString(e1->GetText());
					GlobalVars.Add(var);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Scene, L"Can not find effect name.", LOGLVL_Warning);
				}
			}
			else if (tstr == "gaussblurfilter")
			{
				SceneVariable* var = new SceneVariable();
				var->Type = VARTYPE_GaussBlurFilter;
				var->Name = StringUtils::toWString(name);

				const TiXmlElement* e1 = node->FirstChildElement("MapWidth");
				const TiXmlElement* e2 = node->FirstChildElement("MapHeight");

				var->Value[0] = var->Value[1] = 0;
				if (e1 && e2)
				{
					var->Value[0] = StringUtils::ParseInt32(StringUtils::toWString(e1->GetText()));
					var->Value[1] = StringUtils::ParseInt32(StringUtils::toWString(e2->GetText()));
				}
				else
				{
					e1 = node->FirstChildElement("MapWidthP");
					e2 = node->FirstChildElement("MapHeightP");

					if (e1 && e2)
					{
						float r = StringUtils::ParseSingle(StringUtils::toWString(e1->GetText()));
						var->Value[2] = reinterpret_cast<const uint&>(r);

						r = StringUtils::ParseSingle(StringUtils::toWString(e2->GetText()));
						var->Value[3] = reinterpret_cast<const uint&>(r);
					}
					else
					{
						float r = 1;
						var->Value[2] = reinterpret_cast<const uint&>(r);
						var->Value[3] = reinterpret_cast<const uint&>(r);
					}
				}

				e1 = node->FirstChildElement("BlurAmount");
				if (e1)
				{
					float r = StringUtils::ParseSingle(StringUtils::toWString(e1->GetText()));
					var->Value[4] = reinterpret_cast<const uint&>(r);
				}
				else
				{
					float r = 1;
					var->Value[4] = reinterpret_cast<const uint&>(r);
				}

				e1 = node->FirstChildElement("SampleCount");
				if (e1)
				{
					var->Value[5] = StringUtils::ParseInt32(StringUtils::toWString(e1->GetText()));
				}
				else
				{
					var->Value[5] = 3;
				}
				GlobalVars.Add(var);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Unsupported variable type " + StringUtils::toWString(tstr), LOGLVL_Warning);
			}
		}

		void SceneRenderScriptParser::FillRenderQuad(const TiXmlElement* node, List<SceneInstruction>& instructions)
		{
			SceneInstruction inst;
			inst.Operation = SOP_RenderQuad;

			Effect* effect = 0;

			{
				SceneOpArg arg;
				ParseCallArgVector2(node, "Size", arg, GlobalVars, Vector2Utils::One);
				inst.Args.Add(arg);
			}

			String effectName = StringUtils::toWString(node->Attribute("Effect"));
			{
				SceneOpArg arg;
				arg.IsImmediate = true;

				effect = EffectManager::getSingleton().getEffect(effectName);

				void* ptr = effect;
				memset(arg.DefaultValue, 0, sizeof(arg.DefaultValue));
				memcpy(arg.DefaultValue, &ptr, sizeof(void*));
				inst.Args.Add(arg);
			}
			

			if (effect)
			{
				AutomaticEffect* autoFx = dynamic_cast<AutomaticEffect*>(effect);
				if (autoFx)
				{
					const TiXmlAttribute* att = node->FirstAttribute();
					while (att)
					{
						std::string name = att->Name();
						if (name != "Size" && name != "Effect" && name != "S" && name != "RenderStates")
						{
							std::string value = att->Value();
							std::string::size_type pos = value.find_last_of(':');

							String paramName = StringUtils::toWString(name);
							int idx = autoFx->FindParameterIndex(paramName);

							if (idx!=-1)
							{
								if (pos != std::string::npos)
								{
									SceneOpArg arg;
									arg.IsImmediate = true;

									arg.DefaultValue[0] = (uint)(idx);

									String wvalue = StringUtils::toWString(value);
									pos = wvalue.find_last_of(':');

									String typeString = wvalue.substr(pos+1);
									StringUtils::Trim(typeString);
									StringUtils::ToLowerCase(typeString);


									String valueString = wvalue.substr(0, pos);
									StringUtils::Trim(valueString);
									List<String> vals;
									StringUtils::Split(valueString, vals, L" ,[]()");

									if (vals.getCount() >= 16)
									{
										LogManager::getSingleton().Write(LOG_Scene, L"The parameter " + paramName + L" of " + effectName + L" in scene rendering config is too big .", LOGLVL_Warning);
										continue;
									}

									ScenePostEffectParamType type;
									if (typeString == L"boolean" || typeString == L"bool")
									{
										type = SPFX_TYPE_BOOLS;
									}
									else if (typeString == L"int" || typeString == L"integer")
									{
										type = SPFX_TYPE_INTS;
									}
									else if (typeString == L"single" || typeString == L"float")
									{
										type = SPFX_TYPE_FLOATS;
									}
									else if (StringUtils::StartsWidth(typeString, L"vector"))
									{
										String vDem = typeString.substr(6);
										int dem = StringUtils::ParseInt32(vDem);
										arg.DefaultValue[2] = reinterpret_cast<const uint&>(dem);
										type = SPFX_TYPE_VECTOR;
									}
									else
									{
										LogManager::getSingleton().Write(LOG_Scene, L"Unknown post effect parameter type " + typeString + L".", LOGLVL_Warning);
										continue;
									}

									arg.DefaultValue[1] = ((uint)(SPFX_TYPE_FLOATS) << 16) | (uint)vals.getCount();
									for (int32 i=0;i<vals.getCount();i++)
									{
										switch (type)
										{
										case SPFX_TYPE_FLOATS:
											{
												float d = StringUtils::ParseSingle(vals[i]);
												arg.DefaultValue[i + 2] = reinterpret_cast<const uint&>(d);
											}
											break;
										case SPFX_TYPE_BOOLS:
											{
												bool d = StringUtils::ParseBool(vals[i]);
												arg.DefaultValue[i + 2] = d ? 1 : 0;
											}
											break;
										case SPFX_TYPE_INTS:
											{
												int d = StringUtils::ParseInt32(vals[i]);
												arg.DefaultValue[i + 2] = reinterpret_cast<const uint&>(d);
											}
											break;
										case SPFX_TYPE_VECTOR:
											{
												float d = StringUtils::ParseSingle(vals[i]);
												arg.DefaultValue[i + 3] = reinterpret_cast<const uint&>(d);
											}
											break;
										}
									}

									inst.Args.Add(arg);
								}
								else
								{
									SceneOpArg arg;

									arg.IsImmediate = false;

									if (ParseCallArgAsVar(value,arg,GlobalVars))
									{
										arg.DefaultValue[15] = (uint)(idx);

										inst.Args.Add(arg);
									}
								}
							}
							else
							{
								LogManager::getSingleton().Write(LOG_Graphics, 
									L"Post effect parameter " + paramName + L" is not found in " + effectName, LOGLVL_Warning);
							}

						}
						att = att->Next();
					}
				}

				{
					const char* rs = node->Attribute("RenderStates");
					if (rs)
						ParseCallArgRenderStates(rs, inst.Args);
				}
				instructions.Add(inst);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"Post effect " + effectName + L" is not found. ", LOGLVL_Warning);
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		bool ParseCallArgBool(const TiXmlElement* node,  const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, bool def)
		{
			bool flag;
			int res = node->QueryBoolAttribute(name.c_str(), &flag);
			if (res == TIXML_SUCCESS)
			{
				arg.IsImmediate = true;
				arg.DefaultValue[0] = flag;
				return true;
			}
			if (res == TIXML_WRONG_TYPE)
			{
				return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
			}

			arg.IsImmediate = true;
			arg.DefaultValue[0] = def;

			return false;
		}
		bool ParseCallArgFloat(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, float def)
		{
			float flag;
			int res = node->QueryFloatAttribute(name.c_str(), &flag);
			if (res == TIXML_SUCCESS)
			{
				arg.IsImmediate = true;
				arg.DefaultValue[0] = reinterpret_cast<const uint&>(flag);
				return true;
			}
			if (res == TIXML_WRONG_TYPE)
			{
				return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
			}

			arg.IsImmediate = true;
			arg.DefaultValue[0] = reinterpret_cast<const uint&>(def);

			return false;
		}
		bool ParseCallArgUintHexImm(const TiXmlElement* node, const std::string& name, SceneOpArg& arg)
		{
			uint flag;
			const char* v = node->Attribute(name.c_str());
			if (v)
			{
				flag = StringUtils::ParseUInt32Hex(StringUtils::toWString(v));

				arg.IsImmediate = true;
				arg.DefaultValue[0] = flag;
				return true;

			}
			return false;
		}
		bool ParseCallArgUint(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, uint def)
		{
			uint flag;
			int res = node->QueryUnsignedAttribute(name.c_str(), &flag);
			if (res == TIXML_SUCCESS)
			{
				arg.IsImmediate = true;
				arg.DefaultValue[0] = flag;
				return true;
			}
			if (res == TIXML_WRONG_TYPE)
			{
				return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
			}

			arg.IsImmediate = true;
			arg.DefaultValue[0] = def;

			return false;
		}
		bool ParseCallArgInt(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, int def)
		{
			int flag;
			int res = node->QueryIntAttribute(name.c_str(), &flag);
			if (res == TIXML_SUCCESS)
			{
				arg.IsImmediate = true;
				arg.DefaultValue[0] = reinterpret_cast<const uint&>(def);
				return true;
			}
			if (res == TIXML_WRONG_TYPE)
			{
				return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
			}

			arg.IsImmediate = true;
			arg.DefaultValue[0] = reinterpret_cast<const uint&>(def);

			return false;
		}
		bool ParseCallArgRef(const TiXmlElement* node, const std::string& name, SceneOpArg& arg,
			const FastList<SceneVariable*>& vars)
		{
			return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
		}

		bool ParseCallArgVector2(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars, Vector2 def)
		{
			const std::string* result = node->Attribute(name);

			if (result)
			{
				String str = StringUtils::toWString(*result);
				List<String> comps;
				StringUtils::Split(str, comps, L" ,");
				if (comps.getCount() == 2)
				{
					arg.IsImmediate = true;

					float single = StringUtils::ParseSingle(comps[0]);
					arg.DefaultValue[0] = reinterpret_cast<const uint&>(single);
					single = StringUtils::ParseSingle(comps[1]);
					arg.DefaultValue[1] = reinterpret_cast<const uint&>(single);

					return true;
				}

				return ParseCallArgAsVar(*result,arg,vars);
			}
			{
				float single = Vector2Utils::GetX(def);
				arg.DefaultValue[0] = reinterpret_cast<const uint&>(single);
				single = Vector2Utils::GetY(def);
				arg.DefaultValue[1] = reinterpret_cast<const uint&>(single);

			}
			
			return false;
		}


		bool ParseCallArgAsVar(const std::string& value, SceneOpArg& arg, 
			const FastList<SceneVariable*>& vars)
		{
			//arg.IsImmediate = false;
			//String vname = StringUtils::toWString(node->Attribute(name.c_str()));
			//arg.Var = FindVar(vars, vname);
			//if (!arg.Var)
			//{
			//	LogManager::getSingleton().Write(LOG_Scene, L"Variable " + vname + L" not found", LOGLVL_Warning);
			//	return false;
			//}
			//return true;

			//SceneOpArg arg;

			arg.IsImmediate = false;


			String vname = StringUtils::toWString(value);
			StringUtils::Trim(vname);

			String propName;
			if (StringUtils::StartsWidth(vname, L"[") && StringUtils::EndsWidth(vname, L"]"))
			{
				vname = vname.substr(1, vname.size()-2);

				String::size_type dotPos = vname.find_first_of('.');
				assert(dotPos!=String::npos);

				propName = vname.substr(dotPos+1);
				vname = vname.substr(0, dotPos);
			}

			arg.Var = FindVar(vars, vname);
			if (!arg.Var)
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Variable " + vname + L" not found", LOGLVL_Warning);
				return false;
			}
			arg.StrData = propName;

			return true;
		}

		void ParseCallArgRenderStates(const std::string& value, List<SceneOpArg>& args)
		{
			String val = StringUtils::toWString(value);
			List<String> vals;
			StringUtils::Split(val, vals, L",; ");

			for (int32 i=0;i<vals.getCount();i++)
			{
				String::size_type pos = vals[i].find_first_of('=');
				if (pos != String::npos)
				{
					String k = vals[i].substr(0, pos);
					String v = vals[i].substr(pos+1);

					StringUtils::Trim(k);
					StringUtils::Trim(v);

					if (k == L"IsBlendTransparent" || k == L"DepthTest")
					{
						SceneOpArg arg;
						arg.IsImmediate = true;
						arg.StrData = k;
						arg.DefaultValue[15] = StringUtils::ParseBool(v) ? 1 : 0;
						arg.DefaultValue[0] = arg.DefaultValue[1] = 0xffffffff;
						args.Add(arg);
					}


					// TODO
				}
			}

		}

		PixelFormat ConvertFormat(const std::string& fmt)
		{
			String fmt2(fmt.size(), ' ');
			for (size_t i=0;i<fmt.size();i++)
				fmt2[i] = fmt[i];

			return PixelFormatUtils::ConvertFormat(fmt2);
		}

		DepthFormat ConvertDepthFormat(const std::string& fmt)
		{
			if (fmt.empty())
				return DEPFMT_Count;

			String fmt2(fmt.size(), ' ');
			for (size_t i=0;i<fmt.size();i++)
				fmt2[i] = fmt[i];

			return PixelFormatUtils::ConvertDepthFormat(fmt2);
		}
	}
}