/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#ifndef APOC3DES_GAME_H
#define APOC3DES_GAME_H

#include "EssentialCommon.h"

#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "apoc3d/Input/InputAPI.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::UI;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	/** A class as a framework, which looks similar to XNA's, but with the whole engine's support.
	 *  This class is the event handler for all messages from render window. The game's drawing, updating in the main loop and initialization are
	 *  all handled in this class.
	 *
	 */
	class APEXAPI App : public RenderWindowHandler
	{
	public:
		App(RenderWindow* wnd);
		virtual ~App();

		/** This is the place to load or initialize something before any other 
		 *  events being handled. Remember to call this if overrided.
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
		virtual void Update(const AppTime* time);

		/** This is called when the game should draw a frame.
		*/
		virtual void Draw(const AppTime* time);

		/** This is called before drawing a new frame.
		*/
		virtual void OnFrameStart();
		/** This is called after a frame is drawn.
		*/
		virtual void OnFrameEnd();

		/** Closes the window
		*/
		void Exit();

		RenderDevice* getRenderDevice() const { return m_device; }
		RenderWindow* getWindow() const { return m_window; }

		const StyleSkin* getSystemUISkin() const { return m_UIskin; }

		Console* getConsole() const { return m_console; }

	protected:
		const AppTime* GetRecordCorrectedTime(const AppTime* time);

		RenderWindow* m_window;
		RenderDevice* m_device = nullptr;

		StyleSkin* m_UIskin = nullptr;
		Console* m_console = nullptr;

		Input::InputPlayer* m_player = nullptr;
		Input::InputRecorder* m_recorder = nullptr;
		AppTime m_correctedFrameTime;

		InputCreationParameters m_inputCreationParam;
	};
}
#endif