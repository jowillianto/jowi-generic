module;
#include <algorithm>
#include <array>
#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
export module moderna.generic:static_string;

namespace moderna::generic {
  export enum struct padding_type { front, back };
  template <size_t N, bool is_mutable> class static_string_view {
    using pointer_type = std::conditional_t<is_mutable, char *, const char *>;
    pointer_type __beg;
    pointer_type __end;

  public:
    constexpr static_string_view(pointer_type beg, pointer_type end) : __beg{beg}, __end{end} {}

    // Getters
    constexpr std::optional<std::reference_wrapper<const char>> operator[](size_t id
    ) const noexcept {
      if (id < N) {
        return std::cref(__beg + id);
      } else {
        return std::nullopt;
      }
    }
    constexpr std::optional<std::reference_wrapper<char>> operator[](size_t id) noexcept {
      if (id < N) {
        return std::ref(__beg + id);
      } else {
        return std::nullopt;
      }
    }

    // Iterators
    constexpr pointer_type begin() const noexcept {
      return __beg;
    }
    constexpr pointer_type end() const noexcept {
      return __end;
    }
    constexpr char *begin() noexcept
      requires(is_mutable)
    {
      return __beg;
    }
    constexpr char *end() noexcept
      requires(is_mutable)
    {
      return __end;
    }

    // String Operations
    template <size_t begin_pos, size_t length>
    constexpr static_string_view<length, is_mutable> substr() noexcept
      requires(begin_pos + length < N)
    {
      return static_string_view<length, is_mutable>{
        begin() + begin_pos, begin() + begin_pos + length
      };
    }
    template <size_t begin_pos, size_t length>
    constexpr static_string_view<length, false> substr() const noexcept
      requires(begin_pos + length < N)
    {
      return static_string_view<length, false>{begin() + begin_pos, begin() + begin_pos + length};
    }
    constexpr std::string_view as_view() const noexcept {
      return std::string_view{begin(), begin() + N};
    }
    constexpr std::string as_string() const {
      return std::string{begin(), begin() + N};
    }
  };
  export template <size_t N> class static_string {
    std::array<char, N + 1> __container;

  public:
    template <std::ranges::sized_range range_type>
      requires(std::assignable_from<char &, std::ranges::range_value_t<range_type>>)
    constexpr static_string(
      const range_type &r, char fill_char = '\0', padding_type p = padding_type::back
    ) {
      auto r_size = std::ranges::size(r);
      if (r_size >= N) {
        std::ranges::copy_n(std::ranges::begin(r), N, __container.begin());
      } else {
        if (p == padding_type::back) {
          std::ranges::copy_n(std::ranges::begin(r), r_size, __container.begin());
          std::ranges::fill_n(__container.begin() + r_size, N - r_size, fill_char);
        } else {
          std::ranges::fill_n(__container.begin(), N - r_size, fill_char);
          std::ranges::copy_n(std::ranges::begin(r), r_size, __container.begin() + N - r_size);
        }
      }
      __container[N] = '\0';
    }
    constexpr static_string(const char (&s)[N + 1]) {
      std::ranges::copy_n(std::ranges::begin(s), N, __container.begin());
      __container[N] = '\0';
    }

    // Getters
    constexpr std::optional<std::reference_wrapper<const char>> operator[](size_t id
    ) const noexcept {
      if (id < N) {
        return std::cref(__container[id]);
      } else {
        return std::nullopt;
      }
    }
    constexpr std::optional<std::reference_wrapper<char>> operator[](size_t id) noexcept {
      if (id < N) {
        return std::ref(__container[id]);
      } else {
        return std::nullopt;
      }
    }

    // Iterators
    constexpr const char *begin() const noexcept {
      return __container.begin();
    }
    constexpr const char *end() const noexcept {
      return __container.end();
    }
    constexpr char *begin() noexcept {
      return __container.begin();
    }
    constexpr char *end() noexcept {
      return __container.end();
    }
    size_t size() const noexcept {
      return __container.size();
    }

    // String Operations
    constexpr std::string_view as_view() const noexcept {
      return std::string_view{begin(), begin() + N};
    }
    constexpr std::string as_string() const {
      return std::string{begin(), begin() + N};
    }
    template <size_t begin_pos, size_t length>
    constexpr static_string_view<length, true> substr()
      requires(begin_pos + length < N)
    {
      return borrow_mut().template substr<begin_pos, length>();
    }
    template <size_t begin_pos, size_t length>
    constexpr static_string_view<length, false> substr() const
      requires(begin_pos + length < N)
    {
      return borrow().template substr<begin_pos, length>();
    }
    template <size_t new_size>
    constexpr static_string<new_size> resize(
      char fill_char = '\0', padding_type p = padding_type::back
    ) const
      requires(new_size > N)
    {
      return static_string<new_size>{std::ranges::subrange{begin(), begin() + N}, fill_char, p};
    }
    template <size_t new_size>
    constexpr static_string<new_size> resize() const
      requires(new_size < N)
    {
      return static_string<new_size>{__container};
    }

    // Mutability
    constexpr static_string_view<N, true> borrow_mut() noexcept {
      return static_string_view<N, true>{__container.begin(), __container.begin() + N};
    }
    constexpr static_string_view<N, false> borrow() const noexcept {
      return static_string_view<N, false>{__container.begin(), __container.begin() + N};
    }
  };

  export template <size_t N>
    requires(N != 0)
  constexpr static_string<N - 1> make_static_string(const char (&s)[N]) {
    return static_string<N - 1>{s};
  }
  export template <size_t N, typename... Args>
    requires(N != 0 && std::is_constructible_v<static_string<N>, Args...>)
  constexpr static_string<N> make_static_string(Args &&...args) {
    return static_string<N>{std::forward<Args>(args)...};
  }
}

namespace generic = moderna::generic;

template <size_t N, class char_type> struct std::formatter<generic::static_string<N>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::static_string<N> &v, auto &ctx) {
    return std::format_to(ctx.out(), "{}", v.as_view());
  }
};

template <size_t N, bool is_mutable, class char_type>
struct std::formatter<generic::static_string_view<N, is_mutable>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::static_string_view<N, is_mutable> &v, auto &ctx) {
    return std::format_to(ctx.out(), "{}", v.as_view());
  }
};