#pragma once

#include "apoc3d.Essentials/EssentialCommon.h"
#include "apoc3d/Collections/CollectionsCommon.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	enum struct Weekday
	{
		Sunday,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday
	};

	extern const TypeDualConverter<Weekday, true> WeekdayConverter;

	class Date
	{
		friend struct EqualityComparer<Date>;
	public:
		Date();
		
		explicit Date(int32 days);

		Date(int32 y, int32 m, int32 d);

		int32 GetYear() const;
		int32 GetMonth() const;
		int32 GetDay() const;

		void Unpack(int32& y, int32& m, int32& d) const;

		int64 Epoch(int32 h, int32 m, int32 s) const;

		bool IsWeekend() const;
		Weekday GetWeekDay() const;

		bool ParseDDMMYYYY(const std::string& str);
		bool ParseYYYYMMDD(const std::string& str);
		std::string ToStringYYYYMMDD() const;
		std::string ToStringYYYYMMDD(const char sep) const;

		Date operator+(int32 days) const { return Date(m_days + days); }
		Date operator-(int32 days) const { return Date(m_days - days); }
		int32 operator-(Date o) const { return m_days - o.m_days; }

		Date& operator+=(int32 days);
		Date& operator-=(int32 days);

		Date& operator++();
		Date operator++(int) { Date result = *this; ++(*this); return result; }
		Date& operator--();
		Date operator--(int) { Date result = *this; --(*this); return result; }

		bool operator==(Date o) const { return m_days == o.m_days; }
		bool operator!=(Date o) const { return !this->operator==(o); }

		bool operator<(Date o) const { return m_days < o.m_days; }
		bool operator>(Date o) const { return m_days > o.m_days; }

		bool operator<=(Date o) const { return m_days <= o.m_days; }
		bool operator>=(Date o) const { return m_days >= o.m_days; }

		static Date Today();
		
		template <typename Func>
		static void EnumerateYearDates(int32 year, Func func)
		{
			Date firstDay = { year, 1, 1 };

			for (int32 i = 0; i < 366; i++)
			{
				Date curDay = firstDay + i;

				if (curDay.GetYear() != year)
					break;

				func(curDay);
			}
		}

		static Date CreateFromYYYYMMDD(const std::string& str);

	private:
		struct DebugHelper
		{
			ushort m_year;
			byte m_month;
			byte m_day;
		};

		void _UpdateDebug();

		int32 m_days = 0;
		DebugHelper m_debug;
	};

	class TimeOfDay
	{
	public:
		TimeOfDay() { }

		explicit TimeOfDay(int32 ms)
			: m_time(ms) { }

		TimeOfDay(int32 h, int32 m, int32 s, int32 ms);

		int32 GetHour() const;
		int32 GetMinute() const;
		int32 GetSecond() const;
		int32 GetMillisecond() const;

		void Unpack(int32& h, int32& m, int32& s, int32& ms) const;

		int32 Epoch() const { return m_time; }

		bool ParseHHMMSS(const std::string& str);
		std::string ToStringHHMMSS() const;
		std::string ToStringHHMMSS(const char sep) const;

		bool operator==(TimeOfDay o) const { return m_time == o.m_time; }
		bool operator!=(TimeOfDay o) const { return !this->operator==(o); }

		bool operator<(TimeOfDay o) const { return m_time < o.m_time; }
		bool operator>(TimeOfDay o) const { return m_time > o.m_time; }

		bool operator<=(TimeOfDay o) const { return m_time <= o.m_time; }
		bool operator>=(TimeOfDay o) const { return m_time >= o.m_time; }

		static TimeOfDay Now();
		static TimeOfDay LastMS();

		static TimeOfDay IntraDayAdd(TimeOfDay t, int32 epoch, bool clamp);
	private:

		static int32 Pack(int32 h, int32 m, int32 s, int32 ms);

		int32 m_time = 0;
	};

	struct TimeSpan
	{
		int64 m_ms = 0;

		TimeSpan() { }

		explicit TimeSpan(int64 ms)
			: m_ms(ms) { }

		bool operator==(TimeSpan o) const { return m_ms == o.m_ms; }
		bool operator!=(TimeSpan o) const { return !this->operator==(o); }

		bool operator<(TimeSpan o) const { return m_ms < o.m_ms; }
		bool operator>(TimeSpan o) const { return m_ms > o.m_ms; }

		bool operator<=(TimeSpan o) const { return m_ms <= o.m_ms; }
		bool operator>=(TimeSpan o) const { return m_ms >= o.m_ms; }

		TimeSpan operator-() const { return TimeSpan(-m_ms); }

		TimeSpan operator+(TimeSpan o) const { return TimeSpan(m_ms + o.m_ms); }
		TimeSpan operator-(TimeSpan o) const { return TimeSpan(m_ms - o.m_ms); }

		TimeSpan operator*(int32 o) const { return TimeSpan(m_ms*o); }
		int64 operator/(TimeSpan o) const { return m_ms / o.m_ms; }
		
		static TimeSpan Get(int32 days, int32 hours, int32 minutes, int32 seconds, int32 ms);
		static TimeSpan GetDays(int32 days);
		static TimeSpan GetSeconds(int32 seconds);
		static TimeSpan GetMinutes(int32 minutes);
		static TimeSpan GetHours(int32 hours);
	};

	struct DateTime
	{
		Date m_date;
		TimeOfDay m_timeOfDay;

		DateTime() { }

		explicit DateTime(Date date) : m_date(date) { }
		
		DateTime(Date date, TimeOfDay time)
			: m_date(date), m_timeOfDay(time) { }
		
		explicit DateTime(int64 epoch)
			: m_date((int32)(epoch / 86400000)), m_timeOfDay((int32)(epoch % 86400000))
		{ }

		int64 Epoch() const;

		bool Parse(const std::string& str);
		std::string ToString() const;

		DateTime Offsetted(TimeSpan ts) const;
		void Offset(TimeSpan ts);

		TimeSpan operator-(DateTime o) const { return TimeSpan(Epoch() - o.Epoch()); }

		bool operator==(DateTime o) const { return m_date == o.m_date && m_timeOfDay == o.m_timeOfDay; }
		bool operator!=(DateTime o) const { return !this->operator==(o); }

		bool operator<(DateTime o) const { return Epoch() < o.Epoch(); }
		bool operator>(DateTime o) const { return Epoch() > o.Epoch(); }

		bool operator<=(DateTime o) const { return Epoch() <= o.Epoch(); }
		bool operator>=(DateTime o) const { return Epoch() >= o.Epoch(); }

		static DateTime Now();

		static DateTime RoundToDayStart(DateTime d);
		static DateTime RoundToDayEnd(DateTime d);
		static DateTime RoundToNextDayStart(DateTime d);
		static DateTime RoundToPrevDayStart(DateTime d);
		static DateTime RoundToNextDayEnd(DateTime d);

		static DateTime RoundToWeekStart(DateTime d);
		static DateTime RoundToWeekEnd(DateTime d);

		static DateTime RoundDown(DateTime d, TimeSpan ts);
		static DateTime RoundUp(DateTime d, TimeSpan ts);

		//static DateTime CreateFromString(const std::string str);
	};
}

namespace fmt
{
	inline std::string _ToString(const Apoc3D::Date& t) { return t.ToStringYYYYMMDD(); }
	inline std::string _ToString(const Apoc3D::DateTime& t) { return t.ToString(); }

	DEFINE_FMT_FORMAT(Apoc3D::Date);
	DEFINE_FMT_FORMAT(Apoc3D::DateTime);
}

namespace Apoc3D
{
	namespace Collections
	{
		template <>
		struct EqualityComparer<Apoc3D::Date>
		{
			static bool Equals(Apoc3D::Date const& x, Apoc3D::Date const& y) { return x == y; }
			static int32 GetHashCode(const Apoc3D::Date& obj) { return obj.m_days; }
		};
	}
}