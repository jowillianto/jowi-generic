module;
#include <cmath>
#include <concepts>
#include <format>
export module jowi.generic:prec_fp;
import :is_number;

namespace jowi::generic {
  /*
    Precise Number limits the amount of decimals
  */
  export template <std::floating_point NumType> class PrecFp {
    int __precision;
    NumType __multiplier;
    NumType __value;

    constexpr void __round() {
      if (__precision < 0) {
        return;
      }
      __value = std::round(__multiplier * __value) / __multiplier;
    }

  public:
    template <typename T> requires(std::assignable_from<NumType &, T>)
    constexpr PrecFp(T &&v, int precision) {
      __value = std::forward<T>(v);
      set_precision(precision);
    }

    // Operator overloads
    template <typename T> requires(std::assignable_from<NumType &, T>)
    constexpr PrecFp &operator=(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<T &, NumType>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<NumType> RNumType>
    friend bool operator==(const PrecFp &l, const PrecFp<RNumType> &r) {
      return l.value() == r.value();
    }
    template <IsNumber RNumType> requires(std::equality_comparable_with<NumType, RNumType>)
    friend bool operator==(const PrecFp &l, const RNumType &r) {
      return l.value() == r;
    }

    // Getter Function
    constexpr NumType value() const noexcept {
      return __value;
    }
    constexpr int precision() const noexcept {
      return __precision;
    }

    // Setter Function
    constexpr PrecFp &set_precision(int new_precision) noexcept {
      __precision = new_precision;
      __multiplier = std::pow(10, __precision);
      __round();
      return *this;
    }
    template <typename T> requires(std::assignable_from<NumType &, T>)
    constexpr PrecFp &set_value(T &&v) {
      __value = std::forward<T>(v);
      __round();
      return *this;
    }
  };
  export using PreciseFloat = PrecFp<float>;
  export using PreciseDouble = PrecFp<double>;
}
namespace generic = jowi::generic;

template <std::floating_point NumType, class CharType>
struct std::formatter<generic::PrecFp<NumType>, CharType> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::PrecFp<NumType> &v, auto &ctx) const {
    auto num = v.value();
    if (v.precision() < 0) {
      return std::format_to(ctx.out(), "{}", num);
    }
    auto fmt = std::format("{{:{}}}", v.precision());
    return std::vformat_to(ctx.out(), fmt, std::make_format_args(num));
  }
};
