#ifndef ASYNCPROCESSOR_H
#define ASYNCPROCESSOR_H

#pragma once

#include "..\Resource.h"
#include <queue>

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			class _Export ResourceOperation
			{
			private:
				Resource* m_resource;

			protected:
				ResourceOperation(Resource* res)
					: m_resource(res){ }

			public:
				virtual void Process() = 0;
			};

			class _Export AsyncProcessor
			{
			private:
				
			public:
				AsyncProcessor(void);
				~AsyncProcessor(void);
			};
		}
	}
}
#endif