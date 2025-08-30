import moderna.test_lib;
import moderna.generic;
#include <moderna/test_lib.hpp>
#include <cmath>

namespace test_lib = moderna::test_lib;
namespace generic = moderna::generic;

MODERNA_ADD_TEST(fixed_number_fixed_precision) {
  int precision = 4;
  double num = 1.2434739;
  auto precise_num = generic::precise_double{num, precision};
  test_lib::assert_equal(precise_num, 1.2435);
}

MODERNA_ADD_TEST(no_precision) {
  int precision = -1;
  double rand = test_lib::random_real(0.0, 1000.0);
  auto precise_rand = generic::precise_double{rand, precision};
  test_lib::assert_equal(precise_rand, rand);
}

MODERNA_ADD_TEST(fixed_precision) {
  int precision = 4;
  double rand = test_lib::random_real(0.0, 1000.0);
  auto precise_rand = generic::precise_double{rand, precision};
  test_lib::assert_equal(precise_rand, std::round(rand * 1e4) / 1e4);
}

MODERNA_ADD_TEST(reset_precision) {
  int precision = 4;
  double num = 1.354350820;
  auto precise_num = generic::precise_double{num, precision};
  test_lib::assert_equal(precise_num.set_precision(0), 1);
}

MODERNA_ADD_TEST(reset_value) {
  int precision = 4;
  double num = 0;
  auto precise_num = generic::precise_double{num, precision};
  double new_num = 1.345434;
  test_lib::assert_equal(precise_num.set_value(new_num), 1.3454);
}

MODERNA_ADD_TEST(compare_different) {
  int precision = 0;
  double num_1 = 1.233234242;
  double num_2 = 1.1212123123123;
  auto precise_num_1 = generic::precise_double{num_1, precision};
  auto precise_num_2 = generic::precise_double{num_2, precision};
  test_lib::assert_equal(precise_num_1, precise_num_2);
}