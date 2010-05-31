#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		enum _Export FrustumPlane
		{
			FPL_Right = 0,
			FPL_Left = 1,
			FPL_Bottom = 2,
			FPL_Top = 3,
			FPL_Far = 4,
			FPL_Near = 5
		};

		class _Export Frustum
		{
		private:
			const static int ClipPlaneCount = 6;
			Plane m_planes[ClipPlaneCount];

		public:
			Frustum(void);
			~Frustum(void);

			bool Intersects() const
			{

			}

			void Update(const Matrix& view, const Matrix& proj);
			void Update(const Matrix& viewProj);
		};
	};
};