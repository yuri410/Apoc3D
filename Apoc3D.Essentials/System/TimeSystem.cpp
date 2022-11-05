#include "TimeSystem.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/TypeConverter.h"
#include "apoc3d/Math/Math.h"

#include <ctime>

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	const TypeDualConverter<Weekday, true> WeekdayConverter =
	{
		{ "Sunday", Weekday::Sunday },
		{ "Monday", Weekday::Monday },
		{ "Tuesday", Weekday::Tuesday },
		{ "Wednesday", Weekday::Wednesday },
		{ "Thursday", Weekday::Thursday },
		{ "Friday", Weekday::Friday },
		{ "Saturday", Weekday::Saturday },
	};

	
#pragma region Date

	// Returns number of days since civil 1970-01-01.  Negative values indicate
	//    days prior to 1970-01-01.
	// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
	//                 m is in [1, 12]
	//                 d is in [1, last_day_of_month(y, m)]
	//                 y is "approximately" in
	//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
	//                 Exact range of validity is:
	//                 [civil_from_days(numeric_limits<Int>::min()),
	//                  civil_from_days(numeric_limits<Int>::max()-719468)]
	static int32 days_from_civil(int32 y, uint32 m, uint32 d)
	{
		y -= m <= 2;
		const int32 era = (y >= 0 ? y : y - 399) / 400;
		const uint32 yoe = static_cast<uint32>(y - era * 400);      // [0, 399]
		const uint32 doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
		const uint32 doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;         // [0, 146096]
		return era * 146097 + static_cast<int32>(doe) - 719468;
	}

	// Returns year/month/day triple in civil calendar
	// Preconditions:  z is number of days since 1970-01-01 and is in the range:
	//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
	static void civil_from_days(int32 z, int32& year, uint32& month, uint32& day)
	{
		z += 719468;
		const int32 era = (z >= 0 ? z : z - 146096) / 146097;
		const uint32 doe = static_cast<uint32>(z - era * 146097);          // [0, 146096]
		const uint32 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;  // [0, 399]
		const int32 y = static_cast<int32>(yoe) + era * 400;
		const uint32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);                // [0, 365]
		const uint32 mp = (5 * doy + 2) / 153;                                   // [0, 11]
		const uint32 d = doy - (153 * mp + 2) / 5 + 1;                             // [1, 31]
		const uint32 m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]

		year = y + (m <= 2);
		month = m;
		day = d;
	}

	// Returns day of week in civil calendar [0, 6] -> [Sun, Sat]
	// Preconditions:  z is number of days since 1970-01-01 and is in the range:
	//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-4].
	static uint32 weekday_from_days(int32 z)
	{
		return static_cast<uint32>(z >= -4 ? (z + 4) % 7 : (z + 5) % 7 + 6);
	}

	Date::Date() 
	{
		_UpdateDebug();
	}

	Date::Date(int32 days)
		: m_days(days)
	{
		_UpdateDebug();
	}

	Date::Date(int32 y, int32 m, int32 d)
	{
		assert(m >= 1 && m <= 12);
		assert(d >= 1 && d <= 31);
		m_days = days_from_civil(y, m, d);
		_UpdateDebug();
	}

	int32 Date::GetYear() const
	{
		int32 y, m, d;
		Unpack(y, m, d);
		return y;
	}

	int32 Date::GetMonth() const
	{
		int32 y, m, d;
		Unpack(y, m, d);
		return m;
	}

	int32 Date::GetDay() const
	{
		int32 y, m, d;
		Unpack(y, m, d);
		return d;
	}

	void Date::Unpack(int32& y, int32& m, int32& d) const
	{
		int32 year;
		uint32 month, day;
		civil_from_days(m_days, year, month, day);

		y = year;
		m = (int32)month;
		d = (int32)day;
	}

	bool Date::IsWeekend() const 
	{
		Weekday weekday = GetWeekDay();
		return weekday == Weekday::Sunday || weekday == Weekday::Saturday;
	}

	Weekday Date::GetWeekDay() const
	{
		return (Weekday)weekday_from_days(m_days);
	}

	int64 Date::Epoch(int32 h, int32 m, int32 s) const
	{
		return (int64)m_days * 86400 * 1000 + TimeOfDay(h, m, s, 0).Epoch();
	}

	bool Date::ParseDDMMYYYY(const std::string& str)
	{
		if (str.size() == 8)
		{
			int32 day = StringUtils::ParseInt32(str.substr(0, 2));
			int32 month = StringUtils::ParseInt32(str.substr(2, 2));
			int32 year = StringUtils::ParseInt32(str.substr(4, 4));

			*this = Date(year, month, day);
			return true;
		}
		else
		{
			List<int32> results;
			StringUtils::SplitParseInts(str, results, "-/ ");
			if (results.getCount() == 3)
			{
				*this = Date(results[2], results[1], results[0]);
				return true;
			}
			return false;
		}
	}

	bool Date::ParseYYYYMMDD(const std::string& str)
	{
		if (str.size() == 8)
		{
			int32 year = StringUtils::ParseInt32(str.substr(0, 4));
			int32 month = StringUtils::ParseInt32(str.substr(4, 2));
			int32 day = StringUtils::ParseInt32(str.substr(6, 2));

			*this = Date(year, month, day);
			return true;
		}
		else
		{
			List<int32> results;
			StringUtils::SplitParseInts(str, results, "-/ ");
			if (results.getCount() == 3)
			{
				*this = Date(results[0], results[1], results[2]);
				return true;
			}
			return false;
		}
	}

	std::string Date::ToStringYYYYMMDD() const
	{
		int32 y, m, d;
		Unpack(y, m, d);
		return fmt::format("{0:04d}-{1:02d}-{2:02d}", y, m, d);
	}

	std::string Date::ToStringYYYYMMDD(const char sep) const
	{
		int32 y, m, d;
		Unpack(y, m, d);

		if (sep)
			return fmt::format("{0:04d}{3}{1:02d}{3}{2:02d}", y, m, d, sep);

		return fmt::format("{0:04d}{1:02d}{2:02d}", y, m, d);
	}

	Date& Date::operator+=(int32 days) 
	{
		m_days += days;
		_UpdateDebug();
		return *this; 
	}

	Date& Date::operator-=(int32 days)
	{
		m_days -= days;
		_UpdateDebug();
		return *this;
	}

	Date& Date::operator++()
	{
		m_days++;
		_UpdateDebug();
		return *this;
	}

	Date& Date::operator--()
	{
		m_days--;
		_UpdateDebug();
		return *this; 
	}

	Date Date::Today()
	{
		time_t t = time(NULL);

		struct tm* tmp = gmtime(&t);

		if (tmp)
		{
			return Date(tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday);
		}
		return Date();
	}

	Date Date::CreateFromYYYYMMDD(const std::string& str)
	{
		Date d;
		bool res = d.ParseYYYYMMDD(str);
		assert(res);
		return d;
	}

	void Date::_UpdateDebug()
	{
		int32 y, m, d;
		Unpack(y, m, d);
		m_debug.m_year = y;
		m_debug.m_month = m;
		m_debug.m_day = d;
	}

