#include "RenderOperationBuffer.h"


namespace Apoc3D
{
	namespace Graphics
	{
		void RenderOperationBuffer::Add(const RenderOperation* op, int count, const Matrix& transform)
		{
			Matrix temp;
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				Matrix::Multiply(temp, rop.RootTransform, transform);
				rop.RootTransform = temp;
			}
		}
		//void RenderOperationBuffer::MultiplyTransform(const Matrix& m)
		//{
		//	Matrix temp;
		//	for (int i=0;i<m_oplist.getCount();i++)
		//	{
		//		Matrix::Multiply(temp, m_oplist[i].RootTransform, m);
		//		m_oplist[i].RootTransform = temp;
		//	}
		//}
	}
}