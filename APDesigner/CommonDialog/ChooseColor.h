
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
			ChooseColorDialog(ColorValue defColor = 0)
				: m_selectedColor(0)
			{ }
				
			ColorValue getSelectedColor() const { return m_selectedColor; }
			void setSelectedColor(ColorValue cv);
			DialogResult ShowDialog();

		};
	}
}


#endif