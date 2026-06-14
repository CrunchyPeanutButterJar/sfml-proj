#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utilities/utilities.hpp>
#include <sstream>

TEST(split_into_lines, split_into_lines_is_correctly_split)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "line1   x    y    z    \n#line2    \nline3  \nline4    ";

    auto lines = Utils::Tokenize(std::istringstream(ss.str()));
    ASSERT_EQ(lines.size(), 3);
    EXPECT_THAT(lines, 
    ElementsAre(
        ElementsAre("line1", "x", "y", "z"),
        ElementsAre("line3"),
        ElementsAre("line4")
    ));
}

template<typename... T>
auto ConsumeTokens(std::vector<std::string>& l_tokens)
{
    Utils::ConsumeTokens<std::string>(l_tokens); // discard key
    return Utils::ConsumeTokens<T...>(l_tokens);
}

template <typename... T>
auto ConsumeTokens(std::vector<std::string>&& l_tokens)
{
    return ::ConsumeTokens<T...>(l_tokens);
}

TEST(typed_transform, transform_token_typed_api)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "vec3d   1.0    2.0    3.14    \nage   18    \nname joe  \nfamilyName    Rustom    ";

    auto lines = Utils::Tokenize(std::istringstream(ss.str()));
    ASSERT_EQ(lines.size(), 4);

    auto [x, y, z] = ConsumeTokens<float, float, float>(std::move(lines[0]));
    EXPECT_FLOAT_EQ(x, 1.0f);
    EXPECT_FLOAT_EQ(y, 2.0f);
    EXPECT_FLOAT_EQ(z, 3.14f);

    auto [age] = ConsumeTokens<int>(lines[1]);
    EXPECT_EQ(age, 18);

    auto [name] = ConsumeTokens<std::string>(lines[2]);
    EXPECT_EQ(name, "joe");

    auto [familyName] = ConsumeTokens<std::string>(lines[3]);
    EXPECT_EQ(familyName, "Rustom");
}

