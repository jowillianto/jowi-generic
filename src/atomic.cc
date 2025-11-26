module;
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
export module jowi.generic:atomic;

namespace jowi::generic {
  export template <class Tag> requires(sizeof(Tag) <= 2)
  struct TaggedPtr {
    uint64_t raw_value;

    std::pair<void *, Tag> to_pair() const noexcept {
      return std::pair{raw_ptr(), tag()};
    }

    void *raw_ptr() const noexcept {
      return reinterpret_cast<void *>((raw_value << bit_tag_size()) >> bit_tag_size());
    }

    Tag tag() const noexcept {
      uint16_t tag = static_cast<uint64_t>(raw_value >> bit_pointer_size());
      return *static_cast<Tag *>(static_cast<void *>(&tag));
    }

    friend constexpr bool operator==(const TaggedPtr &l, const TaggedPtr &r) {
      return l.raw_value == r.raw_value;
    }

    static consteval size_t bit_pointer_size() {
      return 48;
    }

    static consteval size_t bit_tag_size() {
      return 16;
    }

    static consteval uint64_t pointer_mask() {
      return 0x0000'FFFF'FFFF'FFFF;
    }

    static consteval uint64_t tag_mask() {
      return 0xFFFF'0000'0000'0000;
    }

    static constexpr TaggedPtr from_pair(const void *ptr, Tag tag) noexcept {
      TaggedPtr p{0};
      std::memcpy(static_cast<void *>(&p.raw_value), static_cast<const void *>(&tag), sizeof(Tag));
      p.raw_value = p.raw_value << bit_pointer_size();
      p.raw_value = p.raw_value | reinterpret_cast<uint64_t>(ptr) & pointer_mask();
      return p;
    }

    static constexpr TaggedPtr null() noexcept {
      return TaggedPtr{0};
    }
    static constexpr TaggedPtr null_tag(void *ptr) {
      TaggedPtr p{0};
      p.raw_value = p.raw_value | reinterpret_cast<uint64_t>(ptr) & pointer_mask();
      return p;
    }
  };

  template struct TaggedPtr<bool>;
  template struct TaggedPtr<uint16_t>;

  export using BoolTaggedPtr = TaggedPtr<bool>;
  export using Uint16TaggedPtr = TaggedPtr<uint16_t>;

  export template <class... Args> requires((sizeof(Args) + ...) <= 8)
  struct PackedByte {
    uint64_t raw_value;
  };
}

namespace generic = jowi::generic;

template <class Tag> requires(sizeof(Tag) <= 2)
struct std::atomic<generic::TaggedPtr<Tag>> {
private:
  std::atomic<uint64_t> __v;

  using TaggedPtr = generic::TaggedPtr<Tag>;

public:
  atomic(TaggedPtr v) : __v{v.raw_value} {}

  TaggedPtr load(std::memory_order m = std::memory_order_seq_cst) const noexcept {
    return TaggedPtr{__v.load(m)};
  }

  void store(TaggedPtr v, std::memory_order m = std::memory_order_seq_cst) noexcept {
    __v.store(v.raw_value, m);
  }

  TaggedPtr exchange(TaggedPtr v, std::memory_order m = std::memory_order_seq_cst) noexcept {
    return TaggedPtr{__v.exchange(v.raw_value, m)};
  }

  bool compare_exchange_weak(
    TaggedPtr &e,
    TaggedPtr d,
    std::memory_order s = std::memory_order_seq_cst,
    std::memory_order f = std::memory_order_seq_cst
  ) noexcept {
    return __v.compare_exchange_weak(e.raw_value, d.raw_value, s, f);
  }

  bool compare_exchange_strong(
    TaggedPtr &e,
    TaggedPtr d,
    std::memory_order s = std::memory_order_seq_cst,
    std::memory_order f = std::memory_order_seq_cst
  ) noexcept {
    return __v.compare_exchange_strong(e.raw_value, d.raw_value, s, f);
  }
};