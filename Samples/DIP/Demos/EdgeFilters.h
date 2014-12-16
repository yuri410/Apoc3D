#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoEdgeFilters : public SubDemo
	{
	public:
		DemoEdgeFilters(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoEdgeFilters();

		virtual void Show() override;

	private:

		void Work21_SubProcess(Texture* original, Texture* resultTex1, Texture* resultTex2, Texture* resultTex3,
			int thres1, int thres2, int thres3);
		void Work21_Process(Button* ctrl);

		Form* m_frmW21;
		Texture* m_w21result1_1;
		Texture* m_w21result1_2;
		Texture* m_w21result1_3;
		Texture* m_w21result2_1;
		Texture* m_w21result2_2;
		Texture* m_w21result2_3;
		TextBox* m_w21Thres1_1;
		TextBox* m_w21Thres1_2;
		TextBox* m_w21Thres1_3;
		TextBox* m_w21Thres2_1;
		TextBox* m_w21Thres2_2;
		TextBox* m_w21Thres2_3;
	};
}