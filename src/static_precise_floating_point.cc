#ifdef MODERNA_GENERIC_MODULES
module;
#endif
#include <concepts>
#include <format>
#if defined(MODERNA_GENERIC_MODULES)
export module moderna.generic:static_precise_floating_point;
import :precise_floating_point;
import :is_number;
#endif

namespace moderna::generic {
  template <std::floating_point num_type, int acc = -1> class static_precise_floating_point {
    precise_floating_point<num_type> __value;

  public:
    template <typename T>
    constexpr static_precise_floating_point(T &&v) : __value{std::forward<T>(v), acc} {}

    // Operator Overloads
    template <typename T>
      requires(std::assignable_from<num_type &, T>)
    constexpr static_precise_floating_point &operator=(T &&v) {
      __value = std::forward<T>(v);
      return *this;
    }
    template <typename T>
      requires(std::assignable_from<T &, num_type>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<num_type> r_num_type, int r_acc>
    friend bool operator==(
      const static_precise_floating_point &l,
      const static_precise_floating_point<r_num_type, r_acc> &r
    ) {
      return l.value() == r.value();
    }
    template <is_number r_num_type>
      requires(std::equality_comparable_with<num_type, r_num_type>)
    friend bool operator==(const static_precise_floating_point &l, const r_num_type &r) {
      return l.value() == r;
    }
    template <std::equality_comparable_with<num_type> r_num_type>
    friend bool operator==(
      const static_precise_floating_point &l, const precise_floating_point<r_num_type> &r
    ) {
      return l.value() == r.value();
    }

    // Getters
    static consteval int precision() noexcept {
      return acc;
    }
    constexpr num_type value() const noexcept {
      return __value.value();
    }

    // Setters
    template <typename T>
      requires(std::assignable_from<num_type &, T>)
    constexpr static_precise_floating_point &set_value(T &&v) {
      __value.set_value(std::forward<T>(v));
      return *this;
    }
    const precise_floating_point<num_type> &to_dynamic() const noexcept {
      return __value;
    }
  };

  // Shortcut definitions
  EXPORT template <int acc = -1>
  using static_precise_float = static_precise_floating_point<float, acc>;
  EXPORT template <int acc = -1>
  using static_precise_double = static_precise_floating_point<double, acc>;
}

namespace generic = moderna::generic;

template <std::floating_point num_type, int acc, class char_type>
struct std::formatter<generic::static_precise_floating_point<num_type, acc>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(
    const generic::static_precise_floating_point<num_type, acc> &v, auto &ctx
  ) const {
    return std::format_to(ctx.out(), "{}", v.to_dynamic());
  }
};