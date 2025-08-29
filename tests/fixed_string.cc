import moderna.test_lib;
import moderna.generic;
#include <moderna/test_lib.hpp>
#include <string>

namespace test_lib = moderna::test_lib;
namespace generic = moderna::generic;

MODERNA_ADD_TEST(string_always_ends_with_null) {
  generic::fixed_string<10> fs{"hello"};
  // The buffer should always have null terminator at the end
  test_lib::assert_equal(fs.c_str()[fs.length()], '\0');

  // Even empty string should be null terminated
  generic::fixed_string<5> empty_fs;
  test_lib::assert_equal(empty_fs.c_str()[0], '\0');
}

MODERNA_ADD_TEST(end_is_always_accessible_and_dereferencable) {
  generic::fixed_string<10> fs{"test"};

  // end() should be dereferenceable and point to null terminator
  test_lib::assert_equal(*fs.end(), '\0');

  // Should work for empty string too
  generic::fixed_string<5> empty_fs;
  test_lib::assert_equal(*empty_fs.end(), '\0');

  // Should work after modifications
  fs.push_back('!');
  test_lib::assert_equal(*fs.end(), '\0');
}

MODERNA_ADD_TEST(emplace_back_increases_size_until_limit) {
  generic::fixed_string<5> fs;
  test_lib::assert_equal(fs.size(), 0u);

  // Add characters one by one
  fs.emplace_back('a');
  test_lib::assert_equal(fs.size(), 1u);

  fs.emplace_back('b');
  test_lib::assert_equal(fs.size(), 2u);

  fs.emplace_back('c');
  test_lib::assert_equal(fs.size(), 3u);

  fs.emplace_back('d');
  test_lib::assert_equal(fs.size(), 4u);

  fs.emplace_back('e');
  test_lib::assert_equal(fs.size(), 5u);

  // At capacity - should do nothing
  fs.emplace_back('f');
  test_lib::assert_equal(fs.size(), 5u);

  // Verify content is correct
  std::string_view sv = fs;
  test_lib::assert_equal(sv, "abcde");
}

MODERNA_ADD_TEST(emplace_format_formats_up_to_size) {
  generic::fixed_string<10> fs;

  // Format string that fits
  fs.emplace_format("{}", "hello");
  test_lib::assert_equal(fs.size(), 5u);
  std::string_view sv = fs;
  test_lib::assert_equal(sv, "hello");

  // Clear and try formatting that would exceed capacity
  fs.truncate();
  fs.emplace_format("{}", "this is a very long string");

  // Should be truncated to fit in 10 characters
  test_lib::assert_equal(fs.size(), 10u);
  std::string_view sv2 = fs;
  test_lib::assert_equal(sv2, "this is a ");
}

MODERNA_ADD_TEST(end_is_always_null_terminator) {
  generic::fixed_string<8> fs{"abc"};
  test_lib::assert_equal(*fs.end(), '\0');

  // Add more characters
  fs.push_back('d');
  fs.push_back('e');
  test_lib::assert_equal(*fs.end(), '\0');

  // Fill to capacity
  fs.push_back('f');
  fs.push_back('g');
  fs.push_back('h');
  test_lib::assert_equal(*fs.end(), '\0');
  test_lib::assert_equal(fs.size(), 8u);
}

MODERNA_ADD_TEST(begin_end_represents_string_without_null) {
  generic::fixed_string<10> fs{"world"};

  // Distance between begin and end should equal length
  test_lib::assert_equal(fs.end() - fs.begin(), static_cast<ptrdiff_t>(fs.length()));

  // The range [begin, end) should not include the null terminator
  std::string constructed_from_range(fs.begin(), fs.end());
  test_lib::assert_equal(constructed_from_range, "world");

  // Verify null terminator is right after end()
  test_lib::assert_equal(*fs.end(), '\0');

  // Test with empty string
  generic::fixed_string<5> empty_fs;
  test_lib::assert_equal(empty_fs.end() - empty_fs.begin(), 0);
  test_lib::assert_equal(*empty_fs.end(), '\0');
}

MODERNA_ADD_TEST(string_view_conversion_excludes_null) {
  generic::fixed_string<15> fs{"test string"};

  std::string_view sv = fs;
  test_lib::assert_equal(sv.length(), fs.length());
  test_lib::assert_equal(sv, "test string");

  // Verify the string_view doesn't include null terminator
  test_lib::assert_equal(sv.data()[sv.length()], '\0');
}

MODERNA_ADD_TEST(truncate_resets_to_empty_with_null_terminator) {
  generic::fixed_string<10> fs{"hello"};
  test_lib::assert_equal(fs.size(), 5u);

  fs.truncate();
  test_lib::assert_equal(fs.size(), 0u);
  test_lib::assert_equal(*fs.end(), '\0');
  test_lib::assert_equal(*fs.begin(), '\0');

  std::string_view sv = fs;
  test_lib::assert_equal(sv.empty(), true);
}

MODERNA_ADD_TEST(equality_operator_same_content) {
  generic::fixed_string<10> fs{"hello"};
  std::string_view sv{"hello"};

  test_lib::assert_equal(fs == sv, true);
  test_lib::assert_equal(fs == "hello", true);
}

MODERNA_ADD_TEST(equality_operator_different_content) {
  generic::fixed_string<10> fs{"hello"};
  std::string_view sv{"world"};

  test_lib::assert_equal(fs == sv, false);
  test_lib::assert_equal(fs == "world", false);
}

MODERNA_ADD_TEST(equality_operator_different_length) {
  generic::fixed_string<10> fs{"hello"};

  test_lib::assert_equal(fs == "hi", false);
  test_lib::assert_equal(fs == "hello world", false);
}

MODERNA_ADD_TEST(equality_operator_empty_strings) {
  generic::fixed_string<10> fs;
  std::string_view empty_sv{""};

  test_lib::assert_equal(fs == empty_sv, true);
  test_lib::assert_equal(fs == "", true);
}

MODERNA_ADD_TEST(equality_operator_after_modifications) {
  generic::fixed_string<10> fs;
  fs.emplace_format("{}", "test");

  test_lib::assert_equal(fs == "test", true);

  fs.push_back('!');
  test_lib::assert_equal(fs == "test!", true);
  test_lib::assert_equal(fs == "test", false);
}