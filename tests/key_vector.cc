import jowi.test_lib;
import jowi.generic;
#include <jowi/test_lib.hpp>
#include <string>
#include <vector>

namespace test_lib = jowi::test_lib;
namespace generic = jowi::generic;

JOWI_ADD_TEST(key_vector_default_constructor) {
  generic::KeyVector<int, std::string> kv;
  test_lib::assert_equal(kv.size(), 0u);
  test_lib::assert_equal(kv.empty(), true);
}

JOWI_ADD_TEST(key_vector_initializer_list_constructor) {
  generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}, {3, "three"}};
  test_lib::assert_equal(kv.size(), 3u);
  test_lib::assert_equal(kv.empty(), false);
}

JOWI_ADD_TEST(key_vector_container_constructor) {
  std::vector<std::pair<int, std::string>> container{{10, "ten"}, {20, "twenty"}};
  generic::KeyVector<int, std::string> kv{std::move(container)};
  test_lib::assert_equal(kv.size(), 2u);
}

JOWI_ADD_TEST(key_vector_get_existing_key) {
  generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}, {3, "three"}};

  auto result = kv.get(2);
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "two");
}

JOWI_ADD_TEST(key_vector_get_nonexistent_key) {
  generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}};

  auto result = kv.get(99);
  test_lib::assert_equal(result.has_value(), false);
}

JOWI_ADD_TEST(key_vector_const_get) {
  const generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}};

  auto result = kv.get(1);
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "one");
}

JOWI_ADD_TEST(key_vector_operator_bracket) {
  generic::KeyVector<int, std::string> kv{{5, "five"}, {10, "ten"}};

  auto result = kv[5];
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "five");

  auto missing = kv[999];
  test_lib::assert_equal(missing.has_value(), false);
}

JOWI_ADD_TEST(key_vector_const_operator_bracket) {
  const generic::KeyVector<int, std::string> kv{{7, "seven"}};

  auto result = kv[7];
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "seven");
}

JOWI_ADD_TEST(key_vector_emplace_new_key) {
  generic::KeyVector<int, std::string> kv;

  auto &value = kv.emplace(42, "forty-two");
  test_lib::assert_equal(value, "forty-two");
  test_lib::assert_equal(kv.size(), 1u);

  auto result = kv.get(42);
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "forty-two");
}

JOWI_ADD_TEST(key_vector_emplace_existing_key_replaces) {
  generic::KeyVector<int, std::string> kv{{1, "original"}};

  auto &value = kv.emplace(1, "replaced");
  test_lib::assert_equal(value, "replaced");
  test_lib::assert_equal(kv.size(), 1u);

  auto result = kv.get(1);
  test_lib::assert_equal(result.value().get(), "replaced");
}

JOWI_ADD_TEST(key_vector_insert) {
  generic::KeyVector<int, std::string> kv;

  auto &value = kv.insert(100, "hundred");
  test_lib::assert_equal(value, "hundred");
  test_lib::assert_equal(kv.size(), 1u);
}

JOWI_ADD_TEST(key_vector_remove_existing) {
  generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}, {3, "three"}};

  auto removed = kv.remove(2);
  test_lib::assert_equal(removed.has_value(), true);
  test_lib::assert_equal(removed.value(), "two");
  test_lib::assert_equal(kv.size(), 2u);

  // Verify it's actually gone
  auto result = kv.get(2);
  test_lib::assert_equal(result.has_value(), false);
}

JOWI_ADD_TEST(key_vector_remove_nonexistent) {
  generic::KeyVector<int, std::string> kv{{1, "one"}};

  auto removed = kv.remove(999);
  test_lib::assert_equal(removed.has_value(), false);
  test_lib::assert_equal(kv.size(), 1u);
}

JOWI_ADD_TEST(key_vector_iteration) {
  generic::KeyVector<int, std::string> kv{{1, "one"}, {2, "two"}, {3, "three"}};

  int count = 0;
  for (const auto &[key, value] : kv) {
    count++;
    // Basic sanity check that we can access key and value
    test_lib::assert_equal(key >= 1 && key <= 3, true);
    test_lib::assert_equal(value.empty(), false);
  }
  test_lib::assert_equal(count, 3);
}

JOWI_ADD_TEST(key_vector_const_iteration) {
  const generic::KeyVector<int, std::string> kv{{10, "ten"}, {20, "twenty"}};

  int count = 0;
  for (const auto &[key, value] : kv) {
    count++;
  }
  test_lib::assert_equal(count, 2);
}

JOWI_ADD_TEST(key_vector_keys_view) {
  generic::KeyVector<int, std::string> kv{{5, "five"}, {15, "fifteen"}, {25, "twenty-five"}};

  auto keys_view = kv.keys();
  std::vector<int> keys_vec;

  for (const auto &key : keys_view) {
    keys_vec.push_back(key);
  }

  test_lib::assert_equal(keys_vec.size(), 3u);
  // Keys should be in insertion order
  test_lib::assert_equal(keys_vec[0], 5);
  test_lib::assert_equal(keys_vec[1], 15);
  test_lib::assert_equal(keys_vec[2], 25);
}

JOWI_ADD_TEST(key_vector_string_keys) {
  generic::KeyVector<std::string, int> kv{{"hello", 1}, {"world", 2}, {"test", 3}};

  auto result = kv.get("world");
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), 2);

  kv.emplace("new_key", 42);
  test_lib::assert_equal(kv.size(), 4u);
}

JOWI_ADD_TEST(key_vector_modification_through_reference) {
  generic::KeyVector<int, std::string> kv{{1, "original"}};

  auto result = kv.get(1);
  test_lib::assert_equal(result.has_value(), true);

  // Modify through reference
  result.value().get() = "modified";

  auto check = kv.get(1);
  test_lib::assert_equal(check.value().get(), "modified");
}

JOWI_ADD_TEST(key_vector_comparable_concept) {
  generic::KeyVector<int, std::string> kv{{42, "answer"}};

  // Test that we can use different but comparable types
  long long big_key = 42LL;
  auto result = kv.get(big_key);
  test_lib::assert_equal(result.has_value(), true);
  test_lib::assert_equal(result.value().get(), "answer");
}

JOWI_ADD_TEST(key_vector_empty_after_removal) {
  generic::KeyVector<int, std::string> kv{{1, "only"}};

  test_lib::assert_equal(kv.empty(), false);

  auto removed = kv.remove(1);
  test_lib::assert_equal(removed.has_value(), true);
  test_lib::assert_equal(kv.empty(), true);
  test_lib::assert_equal(kv.size(), 0u);
}