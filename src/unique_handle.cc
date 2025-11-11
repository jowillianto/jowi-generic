module;
#include <concepts>
#include <optional>
export module jowi.generic:unique_handle;

namespace jowi::generic {
  export template <class ValueType, std::invocable<ValueType> Destructor> struct UniqueHandle {
  private:
    std::optional<ValueType> __v;
    Destructor __d;

  public:
    UniqueHandle(ValueType v, Destructor d) : __v{v}, __d{d} {}
    UniqueHandle(const UniqueHandle &) = delete;
    UniqueHandle(UniqueHandle &&o) : __v{std::move(o.__v)}, __d{o.__d} {}
    UniqueHandle &operator=(const UniqueHandle &o) = delete;
    UniqueHandle &operator=(UniqueHandle &&o) {
      __v = std::move(o.__v);
      return *this;
    }

    /*
     * Accessor Functions
     */
    const ValueType &get() const {
      return __v.value();
    }
    ValueType get_or(ValueType fallback_value) const noexcept {
      return __v.value_or(fallback_value);
    }
    ValueType release() {
      return std::move(__v).value();
    }
    ValueType release_or(ValueType fallback_value) const {
      return std::move(__v).value_or(fallback_value);
    }

    ~UniqueHandle() {
      if (__v) {
        __d(std::move(__v).value());
      }
    }

    static UniqueHandle manage_default(ValueType v) requires(std::constructible_from<Destructor>)
    {
      return UniqueHandle(std::move(v), Destructor{});
    }
    static UniqueHandle manage(ValueType v, Destructor d) {
      return UniqueHandle{std::move(v), std::move(d)};
    }
  };
}