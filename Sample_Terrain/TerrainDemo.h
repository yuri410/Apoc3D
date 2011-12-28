#ifndef TERRAINDEMO_H
#define TERRAINDEMO_H

#include "TerrainCommon.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"

#include "Apoc3D.Essentials/Game.h"

using namespace Apoc3DEx;
using namespace Apoc3D;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::Scene;
using namespace Apoc3D::UI;

namespace SampleTerrain
{
	class TerrainDemo : public Game
	{
	public:
		TerrainDemo(RenderWindow* wnd);
		~TerrainDemo();


		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();


		void UpdateCamera();
	private:
		Sprite* m_sprite;

		SceneRenderer* m_sceneRenderer;
		OctreeSceneManager* m_scene;

		GameCamera* m_camera;
	};
}
#endif