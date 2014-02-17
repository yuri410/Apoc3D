#ifndef FONTDEMO_H
#define FONTDEMO_H

#include "FontCommon.h"

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

namespace SampleFont
{
	class FontDemo : public Game
	{
	public:
		FontDemo(RenderWindow* wnd);
		~FontDemo();

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
		Sprite* m_sprite;

		int32 m_currentPressure;
	};
}
#endif