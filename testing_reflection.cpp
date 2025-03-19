
#include "json.h"
#include <gtest/gtest.h>
#include <string>
#include <ranges>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;

struct spouse_t
{
	std::string name;
	double height;
};

struct child_t
{
	std::string name;
	std::uint16_t age;
};

struct person_t
{
	std::string name;
	std::uint16_t age;
	std::optional<std::vector<child_t>> children;
	bool good_looking;
	std::optional<spouse_t> spouse;
	std::optional<std::uint16_t> height;
	double alcohol_level;
};

struct folks_t
{
	std::vector<person_t> folks;
};

namespace
{
	TEST(ReflectionTest, struct_to_json)
	{
		folks_t folks;

		person_t me;
		me.name = "Sander Greni";
		me.age = 51;
		me.children = std::vector<child_t>();
		me.children->emplace_back("Jo", 21);
		me.children->emplace_back("Mikkel", 18);
		me.good_looking = false;
		me.spouse = spouse_t{ "Marte", 1.66 };
		me.alcohol_level = 3.1415926535897932384626433832795;

		folks.folks.push_back(me);

		folks.folks.emplace_back("Jo Greni", 21, std::nullopt, true, std::nullopt, 179, 0.0);
		ASSERT_EQ(vs::json::to_string(folks), R"({"folks":[{"name":"Sander Greni","age":51,"children":[{"name":"Jo","age":21},{"name":"Mikkel","age":18}],"good_looking":false,"spouse":{"name":"Marte","height":1.66},"height":null,"alcohol_level":3.14159265359},{"name":"Jo Greni","age":21,"children":null,"good_looking":true,"spouse":null,"height":179,"alcohol_level":0.0}]})");
	}

	TEST(ReflectionTest, json_to_struct)
	{
		std::string const json = R"({"name":"Jo","age":21,"something":42,"tp":"2025-01-16T13:06:28.747Z"})";
		auto child = vs::json::from_string<child_t>(json);

		ASSERT_EQ(child.name, "Jo");
		ASSERT_EQ(child.age, 21);
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


