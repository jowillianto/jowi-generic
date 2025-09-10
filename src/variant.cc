module;
#include <concepts>
#include <functional>
#include <optional>
#include <variant>
export module jowi.generic:variant;

namespace jowi::generic {
  template <class test___value, class... test_targets>
  concept is_in_target = (std::same_as<test___value, test_targets> || ...);

  template <class... overloads> struct variant_visitor_overload : overloads... {
    using overloads::operator()...;
  };

  /*
    Variant but with more member functions. This is so that the usage of the variant itself
    becomes more convenient for any API user. No additional features has been added except the fact
    that the variant is now exception safe.
  */
  export template <typename... variants> class variant {
  private:
    using variant_type = std::variant<variants...>;
    variant_type __value;

  public:
    template <typename... Args> requires(std::constructible_from<variant_type, Args...>)
    constexpr variant(Args &&...args) : __value{std::forward<Args>(args)...} {}

    template <class T> requires(std::assignable_from<variant_type &, T>)
    constexpr variant &operator=(T &&t) {
      __value = std::forward<T>(t);
      return *this;
    }
    template <is_in_target<variants...> T, class... Args>
    constexpr variant &emplace(Args &&...args) noexcept {
      __value.template emplace<T>(std::forward<Args>(args)...);
      return *this;
    }

    constexpr int index() const noexcept {
      return __value.index();
    }

    /**
      variant checking functions.
    */
    template <is_in_target<variants...> test_type> constexpr bool is() const noexcept {
      return std::holds_alternative<test_type>(__value);
    }

    /**
      casting functions
    */
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
        __value
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
        __value
      );
    }

    /**
      visitor functions
    */
    template <class... F>
    requires(std::invocable<variant_visitor_overload<F...>, variants &> && ...)
    constexpr auto visit(F &&...f) & {
      return std::visit(variant_visitor_overload<F...>{std::forward<F>(f)...}, __value);
    }
    template <class... F>
    requires(std::invocable<variant_visitor_overload<F...>, const variants &> && ...)
    constexpr auto visit(F &&...f) const & {
      return std::visit(variant_visitor_overload<F...>{std::forward<F>(f)...}, __value);
    }
    template <class... F>
    requires(std::invocable<variant_visitor_overload<F...>, variants &&> && ...)
    constexpr auto visit(F &&...f) && {
      return std::visit(variant_visitor_overload<F...>{std::forward<F>(f)...}, std::move(__value));
    }
  };
}