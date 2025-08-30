module;
#include <cmath>
#include <concepts>
#include <format>
export module moderna.generic:prec_fp;
import :is_number;

namespace moderna::generic {
  /*
    Precise Number limits the amount of decimals
  */
  export template <std::floating_point num_type> class prec_fp {
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
    constexpr prec_fp(T &&v, int precision) {
      __value = std::forward<T>(v);
      set_precision(precision);
    }

    // Operator overloads
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr prec_fp &operator=(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<T &, num_type>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<num_type> r_num_type>
    friend bool operator==(const prec_fp &l, const prec_fp<r_num_type> &r) {
      return l.value() == r.value();
    }
    template <is_number r_num_type> requires(std::equality_comparable_with<num_type, r_num_type>)
    friend bool operator==(const prec_fp &l, const r_num_type &r) {
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
    constexpr prec_fp &set_precision(int new_precision) noexcept {
      __precision = new_precision;
      __multiplier = std::pow(10, __precision);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr prec_fp &set_value(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
  };
  export using precise_float = prec_fp<float>;
  export using precise_double = prec_fp<double>;
}

namespace generic = moderna::generic;

template <std::floating_point num_type, class char_type>
struct std::formatter<generic::prec_fp<num_type>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::prec_fp<num_type> &v, auto &ctx) const {
    auto num = v.value();
    if (v.precision() < 0) {
      return std::format_to(ctx.out(), "{}", num);
    }
    auto fmt = std::format("{{:{}}}", v.precision());
    return std::vformat_to(ctx.out(), fmt, std::make_format_args(num));
  }
};
