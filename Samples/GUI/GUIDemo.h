
#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "DemoCommon.h"

namespace Sample_GUI
{
    class GUIDemo : public Application
	{
	public:
		static const int DMCount = 8;

		GUIDemo(RenderWindow* wnd);
		~GUIDemo();

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const AppTime* time);
		virtual void Draw(const AppTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

	private:

		void ShowAbout(MenuItem* ctrl);

		void Slider_Changed(SliderBar* sb, bool finishing);
		
		void Label_Link(Label*, uint16);

		Form* m_aboutDlg;

		MenuBar* m_mainMenu;
		ProgressBar* m_testPB;
		SliderBar* m_testSB;
		SliderBar* m_testSB2;

		ComboBox* m_testCB;

		TextBox* m_testTB;

		Label* m_testLabel;

		float m_signal1 = 0;
		float m_signal2 = 0;
		int32 m_testVal = 0;

		float m_timer = 0;

		Sprite* m_sprite;
    };
}
#endif
