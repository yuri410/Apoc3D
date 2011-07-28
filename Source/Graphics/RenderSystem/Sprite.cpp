#include "Sprite.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Sprite::Sprite(RenderDevice* rd)
				: m_renderDevice(rd), m_transform(Matrix::Identity)
			{
				
			}

			Sprite::~Sprite()
			{

			}
		}
	}
}