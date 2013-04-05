#include "ExpressionCompiler.h"
#include "Stack.h"
#include "Expression.h"
#include "Datum.h"
#include "Scope.h"
#include "StringUtils.h"

namespace os
{
	
	static const OperatorDef OpDefinitions[] =
	{
		{ OperatorDef::OP_Add,		'+', 4, 3, true, true  },
		{ OperatorDef::OP_Sub,		'-', 4, 3, true, true  },
		{ OperatorDef::OP_Multiply,	'*', 6, 5, true, true  },
		{ OperatorDef::OP_Divide,	'/', 6, 5, true, true  },
		{ OperatorDef::OP_Negate,	'-', 8, 7, false, true  },

		{ OperatorDef::OP_And,		'&', 10, 9, true, true  },
		{ OperatorDef::OP_Or,		'|', 10, 9, true, true  },
		{ OperatorDef::OP_Not,		'!', 12, 11, false, true  },

		{ OperatorDef::OP_LeftBracket,	'(', 0, 13, false, false  },
		{ OperatorDef::OP_RightBracket,	')', 14, 0, false, false  },
		{ OperatorDef::OP_LeftIndexer,	'[', 0, 15, false, false  },
		{ OperatorDef::OP_RightIndexer,	']', 16, 0, false, false  },

		{ OperatorDef::OP_Comma,	',', 2, 1, true, false  },

		// the operator chars for call & indexer are not real, the chars are only placeholders
		{ OperatorDef::OP_Call,		'$', 12, 11, true, true  },
		{ OperatorDef::OP_CallIndexer,		'#', 12, 11, true, true  },

		{ OperatorDef::OP_Assignment,		'=', 2, 1, true, true  }
	};

	class Helper
	{
	public:
		Helper()
		{
			memset(m_isOperatorChar, 0, sizeof(m_isOperatorChar));
			
			int32 count = sizeof(OpDefinitions)  / sizeof(OpDefinitions[0]);
			for (int32 i=0;i<count;i++)
			{
				m_isOperatorChar[OpDefinitions[i].OperatorChar] = true;
			}

			for (int32 i=0;i<256;i++)
				m_opCharToIndex[i] = -1;

			for (int32 i=0;i<count;i++)
			{
				m_opCharToIndex[OpDefinitions[i].OperatorChar] = (char)i;
			}

			memset(m_operatorToOPC, 0, sizeof(m_operatorToOPC));

			m_operatorToOPC[OperatorDef::OP_Add] = Expression::OPC_Add;
			m_operatorToOPC[OperatorDef::OP_Sub] = Expression::OPC_Sub;
			m_operatorToOPC[OperatorDef::OP_Multiply] = Expression::OPC_Multiply;
			m_operatorToOPC[OperatorDef::OP_Divide] = Expression::OPC_Divide;
			m_operatorToOPC[OperatorDef::OP_Negate] = Expression::OPC_Negate;

			m_operatorToOPC[OperatorDef::OP_And] = Expression::OPC_And;
			m_operatorToOPC[OperatorDef::OP_Or] = Expression::OPC_Or;
			m_operatorToOPC[OperatorDef::OP_Not] = Expression::OPC_Not;

			m_operatorToOPC[OperatorDef::OP_Call] = Expression::OPC_Call;
			m_operatorToOPC[OperatorDef::OP_CallIndexer] = Expression::OPC_Indexer;

			m_operatorToOPC[OperatorDef::OP_Assignment] = Expression::OPC_Set;
		}

		bool isOperatorChar(wchar_t ch)
		{
			if (ch == OpDefinitions[OperatorDef::OP_Call].OperatorChar)
				return false;

			return m_isOperatorChar[ch];
		}
		/**
		 *  Get operator definition from char. 
		 *  Note: for some operator like negate it conflict with subtract, 
		 *  but special case handling is done in the compiler.
		 */
		const OperatorDef* getDef(wchar_t ch)
		{
			int32 indx = m_opCharToIndex[ch];
			if (indx!=-1)
			{
				return &OpDefinitions[indx];
			}
			return nullptr;
		}

		/**
		 *  Get the corresponding opcode of an operator.
		 */
		Expression::OpCode GetCode(OperatorDef::OperatorType type)
		{
			return m_operatorToOPC[type];
		}
		
