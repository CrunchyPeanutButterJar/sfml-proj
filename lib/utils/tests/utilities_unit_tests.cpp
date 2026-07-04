#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/utilities.hpp>
#include <utils/bitmask.hpp>
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


TEST(Tokenizer, consume_tokens_return_nullopt_in_case_of_failure)
{
    using namespace ::testing;
    
    {
        std::ostringstream ss;
        ss << "      \n\n\n\n\n\n\n#this is a comment       \n\n\n\n\n";

        Utils::Tokens tokens{std::istringstream(ss.str())};
        auto result = ConsumeTokens<int, float, double>(tokens);
        EXPECT_FALSE(result.has_value());
    }

    {
        std::ostringstream ss;
        ss << "      \n\n\n\n\n\n\n joeeee tempppp       \n\n\n\n\n";

        Utils::Tokens tokens{std::istringstream(ss.str())};
        auto result = ConsumeTokens<int, float, double>(tokens);
        EXPECT_FALSE(result.has_value());
    }
}

TEST(Tokenizer, skips_line_correctly)
{
    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n  this is a   line       \n\n  joe   \n\n\n";
    Utils::Tokens tokens{std::istringstream{ss.str()}};
    tokens.skipLine();
    auto token = ConsumeToken<std::string>(tokens);
    ASSERT_TRUE(token.has_value());
    EXPECT_EQ(*token, "joe");
}

TEST(number_theory, euclid_division_pgcd_ppcm) 
{
    using namespace ::testing;

    size_t n1 = 90;
    size_t n2 = 24;

    EXPECT_EQ(Utils::pgcd(n1, n2), Utils::pgcd(n2, n1));
    EXPECT_EQ(Utils::pgcd(n1, n2), 6);

    EXPECT_EQ(Utils::ppcm(n1, n2), Utils::ppcm(n2, n1));
    EXPECT_EQ(Utils::ppcm(n1, n2), 360);

    size_t w = 1920;
    size_t h = 1080;

    size_t d = Utils::pgcd(1920, 1080);
    size_t a = w/d;
    size_t b = h/d;
    EXPECT_EQ(a, 16);
    EXPECT_EQ(b, 9);

    EXPECT_EQ(Utils::pgcd(55,1), 1);
}

TEST(bitmask, smoke_test)
{
    const Bitset PositionComponent = 1 << 0;
    const Bitset SpriteComponent = 1 << 1;
    const Bitset MovableComponent = 1 << 2;
    const Bitset StateComponent = 1 << 3;

    Bitmask mask{PositionComponent | SpriteComponent | MovableComponent | StateComponent};
    EXPECT_TRUE(mask.getBit(0));
    EXPECT_TRUE(mask.getBit(1));
    EXPECT_TRUE(mask.getBit(2));
    EXPECT_TRUE(mask.getBit(3));

    mask.clear();
    EXPECT_FALSE(mask.getBit(0));
    EXPECT_FALSE(mask.getBit(1));
    EXPECT_FALSE(mask.getBit(2));
    EXPECT_FALSE(mask.getBit(3));

    mask.toggleBit(1);
    EXPECT_FALSE(mask.getBit(0));
    EXPECT_TRUE(mask.getBit(1));
    EXPECT_FALSE(mask.getBit(2));
    EXPECT_FALSE(mask.getBit(3));
    
    mask.toggleBit(1);
    EXPECT_FALSE(mask.getBit(0));
    EXPECT_FALSE(mask.getBit(1));
    EXPECT_FALSE(mask.getBit(2));
    EXPECT_FALSE(mask.getBit(3));

    Bitmask moveAndPosition{};
    moveAndPosition.setMask(MovableComponent | PositionComponent);

    Bitmask moveAndState{};
    moveAndState.setMask(MovableComponent | StateComponent);

    EXPECT_TRUE(moveAndPosition.matches(moveAndState, MovableComponent));
    EXPECT_TRUE(moveAndState.matches(moveAndPosition, MovableComponent));

    EXPECT_FALSE(moveAndPosition.matches(moveAndState));
}