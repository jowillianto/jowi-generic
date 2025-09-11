module;
#include <concepts>
#include <functional>
#include <type_traits>
#include <vector>
export module jowi.generic:dependency_injection;

namespace jowi::generic {
  export template <class T, class F, class R, class... Args>
  concept has_method = requires(T v, F f, Args... args) {
    { std::invoke(f, v, args...) } -> std::same_as<R>;
  };
}