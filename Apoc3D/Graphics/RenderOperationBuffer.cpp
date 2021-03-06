#include "RenderOperationBuffer.h"


namespace Apoc3D
{
	namespace Graphics
	{
		void RenderOperationBuffer::AddWithParamAndMtrl(const RenderOperation* op, int count, Material* mtrl, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.UserData = userPointer;
				rop.Material = mtrl;
			}
		}
		void RenderOperationBuffer::AddWithParamAndMtrl(const RenderOperation* op, int count, const Matrix& transform, Material* mtrl, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
				rop.UserData = userPointer;
				rop.Material = mtrl;
			}
		}
		void RenderOperationBuffer::AddWithParamAndMtrl(const RenderOperation* op, int count, const Matrix& transform, bool isFinal, Material* mtrl, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
				rop.UserData = userPointer;
				rop.Material = mtrl;
				rop.RootTransformIsFinal = isFinal;
			}
		}

		void RenderOperationBuffer::AddWithParam(const RenderOperation* op, int count, const Matrix& transform)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
			}
		}
		void RenderOperationBuffer::AddWithParam(const RenderOperation* op, int count, const Matrix& transform, bool isFinal)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
				rop.RootTransformIsFinal = isFinal;
			}
		}
		void RenderOperationBuffer::AddWithParam(const RenderOperation* op, int count, const Matrix& transform, bool isFinal, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
				rop.UserData = userPointer;
				rop.RootTransformIsFinal = isFinal;
			}
		}
		void RenderOperationBuffer::AddWithParam(const RenderOperation* op, int count, const Matrix& transform, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.RootTransform = transform;
				rop.UserData = userPointer;
			}
		}
		void RenderOperationBuffer::AddWithParam(const RenderOperation* op, int count, void* userPointer)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist.Add(*(op+i));
				RenderOperation& rop = m_oplist[m_oplist.getCount()-1];
				rop.UserData = userPointer;
			}
		}



		void RenderOperationBuffer::SetForAllWithMtrl(Material* mtrl, void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.Material = mtrl;
				rop.UserData = userPointer;
			}
		}
		void RenderOperationBuffer::SetForAllWithMtrl(const Matrix& transform, Material* mtrl, void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.Material = mtrl;
				rop.UserData = userPointer;
				rop.RootTransform = transform;
			}
		}
		void RenderOperationBuffer::SetForAllWithMtrl(const Matrix& transform, bool isFinal, Material* mtrl, void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.Material = mtrl;
				rop.UserData = userPointer;
				rop.RootTransform = transform;
				rop.RootTransformIsFinal = isFinal;
			}
		}

		void RenderOperationBuffer::SetForAll(const Matrix& transform)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.RootTransform = transform;
			}
		}
		void RenderOperationBuffer::SetForAll(const Matrix& transform, bool isFinal)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.RootTransform = transform;
				rop.RootTransformIsFinal = isFinal;
			}
		}
		void RenderOperationBuffer::SetForAll(const Matrix& transform, bool isFinal, void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.RootTransform = transform;
				rop.RootTransformIsFinal = isFinal;
				rop.UserData = userPointer;
			}
		}
		void RenderOperationBuffer::SetForAll(const Matrix& transform, void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.RootTransform = transform;
				rop.UserData = userPointer;
			}
		}
		void RenderOperationBuffer::SetForAll(void* userPointer)
		{
			for (int32 i=0; i<m_oplist.getCount(); i++)
			{
				RenderOperation& rop = m_oplist[i];
				rop.UserData = userPointer;
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