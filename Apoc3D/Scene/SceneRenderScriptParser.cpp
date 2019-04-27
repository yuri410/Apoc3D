/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "SceneRenderScriptParser.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Collections/Stack.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Math/GaussBlurFilter.h"
#include "apoc3d/Library/tinyxml.h"
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
		const int NUM = 256;
		const char LBracket = '(';


		const char OpAnd = '&';
		const char OpOr = '|';
		const char OpNot = '!';
		//const char OpAt = '@';

		const char RBracket = ')';

		const char EndSym = '$';

		struct PriorInfo
		{
			char OperatorChar;
			// left side priority
			int P1;
			// right side priority
			int P2;
		};
		const PriorInfo pList[6] = {
			{ OpAnd, 2, 1 },
			{ OpOr, 2, 1 },
			{ OpNot, 4, 3 },
			//{ OpAt, 4, 5 },
			{ LBracket, 0, 5 },
			{ RBracket, 6, 0 },
			{ EndSym, 0, 0 }
		};


		bool ParseCallArgAsVar(const String& value, SceneOpArg& arg, const List<SceneVariable*>& vars);
		bool ParseCallArgAsVar(const std::string& value, SceneOpArg& arg, const List<SceneVariable*>& vars);

		bool ParseCallArgBool(const TiXmlElement* node,  const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, bool def);
		bool ParseCallArgFloat(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, float def);
		bool ParseCallArgUint(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, uint def);
		bool ParseCallArgInt(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, int def);
		bool ParseCallArgRef(const TiXmlElement* node, const std::string& name, SceneOpArg& arg,
			const List<SceneVariable*>& vars);
		bool ParseCallArgUintHexImm(const TiXmlElement* node, const std::string& name, SceneOpArg& arg);
		bool ParseCallArgVector2(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, Vector2 def);
		void ParseCallArgRenderStates(const std::string& value, List<SceneOpArg>& args);

		PixelFormat ConvertFormat(const std::string& fmt);
		DepthFormat ConvertDepthFormat(const std::string& fmt);


		SceneVariable* FindVar(const List<SceneVariable*>& vars, const String& name)
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
					AP_EXCEPTION(ErrorID::InvalidOperation, L"operator error!");
					return '=';
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

			bool FollowOrderTraverse(ExpressionNode* T, List<SceneInstruction>& insts, const List<SceneVariable*>& vars)
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
					
					String token = StringUtils::UTF8toUTF16( paramList[T->m_opnd] );
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

			void Parse(const std::string& expression)
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
			void FillInstrunctions(const std::string& expression, List<SceneInstruction>& insts, const List<SceneVariable*>& vars)
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
		void SceneRenderScriptParser::Parse(const ResourceLocation& rl)
		{
			TiXmlDocument doc;

			Stream* strm = rl.GetReadStream();
			doc.Load(*strm, TIXML_ENCODING_UNKNOWN);
			delete strm;

			for (const TiXmlNode* i = doc.FirstChild(); i!=0; i=i->NextSibling())
			{
				BuildNode(i, doc);
			}
		}

		void SceneRenderScriptParser::BuildNode(const TiXmlNode* node, const TiXmlDocument& doc)
		{
			int type = node->Type();

			switch (type)
			{
			case TiXmlNode::TINYXML_ELEMENT:
				{
					const TiXmlElement* elem = node->ToElement();

					String strName = doc.GetUTF16ElementName(elem);

					String lowStrName = strName;
					StringUtils::ToLowerCase(lowStrName);

					if (lowStrName == String(L"scene"))
					{
						m_sceneName = doc.GetUTF16AttribValue(elem, "Name");

						for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
						{
							BuildNode(i, doc);
						}
					}
					else if (lowStrName == String(L"pass"))
					{
						BuildPass(elem, doc);
					}
					else if (lowStrName == String(L"declare"))
					{
						ParseGlocalVarNode(elem, doc);
					}
					else
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + strName + L" Node", LOGLVL_Warning);

						for (const TiXmlNode* i = elem->FirstChild(); i!=0; i=i->NextSibling())
						{
							BuildNode(i, doc);
						}
					}
				}
				break;
			}
		}
		void SceneRenderScriptParser::BuildPass(const TiXmlElement* node, const TiXmlDocument& doc)
		{
			ScenePassData passData;
			
			passData.Name = doc.GetUTF16AttribValue(node, "Name");

			if (node->Attribute("SelectorID", &passData.SelectorID) == nullptr)
				ApocLog(LOG_Scene, L"[" + passData.Name + L"] Pass configuration missing selector ID.", LOGLVL_Error);

			passData.CameraID=0;
			node->Attribute("CameraID", &passData.CameraID);

			BuildInstructions(node, &passData, doc);
			PassData.Add(passData);
		}
		void SceneRenderScriptParser::BuildInstructions(const TiXmlElement* node, ScenePassData* data, const TiXmlDocument& doc)
		{
			for (const TiXmlNode* i = node->FirstChild(); i!=0; i=i->NextSibling())
			{
				int type = i->Type();
				switch (type)
				{
				case TiXmlNode::TINYXML_ELEMENT:
					const TiXmlElement* elem = i->ToElement();

					String strName = doc.GetUTF16ElementName(elem);

					String lowStrName = strName;
					StringUtils::ToLowerCase(lowStrName);

					if (lowStrName == String(L"if"))
					{
						FillInstructions(elem->Attribute("E"), data->Instructions);

						SceneInstruction inst(SOP_JZ);
						data->Instructions.Add(inst);
						
						int32 refIdx = data->Instructions.getCount()-1;
						

						BuildInstructions(elem, data, doc);

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
						FillFunctionCall(elem, data->Instructions, doc);
					}
					else
					{
						LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + strName + L" Element", LOGLVL_Warning);
					}

					break;
				}
			}
		}

		void SceneRenderScriptParser::FillFunctionCall(const TiXmlElement* node, List<SceneInstruction>& instructions, const TiXmlDocument& doc)
		{
			String stat = doc.GetUTF16AttribValue(node, "S");
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
				
				instructions.Add(inst);
			}
			else if (stat == L"useds")
			{
				SceneInstruction inst;
				inst.Operation = SOP_UseDS;

				SceneOpArg arg;
				if (node->Attribute("DS") && ParseCallArgRef(node, "DS", arg, GlobalVars))
					inst.Args.Add(arg);
				else
					inst.Args.Add(zeroArg);

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
				FillRenderQuad(node, instructions, doc);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Unknown " + stat + L" function", LOGLVL_Warning);
			}

		}

		void SceneRenderScriptParser::FillInstructions(const std::string& cmd, List<SceneInstruction>& instructions)
		{
			ExpressionCompiler().FillInstrunctions(cmd, instructions, GlobalVars);
		}

		void SceneRenderScriptParser::ParseGlocalVarNode(const TiXmlElement* node, const TiXmlDocument& doc)
		{
			String name = doc.GetUTF16AttribValue(node, "Name");
			String stype = doc.GetUTF16AttribValue(node, "Type");

			SceneVariableType type;
			if (SceneVariableTypeConverter.TryParse(stype, type))
			{
				SceneVariable* var = new SceneVariable();
				var->Name = name;
				var->Type = type;

				bool failed = false;

				switch (type)
				{
					case SceneVariableType::RenderTarget:
					case SceneVariableType::DepthStencil:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Width");
						const TiXmlElement* e2 = node->FirstChildElement("Height");

						if (e1 && e2)
						{
							var->Value[0] = StringUtils::ParseInt32(e1->GetText());
							var->Value[1] = StringUtils::ParseInt32(e2->GetText());
						}
						else
						{
							e1 = node->FirstChildElement("WidthP");
							e2 = node->FirstChildElement("HeightP");

							if (e1 && e2)
							{
								float r = StringUtils::ParseSingle(e1->GetText());
								var->Value[2] = reinterpret_cast<const uint&>(r);

								r = StringUtils::ParseSingle(e2->GetText());
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
						if (type == SceneVariableType::RenderTarget)
						{
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
						}
						else
						{
							if (e1)
							{
								const char* txt = e1->GetText();
								DepthFormat fmt = ConvertDepthFormat(txt ? txt : "");
								var->Value[4] = reinterpret_cast<const uint&>(fmt);
							}
							else
							{
								DepthFormat fmt = m_renderDevice->GetDefaultDepthStencilFormat();
								var->Value[4] = reinterpret_cast<const uint&>(fmt);
							}
						}

						e1 = node->FirstChildElement("SampleCount");
						if (e1)
						{
							var->Value[5] = StringUtils::ParseInt32(e1->GetText());
						}

						break;
					}
					case SceneVariableType::Matrix:
					case SceneVariableType::Vector4:
					case SceneVariableType::Vector3:
					case SceneVariableType::Vector2:
					case SceneVariableType::Single:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Value");
						if (e1)
						{
							List<float> elems = StringUtils::SplitParseSingles(e1->GetText(), " ,");

							int32 expectedElemCount = 1;
							if (type == SceneVariableType::Matrix)
								expectedElemCount = 16;
							else if (type == SceneVariableType::Vector4)
								expectedElemCount = 4;
							else if (type == SceneVariableType::Vector3)
								expectedElemCount = 3;
							else if (type == SceneVariableType::Vector2)
								expectedElemCount = 2;

							if (elems.getCount() != expectedElemCount)
							{
								LogManager::getSingleton().Write(LOG_Scene, L"Invalid value for " + stype + L":(" + var->Name + L")", LOGLVL_Warning);
							}
							else
							{
								for (int32 i = 0; i < expectedElemCount; i++)
								{
									var->Value[i] = reinterpret_cast<const uint&>(elems[i]);
								}
							}
						}

						break;
					}
					case SceneVariableType::Texture:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Source");
						if (e1)
						{
							var->DefaultStringValue = doc.GetUTF16ElementText(e1);
						}
						else
						{
							failed = true;
							LogManager::getSingleton().Write(LOG_Scene, L"Texture source not defined for " + var->Name, LOGLVL_Warning);
						}
						break;
					}
					case SceneVariableType::Integer:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Value");
						if (e1)
						{
							int iv = StringUtils::ParseInt32(e1->GetText());
							var->Value[0] = reinterpret_cast<const uint&>(iv);
						}
						break;
					}
					case SceneVariableType::Boolean:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Value");
						if (e1)
						{
							var->Value[0] = StringUtils::ParseBool(e1->GetText());
						}
						break;
					}
					case SceneVariableType::Effect:
					{
						const TiXmlElement* e1 = node->FirstChildElement("Source");
						if (e1)
						{
							var->DefaultStringValue = doc.GetUTF16ElementText(e1);
						}
						else
						{
							failed = true;
							LogManager::getSingleton().Write(LOG_Scene, L"Can not find effect name.", LOGLVL_Warning);
						}
						break;
					}
					case SceneVariableType::GaussBlurFilter:
					{
						const TiXmlElement* e1 = node->FirstChildElement("MapWidth");
						const TiXmlElement* e2 = node->FirstChildElement("MapHeight");

						if (e1 && e2)
						{
							var->Value[0] = StringUtils::ParseInt32(e1->GetText());
							var->Value[1] = StringUtils::ParseInt32(e2->GetText());
						}
						else
						{
							e1 = node->FirstChildElement("MapWidthP");
							e2 = node->FirstChildElement("MapHeightP");

							if (e1 && e2)
							{
								float r = StringUtils::ParseSingle(e1->GetText());
								var->Value[2] = reinterpret_cast<const uint&>(r);

								r = StringUtils::ParseSingle(e2->GetText());
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
							float r = StringUtils::ParseSingle(e1->GetText());
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
							var->Value[5] = StringUtils::ParseInt32(e1->GetText());
						}
						else
						{
							var->Value[5] = 3;
						}
						break;
					}
					default:
						assert(0);
				}

				if (!failed)
					GlobalVars.Add(var);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Scene, L"Unsupported variable type " + stype, LOGLVL_Warning);
			}
		}

		void SceneRenderScriptParser::FillRenderQuad(const TiXmlElement* node, List<SceneInstruction>& instructions, const TiXmlDocument& doc)
		{
			SceneInstruction inst;
			inst.Operation = SOP_RenderQuad;

			Effect* effect = 0;

			{
				SceneOpArg arg;
				ParseCallArgVector2(node, "Size", arg, GlobalVars, Vector2::One);
				inst.Args.Add(arg);
			}

			String effectName = doc.GetUTF16AttribValue(node, "Effect");
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
				AutomaticEffect* autoFx = up_cast<AutomaticEffect*>(effect);
				if (autoFx)
				{
					const TiXmlAttribute* att = node->FirstAttribute();
					while (att)
					{
						String name = doc.GetUTF16AttribName(att);
						if (name != L"Size" && name != L"Effect" && name != L"S" && name != L"RenderStates")
						{
							String value = doc.GetUTF16AttribValue(att);
							String::size_type pos = value.find_last_of(':');

							String paramName = name;
							int idx = autoFx->FindParameterIndex(paramName);

							if (idx!=-1)
							{
								if (pos != std::string::npos)
								{
									SceneOpArg arg;
									arg.IsImmediate = true;

									arg.DefaultValue[0] = (uint)(idx);

									String wvalue = value;
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
									else if (StringUtils::StartsWith(typeString, L"vector"))
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
			const List<SceneVariable*>& vars, bool def)
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
			const List<SceneVariable*>& vars, float def)
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
				flag = StringUtils::ParseUInt32Hex(v);

				arg.IsImmediate = true;
				arg.DefaultValue[0] = flag;
				return true;

			}
			return false;
		}
		bool ParseCallArgUint(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, uint def)
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
			const List<SceneVariable*>& vars, int def)
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
			const List<SceneVariable*>& vars)
		{
			return ParseCallArgAsVar(*node->Attribute(name), arg,vars);
		}

		bool ParseCallArgVector2(const TiXmlElement* node, const std::string& name, SceneOpArg& arg, 
			const List<SceneVariable*>& vars, Vector2 def)
		{
			const std::string* result = node->Attribute(name);

			if (result)
			{
				List<std::string> comps = StringUtils::Split(*result, " ,");
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
				float single = def.X;
				arg.DefaultValue[0] = reinterpret_cast<const uint&>(single);
				single = def.Y;
				arg.DefaultValue[1] = reinterpret_cast<const uint&>(single);
			}
			
			return false;
		}


		bool ParseCallArgAsVar(const String& value, SceneOpArg& arg, 
			const List<SceneVariable*>& vars)
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


			String vname = value;//StringUtils::UTF8toUTF16(value);
			StringUtils::Trim(vname);

			String propName;
			if (StringUtils::StartsWith(vname, L"[") && StringUtils::EndsWith(vname, L"]"))
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

		bool ParseCallArgAsVar(const std::string& value, SceneOpArg& arg, const List<SceneVariable*>& vars)
		{
			arg.IsImmediate = false;

			String vname = StringUtils::UTF8toUTF16(value);
			StringUtils::Trim(vname);

			String propName;
			if (StringUtils::StartsWith(vname, L"[") && StringUtils::EndsWith(vname, L"]"))
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
			String val = StringUtils::UTF8toUTF16(value);
			List<String> vals;
			StringUtils::Split(val, vals, L",; ");

			for (int32 i = 0; i < vals.getCount(); i++)
			{
				String::size_type pos = vals[i].find_first_of('=');
				if (pos != String::npos)
				{
					String k = vals[i].substr(0, pos);
					String v = vals[i].substr(pos + 1);

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
			for (size_t i = 0; i < fmt.size(); i++)
				fmt2[i] = fmt[i];

			return PixelFormatUtils::ConvertFormat(fmt2);
		}

		DepthFormat ConvertDepthFormat(const std::string& fmt)
		{
			if (fmt.empty())
				return DEPFMT_Count;

			String fmt2(fmt.size(), ' ');
			for (size_t i = 0; i < fmt.size(); i++)
				fmt2[i] = fmt[i];

			return PixelFormatUtils::ConvertDepthFormat(fmt2);
		}
	}
}