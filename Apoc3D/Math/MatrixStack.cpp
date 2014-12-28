#include "MatrixStack.h"
#include "apoc3d/Exception.h"

namespace Apoc3D
{
	namespace Math
	{
		MatrixStack::MatrixStack(int reserve)
			: m_stack(reserve) { }

		MatrixStack::~MatrixStack()
		{ }

		void MatrixStack::PushMultply(const Matrix& mat)
		{
			if (m_stack.getCount())
			{
				Matrix newMat;
				Matrix::Multiply(newMat, m_stack.Peek(), mat);
				m_stack.Push(newMat);
			}
			else
			{
				m_stack.Push(mat);
			}
		}

		void MatrixStack::PushMatrix(const Matrix& mat)
		{
			m_stack.Push(mat);
		}

		bool MatrixStack::PopMatrix()
		{
			if (m_stack.getCount())
			{
				m_stack.FastPop();
				return true;
			}
			return false;
		}

		bool MatrixStack::PopMatrix(Matrix& mat)
		{
			if (m_stack.getCount())
			{
				mat = m_stack.Pop();
				return true;
			}
			return false;
		}

		Matrix& MatrixStack::Peek()
		{
			if (m_stack.getCount())
			{
				return m_stack.Peek();
			}
			throw AP_EXCEPTION(ExceptID::InvalidOperation, L"The stack is empty.");
		}

		const Matrix& MatrixStack::Peek() const
		{
			if (m_stack.getCount())
			{
				return m_stack.Peek();
			}
			throw AP_EXCEPTION(ExceptID::InvalidOperation, L"The stack is empty.");
		}

		int MatrixStack::getCount() const { return m_stack.getCount(); }
	}
}