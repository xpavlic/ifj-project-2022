#include "src/strings.h"

#include <array>

#include "gtest/gtest.h"

class EmptyString : public ::testing::Test {
   protected:
    dyn_str str;
    dyn_str *strPtr = &str;
};
class NonEmptyString : public ::testing::Test {
   protected:
    void setUp() {
        char chars[13] = {'H', '3', 'l', 'l', 'o', ' ',
                          'W', 'o', 'r', 'l', 'd', '!'};
        for (char c : chars) {
            add_char(strPtr, c);
        }
    }

    dyn_str str;
    dyn_str *strPtr = &str;
};

TEST_F(EmptyString, init_test) {
    EXPECT_FALSE(init_str(strPtr));
    EXPECT_EQ(strPtr->length, 0);
    EXPECT_EQ(strPtr->size, 16);
    EXPECT_STREQ(strPtr->str, "\0");
}
TEST_F(EmptyString, add_test) {
    init_str(strPtr);
    std::array<char, 13> chars = {'H', '3', 'l', 'l', 'o', ' ', 'W',
                                  'o', 'r', 'l', 'd', '!', '\0'};
    for (char c : chars) {
        add_char(strPtr, c);
    }
    EXPECT_STREQ(strPtr->str, &chars[0]);
}