	private:
		bool m_isOperatorChar[256];
		char m_opCharToIndex[256];
		Expression::OpCode m_operatorToOPC[sizeof(OpDefinitions)  / sizeof(OpDefinitions[0])];

	};
	static Helper HelperInstance;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	Expression* ExpressionCompiler::Compile(const String& expression, Scope* runtime)
	{
		m_tokenPosition = 0;
		m_expressionTokens.Clear();

		ParseTokens(expression);

		ExpressionNode* tree = CreateExpressionTree();

		Expression* result = new Expression();
		Traverse(tree, runtime, result);

		delete tree;

		return result;
	}

	ExpressionNode* ExpressionCompiler::CreateExpressionTree()
	{
		Stack<ExpressionNode*> expressionStack;
		Stack<const OperatorDef*> operatorStack;
		operatorStack.Push(nullptr);

		do 
		{
			const String& token = m_expressionTokens[m_tokenPosition];

			bool isOperator = token.size() == 1 && HelperInstance.isOperatorChar(token[0]);
			bool isFunctionCallName = false;
			bool isFunctionCallIndexer = false;

			if (!isOperator)
			{
				// maybe func call
				if (m_tokenPosition+1<(int32)m_expressionTokens.Size())
				{
					const String& nextToken = m_expressionTokens[m_tokenPosition+1];

					if (nextToken.size() == 1)
					{
						isFunctionCallIndexer = OpDefinitions[OperatorDef::OP_LeftIndexer].OperatorChar == nextToken[0];
						isFunctionCallName = isFunctionCallIndexer || OpDefinitions[OperatorDef::OP_LeftBracket].OperatorChar == nextToken[0];
					}
				}
			}

			if (!isOperator || isFunctionCallName)
			{
				// variables
				expressionStack.Push(new ExpressionNode(token));
				m_tokenPosition++;

				if (isFunctionCallName)
				{
					// an additional "func call" operator is added
					operatorStack.Push(&OpDefinitions[isFunctionCallIndexer ? OperatorDef::OP_CallIndexer : OperatorDef::OP_Call]);
				}
			}
			else
			{
				const OperatorDef* leftOpDef = nullptr;
				if (operatorStack.Size())
				{
					leftOpDef = operatorStack.Top();
				}

				const OperatorDef* currentOpDef = HelperInstance.getDef(token[0]);
				assert(currentOpDef);

				// special case handing
				if (currentOpDef->Type == OperatorDef::OP_Sub || currentOpDef->Type == OperatorDef::OP_Negate)
				{
					// check which one the op is
					if (m_tokenPosition>0)
					{
						const String& lastToken = m_expressionTokens[m_tokenPosition-1];
						bool isLastTokenOperator = lastToken.size() == 1 && HelperInstance.isOperatorChar(lastToken[0]);

						currentOpDef = isLastTokenOperator ? &OpDefinitions[OperatorDef::OP_Negate] : &OpDefinitions[OperatorDef::OP_Sub];
					}
					else
						currentOpDef = &OpDefinitions[OperatorDef::OP_Negate];
				}

				if (ProcessOperator(leftOpDef, currentOpDef, operatorStack, expressionStack))
				{
					m_tokenPosition++;
				}
			}

		} while (m_tokenPosition<(int32)m_expressionTokens.Size());

		// end operators
		while (operatorStack.Size() && operatorStack.Top() != nullptr)
		{
			ProcessOperator(operatorStack.Top(), nullptr, operatorStack, expressionStack);
		}

		assert(expressionStack.Size() == 1);

		return expressionStack.Top();
	}

