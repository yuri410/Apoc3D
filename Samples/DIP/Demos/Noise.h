#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoNoise : public SubDemo
	{
	public:
		DemoNoise(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoNoise();

		virtual void Show() override;

	private:
		
		void Process(Button* ctrl);

		void SliderPercentage1_ValueChange(SliderBar* sb, bool);
		void SliderPercentage2_ValueChange(SliderBar* sb, bool);

		Form* m_form;
		Texture* m_noise;
		Texture* m_noise2;
		Texture* m_result1_1;
		Texture* m_result1_2;
		Texture* m_result2_1;
		Texture* m_result2_2;

		SliderBar* m_sbPercentage1;
		SliderBar* m_sbPercentage2;

		Label* m_sbpLabel1;
		Label* m_sbpLabel2;
		Label* m_noiseLabel1;
		Label* m_noiseLabel2;

		float m_percentage1 = 0.2f;
		float m_percentage2 = 0.2f;
	};
}