module;
#include <algorithm>
#include <array>
#include <format>
#include <string_view>
export module moderna.generic:fixed_string;

namespace moderna::generic {
  /*
    fixed_string
    a static buffer that guarantees that the last character is a null character. Most of the time
    this can be used to written exceptions so that everything is noexcept.
  */
  export template <size_t N> requires(N > 1)
  struct fixed_string {
  private:
    // The last character is reserved for the null string and should never be touched.
    std::array<char, N + 1> __buf;
    // This is the current theoretical length of the string.
    size_t __len;

  public:
    using value_type = char;
    constexpr fixed_string() noexcept : __buf{0}, __len{0} {}
    template <size_t N2> requires(N2 < N)
    constexpr fixed_string(const char (&s)[N2]) noexcept : fixed_string() {
      std::ranges::copy_n(s, N2 - 1, __buf.begin());
      __len = N2 - 1;
    }
    constexpr fixed_string(const char (&s)[N]) noexcept : fixed_string() {
      std::ranges::copy_n(s, N - 1, __buf.begin());
      __len = N - 1;
    }
    constexpr fixed_string(std::string_view c) noexcept : fixed_string() {
      __len = std::min(c.length(), N);
      std::ranges::copy_n(c.begin(), __len, __buf.begin());
    }
    constexpr operator std::string_view() const noexcept {
      return std::string_view{begin(), end()};
    }

    // const functions. This indexes into the string
    constexpr std::optional<char> operator[](size_t id) const noexcept {
      if (id > __len) {
        return std::nullopt;
      }
      return std::optional{__buf[id]};
    }
    constexpr const char *begin() const noexcept {
      return __buf.begin();
    }
    constexpr const char *end() const noexcept {
      return __buf.begin() + __len;
    }
    constexpr char *begin() noexcept {
      return __buf.begin();
    }
    constexpr char *end() noexcept {
      return __buf.begin() + __len;
    }
    constexpr size_t length() const noexcept {
      return __len;
    }
    constexpr size_t size() const noexcept {
      return __len;
    }
    constexpr size_t empty_space() const noexcept {
      return N - __len;
    }
    constexpr const char *c_str() const noexcept {
      return __buf.begin();
    }

    // Modification Functions
    constexpr void truncate() noexcept {
      std::ranges::fill_n(begin(), __len, 0);
      __len = 0;
    }
    constexpr char &emplace_back(char c) noexcept {
      if (__len < N) {
        __buf[__len] = c;
        __len += 1;
      }
      return __buf[__len - 1];
    }
    constexpr char &push_back(char c) noexcept {
      return emplace_back(c);
    }
    template <class... Args> requires(std::formattable<Args, char> && ...)
    constexpr void emplace_format(std::format_string<Args...> fmt, Args &&...args) {
      auto inserter = std::back_inserter(*this);
      std::format_to_n(inserter, empty_space(), fmt, std::forward<Args>(args)...);
    }

    constexpr std::optional<std::reference_wrapper<char>> operator[](size_t id) noexcept {
      if (id > __len) {
        return std::nullopt;
      }
      return std::optional{std::ref(__buf[id])};
    }

    // Comparison Operator
    friend constexpr bool operator==(const fixed_string<N> &l, std::string_view r) {
      if (l.length() != r.length()) {
        return false;
      }
      for (size_t i = 0; i < l.length(); i += 1) {
        if (l[i].value() != r[i]) {
          return false;
        }
      }
      return true;
    }
    template <size_t N_R>
    friend constexpr bool operator==(const fixed_string<N> &l, const char (&r)[N_R]) {
      return l == std::string_view{r, r + N_R};
    }
    friend constexpr bool operator==(const fixed_string<N> &l, const char *r) {
      return l == std::string_view{r};
    }
  };
}

namespace generic = moderna::generic;
template <size_t N, class char_type> struct std::formatter<generic::fixed_string<N>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::fixed_string<N> &s, auto &ctx) const {
    std::format_to(ctx.out(), "{}", std::string_view{s});
    return ctx.out();
  }
};

/*
  constexpr tests
*/
#ifdef MODERNA_GENERIC_CONSTEXPR_TESTS
namespace moderna::generic {
  static_assert(fixed_string{"HELLO"}.length() == 5);
  static_assert(fixed_string<7>{"HELLO"}.length() == 5);
  static_assert(fixed_string{"HELLO"}[0].value().get() == 'H');

  constexpr fixed_string<20> test_hello_format() {
    fixed_string<20> v;
    v.emplace_format("{}", "Hello World");
    return v;
  }
}
#endif