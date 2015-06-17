#include "ScenePassTypes.h"
#include "apoc3d/Utility/TypeConverter.h"

namespace Apoc3D
{
	namespace Scene
	{
		const Apoc3D::Utility::TypeDualConverter<SceneVariableType> SceneVariableTypeConverter = 
		{
			{ L"RenderTarget", SceneVariableType::RenderTarget },
			{ L"DepthStencil", SceneVariableType::DepthStencil },
			{ L"Matrix", SceneVariableType::Matrix },
			{ L"Vector4", SceneVariableType::Vector4 },
			{ L"Vector3", SceneVariableType::Vector3 },
			{ L"Vector2", SceneVariableType::Vector2 },
			{ L"Single", SceneVariableType::Single },

			{ L"Texture", SceneVariableType::Texture },

			{ L"Integer", SceneVariableType::Integer },
			{ L"Boolean", SceneVariableType::Boolean },
			{ L"Effect", SceneVariableType::Effect },
			{ L"GaussBlurFilter", SceneVariableType::GaussBlurFilter }
		};

	}
}