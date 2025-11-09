module;
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <format>
export module jowi.generic:staged_error;
import :static_string;
import :is_formattable_error;

namespace jowi::generic {
  template <StaticString stage, StaticString... stages>
  std::optional<std::string> get_stage_name(size_t id) {
    if (id == 0) {
      return stage.as_string();
    } else if constexpr (sizeof...(stages) == 0) {
      return std::nullopt;
    } else {
      return get_stage_name<stages...>(id);
    }
  }
  export template <StaticString... stages> class StagedError : public std::exception {
    std::string __msg;
    size_t __stage;

  public:
    StagedError(std::string msg, size_t stage) : __msg{std::move(msg)}, __stage{stage} {}
    size_t stage() const noexcept {
      return __stage;
    }
    std::optional<std::string> stage_name() const noexcept {
      return get_stage_name<stages...>(__stage);
    }
    const char *what() const noexcept {
      return __msg.c_str();
    }
    std::string_view as_view() const noexcept {
      return std::string_view{__msg.c_str()};
    }
  };
  export template <StaticString... stages> struct StagedErrorCreator {
    using ErrorType = StagedError<stages...>;
    size_t stage;

    ErrorType operator()(const IsFormattableError auto &e) {
      return ErrorType{std::format("{}", ErrorFormatter{e}), stage};
    }
  };
}
namespace generic = jowi::generic;
