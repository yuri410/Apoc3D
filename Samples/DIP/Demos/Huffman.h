#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoHuffman : public SubDemo
	{
	public:
		DemoHuffman(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoHuffman();

		virtual void Show() override;

	private:

		void Work23_Process(Button* ctrl);
		void Work23_SelImage(ComboBox* ctrl);

		Form* m_frmW23;
		ListView* m_w23result;
		Label* m_w23comRate;
		PictureBox* m_w23OriginalImage;
		ComboBox* m_w23ImageSel;

	};
}