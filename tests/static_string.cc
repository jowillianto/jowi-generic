import jowi.test_lib;
import jowi.generic;
#include <jowi/test_lib.hpp>
#include <string>
#include <string_view>

namespace test_lib = jowi::test_lib;
namespace generic = jowi::generic;

JOWI_ADD_TEST(make_static_string) {
  constexpr auto str = generic::make_static_string("HELLO WORLD");
  static_assert(str.as_view() == "HELLO WORLD");
}

JOWI_ADD_TEST(make_substr) {
  constexpr auto str = generic::make_static_string("HELLO WORLD");
  static_assert(str.substr<0, 3>().as_view() == "HEL");
}

JOWI_ADD_TEST(make_resize_pad_space_behind) {
  constexpr auto str = generic::make_static_string("HELLO");
  static_assert(str.resize<11>(' ').as_view() == "HELLO      ");
}

JOWI_ADD_TEST(make_resize_pad_space_front) {
  constexpr auto str = generic::make_static_string("HELLO");
  static_assert(str.resize<11>(' ', generic::padding_type::front).as_view() == "      HELLO");
}

JOWI_ADD_TEST(make_static_string_str_cut) {
  constexpr auto str = generic::make_static_string<5>(std::string{"HELLO WORLD"});
  static_assert(str.as_view() == "HELLO");
}

JOWI_ADD_TEST(make_string_view) {
  static constexpr auto str = generic::make_static_string("HELLO WORLD");
  constexpr auto str_view = str.borrow();
  static_assert(str[0].value().get() == 'H');
}