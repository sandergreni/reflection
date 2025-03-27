#include "json.h"

namespace vs::date_time
{
	time_point now()
	{
		auto const utc_now = std::chrono::utc_clock::now();
		return std::chrono::time_point_cast<std::chrono::milliseconds>(utc_now);
	}
}

namespace vs::json::set
{
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::string const& str)
	{
		v.SetString(str.data(), static_cast<::rapidjson::SizeType>(str.size()), mpa);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::string_view str)
	{
		v.SetString(str.data(), static_cast<::rapidjson::SizeType>(str.size()), mpa);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::int32_t const i)
	{
		v.SetInt(i);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::int64_t const i)
	{
		v.SetInt64(i);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::uint32_t const u)
	{
		v.SetUint(u);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::uint64_t const u)
	{
		v.SetUint64(u);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, bool const b)
	{
		v.SetBool(b);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::nullopt_t)
	{
		v.SetNull();
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, double const f, std::uint16_t const precision)
	{
		double const scale = std::pow(10, precision);
		v.SetDouble(std::round(f * scale) / scale);
	}

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, vs::date_time::time_point const& tp)
	{
		auto const ms = std::chrono::time_point_cast<std::chrono::milliseconds>(tp); // Get rid of microseconds
		value(v, mpa, std::format("{:%FT%TZ}", ms));
	}
}

namespace vs::json::get
{
	void value(::rapidjson::Value const& v, std::string& str)
	{
		str = v.GetString();
	}

	void value(::rapidjson::Value const& v, std::int32_t& i)
	{
		i = v.GetInt();
	}

	void value(::rapidjson::Value const& v, std::int64_t& i)
	{
		i = v.GetInt64();
	}

	void value(::rapidjson::Value const& v, std::uint16_t& u)
	{
		u = v.GetUint();
	}

	void value(::rapidjson::Value const& v, std::uint32_t& u)
	{
		u = v.GetUint();
	}

	void value(::rapidjson::Value const& v, std::uint64_t& u)
	{
		u = v.GetUint64();
	}

	void value(::rapidjson::Value const& v, bool& b)
	{
		b = v.GetBool();
	}

	void value(::rapidjson::Value const& v, double& f)
	{
		f = v.GetDouble();
	}

	void value(::rapidjson::Value const& v, vs::date_time::time_point& tp)
	{
		if (auto str_len = v.GetStringLength(); str_len == 24) //YYYY-MM-DDThh:mm:ss.sssZ
		{
			std::stringstream ss;
			ss << v.GetString();

			ss >> std::chrono::parse("%FT%TZ", tp);
			//tp = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
		}
	}
}