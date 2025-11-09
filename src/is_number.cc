module;
#include <concepts>
export module jowi.generic:is_number;

namespace jowi::generic {
  export template <class NumType>
  concept IsNumber = std::floating_point<NumType> || std::integral<NumType>;
}