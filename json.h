
#ifndef __JSON_H__
#define __JSON_H__

#include "reflection.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <optional>
#include <chrono>

namespace vs::date_time
{
	using time_point = std::chrono::time_point<std::chrono::utc_clock, std::chrono::milliseconds>;

	time_point now();
}

namespace vs::json::set
{
	template <typename T>
	requires vs::reflection::reflective<T>
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::vector<T> const& vec);

	template <typename T>
	requires vs::reflection::reflective<T>
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, T const& pod);

	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::string const& str);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::string_view str);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::int32_t const i);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::int64_t const i);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::uint32_t const u);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::uint64_t const u);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, bool const b);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, std::nullopt_t);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>&, double const f);
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, vs::date_time::time_point const& tp);

	template <typename T>
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::optional<T> const& opt)
	{
		if (opt.has_value())
		{
			value(v, mpa, opt.value());
		}
	}

	template <typename T>
	requires vs::reflection::reflective<T>
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, T const& pod)
	{
		auto& obj = v.SetObject();

		vs::reflection::for_each_member(pod, [&obj, &mpa](std::string_view name, auto const& field_value)
			{
				::rapidjson::Value jname(::rapidjson::StringRef(name.data(), name.size()));
				::rapidjson::Value jvalue;
				value(jvalue, mpa, field_value);

				obj.AddMember(jname, jvalue, mpa);
			});

	}

	template <typename T>
	requires vs::reflection::reflective<T>
	void value(::rapidjson::Value& v, ::rapidjson::MemoryPoolAllocator<>& mpa, std::vector<T> const& vec)
	{
		::rapidjson::Value arr(::rapidjson::kArrayType);

		for (auto const& node : vec)
		{
			::rapidjson::Value jvalue;
			value(jvalue, mpa, node);
			arr.PushBack(jvalue, mpa);
		}

		v = arr;
	}

	template <vs::reflection::reflective T>
	void set_doc(::rapidjson::Document& doc, T const& pod)
	{
		auto& mpa = doc.GetAllocator();
		doc.SetObject();

		value(doc, mpa, pod);
	}
}

namespace vs::json::get
{
	void value(::rapidjson::Value const& v, std::string& str);
	void value(::rapidjson::Value const& v, std::int32_t& i);
	void value(::rapidjson::Value const& v, std::int64_t& i);
	void value(::rapidjson::Value const& v, std::uint16_t& u);
	void value(::rapidjson::Value const& v, std::uint32_t& u);
	void value(::rapidjson::Value const& v, std::uint64_t& u);
	void value(::rapidjson::Value const& v, bool& b);
	void value(::rapidjson::Value const& v, double& f);
	void value(::rapidjson::Value const& v, vs::date_time::time_point& tp);

	template <typename T>
	void value(::rapidjson::Value const& v, std::optional<T>& opt)
	{
		T val;
		value(v, val);
		opt = val;
	}

	template <typename T>
	requires vs::reflection::reflective<T>
	void value(::rapidjson::Value const& v, T& pod)
	{
		vs::reflection::for_each_member(pod, [&v](std::string_view name, auto& field_value)
			{
				::rapidjson::Value jname(::rapidjson::StringRef(name.data(), name.size()));
				if (auto it = v.FindMember(jname); it != v.MemberEnd())
				{
					value(it->value, field_value);
				}
			});
	}
}

namespace vs::json
{
	template <vs::reflection::reflective T>
	std::string to_string(T const& plain_old_data)
	{
		::rapidjson::Document doc;

		set::set_doc(doc, plain_old_data);

		::rapidjson::StringBuffer buffer;
		::rapidjson::Writer<::rapidjson::StringBuffer> writer(buffer);

		doc.Accept(writer);

		return buffer.GetString();
	}

	template <vs::reflection::reflective T>
	T from_string(std::string_view json)
	{
		::rapidjson::Document doc;
		doc.Parse(json.data(), json.size());

		if (doc.HasParseError())
		{
			throw std::runtime_error("JSON parse error");
		}

		T pod{};
		get::value(doc, pod);

		return pod;
	}
}

#endif
