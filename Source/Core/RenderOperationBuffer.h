#pragma once

#include <list>
#include "RenderOperation.h"

class RenderOperationBuffer
{
private:
	RenderOperation* oplist;

	void Resize(int size)
	{

	}

public:
	RenderOperationBuffer(void)
	{

	}
	~RenderOperationBuffer(void)
	{

	}

	void Add(const RenderOperation& op)
	{
		//oplist.push_back(op);
	}
	void Add(RenderOperation* op, int count)
	{
		for (int i=0;i<count;i++)
		{
			oplist.push_back(op[count]);
		}
	}

	void Clear(){ oplist.clear(); }

	RenderOperation get(int i)
	{

	}

	int getCount(){}
};

