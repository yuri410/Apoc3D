#pragma once
#ifndef FONTDEMO_H
#define FONTDEMO_H

#include "FontCommon.h"

namespace SampleFont
{
	class FontDemo : public Application
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
		virtual void Update(const AppTime* time);

		virtual void UpdateConstrainedVarTimeStep(const AppTime* time);

		/** This is called when the game should draw a frame.
		*/
		virtual void Draw(const AppTime* time);

		/** This is called before drawing a new frame.
		*/
		virtual void OnFrameStart();

		/** This is called after a frame is drawn.
		*/
		virtual void OnFrameEnd();

	private:
		Sprite* m_sprite;

		int32 m_currentPressure;

		float m_lengthDissolveProgress = 0;
		float m_wordDissolveProgress = 0;
		float m_allDissolveProgress = 0;

		float m_test = 0;
		//float m_testI = 0;
		float m_testDirection = 1;
	};
}
#endif