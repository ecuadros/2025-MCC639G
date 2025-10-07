#include <gtest/gtest.h>
#include <sstream>
#include "linked_list/LinkedListTraits.h"

// Test fixture for LinkedListTraits tests
class LinkedListTraitsTest : public ::testing::Test {
protected:
    LinkedListTraits<int> list;
};

// Test that a new list is empty
TEST_F(LinkedListTraitsTest, IsEmptyInitially) {
    ASSERT_EQ(list.begin(), list.end());
}

// Test adding elements to the list
TEST_F(LinkedListTraitsTest, AddElements) {
    list.add(1);
    list.add(2);
    
    auto it = list.begin();
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 1);
    ++it;
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(it, list.end());
}

// Test the copy constructor
TEST_F(LinkedListTraitsTest, CopyConstructor) {
    list.add(10); list.add(20);
    LinkedListTraits<int> copied_list = list; // Copy constructor

    // Check that the content is the same
    std::stringstream original_ss, copied_ss;
    original_ss << list;
    copied_ss << copied_list;
    ASSERT_EQ(original_ss.str(), copied_ss.str());

    // Check that they are independent (deep copy)
    copied_list.add(30);
    original_ss.str(""); // Clear stringstream
    copied_ss.str("");
    original_ss << list;
    copied_ss << copied_list;
    ASSERT_NE(original_ss.str(), copied_ss.str());
}

// Test the move constructor
TEST_F(LinkedListTraitsTest, MoveConstructor) {
    list.add(10); list.add(20);
    LinkedListTraits<int> moved_list = std::move(list);

    // Check that the new list has the content
    std::stringstream ss;
    ss << moved_list;
    ASSERT_EQ(ss.str(), "10 20 ");

    // Check that the old list is empty
    ASSERT_EQ(list.begin(), list.end());
}

// Test the output stream operator
TEST_F(LinkedListTraitsTest, OutputStream) {
    list.add(1); list.add(2); list.add(3);
    std::stringstream ss;
    ss << list;
    ASSERT_EQ(ss.str(), "1 2 3 ");
}

// Test the input stream operator
TEST_F(LinkedListTraitsTest, InputStream) {
    std::stringstream ss("10 20 30");
    ss >> list;

    std::stringstream output_ss;
    output_ss << list;
    ASSERT_EQ(output_ss.str(), "10 20 30 ");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
