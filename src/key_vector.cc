module;
#include <algorithm>
#include <functional>
#include <optional>
#include <ranges>
#include <vector>
export module jowi.generic:key_vector;

namespace jowi::generic {
  export template <class ValueType, class OtherType>
  concept IsComparable = requires(std::decay_t<ValueType> l, std::decay_t<OtherType> r) {
    { l == r } -> std::same_as<bool>;
  };
  export template <class KeyType, class ValueType> class KeyVector {
    using EntryType = std::pair<KeyType, ValueType>;
    using ContainerType = std::vector<EntryType>;
    ContainerType __values;

  public:
    constexpr KeyVector() : __values{} {}
    constexpr KeyVector(ContainerType container) : __values{std::move(container)} {}
    constexpr KeyVector(std::initializer_list<std::pair<KeyType, ValueType>> list) {
      __values.reserve(list.size());
      for (auto &&[key, value] : list) {
        insert(std::move(key), std::move(value));
      }
    }

    /*
      element getters.
    */
    template <IsComparable<KeyType> Key>
    constexpr std::optional<std::reference_wrapper<const ValueType>> get(Key &&k) const noexcept {
      auto it = std::ranges::find(__values, std::forward<Key>(k), &EntryType::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        return std::cref(it->second);
      }
    }
    template <IsComparable<KeyType> Key>
    constexpr std::optional<std::reference_wrapper<ValueType>> get(Key &&k) noexcept {
      auto it = std::ranges::find(__values, std::forward<Key>(k), &EntryType::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        return std::ref(it->second);
      }
    }
    template <IsComparable<KeyType> Key>
    constexpr std::optional<std::reference_wrapper<const ValueType>> operator[](
      Key &&key
    ) const noexcept {
      return get(std::forward<Key>(key));
    }
    template <IsComparable<KeyType> Key>
    constexpr std::optional<std::reference_wrapper<ValueType>> operator[](Key &&key) noexcept {
      return get(std::forward<Key>(key));
    }

    /*
      element inserts
    */
    template <IsComparable<KeyType> Key, class... Args>
    requires(std::is_constructible_v<KeyType, Key> && std::is_constructible_v<ValueType, Args...>)
    constexpr ValueType &emplace(Key &&key, Args &&...args) {
      auto it = std::ranges::find(__values, key, &EntryType::first);
      if (it != __values.end()) {
        it->second = ValueType{std::forward<Args>(args)...};
        return it->second;
      } else {
        return __values
          .emplace_back(KeyType{std::forward<Key>(key)}, ValueType{std::forward<Args>(args)...})
          .second;
      }
    }
    constexpr ValueType &insert(const KeyType &key, ValueType value) {
      return emplace(key, std::move(value));
    }
    template <IsComparable<KeyType> Key> constexpr std::optional<ValueType> remove(Key &&key) {
      auto it = std::ranges::find(__values, std::forward<Key>(key), &EntryType::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        ValueType value = std::move(it->second);
        __values.erase(it);
        return std::optional{std::move(value)};
      }
    }

    constexpr size_t size() const noexcept {
      return __values.size();
    }

    constexpr auto begin() const noexcept {
      return __values.begin();
    }
    constexpr auto end() const noexcept {
      return __values.end();
    }
    constexpr auto begin() noexcept {
      return __values.begin();
    }
    constexpr auto end() noexcept {
      return __values.end();
    }
    constexpr bool empty() const noexcept {
      return __values.empty();
    }

    constexpr auto keys() const noexcept {
      return std::ranges::transform_view{__values, &EntryType::first};
    }
  };
}
