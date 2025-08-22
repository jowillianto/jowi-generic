#include <moderna/generic.hpp>
#include <print>

namespace generic = moderna::generic;

int main(int argc, const char **argv) {
  generic::key_vector<std::string, int> arg_counter;
  for (int i = 0; i < argc; i += 1) {
    auto prev_count = arg_counter.get(argv[i]).transform([](int x) { return x; }).value_or(0);
    arg_counter.emplace(argv[i], prev_count + 1);
  }

  for (const auto &[k, v] : arg_counter) {
    std::println("{}: {}", k, v);
  }
}