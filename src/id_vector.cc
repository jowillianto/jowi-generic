module;
#include <atomic>
#include <functional>
#include <algorithm>
#include <ranges>
#include <optional>
#include <type_traits>
#include <vector>
export module jowi.generic:id_vector;
import :thread_guard;

namespace jowi::generic {
  template <class T> struct IdVectorEntry {
    const size_t id;
    std::shared_ptr<T> value;
  };
  template <class T, class MutexType = BoolMutex> class IdVector {
    using ContainerType = std::vector<IdVectorEntry<T>>;
    using ValueType = IdVectorEntry<T>;
    std::atomic<size_t> __ctr;
    ThreadGuard<ContainerType, MutexType> __container;

    size_t __incr_ctr() {
      return __ctr.fetch_add(1, std::memory_order_relaxed);
    }

  public:
    template <class... Args>
      requires(std::is_constructible_v<T, Args> && ...)
    IdVector(Args &&...args) : __ctr{sizeof...(Args)} {
      size_t id = 0;
      (__container.unsafe_value().emplace_back(id++, std::make_shared<T>(std::forward<Args>(args))), ...);
    }

    template <class... Args>
      requires(std::is_constructible_v<T, Args...>)
    ValueType emplace_back(Args &&...args) {
      return __container.read_mut([&](ContainerType &container) {
        size_t cur_id = __incr_ctr();
        return container.emplace_back(cur_id, std::make_shared<T>(std::forward<Args>(args)...));
      });
    }

    template <class... Args>
      requires(std::is_constructible_v<T, Args...>)
    ValueType push_back(Args &&...args) {
      return emplace_back(std::forward<Args>(args)...);
    }

    std::optional<IdVectorEntry<const T>> operator[](size_t id) const noexcept {
      using ReturnType = std::optional<std::reference_wrapper<const ValueType>>;
      return __container.read([&](const ContainerType &container) {
        auto it = std::ranges::find(container, id, &ValueType::id);
        if (it == container.end()) {
          return ReturnType{std::nullopt};
        } else {
          return ReturnType{*it};
        }
      });
    }

    std::optional<IdVectorEntry<T>> operator[](size_t id) noexcept {
      using ReturnType = std::optional<std::reference_wrapper<const ValueType>>;
      return __container.read([&](ContainerType &container) {
        auto it = std::ranges::find(container, id, &ValueType::id);
        if (it == container.end()) {
          return ReturnType{std::nullopt};
        } else {
          return ReturnType{*it};
        }
      });
    }

    std::optional<ValueType> erase(size_t id) {
      using ReturnType = std::optional<ValueType>; 
      return __container.read([&](ContainerType &container) {
        auto it = std::ranges::find(container, id, &ValueType::id);
        if (it == container.end()) {
          return ReturnType{std::nullopt};
        }
        else {
          auto e = std::move(*it);
          container.erase(it);
          return ReturnType{std::move(e)};
        }
      });
    }
  };

  IdVector<int> vec;
}
