#include "Win32Keyboard.h"
#include "Core/GameTime.h"

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			Win32Keyboard::Win32Keyboard(OIS::InputManager* mgr)
				: m_inpMgr(mgr)
			{
				m_keyboard = static_cast<OIS::Keyboard*>(mgr->createInputObject(OIS::OISKeyboard, true));
			}
			Win32Keyboard::~Win32Keyboard()
			{
				m_inpMgr->destroyInputObject(m_keyboard);
			}

			void Win32Keyboard::Update(const GameTime* const time)
			{

			}

			bool Win32Keyboard::keyPressed( const OIS::KeyEvent &arg ) {
				std::cout << " KeyPressed {" << arg.key
					<< ", " << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key)
					<< "} || Character (" << (char)arg.text << ")" << std::endl;
				return true;
			}
			bool Win32Keyboard::keyReleased( const OIS::KeyEvent &arg ) {
				//if( arg.key == OIS::KC_ESCAPE || arg.key == OIS::KC_Q )
					//appRunning = false;
				std::cout << "KeyReleased {" << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key) << "}\n";
				return true;
			}
		}
	}
}