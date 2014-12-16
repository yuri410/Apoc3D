#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoTransformation : public SubDemo
	{
	public:
		DemoTransformation(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoTransformation();

		virtual void Show() override;

	private:

		void Work13(MenuItem* ctrl);
		void Button_Process(Button* ctrl);


		Form* m_form;
		Texture* m_result;

	};
}