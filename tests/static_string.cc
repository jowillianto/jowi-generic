import moderna.test_lib;
import moderna.generic;
#include <moderna/test_lib.hpp>
#include <string>
#include <string_view>

namespace test_lib = moderna::test_lib;
namespace generic = moderna::generic;

MODERNA_ADD_TEST(make_static_string) {
  constexpr auto str = generic::make_static_string("HELLO WORLD");
  static_assert(str.as_view() == "HELLO WORLD");
}

MODERNA_ADD_TEST(make_substr) {
  constexpr auto str = generic::make_static_string("HELLO WORLD");
  static_assert(str.substr<0, 3>().as_view() == "HEL");
}

MODERNA_ADD_TEST(make_resize_pad_space_behind) {
  constexpr auto str = generic::make_static_string("HELLO");
  static_assert(str.resize<11>(' ').as_view() == "HELLO      ");
}

MODERNA_ADD_TEST(make_resize_pad_space_front) {
  constexpr auto str = generic::make_static_string("HELLO");
  static_assert(str.resize<11>(' ', generic::padding_type::front).as_view() == "      HELLO");
}

MODERNA_ADD_TEST(make_static_string_str_cut) {
  constexpr auto str = generic::make_static_string<5>(std::string{"HELLO WORLD"});
  static_assert(str.as_view() == "HELLO");
}

MODERNA_ADD_TEST(make_string_view) {
  static constexpr auto str = generic::make_static_string("HELLO WORLD");
  constexpr auto str_view = str.borrow();
  static_assert(str[0].value().get() == 'H');
}