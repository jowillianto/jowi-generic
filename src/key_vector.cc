module;
#include <algorithm>
#include <functional>
#include <optional>
#include <ranges>
#include <vector>
export module moderna.generic:key_vector;

namespace moderna::generic {
  export template <class value_type, class other_type>
  concept is_comparable = requires(std::decay_t<value_type> l, std::decay_t<other_type> r) {
    { l == r } -> std::same_as<bool>;
  };
  export template <class key_type, class value_type> class key_vector {
    using entry_type = std::pair<key_type, value_type>;
    using container_type = std::vector<entry_type>;
    container_type __values;

  public:
    constexpr key_vector() : __values{} {}
    constexpr key_vector(container_type container) : __values{container} {}
    constexpr key_vector(std::initializer_list<std::pair<key_type, value_type>> list) {
      __values.reserve(list.size());
      for (const auto &[key, value] : list) {
        insert(std::move(key), std::move(value));
      }
    }

    template <is_comparable<key_type> search_key_type>
    constexpr std::optional<std::reference_wrapper<const value_type>> get(
      search_key_type &&k
    ) const noexcept {
      auto it = std::ranges::find(__values, std::forward<search_key_type>(k), &entry_type::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        return std::cref(it->second);
      }
    }

    template <is_comparable<key_type> search_key_type>
    constexpr std::optional<std::reference_wrapper<value_type>> get(search_key_type &&k) noexcept {
      auto it = std::ranges::find(__values, std::forward<search_key_type>(k), &entry_type::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        return std::ref(it->second);
      }
    }

    constexpr value_type &insert(const key_type &key, value_type value) {
      auto cur_entry = get(key);
      if (cur_entry) {
        cur_entry.value().get() = std::move(value);
        return cur_entry.value().get();
      } else {
        return __values.emplace_back(entry_type{key, std::move(value)}).second;
      }
    }
    template <is_comparable<key_type> search_key_type>
    constexpr std::optional<value_type> remove(search_key_type &&key) {
      auto it = std::ranges::find(__values, std::forward<search_key_type>(key), &entry_type::first);
      if (it == __values.end()) {
        return std::nullopt;
      } else {
        value_type &value = std::move(it->value);
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
      return std::ranges::transform_view{__values, entry_type::first};
    }
  };
}