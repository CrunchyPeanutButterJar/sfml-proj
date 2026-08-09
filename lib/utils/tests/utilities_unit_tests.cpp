#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include <utils/bitmask.hpp>
#include <utils/utilities.hpp>

TEST(Tokenizer, consume_typed_tokens)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "vec3d   1.0    2.0    3.14 \n\n\n    \nage   18 \n\n\n\n\n   #this is a comment do "
          "ignoree     this line       \nname joe\nfamilyName    Rustom    ";

    utils::Tokens tokens{std::istringstream(ss.str())};

    float x = NAN;
    float y = NAN;
    float z = NAN;

    std::tie(std::ignore, x, y, z) = *consumeTokens<std::string, float, float, float>(tokens);
    EXPECT_FLOAT_EQ(x, 1.0F);
    EXPECT_FLOAT_EQ(y, 2.0F);
    EXPECT_FLOAT_EQ(z, 3.14F);

    int age = 0;

    std::tie(std::ignore, age) = *consumeTokens<std::string, int>(tokens);
    EXPECT_EQ(age, 18);

    std::string name;

    std::tie(std::ignore, name) = *consumeTokens<std::string, std::string>(tokens);
    EXPECT_EQ(name, "joe");

    std::string family_name;

    std::tie(std::ignore, family_name) = *consumeTokens<std::string, std::string>(tokens);
    EXPECT_EQ(family_name, "Rustom");

    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, ignore_whole_comment_line)
{
    using namespace ::testing;

    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n#this is a comment       \n\n\n\n\n";

    utils::Tokens tokens{std::istringstream(ss.str())};
    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, consume_tokens_return_nullopt_in_case_of_failure)
{
    using namespace ::testing;

    {
        std::ostringstream ss;
        ss << "      \n\n\n\n\n\n\n#this is a comment       \n\n\n\n\n";

        utils::Tokens tokens{std::istringstream(ss.str())};
        auto          result = consumeTokens<int, float, double>(tokens);
        EXPECT_FALSE(result.has_value());
    }

    {
        std::ostringstream ss;
        ss << "      \n\n\n\n\n\n\n joeeee tempppp       \n\n\n\n\n";

        utils::Tokens tokens{std::istringstream(ss.str())};
        auto          result = consumeTokens<int, float, double>(tokens);
        EXPECT_FALSE(result.has_value());
    }
}

TEST(Tokenizer, skips_line_correctly)
{
    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n  this is a   line       \n\n  joe   \n\n\n";
    utils::Tokens tokens{std::istringstream{ss.str()}};
    tokens.skipLine();
    auto token = consumeToken<std::string>(tokens);
    ASSERT_TRUE(token.has_value());
    EXPECT_EQ(*token, "joe");
}

TEST(Tokenizer, capture_quoted_strings)
{
    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n  \"this is a   string \n \n i want to capture\"       \n\n  joe   "
          "\n\n\n";

    utils::Tokens tokens{std::istringstream{ss.str()}};
    tokens.captureQuotedStrings('"');

    const auto QuotedString = *consumeToken<std::string>(tokens);
    EXPECT_EQ(QuotedString, "this is a   string \n \n i want to capture");

    const auto Joe = *consumeToken<std::string>(tokens);
    EXPECT_EQ(Joe, "joe");

    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, capture_strings_test)
{
    std::ostringstream ss;
    ss << "      \n\n\n\n\n\n\n     joe and jp   ---          \n\n\n\n      -----"
          "\n\n\n";

    utils::Tokens tokens{std::istringstream{ss.str()}, '-'};

    const auto Str = *consumeToken<std::string>(tokens);
    EXPECT_EQ(Str, "joe and jp");

    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, change_delimiter)
{
    std::ostringstream ss;
    ss << "\n\n\n\n\n    Component 6 footstep:\"1,4\" joe\n\n\n\n\n  \n \n \n \n \n      ";

    utils::Tokens tokens{std::istringstream{ss.str()}};

    consumeTokens<std::string, int>(tokens);

    {
        auto scoped = tokens.setDelimiterScoped(':');
        auto str    = *consumeToken<std::string>(tokens);
        EXPECT_EQ(str, "footstep");
    }

    tokens.captureQuotedStrings('"');
    auto value = *consumeToken<std::string>(tokens);
    tokens.captureQuotedStrings({});

    utils::Tokens    inner_tokens{std::istringstream{value}, ','};
    std::vector<int> frames;
    while (!inner_tokens.empty())
    {
        frames.push_back(*consumeToken<int>(inner_tokens));
    }

    ASSERT_EQ(frames.size(), 2);
    EXPECT_EQ(frames[0], 1);
    EXPECT_EQ(frames[1], 4);

    auto joe = *consumeToken<std::string>(tokens);
    EXPECT_EQ(joe, "joe");

    EXPECT_TRUE(tokens.empty());
}

TEST(Tokenizer, capture_lines)
{
    std::ostringstream ss;
    ss << "\n\n       \n\n\n    joee this is   aa  test        \n\n\n\n   last line 1 2 3      \n  "
          "  \n";

    utils::Tokens tokens{std::istringstream{ss.str()}, '\n'};
    auto          first_line = *consumeToken<std::string>(tokens);
    EXPECT_EQ(first_line, "joee this is   aa  test");

    auto second_line = *consumeToken<std::string>(tokens);
    EXPECT_EQ(second_line, "last line 1 2 3");

    EXPECT_TRUE(tokens.empty());
}

TEST(number_theory, euclid_division_pgcd_ppcm)
{
    using namespace ::testing;

    size_t n1 = 90;
    size_t n2 = 24;

    EXPECT_EQ(utils::pgcd(n1, n2), utils::pgcd(n2, n1));
    EXPECT_EQ(utils::pgcd(n1, n2), 6);

    EXPECT_EQ(utils::ppcm(n1, n2), utils::ppcm(n2, n1));
    EXPECT_EQ(utils::ppcm(n1, n2), 360);

    size_t w = 1920;
    size_t h = 1080;

    size_t d = utils::pgcd(1920, 1080);
    size_t a = w / d;
    size_t b = h / d;
    EXPECT_EQ(a, 16);
    EXPECT_EQ(b, 9);

    EXPECT_EQ(utils::pgcd(55, 1), 1);
}

TEST(bitmask, smoke_test)
{
    const utils::Bitset PositionComponent = 1 << 0;
    const utils::Bitset SpriteComponent   = 1 << 1;
    const utils::Bitset MovableComponent  = 1 << 2;
    const utils::Bitset StateComponent    = 1 << 3;

    utils::Bitmask mask{PositionComponent | SpriteComponent | MovableComponent | StateComponent};
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

    utils::Bitmask move_and_position{};
    move_and_position.setMask(MovableComponent | PositionComponent);

    utils::Bitmask move_and_state{};
    move_and_state.setMask(MovableComponent | StateComponent);

    EXPECT_TRUE(move_and_position.matches(move_and_state, MovableComponent));
    EXPECT_TRUE(move_and_state.matches(move_and_position, MovableComponent));

    EXPECT_FALSE(move_and_position.matches(move_and_state));
}