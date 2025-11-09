module;
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
export module jowi.generic:static_string;

namespace jowi::generic {
  export enum struct padding_type { front, back };
  export template <size_t N, bool IsMutable> struct StaticStringView {
    using PointerType = std::conditional_t<IsMutable, char *, const char *>;
    PointerType __beg;
    PointerType __end;
    constexpr StaticStringView(PointerType Beg, PointerType end) : __beg{Beg}, __end{end} {}
    constexpr StaticStringView(const StaticStringView<N, false> &s) :
      __beg{s.__beg}, __end{s.__end} {}

    // Getters
    constexpr std::optional<std::reference_wrapper<const char>> operator[](
      size_t id
    ) const noexcept {
      if (id < N) {
        return std::cref(*(__beg + id));
      } else {
        return std::nullopt;
      }
    }
    constexpr std::optional<std::reference_wrapper<char>> operator[](size_t id) noexcept
      requires(IsMutable)
    {
      if (id < N) {
        return std::ref(*(__beg + id));
      } else {
        return std::nullopt;
      }
    }

    // Iterators
    constexpr PointerType begin() const noexcept {
      return __beg;
    }
    constexpr PointerType end() const noexcept {
      return __end;
    }
    constexpr char *begin() noexcept requires(IsMutable)
    {
      return __beg;
    }
    constexpr char *end() noexcept requires(IsMutable)
    {
      return __end;
    }

    // String Operations
    template <size_t BeginPos, size_t Length> requires(BeginPos + Length <= N)
    constexpr StaticStringView<Length, IsMutable> substr() noexcept {
      return StaticStringView<Length, IsMutable>{
        begin() + BeginPos, begin() + BeginPos + Length
      };
    }
    template <size_t BeginPos, size_t Length> requires(BeginPos + Length <= N)
    constexpr StaticStringView<Length, false> substr() const noexcept {
      return StaticStringView<Length, false>{begin() + BeginPos, begin() + BeginPos + Length};
    }
    constexpr std::string_view as_view() const noexcept {
      return std::string_view{begin(), begin() + N};
    }
    constexpr std::string as_string() const {
      return std::string{begin(), begin() + N};
    }
    template <size_t Beg>
    constexpr StaticStringView<N, IsMutable> &emplace(const char (&str)[N - Beg + 1]) noexcept
      requires(IsMutable)
    {
      std::ranges::copy_n(std::ranges::begin(str), N - Beg, begin() + Beg);
      return *this;
    }
    constexpr StaticStringView<N, IsMutable> &emplace(const char (&str)[N + 1]) noexcept
      requires(IsMutable)
    {
      return emplace<0>(str);
    }
    template <size_t Beg, std::ranges::input_range RangeT>
    requires(std::same_as<std::ranges::range_value_t<RangeT>, char>)
    constexpr StaticStringView<N, IsMutable> &emplace(const RangeT &s) noexcept
      requires(IsMutable)
    {
      std::ranges::copy_n(std::ranges::cbegin(s), N - Beg, begin() + Beg);
      return *this;
    }
    template <std::ranges::input_range RangeT>
    requires(std::same_as<std::ranges::range_value_t<RangeT>, char>)
    constexpr StaticStringView<N, IsMutable> &emplace(const RangeT &s) noexcept
      requires(IsMutable)
    {
      return emplace<0>(s);
    }
  };
  export template <size_t N> struct StaticString {
    std::array<char, N + 1> __container;
    template <std::ranges::sized_range RangeType>
    requires(std::assignable_from<char &, std::ranges::range_value_t<RangeType>>)
    constexpr StaticString(
      const RangeType &r, char fill_char = '\0', padding_type p = padding_type::back
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
    constexpr StaticString(const char (&s)[N + 1]) {
      std::ranges::copy_n(std::ranges::begin(s), N, __container.begin());
      __container[N] = '\0';
    }
    constexpr StaticString(char fill_char = ' ') {
      std::ranges::fill_n(__container.begin(), N, fill_char);
      __container[N] = '\0';
    }

    // Getters
    constexpr std::optional<std::reference_wrapper<const char>> operator[](
      size_t id
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
    constexpr operator StaticStringView<N, false>() const noexcept {
      return borrow();
    }
    constexpr operator StaticStringView<N, true>() noexcept {
      return borrow_mut();
    }
    constexpr operator StaticStringView<N, false>() noexcept {
      return borrow();
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
    template <size_t BeginPos, size_t Length> constexpr StaticStringView<Length, true> substr() {
      return borrow_mut().template substr<BeginPos, Length>();
    }
    template <size_t BeginPos, size_t Length> requires(BeginPos + Length < N)
    constexpr StaticStringView<Length, false> substr() const {
      return borrow().template substr<BeginPos, Length>();
    }
    template <size_t NewSize>
    constexpr StaticString<NewSize> resize(
      char fill_char = '\0', padding_type p = padding_type::back
    ) const requires(NewSize >= N)
    {
      return StaticString<NewSize>{
        std::ranges::subrange{begin(), begin() + N}, fill_char, p
      };
    }
    template <size_t NewSize> requires(NewSize < N)
    constexpr StaticString<NewSize> resize() const {
      return StaticString<NewSize>{__container};
    }

    // Mutability
    constexpr StaticStringView<N, true> borrow_mut() noexcept {
      return StaticStringView<N, true>{__container.begin(), __container.begin() + N};
    }
    constexpr StaticStringView<N, false> borrow() const noexcept {
      return StaticStringView<N, false>{__container.begin(), __container.begin() + N};
    }

    template <size_t Beg>
    constexpr StaticString<N> &emplace(const char (&str)[N - Beg + 1]) noexcept {
      borrow_mut().template emplace<Beg>(str);
      return *this;
    }
    constexpr StaticString<N> &emplace(const char (&str)[N + 1]) noexcept {
      return emplace<0>(str);
    }
    template <size_t Beg, std::ranges::input_range RangeT>
    requires(std::same_as<std::ranges::range_value_t<RangeT>, char>)
    constexpr StaticString<N> &emplace(const RangeT &s) noexcept {
      borrow_mut().template emplace<Beg, RangeT>(s);
      return *this;
    }
    template <std::ranges::input_range RangeT>
    requires(std::same_as<std::ranges::range_value_t<RangeT>, char>)
    constexpr StaticString<N> &emplace(const RangeT &s) noexcept {
      return emplace<0>(s);
    }
  };

  export template <size_t N> requires(N != 0)
  constexpr StaticString<N - 1> make_static_string(const char (&s)[N]) {
    return StaticString<N - 1>{s};
  }
  export template <size_t N, typename... Args>
  requires(N != 0 && std::is_constructible_v<StaticString<N>, Args...>)
  constexpr StaticString<N> make_static_string(Args &&...args) {
    return StaticString<N>{std::forward<Args>(args)...};
  }
}


namespace generic = jowi::generic;
template <size_t N, class CharType> struct std::formatter<generic::StaticString<N>, CharType> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::StaticString<N> &v, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", v.as_view());
  }
};

template <size_t N, bool IsMutable, class CharType>
struct std::formatter<generic::StaticStringView<N, IsMutable>, CharType> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::StaticStringView<N, IsMutable> &v, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", v.as_view());
  }
};
