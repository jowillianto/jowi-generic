module;
#include <concepts>
#include <format>
export module jowi.generic:static_prec_fp;
import :prec_fp;
import :is_number;

namespace jowi::generic {
  template <std::floating_point NumType, int Acc = -1> class StaticPrecFp {
    PrecFp<NumType> __value;

  public:
    template <typename T> constexpr StaticPrecFp(T &&v) : __value{std::forward<T>(v), Acc} {}

    // Operator Overloads
    template <typename T> requires(std::assignable_from<NumType &, T>)
    constexpr StaticPrecFp &operator=(T &&v) {
      __value = std::forward<T>(v);
      return *this;
    }
    template <typename T> requires(std::assignable_from<T &, NumType>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<NumType> RNumType, int RAcc>
    friend bool operator==(const StaticPrecFp &l, const StaticPrecFp<RNumType, RAcc> &r) {
      return l.value() == r.value();
    }
    template <IsNumber RNumType> requires(std::equality_comparable_with<NumType, RNumType>)
    friend bool operator==(const StaticPrecFp &l, const RNumType &r) {
      return l.value() == r;
    }
    template <std::equality_comparable_with<NumType> RNumType>
    friend bool operator==(const StaticPrecFp &l, const PrecFp<RNumType> &r) {
      return l.value() == r.value();
    }

    // Getters
    static consteval int precision() noexcept {
      return Acc;
    }
    constexpr NumType value() const noexcept {
      return __value.value();
    }

    // Setters
    template <typename T> requires(std::assignable_from<NumType &, T>)
    constexpr StaticPrecFp &set_value(T &&v) {
      __value.set_value(std::forward<T>(v));
      return *this;
    }
    const PrecFp<NumType> &as_dyn() const noexcept {
      return __value;
    }
  };

  // Shortcut definitions
  export template <int Acc = -1> using StaticPreciseFloat = StaticPrecFp<float, Acc>;
  export template <int Acc = -1> using StaticPreciseDouble = StaticPrecFp<double, Acc>;
}

namespace generic = jowi::generic;
template <std::floating_point NumType, int Acc, class CharType>
struct std::formatter<generic::StaticPrecFp<NumType, Acc>, CharType> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::StaticPrecFp<NumType, Acc> &v, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", v.as_dyn());
  }
};
