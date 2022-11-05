#pragma once

#include "Apoc3D.Essentials/System/TimeSystem.h"

namespace Apoc3D
{
	template <typename T>
	void JsonParse(Optional<T>& dst, json& j, const char* name)
	{
		dst.Clear();

		auto i = j.find(name);

		if (i != j.end() && !i->is_null())
			dst = i->get<T>();
	}

	inline void JsonParse(Optional<DateTime> & dst, json & j, const char* name)
	{
		dst.Clear();

		Optional<int64> epoch;
		JsonParse(epoch, j, name);

		if (epoch.isSet())
		{
			dst = DateTime(epoch.getContent());
		}
	}

	template <typename T>
	void JsonStore(const Optional<T>& src, json& j, const char* name)
	{
		if (src.isSet())
		{
			j[name] = src.getContent();
		}
	}
}
