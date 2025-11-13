#include "../src/statsig.h"
#include <cassert>
#include <gtest/gtest.h>

TEST(UserTest, Typing) {
    statsig_cpp_core::User user = statsig_cpp_core::User("user_id");
}

TEST(UserTest, Serialization) {

}