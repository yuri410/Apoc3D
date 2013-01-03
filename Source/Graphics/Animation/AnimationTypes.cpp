#include "AnimationTypes.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			void ModelAnimationClip::Transform(const Matrix& t)
			{
				for (int i=0;i<m_keyFrames.getCount();i++)
				{
					const ModelKeyframe& f = m_keyFrames[i];

					Matrix newT;
					Matrix::Multiply(newT, f.getTransform(), t);

					ModelKeyframe frame(f.getObjectIndex(), f.getTime(), newT);

					m_keyFrames[i] = frame;
				}
			}
		}
	}
}