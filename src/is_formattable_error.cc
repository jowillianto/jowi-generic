module;
#include <format>
#include <memory>
#include <string>
export module moderna.generic:is_formattable_error;

namespace moderna::generic {
  export template <class E>
  concept is_formattable_error =
    std::is_constructible_v<std::string, decltype(std::declval<E>().what())> ||
    std::formattable<E, char>;

  export struct error_formatter {
    std::string msg;
    template <is_formattable_error E> error_formatter(const E &e) {
      if constexpr (std::formattable<E, char>) {
        msg = std::format("{}", e);
      } else {
        msg = std::string{e.what()};
      }
    }
  };

  export class generic_error : public std::exception {
    std::unique_ptr<std::exception> __err;

  public:
    generic_error(std::unique_ptr<std::exception> e) : __err{std::move(e)} {}
    template <std::derived_from<std::exception> T>
    generic_error(T err) : __err{std::make_unique<T>(std::move(err))} {}

    const char *what() const noexcept {
      return __err->what();
    }
  };
}
namespace generic = moderna::generic;
template <class char_type> struct std::formatter<generic::error_formatter, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::error_formatter &e, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", e.msg);
  }
};