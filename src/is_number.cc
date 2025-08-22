#ifdef MODERNA_GENERIC_MODULES
module;
#endif
#include <concepts>
#if defined(MODERNA_GENERIC_MODULES)
export module moderna.generic:is_number;
#endif

namespace moderna::generic {
  template <class num_type>
  concept is_number = std::floating_point<num_type> || std::integral<num_type>;
}