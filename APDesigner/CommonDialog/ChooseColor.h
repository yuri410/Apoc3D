
#ifndef CHOOSECOLOR_H
#define CHOOSECOLOR_H

#include "APDesigner/APDCommon.h"

#include "DialogCommon.h"

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
				: m_selectedColor(defColor)
			{ }
				
			ColorValue getSelectedColor() const { return m_selectedColor; }
			void setSelectedColor(ColorValue cv);
			DialogResult ShowDialog();

		};
	}
}


#endif