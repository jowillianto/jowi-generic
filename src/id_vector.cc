#ifdef MODERNA_GENERIC_MODULES
module;
#endif
#include <algorithm>
#include <atomic>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>
#if defined(MODERNA_GENERIC_MODULES)
export module moderna.generic:id_vector;
import :thread_guard;
#endif

namespace moderna::generic {
  template <class T> struct id_vector_entry {
    const size_t id;
    std::shared_ptr<T> value;
  };
  template <class T, class mutex_type = bool_mutex> class id_vector {
    using container_type = std::vector<id_vector_entry<T>>;
    using value_type = id_vector_entry<T>;
    std::atomic<size_t> __ctr;
    thread_guard<container_type, mutex_type> __container;

    size_t __incr_ctr() {
      return __ctr.fetch_add(1, std::memory_order_relaxed);
    }

  public:
    template <class... Args>
      requires(std::is_constructible_v<T, Args> && ...)
    id_vector(Args &&...args) : __ctr{sizeof...(Args)} {
      size_t id = 0;
      (__container.unsafe_value().emplace_back(id++, std::make_shared<T>(std::forward<Args>(args))),
       ...);
    }

    template <class... Args>
      requires(std::is_constructible_v<T, Args...>)
    value_type emplace_back(Args &&...args) {
      return __container.read_mut([&](container_type &container) {
        size_t cur_id = __incr_ctr();
        return container.emplace_back(cur_id, std::make_shared<T>(std::forward<Args>(args)...));
      });
    }

    template <class... Args>
      requires(std::is_constructible_v<T, Args...>)
    value_type push_back(Args &&...args) {
      return emplace_back(std::forward<Args>(args)...);
    }

    std::optional<id_vector_entry<const T>> operator[](size_t id) const noexcept {
      using return_type = std::optional<std::reference_wrapper<const value_type>>;
      return __container.read([&](const container_type &container) {
        auto it = std::ranges::find(container, id, &value_type::id);
        if (it == container.end()) {
          return return_type{std::nullopt};
        } else {
          return return_type{*it};
        }
      });
    }

    std::optional<id_vector_entry<T>> operator[](size_t id) noexcept {
      using return_type = std::optional<std::reference_wrapper<const value_type>>;
      return __container.read([&](container_type &container) {
        auto it = std::ranges::find(container, id, &value_type::id);
        if (it == container.end()) {
          return return_type{std::nullopt};
        } else {
          return return_type{*it};
        }
      });
    }

    std::optional<value_type> erase(size_t id) {
      using return_type = std::optional<value_type>;
      return __container.read([&](container_type &container) {
        auto it = std::ranges::find(container, id, &value_type::id);
        if (it == container.end()) {
          return return_type{std::nullopt};
        } else {
          auto e = std::move(*it);
          container.erase(it);
          return return_type{std::move(e)};
        }
      });
    }
  };

  id_vector<int> vec;
}