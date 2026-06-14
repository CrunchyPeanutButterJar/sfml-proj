#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utilities.hpp>
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

TEST(typed_transform, transform_token_typed_api)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "vec3d   1.0    2.0    3.14    \nage   18    \nname joe  \nfamilyName    Rustom    ";

    auto lines = Utils::Tokenize(std::istringstream(ss.str()));
    ASSERT_EQ(lines.size(), 4);

    auto [x, y, z] = Utils::ReadTokenValues<float, float, float>(lines[0]);
    EXPECT_FLOAT_EQ(x, 1.0f);
    EXPECT_FLOAT_EQ(y, 2.0f);
    EXPECT_FLOAT_EQ(z, 3.14f);

    auto [age] = Utils::ReadTokenValues<int>(lines[1]);
    EXPECT_EQ(age, 18);
    
    auto [name] = Utils::ReadTokenValues<std::string>(lines[2]);
    EXPECT_EQ(name, "joe");

    auto [familyName] = Utils::ReadTokenValues<std::string>(lines[3]);
    EXPECT_EQ(familyName, "Rustom");
}

