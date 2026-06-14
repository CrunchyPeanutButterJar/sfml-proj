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

