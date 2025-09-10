import jowi.test_lib;
import jowi.generic;
#include <jowi/test_lib.hpp>
#include <cmath>

namespace test_lib = jowi::test_lib;
namespace generic = jowi::generic;

JOWI_ADD_TEST(fixed_number_fixed_precision) {
  constexpr int precision = 4;
  double num = 1.2434739;
  auto precise_num = generic::static_precise_double<precision>{num};
  test_lib::assert_equal(precise_num, 1.2435);
}

JOWI_ADD_TEST(no_precision) {
  constexpr int precision = -1;
  double rand = test_lib::random_real(0.0, 1000.0);
  auto precise_rand = generic::static_precise_double<precision>{rand};
  test_lib::assert_equal(precise_rand, rand);
}

JOWI_ADD_TEST(fixed_precision) {
  constexpr int precision = 4;
  double rand = test_lib::random_real(0.0, 1000.0);
  auto precise_rand = generic::static_precise_double<precision>{rand};
  test_lib::assert_equal(precise_rand, std::round(rand * 1e4) / 1e4);
}

JOWI_ADD_TEST(reset_value) {
  constexpr int precision = 4;
  double num = 0;
  auto precise_num = generic::static_precise_double<precision>{num};
  double new_num = 1.345434;
  test_lib::assert_equal(precise_num.set_value(new_num), 1.3454);
}

JOWI_ADD_TEST(compare_different) {
  double num_1 = 1.233234242;
  double num_2 = 1.1212123123123;
  auto precise_num_1 = generic::static_precise_double<0>{num_1};
  auto precise_num_2 = generic::static_precise_double<0>{num_2};
  test_lib::assert_equal(precise_num_1, precise_num_2);
}

JOWI_ADD_TEST(compare_static_and_dynamic) {
  auto precise_num_1 = generic::static_precise_double<0>{1.232859350341289502374};
  auto precise_num_2 = generic::precise_double{1.2342342480830849349374395, 0};
  test_lib::assert_equal(precise_num_1, precise_num_2);
}