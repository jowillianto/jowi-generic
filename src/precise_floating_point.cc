module;
#include <cmath>
#include <concepts>
#include <format>
export module moderna.generic:precise_floating_point;
import :is_number;

namespace moderna::generic {
  /*
    Precise Number limits the amount of decimals
  */
  export template <std::floating_point num_type> class precise_floating_point {
    int __precision;
    num_type __multiplier;
    num_type __value;

    constexpr void __round() {
      if (__precision < 0) {
        return;
      }
      __value = std::round(__multiplier * __value) / __multiplier;
    }

  public:
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr precise_floating_point(T &&v, int precision) {
      __value = std::forward<T>(v);
      set_precision(precision);
    }

    // Operator overloads
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr precise_floating_point &operator=(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<T &, num_type>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<num_type> r_num_type>
    friend bool operator==(
      const precise_floating_point &l, const precise_floating_point<r_num_type> &r
    ) {
      return l.value() == r.value();
    }
    template <is_number r_num_type> requires(std::equality_comparable_with<num_type, r_num_type>)
    friend bool operator==(const precise_floating_point &l, const r_num_type &r) {
      return l.value() == r;
    }

    // Getter Function
    constexpr num_type value() const noexcept {
      return __value;
    }
    constexpr int precision() const noexcept {
      return __precision;
    }

    // Setter Function
    constexpr precise_floating_point &set_precision(int new_precision) noexcept {
      __precision = new_precision;
      __multiplier = std::pow(10, __precision);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr precise_floating_point &set_value(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
  };
  export using precise_float = precise_floating_point<float>;
  export using precise_double = precise_floating_point<double>;
}

namespace generic = moderna::generic;

template <std::floating_point num_type, class char_type>
struct std::formatter<generic::precise_floating_point<num_type>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::precise_floating_point<num_type> &v, auto &ctx) const {
    auto num = v.value();
    if (v.precision() < 0) {
      return std::format_to(ctx.out(), "{}", num);
    }
    auto fmt = std::format("{{:{}}}", v.precision());
    return std::vformat_to(ctx.out(), fmt, std::make_format_args(num));
  }
};
