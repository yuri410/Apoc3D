#pragma once

#include "..\Common.h"

class Camera
{
private:
	
	D3DMATRIX m_view;
	D3DMATRIX m_proj;
public:
	
	const D3DMATRIX& getViewMatrix() { return m_view; }
	const D3DMATRIX& getProjMatrix() { return m_proj; }

	void setViewMatrix(const D3DMATRIX& value) { m_view = value; }
	void setProjMatrix(const D3DMATRIX& value) { m_proj = value; }


	Camera(void);
	~Camera(void);
};

