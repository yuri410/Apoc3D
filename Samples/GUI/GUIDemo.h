
#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "DemoCommon.h"

namespace Sample_GUI
{
    class GUIDemo : public Apoc3DEx::Game
	{
	public:
		static const int DMCount = 8;
		GUIDemo(RenderWindow* wnd);
		~GUIDemo();


		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

	private:

		void ShowAbout(MenuItem* ctrl);


		Form* m_aboutDlg;

		Menu* m_mainMenu;

		Sprite* m_sprite;
    };
}
#endif
