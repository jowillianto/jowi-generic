module;
#include <algorithm>
#include <array>
#include <format>
#include <string_view>
export module moderna.generic:sized_string;

namespace moderna::generic {
  export template <size_t N> requires(N > 1)
  struct sized_string {
  private:
    std::array<char, N> __buf;
    size_t __len;

  public:
    constexpr sized_string() noexcept : __buf{0}, __len{0} {}
    template <size_t N2> requires(N2 < N)
    constexpr sized_string(const char (&s)[N2]) noexcept {
      std::ranges::copy_n(s, N2, __buf.begin());
      __len = N2 - 1;
    }
    constexpr sized_string(const char (&s)[N]) noexcept {
      std::ranges::copy_n(s, N, __buf.begin());
      __len = N - 1;
    }

    // const functions
    constexpr std::optional<char> operator[](size_t id) const noexcept {
      if (id > __len) {
        return std::nullopt;
      }
      return std::optional{id};
    }
    constexpr const char *begin() const noexcept {
      return __buf.begin();
    }
    constexpr const char *end() const noexcept {
      return __buf.begin() + __len;
    }
    constexpr size_t length() const noexcept {
      return __len;
    }
    constexpr std::string_view as_view() const noexcept {
      return std::string_view{begin(), end()};
    }

    // modifying functions
    constexpr char *begin() noexcept {
      return __buf.begin();
    }
    constexpr char *end() noexcept {
      return __buf.begin() + N - 1;
    }
    /*
      This will update the length of the current sized_string.
      i.e. recompute.
    */
    constexpr void compute_length() noexcept {
      auto pos = std::ranges::find(__buf, static_cast<char>(0));
      __len = std::distance(__buf.begin(), pos);
    }
  };
}

namespace generic = moderna::generic;
template <size_t N, class char_type> struct std::formatter<generic::sized_string<N>, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const generic::sized_string<N> &s, auto &ctx) const {
    std::format_to(ctx.out(), "{}", s.as_view());
    return ctx.out();
  }
};

/*
  constexpr tests
*/
#ifdef MODERNA_GENERIC_CONSTEXPR_TESTS
namespace moderna::generic {
  static_assert(sized_string{"HELLO"}.length() == 5);
  static_assert(sized_string<7>{"HELLO"}.length() == 5);

  consteval size_t test_copy_mutate() {
    sized_string<11> s{};
    std::ranges::copy_n("HELLO", 5, s.begin());
    s.compute_length();
    return s.length();
  }
  static_assert(test_copy_mutate() == 5);
}
#endif