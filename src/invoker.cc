#ifdef MODERNA_GENERIC_MODULES
module;
#endif
#include <concepts>
#include <tuple>
#if defined(MODERNA_GENERIC_MODULES)
export module moderna.generic:invoker;
#endif
namespace moderna::generic {
  EXPORT template <class... Args> struct invoker {
    std::tuple<Args...> args;
    invoker(Args... args) : args{std::forward<Args>(args)...} {}

    template <std::invocable<Args...> F> std::invoke_result_t<F, Args...> operator()(F &&f) {
      return std::apply(
        [&](auto &&...args) {
          return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        },
        std::move(args)
      );
    }
  };
};