#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/utilities.hpp>
#include <sstream>

TEST(Tokenizer, consume_typed_tokens)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "vec3d   1.0    2.0    3.14 \n\n\n    \nage   18 \n\n\n\n\n   #this is a comment do ignoree     this line       \nname joe\nfamilyName    Rustom    ";

    Utils::Tokens tokens{std::istringstream(ss.str())};

    float x,y,z;

    std::tie(std::ignore, x, y, z) = *ConsumeTokens<std::string, float, float, float>(tokens);
    EXPECT_FLOAT_EQ(x, 1.0f);
    EXPECT_FLOAT_EQ(y, 2.0f);
    EXPECT_FLOAT_EQ(z, 3.14f);

    int age;

    std::tie(std::ignore, age) = *ConsumeTokens<std::string, int>(tokens);
    EXPECT_EQ(age, 18);

    std::string name;

    std::tie(std::ignore, name) = *ConsumeTokens<std::string, std::string>(tokens);
    EXPECT_EQ(name, "joe");

    std::string familyName;
    
    std::tie(std::ignore, familyName) = *ConsumeTokens<std::string, std::string>(tokens);
    EXPECT_EQ(familyName, "Rustom");

    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, ignore_whole_comment_line)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n#this is a comment       \n\n\n\n\n";

    Utils::Tokens tokens{std::istringstream(ss.str())};
    EXPECT_TRUE(tokens.empty());
}
