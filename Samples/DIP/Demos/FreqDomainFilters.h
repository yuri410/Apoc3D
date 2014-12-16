#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoFreqDomainFilters : public SubDemo
	{
	public:
		DemoFreqDomainFilters(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoFreqDomainFilters();

		virtual void Show() override;

	private:

		void Work22_Subprocess(const Complex* source, const FreqDomainFilter& filter, Texture* resultTex);
		void Work22_Process(Button* ctrl);
		void Work22_SelImage(ComboBox* ctrl);

		Form* m_frmW22;
		Texture* m_w22resultAmp;
		Texture* m_w22resultPhase;

		Texture* m_w22result2_1;
		Texture* m_w22result1_1;
		Texture* m_w22result2_2;
		Texture* m_w22result1_2;
		TextBox* m_w22Thres1_1;
		TextBox* m_w22Thres2_1;
		ComboBox* m_w22ImageSel;
		PictureBox* m_w22OriginalImage;
	};
}