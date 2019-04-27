#include "MatrixStack.h"

namespace Apoc3D
{
	namespace Math
	{
		static Matrix UndefinedMatrix = Matrix::Identity;

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

			AP_EXCEPTION(ErrorID::InvalidOperation, L"The stack is empty.");
			return UndefinedMatrix;
		}

		const Matrix& MatrixStack::Peek() const
		{
			if (m_stack.getCount())
			{
				return m_stack.Peek();
			}
			AP_EXCEPTION(ErrorID::InvalidOperation, L"The stack is empty.");
			return UndefinedMatrix;
		}

		int MatrixStack::getCount() const { return m_stack.getCount(); }
	}
}