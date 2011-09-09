#include "RenderOperationBuffer.h"


namespace Apoc3D
{
	namespace Graphics
	{
		void RenderOperationBuffer::MultiplyTransform(const Matrix& m)
		{
			Matrix temp;
			for (int i=0;i<m_oplist.getCount();i++)
			{
				Matrix::Multiply(temp, m_oplist[i].RootTransform, m);
				m_oplist[i].RootTransform = temp;
			}
		}
	}
}