#pragma endregion

#pragma region TimeOfDay

	//////////////////////////////////////////////////////////////////////////
	
	TimeOfDay::TimeOfDay(int32 h, int32 m, int32 s, int32 ms)
		: m_time(Pack(h, m, s, ms))
	{
		assert(h >= 0 && h < 24);
		assert(m >= 0 && m < 60);
		assert(s >= 0 && s < 60);
		assert(ms >= 0 && ms < 1000);
	}

	int32 TimeOfDay::GetHour() const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);
		return h;
	}
	int32 TimeOfDay::GetMinute() const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);
		return m;
	}
	int32 TimeOfDay::GetSecond() const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);
		return s;
	}
	int32 TimeOfDay::GetMillisecond() const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);
		return ms;
	}

	void TimeOfDay::Unpack(int32& h, int32& m, int32& s, int32& ms) const
	{
		int32 t = m_time;
		ms = m_time % 1000;
		t /= 1000;

		s = t % 60;
		t /= 60;

		m = t % 60;
		t /= 60;

		h = t;
	}

	int32 TimeOfDay::Pack(int32 h, int32 m, int32 s, int32 ms)
	{
		return (h * 3600 + m * 60 + s) * 1000 + ms;
	}

	bool TimeOfDay::ParseHHMMSS(const std::string& str)
	{
		if (str.size() == 6)
		{
			int32 hour = StringUtils::ParseInt32(str.substr(0, 2));
			int32 minute = StringUtils::ParseInt32(str.substr(2, 2));
			int32 sec = StringUtils::ParseInt32(str.substr(4, 2));

			*this = TimeOfDay(hour, minute, sec, 0);
			return true;
		}
		else
		{
			List<int32> results;
			StringUtils::SplitParseInts(str, results, "-/.: ");
			if (results.getCount() == 3)
			{
				*this = TimeOfDay(results[0], results[1], results[2], 0);
				return true;
			}
			else if (results.getCount() == 4)
			{
				*this = TimeOfDay(results[0], results[1], results[2], results[3]);
				return true;
			}
			return false;
		}
	}

	std::string TimeOfDay::ToStringHHMMSS() const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);
		return fmt::format("{0:02d}:{1:02d}:{2:02d}:{3:03d}", h, m, s, ms);
	}

	std::string TimeOfDay::ToStringHHMMSS(const char sep) const
	{
		int32 h, m, s, ms;
		Unpack(h, m, s, ms);

		if (sep)
			return fmt::format("{0:02d}{3}{1:02d}{3}{2:02d}", h, m, s, sep);
		
		return fmt::format("{0:02d}{1:02d}{2:02d}", h, m, s);
	}

	TimeOfDay TimeOfDay::Now()
	{
		time_t t = time(NULL);

		struct tm* tmp = gmtime(&t);

		if (tmp)
		{
			return TimeOfDay(tmp->tm_hour, tmp->tm_min, Math::Min(59, tmp->tm_sec), 0);
		}
		return TimeOfDay();
	}

	TimeOfDay TimeOfDay::LastMS()
	{
		return TimeOfDay(23, 59, 59, 999);
	}

	TimeOfDay TimeOfDay::IntraDayAdd(TimeOfDay t, int32 epoch, bool clamp)
	{
		int64 newEpoch = t.m_time + epoch;
		if (clamp)
		{
			if (newEpoch > LastMS().m_time)
				newEpoch = LastMS().m_time;
		}
		return TimeOfDay((int32)newEpoch);
	}

