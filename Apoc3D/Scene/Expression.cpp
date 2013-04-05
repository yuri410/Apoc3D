#include "Expression.h"
#include "Datum.h"
#include "Scope.h"
#include "Exception.h"
#include "StringUtils.h"
#include "ExpressionCompiler.h"

namespace os
{
	Expression::Expression()
	{
		
	}
	Expression::~Expression()
	{
		ResetStack();
		while (m_evaluationStack.Size())
		{
			EvaluationStackItem d = m_evaluationStack.Pop();
			if (d.IsOwned)
				delete d.Data;
		}
		for (uint32 i=0;i<m_instructions.Size();i++)
		{
			delete m_instructions[i];
		}
	}
	Datum* Expression::Invoke()
	{
		ResetStack();

		for (uint32 i=0;i<m_instructions.Size();i++)
		{
			const Instruction* inst = m_instructions[i];

			// if these are used, they will be checked each instruction to release memory
			EvaluationStackItem opndR = { false, nullptr };
			EvaluationStackItem opndL = { false, nullptr };

			switch (inst->Type)
			{
			case OPC_Push:
				{
					assert(inst->OperandLeft);

					EvaluationStackItem r = { false, inst->OperandLeft };
					m_evaluationStack.Push(r);
					break;
				}
			case OPC_Add:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) + opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) + opndR.Data->Get<float>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) + opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<int32>(0) + opndR.Data->Get<float>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_String || opndR.Data->GetType() == Datum::DAT_String)
					{
						String leftString;
						if (opndL.Data->GetType() == Datum::DAT_String)
						{
							leftString = opndL.Data->Get<String>(0);
						}
						else if (opndL.Data->GetType() == Datum::DAT_Float)
						{
							leftString = StringUtils::ToString(opndL.Data->Get<float>(0), 5, 0, ' ', ios::fixed);
						}
						else if (opndL.Data->GetType() == Datum::DAT_Int32)
						{
							leftString = StringUtils::ToString(opndL.Data->Get<int>(0));
						}

						String rightString;
						if (opndR.Data->GetType() == Datum::DAT_String)
						{
							rightString = opndR.Data->Get<String>(0);
						}
						else if (opndR.Data->GetType() == Datum::DAT_Float)
						{
							rightString = StringUtils::ToString(opndR.Data->Get<float>(0), 5, 0, ' ', ios::fixed);
						}
						else if (opndR.Data->GetType() == Datum::DAT_Int32)
						{
							rightString = StringUtils::ToString(opndR.Data->Get<int>(0));
						}

						dat = new Datum();
						dat->Set(leftString+rightString);
					}

					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Sub:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) - opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) - opndR.Data->Get<float>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) - opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<int32>(0) - opndR.Data->Get<float>(0)));
					}

					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Multiply:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) * opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) * opndR.Data->Get<float>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) * opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<int32>(0) * opndR.Data->Get<float>(0)));
					}
					
					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Divide:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) / opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) / opndR.Data->Get<float>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<float>(0) / opndR.Data->Get<int32>(0)));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set((float)(opndL.Data->Get<int32>(0) / opndR.Data->Get<float>(0)));
					}

					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Negate:
				{
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set(-opndL.Data->Get<int32>(0));
					}
					else if (opndL.Data->GetType() == Datum::DAT_Float)
					{
						dat = new Datum();
						dat->Set(-opndL.Data->Get<float>(0));
					}

					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}

			case OPC_And:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) & opndR.Data->Get<int32>(0)));
					}
					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Or:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32 && opndR.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set((int32)(opndL.Data->Get<int32>(0) | opndR.Data->Get<int32>(0)));
					}
					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Not:
				{
					opndL = m_evaluationStack.Pop();

					Datum* dat = nullptr;
					if (opndL.Data->GetType() == Datum::DAT_Int32)
					{
						dat = new Datum();
						dat->Set(~opndL.Data->Get<int32>(0));
					}

					if (dat)
					{
						EvaluationStackItem r = { true, dat };
						m_evaluationStack.Push(r);
					}
					else throw Exception(L"Type not compatible");
					break;
				}
			case OPC_Indexer:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					Datum* dat = &(opndL.Data->Get<Scope*>(0)->operator [](opndR.Data->Get<int32>(0)));

					EvaluationStackItem r = { false, dat };
					m_evaluationStack.Push(r);
					break;
				}
			case OPC_Call:
				{
					SList<EvaluationStackItem> params;
					for (;;)
					{
						// keeps popping until the function descriptor is found
						// stack item before will be treated as function parameters.

						EvaluationStackItem item = m_evaluationStack.Pop();

						if (item.Data->GetType() == Datum::DAT_Pointer)
						{
							ExpresssionScriptLibrary::SFFunction* func = item.Data->Get<RTTI*>(0)->As<ExpresssionScriptLibrary::SFFunction>();

							if (func)
							{
								assert((int32)params.Size() == func->getParamCount());

								Datum* dat;
								if (func->getParamCount()<8)
								{
									// small stack buffer
									Datum* paramBuffer[8];
									SList<EvaluationStackItem>::Iterator iter = params.Begin();
									for (int32 i=0;i<func->getParamCount();i++)
									{
										paramBuffer[i] = (*iter).Data;
										++iter;
									}
									dat = func->Invoke(paramBuffer);
								}
								else
								{
									// large amount of arguments
									Datum** paramBuffer = new Datum*[func->getParamCount()];
									SList<EvaluationStackItem>::Iterator iter = params.Begin();
									for (int32 i=0;i<func->getParamCount();i++)
									{
										paramBuffer[i] = (*iter).Data;
										++iter;
									}
									dat = func->Invoke(paramBuffer);
								}

								if (dat)
								{
									EvaluationStackItem r = { true, dat };
									m_evaluationStack.Push(r);
								}

								// manually release
								for (SList<EvaluationStackItem>::Iterator iter = params.Begin(); iter != params.End(); ++iter)
								{
									EvaluationStackItem& i = *iter;
									if (i.IsOwned)
										delete i.Data;
								}

								break;
							}
							else
							{
								// treat as params
								params.PushFront(item);
							}
						}
						else
						{
							// treat as params
							params.PushFront(item);
						}
					}
					break;
				}
			case OPC_Set:
				{
					opndR = m_evaluationStack.Pop();
					opndL = m_evaluationStack.Pop();

					opndL.Data->operator=(*opndR.Data);

					EvaluationStackItem r = { false, opndL.Data };
					m_evaluationStack.Push(r);

					break;
				}
			default:
				throw Exception(L"Invalid Instruction");
			}


			if (opndL.IsOwned && opndL.Data)
				delete opndL.Data;
			if (opndR.IsOwned && opndR.Data)
				delete opndR.Data;
		}
		
		if (m_evaluationStack.Size()>1)
			throw Exception(L"Evaluation Error(stack not balanced)");

		return GetResult();
	}

	Datum* Expression::GetResult()
	{
		if (m_evaluationStack.Size())
			return m_evaluationStack.Top().Data;
		return nullptr;
	}

	void Expression::ResetStack()
	{
		while (m_evaluationStack.Size())
		{
			EvaluationStackItem d = m_evaluationStack.Pop();
			if (d.IsOwned)
				delete d.Data;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	Expression::Instruction::Instruction(OpCode op, Datum* l, bool isLImm, Datum* r, bool isRImm)
		: Type(op), IsLeftImmediate(isLImm), IsRightImmediate(isRImm), OperandLeft(l), OperandRight(r)
	{

	}
	Expression::Instruction::~Instruction()
	{
		if (IsLeftImmediate && OperandLeft)
			delete OperandLeft;
		if (IsRightImmediate && OperandRight)
			delete OperandRight;
	}
}