	bool ExpressionCompiler::ProcessOperator(const OperatorDef* ol, const OperatorDef* or, 
		Stack<const OperatorDef*>& operatorStack, Stack<ExpressionNode*>& expressionStack)
	{
		int32 leftPriority = ol ? ol->PriorityLeft : 0;
		int32 rightPriority = or ? or->PriorityRight : 0;

		int32 diff = leftPriority - rightPriority;
		bool result = false;

		if (diff == 0)
		{
			// this ordinarily happen for brackets as they pairs based on their priority configuration
			// Note: watch out for incorrect operator priority configuration.
			operatorStack.Pop();
			result = true;
		}
		else if (diff < 0)
		{
			operatorStack.Push(or);
			result = true;
		}
		else if (diff > 0)
		{
			// decreased level: expression part can be emitted.

			const OperatorDef* leftOp = operatorStack.Pop();
			assert(leftOp);

			ExpressionNode* newNode;
			if (!leftOp->IsBinary)
			{
				ExpressionNode* opnd2 = expressionStack.Pop();
				newNode = new ExpressionNode(*leftOp, opnd2, nullptr);
			}
			else
			{
				ExpressionNode* opnd2 = expressionStack.Pop();
				ExpressionNode* opnd1 = expressionStack.Pop();

				newNode = new ExpressionNode(*leftOp, opnd1, opnd2);
			}

			expressionStack.Push(newNode);
		}
		return result;
	}

	void ExpressionCompiler::ParseTokens(const String& expression)
	{
		String current;
		bool ended = false;

		for (size_t i=0;i<expression.length();i++)
		{
			wchar_t curChar = expression[i];
			if (curChar == '\t' || curChar == ' ')
				continue;

			if (HelperInstance.isOperatorChar(curChar))
			{
				if (current.size())
				{
					String varName = current;
					m_expressionTokens.PushBack(varName);
				}

				m_expressionTokens.PushBack(String(1, curChar));

				ended = true;

				current.clear();
			}
			else
			{
				current.append(1, curChar);
				ended = false;
			}
		}

		if (!ended)
		{
			String varName = current;
			m_expressionTokens.PushBack(varName);
		}
	}

	bool ExpressionCompiler::Traverse(ExpressionNode* node, Scope* runtime, Expression* result)
	{
		if (!node)
			return true;

		if (node->Type == ExpressionNode::EN_Operator)
		{
			if (Traverse(node->Left, runtime, result))
			{
				if (Traverse(node->Right, runtime, result))
				{
					if (node->Operator->EmitOpcode)
					{
						result->m_instructions.PushBack(
							new Expression::Instruction(HelperInstance.GetCode(node->Operator->Type),
							nullptr, false, nullptr, false));
					}
					else
					{
						assert(1);
					}

					return true;
				}
			}
			return false;
		}
		else
		{
			bool isLeftImm = false;
			Datum* leftOp = ResolveName(node->VarableName, runtime, isLeftImm);

			Expression::Instruction* inst = new Expression::Instruction(Expression::OPC_Push,
				leftOp, isLeftImm, nullptr, false);
			result->m_instructions.PushBack(inst);

			return true;
		}
	}

