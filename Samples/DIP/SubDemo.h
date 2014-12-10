#pragma once

#include "DIPCommon.h"

namespace dip
{
	class SubDemo
	{
	public:
		SubDemo(const String& name)
			: m_name(name) { }
		virtual ~SubDemo() { }

		virtual void Update(const GameTime* time) { }
		virtual void Draw(const GameTime* time, Sprite* spr) { }
		virtual void Show() = 0;

		const String& getName() const { return m_name; }
	private:
		String m_name;
	};


}

