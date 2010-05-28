#pragma once

#include "..\Common.h"

class RenderOperationBuffer;

class IRenderable
{
public:
	virtual RenderOperationBuffer* GetRenderOperation(int level);
	virtual RenderOperationBuffer* GetRenderOperation() { return GetRenderOperation(0); }

protected:
	IRenderable(void)
	{
	}

	~IRenderable(void)
	{
	}
};

