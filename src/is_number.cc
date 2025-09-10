module;
#include <concepts>
export module jowi.generic:is_number;

namespace jowi::generic {
  export template <class num_type>
  concept is_number = std::floating_point<num_type> || std::integral<num_type>;
}