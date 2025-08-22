#ifdef MODERNA_GENERIC_MODULES
module;
#endif
#include <concepts>
#include <functional>
#include <optional>
#include <variant>
#ifdef MODERNA_GENERIC_MODULES
export module moderna.generic:variant;
#endif

namespace moderna::generic {
  template <class test_value, class... test_targets>
  concept is_in_target = (std::same_as<test_value, test_targets> || ...);

  /*
    Variant but with more member functions. This is so that the usage of the variant itself
    becomes more convenient for any API user. No additional features has been added except the fact
    that the variant is now exception safe.
  */
  EXPORT template <typename... variants> struct variant {
    using variant_type = std::variant<variants...>;
    variant_type value;

    template <typename... Args>
      requires(std::is_constructible_v<variant_type, Args...>)
    constexpr variant(Args &&...args) : value{std::forward<Args>(args)...} {}

    template <class T>
      requires(std::is_assignable_v<variant_type &, T>)
    constexpr variant &operator=(T &&t) {
      value = std::forward<T>(t);
      return *this;
    }
    template <is_in_target<variants...> T, class... Args>
    constexpr variant &emplace(Args &&...args) noexcept {
      value.template emplace<T>(std::forward<Args>(args)...);
      return *this;
    }
    constexpr int index() const noexcept {
      return value.index();
    }
    template <is_in_target<variants...> test_type> constexpr bool is() const noexcept {
      return std::holds_alternative<test_type>(value);
    }
    template <is_in_target<variants...> test_type>
    constexpr std::optional<std::reference_wrapper<test_type>> as() noexcept {
      using optional_type = std::optional<std::reference_wrapper<test_type>>;
      return std::visit(
        [](auto &v) -> optional_type {
          using current_type = std::decay_t<decltype(v)>;
          if constexpr (std::same_as<current_type, test_type>) {
            return optional_type{std::ref(v)};
          } else {
            return optional_type{};
          }
        },
        value
      );
    }
    template <is_in_target<variants...> test_type>
    constexpr std::optional<std::reference_wrapper<const test_type>> as() const noexcept {
      using optional_type = std::optional<std::reference_wrapper<const test_type>>;
      return std::visit(
        [](auto &v) -> optional_type {
          using current_type = std::decay_t<decltype(v)>;
          if constexpr (std::same_as<current_type, test_type>) {
            return optional_type{std::cref(v)};
          } else {
            return optional_type{};
          }
        },
        value
      );
    }
    template <class F>
      requires(std::invocable<F, variants &> && ...)
    constexpr auto visit(F &&f) {
      return std::visit(std::forward<F>(f), value);
    }
    template <class F>
      requires(std::invocable<F, const variants &> && ...)
    constexpr auto visit(F &&f) const {
      return std::visit(std::forward<F>(f), value);
    }
    template <class F>
      requires(std::invocable<F, variants &&> && ...)
    constexpr auto visit(F &&f) {
      return std::visit(std::forward<F>(f), std::move(value));
    }
  };
}