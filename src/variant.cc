module;
#include <concepts>
#include <functional>
#include <optional>
#include <variant>
export module jowi.generic:variant;

namespace jowi::generic {
  template <class TestValue, class... TestTargets>
  concept IsInTarget = (std::same_as<TestValue, TestTargets> || ...);

  template <class... Overloads> struct VariantVisitorOverload : Overloads... {
    using Overloads::operator()...;
  };

  /*
    Variant but with more member functions. This is so that the usage of the variant itself
    becomes more convenient for any API user. No additional features has been added except the fact
    that the variant is now exception safe.
  */
  export template <typename... Variants> class Variant {
  private:
    using VariantType = std::variant<Variants...>;
    VariantType __value;

  public:
    template <typename... Args> requires(std::constructible_from<VariantType, Args...>)
    constexpr Variant(Args &&...args) : __value{std::forward<Args>(args)...} {}

    template <class T> requires(std::assignable_from<VariantType &, T>)
    constexpr Variant &operator=(T &&t) {
      __value = std::forward<T>(t);
      return *this;
    }
    template <IsInTarget<Variants...> T, class... Args>
    constexpr Variant &emplace(Args &&...args) noexcept {
      __value.template emplace<T>(std::forward<Args>(args)...);
      return *this;
    }

    constexpr int index() const noexcept {
      return __value.index();
    }

    /**
      variant checking functions.
    */
    template <IsInTarget<Variants...> TestType> constexpr bool is() const noexcept {
      return std::holds_alternative<TestType>(__value);
    }

    /**
      casting functions
    */
    template <IsInTarget<Variants...> TestType>
    constexpr std::optional<std::reference_wrapper<TestType>> as() noexcept {
      using OptionalType = std::optional<std::reference_wrapper<TestType>>;
      return std::visit(
        [](auto &v) -> OptionalType {
          using CurrentType = std::decay_t<decltype(v)>;
          if constexpr (std::same_as<CurrentType, TestType>) {
            return OptionalType{std::ref(v)};
          } else {
            return OptionalType{};
          }
        },
        __value
      );
    }

    template <IsInTarget<Variants...> TestType>
    constexpr std::optional<std::reference_wrapper<const TestType>> as() const noexcept {
      using OptionalType = std::optional<std::reference_wrapper<const TestType>>;
      return std::visit(
        [](auto &v) -> OptionalType {
          using CurrentType = std::decay_t<decltype(v)>;
          if constexpr (std::same_as<CurrentType, TestType>) {
            return OptionalType{std::cref(v)};
          } else {
            return OptionalType{};
          }
        },
        __value
      );
    }

    /**
      visitor functions
    */
    template <class... Functions>
    requires(std::invocable<VariantVisitorOverload<Functions...>, Variants &> && ...)
    constexpr auto visit(Functions &&...f) & {
      return std::visit(VariantVisitorOverload<Functions...>{std::forward<Functions>(f)...}, __value);
    }
    template <class... Functions>
    requires(std::invocable<VariantVisitorOverload<Functions...>, const Variants &> && ...)
    constexpr auto visit(Functions &&...f) const & {
      return std::visit(VariantVisitorOverload<Functions...>{std::forward<Functions>(f)...}, __value);
    }
    template <class... Functions>
    requires(std::invocable<VariantVisitorOverload<Functions...>, Variants &&> && ...)
    constexpr auto visit(Functions &&...f) && {
      return std::visit(VariantVisitorOverload<Functions...>{std::forward<Functions>(f)...}, std::move(__value));
    }
  };
}
