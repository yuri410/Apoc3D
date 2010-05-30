
#include "RenderOperationBuffer.h"
#include "RenderOperation.h"

namespace Apoc3D
{
	namespace Core
	{
		void RenderOperationBuffer::Add(const RenderOperation& op)
		{
			m_oplist[m_internalPointer++] = op;
		}
		void RenderOperationBuffer::Add(const RenderOperation* op, int count)
		{
			for (int i=0;i<count;i++)
			{
				m_oplist[m_internalPointer++] = *(op+i);
			}
		}
		
		void RenderOperationBuffer::Clear()
		{ 
			m_oplist.erase(m_oplist.begin(), m_oplist.end());
			m_internalPointer = 0;
		}
	};
};