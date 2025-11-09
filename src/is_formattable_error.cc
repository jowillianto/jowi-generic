module;
#include <format>
#include <string>
export module jowi.generic:is_formattable_error;

namespace jowi::generic {
  // use DI instead
  export template <class E>
  concept IsFormattableError = std::formattable<E, char> ||
    requires(const E &e) { std::formattable<decltype(e.what()), char>; };

  export struct ErrorFormatter {
    std::string msg;
    template <IsFormattableError E> constexpr ErrorFormatter(const E &e) {
      if constexpr (std::formattable<E, char>) {
        msg = std::format("{}", e);
      } else {
        msg = std::format("{}", e.what());
      }
    }
  };
}
namespace generic = jowi::generic;
template <class CharType> struct std::formatter<generic::ErrorFormatter, CharType> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::ErrorFormatter &e, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", e.msg);
  }
};
