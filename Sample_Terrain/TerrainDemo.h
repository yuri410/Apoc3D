#ifndef TERRAINDEMO_H
#define TERRAINDEMO_H

#include "TerrainCommon.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"
#include "Graphics/Material.h"
#include "UILib/CheckBox.h"
#include "Apoc3D.Essentials/Game.h"

using namespace Apoc3DEx;
using namespace Apoc3D;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::Scene;
using namespace Apoc3D::UI;

namespace SampleTerrain
{
	/** This class is the event handler for all messages from render window. The game's drawing, updating in the main loop and initialization are
	 *  all handled in this class.
	 *
	 *  In this demo, it inherits the Apoc3D.Essentials.Game class, which has some fundamental features like UI and Consoles. When using this
	 *  game class, the application will have a dependency on the 'system.pak' file, which includes the basic font and UI elements.
	 *
	 *  The terrain is divided into blocks, or chunks for another name. The entire terrain is NumBlockOnEdge by NumBlockOnEdge big.
	 *  Every block has the same horizontal size(width and height). The terrain mesh has the same width and height, and different height at different point. 
	 *  The terrain mesh's width and height are calculated by multiplying CellLength and the number of vertices on the edge -1.
	 *
	 */
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

	private:
		
		static const int NumBlockOnEdge = 64;
		static const int MinBlockCoord = -NumBlockOnEdge/2;
		static const int MaxBlockCoord = NumBlockOnEdge/2-1;

		Sprite* m_sprite;

		SceneRenderer* m_sceneRenderer;
		OctreeSceneManager* m_scene;

		GameCamera* m_camera;

		Terrain* m_terrainBlocks[NumBlockOnEdge][NumBlockOnEdge];

		bool m_isMoving;
		bool m_allowTakingDownTrees;
		CheckBox* m_cbPushTrees;
		bool m_isLoading;
		int m_sceneContentLoaded;
		int m_zeroOpFrameCounter;
		Texture* m_loadingScreen;
		Texture* m_helpMove;
		Texture* m_helpLook;
		float m_helpShowState;

		void UpdateCamera();
		void UpdateUI(const GameTime* const time);
		void DrawUI(Sprite* sprite);

		void LoadScene();
		void LoadUI();
	};
}
#endif