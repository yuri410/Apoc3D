
namespace Apoc3D
{
	namespace Core
	{
		typedef std::vector<SceneObject*> ObjectList;

		/* Represents a collection of scene objects in the scene
		*/
		class SceneNode
		{
		private:
			ObjectList m_attached;

		public:
			
			SceneNode(void);
			~SceneNode(void);
		};
	};
};