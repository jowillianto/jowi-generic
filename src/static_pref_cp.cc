module;
#include <concepts>
#include <format>
export module moderna.generic:static_prec_fp;
import :prec_fp;
import :is_number;

namespace moderna::generic {
  template <std::floating_point num_type, int acc = -1> class static_prec_fp {
    prec_fp<num_type> __value;

  public:
    template <typename T> constexpr static_prec_fp(T &&v) : __value{std::forward<T>(v), acc} {}

    // Operator Overloads
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr static_prec_fp &operator=(T &&v) {
      __value = std::forward<T>(v);
      return *this;
    }
    template <typename T> requires(std::assignable_from<T &, num_type>)
    constexpr operator T() const {
      return __value;
    }
    template <std::equality_comparable_with<num_type> r_num_type, int r_acc>
    friend bool operator==(const static_prec_fp &l, const static_prec_fp<r_num_type, r_acc> &r) {
      return l.value() == r.value();
    }
    template <is_number r_num_type> requires(std::equality_comparable_with<num_type, r_num_type>)
    friend bool operator==(const static_prec_fp &l, const r_num_type &r) {
      return l.value() == r;
    }
    template <std::equality_comparable_with<num_type> r_num_type>
    friend bool operator==(const static_prec_fp &l, const prec_fp<r_num_type> &r) {
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
    template <typename T> requires(std::assignable_from<num_type &, T>)
    constexpr static_prec_fp &set_value(T &&v) {
      __value.set_value(std::forward<T>(v));
      return *this;
    }
    const prec_fp<num_type> &as_dyn() const noexcept {
      return __value;
    }
  };

  // Shortcut definitions
  export template <int acc = -1> using static_precise_float = static_prec_fp<float, acc>;
  export template <int acc = -1> using static_precise_double = static_prec_fp<double, acc>;
}

namespace generic = moderna::generic;

template <std::floating_point num_type, int acc, class char_type>
struct std::formatter<generic::static_prec_fp<num_type, acc>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::static_prec_fp<num_type, acc> &v, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", v.as_dyn());
  }
};