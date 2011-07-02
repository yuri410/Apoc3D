
#ifndef CHOOSECOLOR_H
#define CHOOSECOLOR_H

#include "APDCommon.h"
#include "DialogCommon.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Math;

namespace APDesigner
{
	namespace CommonDialog
	{
		class ChooseColorDialog
		{
		private:
			ColorValue m_selectedColor;
		public:
			ColorValue getSelectedColor() const { return m_selectedColor; }

			DialogResult ShowDialog();

		};
	}
}


#endif