module;
#include <concepts>
#include <string_view>
export module moderna.generic:is_whatable_error;

namespace moderna::generic {
  export template <class error_type>
  concept is_whatable_error =
    std::constructible_from<std::string_view, decltype(std::declval<error_type>().what())>;
}