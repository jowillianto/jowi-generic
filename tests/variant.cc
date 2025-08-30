import moderna.test_lib;
import moderna.generic;
#include <moderna/test_lib.hpp>
#include <string>
#include <vector>

namespace test_lib = moderna::test_lib;
namespace generic = moderna::generic;
;

MODERNA_ADD_TEST(variant_index_checking) {
  generic::variant<int, std::string, double> v{42};
  test_lib::assert_equal(v.index(), 0);

  v = std::string("hello");
  test_lib::assert_equal(v.index(), 1);

  v = 3.14;
  test_lib::assert_equal(v.index(), 2);

  v = std::string("changed");
  test_lib::assert_equal(v.index(), 1);

  v = 2.71;
  test_lib::assert_equal(v.index(), 2);
}

MODERNA_ADD_TEST(variant_is_correct_type) {
  generic::variant<int, std::string, double> v{42};
  test_lib::assert_true(v.is<int>());
  test_lib::assert_false(v.is<std::string>());
  test_lib::assert_false(v.is<double>());
}

MODERNA_ADD_TEST(variant_is_after_type_change) {
  generic::variant<int, std::string, double> v{42};
  test_lib::assert_true(v.is<int>());

  v = std::string("hello");
  test_lib::assert_false(v.is<int>());
  test_lib::assert_true(v.is<std::string>());
  test_lib::assert_false(v.is<double>());

  v = 3.14;
  test_lib::assert_false(v.is<int>());
  test_lib::assert_false(v.is<std::string>());
  test_lib::assert_true(v.is<double>());
}

MODERNA_ADD_TEST(variant_as_successful_conversion) {
  generic::variant<int, std::string, double> v{42};
  auto result = v.as<int>();
  test_lib::assert_true(result.has_value());
  test_lib::assert_equal(result->get(), 42);
}

MODERNA_ADD_TEST(variant_as_failed_conversion) {
  generic::variant<int, std::string, double> v{42};
  auto result = v.as<std::string>();
  test_lib::assert_false(result.has_value());
}

MODERNA_ADD_TEST(variant_as_const_version) {
  const generic::variant<int, std::string, double> v{std::string("hello")};
  auto result = v.as<std::string>();
  test_lib::assert_true(result.has_value());
  test_lib::assert_equal(result->get(), std::string("hello"));
}

MODERNA_ADD_TEST(variant_as_reference_modification) {
  generic::variant<int, std::string, double> v{std::string("original")};
  auto result = v.as<std::string>();
  test_lib::assert_true(result.has_value());

  // Modify through reference
  result->get() = "modified";

  // Check that original was modified
  auto check_result = v.as<std::string>();
  test_lib::assert_equal(check_result->get(), std::string("modified"));
}

MODERNA_ADD_TEST(variant_emplace_functionality) {
  generic::variant<int, std::string, double> v{42};
  test_lib::assert_true(v.is<int>());

  v.emplace<std::string>("emplaced");
  test_lib::assert_true(v.is<std::string>());
  test_lib::assert_equal(v.as<std::string>()->get(), std::string("emplaced"));

  v.emplace<double>(3.14);
  test_lib::assert_true(v.is<double>());
  test_lib::assert_equal(v.as<double>()->get(), 3.14);
}

MODERNA_ADD_TEST(variant_visit_single_function) {
  generic::variant<int, std::string, double> v{42};

  int result = v.visit([](auto &value) -> int {
    if constexpr (std::same_as<std::decay_t<decltype(value)>, int>) {
      return value * 2;
    } else if constexpr (std::same_as<std::decay_t<decltype(value)>, std::string>) {
      return static_cast<int>(value.length());
    } else {
      return static_cast<int>(value);
    }
  });

  test_lib::assert_equal(result, 84);
}

MODERNA_ADD_TEST(variant_visit_overload_functions) {
  generic::variant<int, std::string, double> v{std::string("hello")};

  v.visit(
    [](int &i) { /* handle int */ },
    [](std::string &s) { /* handle string */ },
    [](double &d) { /* handle double */ }
  );
}

MODERNA_ADD_TEST(variant_visit_overload_different_types) {
  generic::variant<int, std::string, double> v{3.14};

  v.visit(
    [](int &i) { /* handle int */ },
    [](std::string &s) { /* handle string */ },
    [](double &d) { /* handle double */ }
  );
}

MODERNA_ADD_TEST(variant_visit_const_version) {
  const generic::variant<int, std::string, double> v{3.14159};

  v.visit(
    [](const int &i) { /* handle const int */ },
    [](const std::string &s) { /* handle const string */ },
    [](const double &d) { /* handle const double */ }
  );
}

MODERNA_ADD_TEST(variant_visit_rvalue_version) {

  generic::variant<int, std::string, double>{std::string("moved")}.visit(
    [](int &&i) { /* handle int rvalue */ },
    [](std::string &&s) { /* handle string rvalue */ },
    [](double &&d) { /* handle double rvalue */ }
  );
}