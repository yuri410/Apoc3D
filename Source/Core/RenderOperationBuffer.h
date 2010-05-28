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

	}

	void Clear(){ }

	RenderOperation get(int i)
	{

	}

	int getCount(){}
};

