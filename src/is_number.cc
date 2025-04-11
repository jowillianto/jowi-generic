module;
#include <concepts>
export module moderna.generic:is_number;

namespace moderna::generic {
  template <class num_type>
  concept is_number = std::floating_point<num_type> || std::integral<num_type>;
}