	Datum* ExpressionCompiler::ResolveName(const String& token, Scope* runtime, bool& isImmediate)
	{
		String text = token;
		StringUtils::Trim(text);

		// fixed functions with ExpresssionScriptLibrary
		if (StringUtils::StartsWidth(text, L"sf::"))
		{
			String funcName = text.substr(4);
			Datum* desc = ExpresssionScriptLibrary::GetFunctionDescriptor(funcName);
			if (desc)
				return desc;
		}
		else
		{
			Datum::Type immediateType = Datum::DAT_Unknown;

			int32 dotCount = 0;

			//////////////////////////////////////////////////////////////////////////
			// Immediate values
			
			// string with ""
			if (text.size()>2 && StringUtils::StartsWidth(text, L"\"") && StringUtils::EndsWidth(text, L"\""))
			{
				immediateType = Datum::DAT_String;

				Datum* dat = new Datum();
				dat->Set(text.substr(1, text.size()-2), 0);
				isImmediate = true;

				return dat;
			}
			else
			{
				// float point value format (.f)
				int32 scanLen = (int32)text.size();
				if (StringUtils::EndsWidth(text, L"f", true))
				{
					if (text.size()>1)
					{
						scanLen--;
					}
				}

				bool isAllNumbers = true;
				for (int32 i=0;i<scanLen;i++)
				{
					if (text[i] == '.')
						dotCount++;
					else if (text[i] < '0' || text[i] > '9')
						isAllNumbers = false;
				}

				if (isAllNumbers && dotCount == 1)
				{
					immediateType = Datum::DAT_Float;

					Datum* dat = new Datum();
					dat->Set(StringUtils::ParseSingle(text.substr(0, scanLen)), 0);
					isImmediate = true;

					return dat;
				}
				else if (isAllNumbers && dotCount == 0)
				{
					immediateType = Datum::DAT_Int32;

					Datum* dat = new Datum();
					dat->Set(StringUtils::ParseInt32(text), 0);
					isImmediate = true;

					return dat;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// References

			if (immediateType == Datum::DAT_Unknown)
			{
				Datum* result = nullptr;

				// look for scope members
				if (dotCount == 0)
				{
					result = runtime->Find(text);
				}
				else
				{
					Scope* currentScope = runtime;
					Vector<String> names = StringUtils::Split(text, L".");
					for (uint32 i=0;i<names.Size();i++)
					{
						StringUtils::Trim(names[i]);

						if (i == names.Size() -1)
						{
							result = currentScope->Find(names[i]);
						}
						else
						{
							Datum* dat = currentScope->Find(names[i]);
							assert(dat->GetType() == Datum::DAT_Table);

							currentScope = dat->Get<Scope*>(0);
						}
					}
				}

				if (result!=nullptr)
					return result;

				throw Exception(L"Undefined");
			}
		}

		throw Exception(L"Undefined");
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	ExpresssionScriptLibrary::FunctionTable ExpresssionScriptLibrary::m_lookupTable(20, true);
	
	class SFSin : public ExpresssionScriptLibrary::SFFunction
	{
		RTTI_DECLARATIONS(SFSin, SFFunction);
	public:
		SFSin() : SFFunction(L"sin", 1) { }

		virtual Datum* Invoke(Datum** parameters)
		{
			float inputVal = 0;
			bool typeOk = false;

			if (parameters[0]->GetType() == Datum::DAT_Float)
			{
				inputVal = parameters[0]->Get<float>(0);
				typeOk = true;
			}
			else if (parameters[0]->GetType() == Datum::DAT_Int32)
			{
				inputVal = ((float)parameters[0]->Get<int>(0));
				typeOk = true;
			}

			assert(typeOk);

			Datum* result = new Datum();
			result->SetType(Datum::DAT_Float);
			result->Set(sinf(inputVal));
			return result;
		}
	};

	void ExpresssionScriptLibrary::Initialize()
	{
		if (m_lookupTable.Size())
			return;

		SFFunction* sf = new SFSin();
		Datum* dat = new Datum();
		dat->Set(sf,0);
		m_lookupTable.Insert(std::make_pair(sf->getName(), std::make_pair(sf, dat)));
	}
	void ExpresssionScriptLibrary::Finalize()
	{
		for (FunctionTable::Iterator iter = m_lookupTable.Begin(); iter != m_lookupTable.End(); ++iter)
		{
			SFFunction* sf = iter->second.first;
			Datum* dat = iter->second.second;	

			delete sf;
			delete dat;
		}
		m_lookupTable.Clear();
	}

	void ExpresssionScriptLibrary::RegisterFunction(SFFunction* func)
	{
		Datum* dat = new Datum();
		dat->Set(func,0);
		m_lookupTable.Insert(std::make_pair(func->getName(), std::make_pair(func, dat)));
	}
	void ExpresssionScriptLibrary::UnregisterFunction(SFFunction* func)
	{
		FunctionTable::Iterator iter = m_lookupTable.Find(func->getName());
		
		Datum* dat = iter->second.second;	

		delete dat;

		m_lookupTable.Remove(func->getName());
	}

	Datum* ExpresssionScriptLibrary::Invoke(const String& funcName, Datum** parameters)
	{
		FunctionTable::Iterator iter = m_lookupTable.Find(funcName);
		if (iter != m_lookupTable.End())
		{
			return iter->second.first->Invoke(parameters);
		}
		throw Exception(L"Not supported");
	}
	Datum* ExpresssionScriptLibrary::GetFunctionDescriptor(const String& funcName)
	{
		FunctionTable::Iterator iter = m_lookupTable.Find(funcName);
		if (iter != m_lookupTable.End())
		{
			return iter->second.second;
		}
		return nullptr;
	}

}