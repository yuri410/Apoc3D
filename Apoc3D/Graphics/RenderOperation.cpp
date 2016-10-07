#include "RenderOperation.h"

namespace Apoc3D
{
	namespace Graphics
	{
		/************************************************************************/
		/*   PartTransforms                                                     */
		/************************************************************************/

		bool PartTransforms::operator ==(const PartTransforms& another) const
		{
			if (Count == another.Count)
			{
				if (Transfroms == another.Transfroms)
					return true;

				if (Transfroms && another.Transfroms)
					return memcmp(Transfroms, another.Transfroms, sizeof(Matrix) * Count) != 0;
				return false;
			}
			return false;
		}

		bool PartTransforms::operator !=(const PartTransforms& other) const
		{
			return !this->operator==(other); 
		}

		/************************************************************************/
		/*   RenderOperation                                                    */
		/************************************************************************/

		bool RenderOperation::operator ==(const RenderOperation& other)
		{
			return GeometryData == other.GeometryData &&
				Material == other.Material &&
				RootTransform == other.RootTransform &&
				PartTransform == other.PartTransform &&
				RootTransformIsFinal == other.RootTransformIsFinal &&
				UserData == other.UserData;
		}

		bool RenderOperation::operator !=(const RenderOperation& other) 
		{
			return !this->operator==(other); 
		}

	}
}