#pragma endregion

#pragma region TimeSpan

	//////////////////////////////////////////////////////////////////////////

	TimeSpan TimeSpan::Get(int32 days, int32 hours, int32 minutes, int32 seconds, int32 ms)
	{
		return TimeSpan((int64)days * 86400 * 1000 + TimeOfDay(hours, minutes, seconds, ms).Epoch());
	}
	TimeSpan TimeSpan::GetDays(int32 days) 
	{
		return TimeSpan((int64)days * 86400 * 1000);
	}
	TimeSpan TimeSpan::GetHours(int32 hours)
	{
		return TimeSpan((int64)hours * 3600 * 1000);
	}
	TimeSpan TimeSpan::GetMinutes(int32 minutes)
	{
		return TimeSpan((int64)minutes * 60 * 1000);
	}
	TimeSpan TimeSpan::GetSeconds(int32 seconds)
	{
		return TimeSpan((int64)seconds * 1000);
	}

#pragma endregion


#pragma region DateTime

	//////////////////////////////////////////////////////////////////////////

	int64 DateTime::Epoch() const
	{
		return m_date.Epoch(0, 0, 0) + m_timeOfDay.Epoch();
	}

	bool DateTime::Parse(const std::string& str)
	{
		size_t pos = str.find(' ');
		if (pos != std::string::npos)
		{
			return m_date.ParseYYYYMMDD(str.substr(0, pos)) && m_timeOfDay.ParseHHMMSS(str.substr(pos + 1));
		}

		m_timeOfDay = TimeOfDay(0);
		return m_date.ParseYYYYMMDD(str);
	}

	std::string DateTime::ToString() const
	{
		return m_date.ToStringYYYYMMDD() + " " + m_timeOfDay.ToStringHHMMSS();
	}

	DateTime DateTime::Offsetted(TimeSpan ts) const
	{
		int64 epoch = Epoch() + ts.m_ms;
		return DateTime(epoch);
	}
	void DateTime::Offset(TimeSpan ts)
	{
		*this = Offsetted(ts);
	}

	DateTime DateTime::Now()
	{
		return DateTime(Date::Today(), TimeOfDay::Now());
	}

	DateTime DateTime::RoundToDayStart(DateTime d)
	{
		d.m_timeOfDay = TimeOfDay(0);
		return d;
	}
	DateTime DateTime::RoundToDayEnd(DateTime d)
	{
		d.m_timeOfDay = TimeOfDay::LastMS();
		return d;
	}
	DateTime DateTime::RoundToNextDayStart(DateTime d)
	{
		d.m_date++;
		d.m_timeOfDay = TimeOfDay(0);
		return d;
	}
	DateTime DateTime::RoundToPrevDayStart(DateTime d)
	{
		d.m_date--;
		d.m_timeOfDay = TimeOfDay(0);
		return d;
	}
	DateTime DateTime::RoundToNextDayEnd(DateTime d)
	{
		d.m_date++;
		d = RoundToDayEnd(d);
		return d;
	}

	DateTime DateTime::RoundToWeekStart(DateTime d)
	{
		d = RoundToDayStart(d);
		while (d.m_date.GetWeekDay() != Weekday::Sunday)
		{
			d.m_date--;
		}
		return d;
	}
	DateTime DateTime::RoundToWeekEnd(DateTime d)
	{
		while (d.m_date.GetWeekDay() != Weekday::Saturday)
		{
			d.m_date++;
		}
		d = RoundToDayEnd(d);
		return d;
	}

	/*
	DateTime DateTime::RoundUpIntraday(DateTime d, int32 seconds)
	{
		if (seconds > 0)
		{
			int32 epoch = d.m_timeOfDay.Epoch();
			int32 ms = seconds * 1000;

			epoch += ms - (epoch % ms);

			if (epoch >= TimeOfDay::LastMS().Epoch())
			{
				d.m_date++;
				d.m_timeOfDay = TimeOfDay(0);
			}
			else
				d.m_timeOfDay = TimeOfDay(epoch);
			
		}
		return d;
	}*/

	DateTime DateTime::RoundDown(DateTime d, TimeSpan ts)
	{
		if (ts.m_ms > 0)
		{
			int64 epoch = d.Epoch();
			int64 ms = ts.m_ms;
			epoch -= epoch % ms;

			d = DateTime(epoch);
		}
		return d;
	}
	DateTime DateTime::RoundUp(DateTime d, TimeSpan ts)
	{
		if (ts.m_ms > 0)
		{
			int64 epoch = d.Epoch();
			int64 ms = ts.m_ms;
			//epoch += ms - (epoch % ms);
			epoch += ms - 1 - ((epoch - 1) % ms);

			d = DateTime(epoch);
		}
		return d;
	}

	//DateTime DateTime::RoundDownIntraday(DateTime d, int32 seconds)
	//{
	//	if (seconds > 0)
	//	{
	//		int32 epoch = d.m_timeOfDay.Epoch();
	//		int32 ms = seconds * 1000;
	//		
	//		epoch -= epoch % ms;
	//
	//		d.m_timeOfDay = TimeOfDay(epoch);
	//	}
	//	return d;
	//}

#pragma endregion
	
}