module;
#include <atomic>
#include <concepts>
#include <mutex>
#include <thread>
export module jowi.generic:thread_guard;

namespace jowi::generic {
  export class bool_mutex {
    std::atomic_flag __flag;

  public:
    bool_mutex() : __flag{false} {}

    void lock() noexcept {

      // this loop will break if and only if test_and_set returns false, meaning, the previous
      // value of the flag have to be false, i.e. it has to be unlocked.
      for (bool is_locked = __flag.test_and_set(); is_locked; is_locked = __flag.test_and_set()) {
        __flag.wait(true);
      }
    }

    void unlock() noexcept {
      __flag.clear();
      __flag.notify_one();
    }

    bool try_lock() noexcept {
      bool is_previously_locked = __flag.test_and_set();
      // return true if prev_value is false, meaning the lock is not locked.
      return !is_previously_locked;
    }
  };
  export template <class T, class mutex_type = bool_mutex> class thread_guard {
    T __value;
    mutable mutex_type __mut;

  public:
    thread_guard(T value) : __value{std::move(value)} {}
    thread_guard()
      requires(std::is_default_constructible_v<T>)
    {}

    template <std::invocable<const T &> F>
    std::invoke_result_t<F, const T &> read(F &&f) const noexcept {
      std::unique_lock l{__mut};
      return std::invoke(std::forward<F>(f), static_cast<const T &>(__value));
    }

    template <std::invocable<T &> F> std::invoke_result_t<F, T &> read_mut(F &&f) noexcept {
      std::unique_lock l{__mut};
      return std::invoke(std::forward<F>(f), static_cast<T &>(__value));
    }

    T &unsafe_value() {
      return __value;
    }
    const T &unsafe_value() const {
      return __value;
    }
  };
}
