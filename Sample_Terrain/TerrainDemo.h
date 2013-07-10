#ifndef TERRAINDEMO_H
#define TERRAINDEMO_H

#include "TerrainCommon.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/UILib/CheckBox.h"
#include "Apoc3D.Essentials/Game.h"

using namespace Apoc3DEx;
using namespace Apoc3D;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::Scene;
using namespace Apoc3D::UI;

namespace SampleTerrain
{
	/** This class is the event handler for all messages from render window. 
	 *  The game's drawing, updating in the main loop and initialization are
	 *  all handled in this class.
	 *
	 *  In this demo, it inherits the Apoc3D.Essentials.Game class,
	 *  which has some fundamental features like UI and Consoles. When using this
	 *  game class, the application will have a dependency on the 
	 *  'system.pak' file, which includes the basic font and UI elements.
	 *
	 *  The terrain is divided into blocks, or chunks for another name.
	 *  The entire terrain is NumBlockOnEdge by NumBlockOnEdge big.
	 *  Every block has the same horizontal size(width and height). 
	 *  The terrain mesh has the same width and height, and different height at different point. 
	 *  The terrain mesh's width and height are calculated by
	 *  multiplying CellLength and the number of vertices on the edge -1.
	 *
	 */
	class TerrainDemo : public Game
	{
	public:
		TerrainDemo(RenderWindow* wnd);
		~TerrainDemo();

		/** This is the place to load or initialize something before any other 
		 *  events being handled.
		 */
		virtual void Initialize();
		/** This is opposed to Initialize, which is called last(after Unload)
		 *  and is supposed release the resources created in Initialize here.
		 */
		virtual void Finalize();

		/** Load will be called to load contents. 
		 *  Graphics content should be loaded here, if there are any to load as the game starts up.
		 */
		virtual void Load();

		/** Opposite to Load, this is the place to unload content, called once the game window is closed.
		*/
		virtual void Unload();
		
		/** Allows the game to run logic such as updating the world,
		 *  checking for collisions, gathering input, playing audio and etc.
		 */
		virtual void Update(const GameTime* const time);

		/** This is called when the game should draw a frame.
		*/
		virtual void Draw(const GameTime* const time);

		/** This is called before drawing a new frame.
		*/
		virtual void OnFrameStart();

		/** This is called after a frame is drawn.
		*/
		virtual void OnFrameEnd();

	private:
#if _DEBUG
		static const int NumBlockOnEdge = 16;
#else
		static const int NumBlockOnEdge = 64;
#endif
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