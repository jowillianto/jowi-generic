module;
#include <format>
#include <string>
export module jowi.generic:is_formattable_error;

namespace jowi::generic {
  // use DI instead
  export template <class E>
  concept is_formattable_error = std::formattable<E, char> ||
    requires(const E &e) { std::formattable<decltype(e.what()), char>; };

  export struct error_formatter {
    std::string msg;
    template <is_formattable_error E> constexpr error_formatter(const E &e) {
      if constexpr (std::formattable<E, char>) {
        msg = std::format("{}", e);
      } else {
        msg = std::format("{}", e.what());
      }
    }
  };
}
namespace generic = jowi::generic;
template <class char_type> struct std::formatter<generic::error_formatter, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::error_formatter &e, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", e.msg);
  }
};