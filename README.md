# Generic

The `moderna.generic` module provides enhanced generic data types with additional convenience methods and improved safety features.

## 1. prec_fp<num_type>

[Documentation to be provided]

## 2. static_prec_fp<num_type, int acc>

[Documentation to be provided]

## 3. variant<variants...>

A wrapper around `std::variant` that provides additional convenience methods for easier usage.

**Template Parameters:**
- `variants...`: The types that the variant can hold

#### Constructor

```cpp
template <typename... Args> 
requires(std::constructible_from<variant_type, Args...>)
constexpr variant(Args &&...args)
```

Constructs a variant with the given arguments.

**Examples:**
```cpp
// Construct with int
generic::variant<int, std::string, double> v1{42};

// Construct with string
generic::variant<int, std::string, double> v2{std::string("hello")};

// Construct with double
generic::variant<int, std::string, double> v3{3.14};

// Construct with string literal (deduced as string)
generic::variant<int, std::string, double> v4{"world"};
```

#### Assignment Operator

```cpp
template <class T> 
requires(std::assignable_from<variant_type &, T>)
constexpr variant &operator=(T &&t)
```

Assigns a new value to the variant.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};

// Assign different types
v = std::string("changed to string");
v = 2.71;
v = 100;
```

#### index()

```cpp
constexpr int index() const noexcept
```

Returns the zero-based index of the currently held type.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};
assert(v.index() == 0);  // int is at index 0

v = std::string("hello");
assert(v.index() == 1);  // string is at index 1

v = 3.14;
assert(v.index() == 2);  // double is at index 2
```

#### is<T>()

```cpp
template <is_in_target<variants...> test_type> 
constexpr bool is() const noexcept
```

Checks if the variant currently holds the specified type.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};

assert(v.is<int>() == true);
assert(v.is<std::string>() == false);
assert(v.is<double>() == false);

v = std::string("hello");
assert(v.is<int>() == false);
assert(v.is<std::string>() == true);
assert(v.is<double>() == false);
```

#### as<T>() (non-const)

```cpp
template <is_in_target<variants...> test_type>
constexpr std::optional<std::reference_wrapper<test_type>> as() noexcept
```

Returns an optional reference wrapper to the held value if it matches the requested type.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};

// Successful conversion
if (auto result = v.as<int>(); result.has_value()) {
    int& value = result->get();
    value = 100;  // Modify the variant's value
    assert(v.as<int>()->get() == 100);
}

// Failed conversion
auto failed = v.as<std::string>();
assert(!failed.has_value());

// String example with modification
v = std::string("original");
if (auto str_result = v.as<std::string>(); str_result.has_value()) {
    str_result->get() = "modified";
    assert(v.as<std::string>()->get() == "modified");
}
```

#### as<T>() (const)

```cpp
template <is_in_target<variants...> test_type>
constexpr std::optional<std::reference_wrapper<const test_type>> as() const noexcept
```

Returns an optional const reference wrapper to the held value if it matches the requested type.

**Examples:**
```cpp
const generic::variant<int, std::string, double> v{std::string("hello")};

// Successful const conversion
if (auto result = v.as<std::string>(); result.has_value()) {
    const std::string& value = result->get();
    assert(value == "hello");
    // value = "cannot modify"; // Would be a compile error
}

// Failed const conversion
auto failed = v.as<int>();
assert(!failed.has_value());
```

#### emplace<T>()

```cpp
template <is_in_target<variants...> T, class... Args>
constexpr variant &emplace(Args &&...args) noexcept
```

Constructs a new value of type T in-place, replacing the current value.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};

// Emplace string with constructor arguments
v.emplace<std::string>("emplaced string");
assert(v.is<std::string>());
assert(v.as<std::string>()->get() == "emplaced string");

// Emplace string with size and character
v.emplace<std::string>(5, 'x');
assert(v.as<std::string>()->get() == "xxxxx");

// Emplace double
v.emplace<double>(2.71828);
assert(v.is<double>());
assert(v.as<double>()->get() == 2.71828);

// Emplace int
v.emplace<int>(999);
assert(v.as<int>()->get() == 999);
```

#### visit() (lvalue reference)

```cpp
template <class... F>
requires(std::invocable<variant_visitor_overload<F...>, variants &> && ...)
constexpr auto visit(F &&...f) &
```

Visits the variant with overloaded functions, automatically dispatching to the correct overload.

**Examples:**
```cpp
generic::variant<int, std::string, double> v{42};

// Visit with overloaded lambdas
v.visit(
    [](int& i) { 
        std::cout << "Got int: " << i << std::endl;
        i *= 2;  // Can modify the value
    },
    [](std::string& s) { 
        std::cout << "Got string: " << s << std::endl;
        s += " modified";
    },
    [](double& d) { 
        std::cout << "Got double: " << d << std::endl;
        d += 1.0;
    }
);
// Prints: "Got int: 42" and modifies v to contain 84

// Visit with different return types (if all return same type)
std::string description = v.visit(
    [](int& i) -> std::string { return "Integer: " + std::to_string(i); },
    [](std::string& s) -> std::string { return "String: " + s; },
    [](double& d) -> std::string { return "Double: " + std::to_string(d); }
);
```

#### visit() (const lvalue reference)

```cpp
template <class... F>
requires(std::invocable<variant_visitor_overload<F...>, const variants &> && ...)
constexpr auto visit(F &&...f) const &
```

Visits a const variant with overloaded functions.

**Examples:**
```cpp
const generic::variant<int, std::string, double> v{std::string("hello")};

v.visit(
    [](const int& i) { 
        std::cout << "Const int: " << i << std::endl;
        // i = 10; // Would be compile error
    },
    [](const std::string& s) { 
        std::cout << "Const string: " << s << std::endl;
        // s += "x"; // Would be compile error
    },
    [](const double& d) { 
        std::cout << "Const double: " << d << std::endl;
        // d = 1.0; // Would be compile error
    }
);
// Prints: "Const string: hello"
```

#### visit() (rvalue reference)

```cpp
template <class... F>
requires(std::invocable<variant_visitor_overload<F...>, variants &&> && ...)
constexpr auto visit(F &&...f) &&
```

Visits an rvalue variant with overloaded functions, allowing move operations.

**Examples:**
```cpp
auto create_variant = []() {
    return generic::variant<int, std::string, double>{std::string("movable")};
};

std::string result = std::move(create_variant()).visit(
    [](int&& i) -> std::string { 
        return "Moved int: " + std::to_string(i);
    },
    [](std::string&& s) -> std::string { 
        return "Moved string: " + std::move(s);  // Can move from the value
    },
    [](double&& d) -> std::string { 
        return "Moved double: " + std::to_string(d);
    }
);
// result contains: "Moved string: movable"

// Direct rvalue visit
generic::variant<int, std::string, double>{42}.visit(
    [](int&& i) { std::cout << "Temporary int: " << i << std::endl; },
    [](std::string&& s) { std::cout << "Temporary string: " << s << std::endl; },
    [](double&& d) { std::cout << "Temporary double: " << d << std::endl; }
);
// Prints: "Temporary int: 42"
```

## 4. static_string<size_t length>

[Documentation to be provided]

## 5. fixed_string<size_t N>

A wrapper around `std::variant` that provides additional convenience methods for easier usage.

**Template Parameters:**
- `N`: The maximum capacity of the string (requires N > 1)

A fixed-size buffer that encapsulates a string with compile-time determined maximum capacity. The actual buffer size is N + 1 where the last character always points to a null character, ensuring null-termination.

#### Default Constructor

```cpp
constexpr fixed_string() noexcept
```

Creates an empty fixed_string.

**Examples:**
```cpp
generic::fixed_string<10> empty_str;
assert(empty_str.length() == 0);
assert(empty_str.empty_space() == 10);
```

#### String Literal Constructor (smaller)

```cpp
template <size_t N2> requires(N2 < N)
constexpr fixed_string(const char (&s)[N2]) noexcept
```

Constructs from a string literal smaller than the capacity.

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello"};  // N2 = 6, N = 20
assert(str.length() == 5);
assert(str == "Hello");
```

#### String Literal Constructor (exact size)

```cpp
constexpr fixed_string(const char (&s)[N]) noexcept
```

Constructs from a string literal that exactly matches the capacity.

**Examples:**
```cpp
generic::fixed_string<5> str{"Hello"};  // Exactly fits
assert(str.length() == 5);
assert(str == "Hello");
```

#### String View Conversion

```cpp
constexpr operator std::string_view() const noexcept
```

Implicitly converts to string_view for compatibility with string algorithms.

**Examples:**
```cpp
generic::fixed_string<10> str{"test"};
std::string_view view = str;  // Implicit conversion
assert(view == "test");

// Use with string algorithms
bool starts_with_t = view.starts_with('t');
```

#### operator[] (const)

```cpp
constexpr std::optional<char> operator[](size_t id) const noexcept
```

Returns the character at the given index, or nullopt if index is out of bounds.

**Examples:**
```cpp
generic::fixed_string<10> str{"Hello"};
assert(str[0].value() == 'H');
assert(str[4].value() == 'o');
assert(!str[10].has_value());  // Out of bounds
```

#### operator[] (non-const)

```cpp
constexpr std::optional<std::reference_wrapper<char>> operator[](size_t id) noexcept
```

Returns a reference wrapper to the character at the given index, or nullopt if out of bounds.

**Examples:**
```cpp
generic::fixed_string<10> str{"Hello"};
if (auto ch = str[0]; ch.has_value()) {
    ch->get() = 'h';  // Modify the character
    assert(str == "hello");
}
```

#### begin() / end() (const)

```cpp
constexpr const char *begin() const noexcept
constexpr const char *end() const noexcept
```

Returns const iterators to the beginning and end of the string.

**Examples:**
```cpp
generic::fixed_string<10> str{"Hello"};
for (auto it = str.begin(); it != str.end(); ++it) {
    std::cout << *it;  // Prints: Hello
}

// Range-based for loop
for (char c : str) {
    std::cout << c;  // Prints: Hello
}
```

#### begin() / end() (non-const)

```cpp
constexpr char *begin() noexcept
constexpr char *end() noexcept
```

Returns mutable iterators to the beginning and end of the string.

**Examples:**
```cpp
generic::fixed_string<10> str{"Hello"};
std::ranges::transform(str, str.begin(), [](char c) { 
    return std::toupper(c); 
});
assert(str == "HELLO");
```

#### length() / size()

```cpp
constexpr size_t length() const noexcept
constexpr size_t size() const noexcept
```

Returns the current length of the string.

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello World"};
assert(str.length() == 11);
assert(str.size() == 11);  // Same as length()
```

#### empty_space()

```cpp
constexpr size_t empty_space() const noexcept
```

Returns the remaining capacity (N - current length).

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello"};
assert(str.empty_space() == 15);  // 20 - 5
assert(str.length() + str.empty_space() == 20);
```

#### c_str()

```cpp
constexpr const char *c_str() const noexcept
```

Returns a null-terminated C-style string pointer.

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello"};
const char* cstr = str.c_str();
printf("%s\n", cstr);  // Prints: Hello
assert(std::strlen(cstr) == str.length());
```

#### truncate()

```cpp
constexpr void truncate() noexcept
```

Clears the string and resets length to 0.

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello World"};
assert(str.length() == 11);

str.truncate();
assert(str.length() == 0);
assert(str.empty_space() == 20);
assert(str == "");
```

#### emplace_back() / push_back()

```cpp
constexpr char &emplace_back(char c) noexcept
constexpr char &push_back(char c) noexcept
```

Adds a character to the end of the string. If at capacity, does nothing.

**Examples:**
```cpp
generic::fixed_string<10> str{"Hell"};
assert(str.length() == 4);

char& added = str.emplace_back('o');
assert(str == "Hello");
assert(added == 'o');

str.push_back('!');  // Same as emplace_back
assert(str == "Hello!");

// At capacity, does nothing
generic::fixed_string<5> full_str{"Hello"};
full_str.push_back('!');  // No effect, already at capacity
assert(full_str == "Hello");
```

#### emplace_format()

```cpp
template <class... Args> requires(std::formattable<Args, char> && ...)
constexpr void emplace_format(std::format_string<Args...> fmt, Args &&...args)
```

Formats and appends text to the string using std::format, respecting capacity limits.

**Examples:**
```cpp
generic::fixed_string<50> str;
str.emplace_format("Hello {}", "World");
assert(str == "Hello World");

str.truncate();
str.emplace_format("Number: {}, Pi: {:.2f}", 42, 3.14159);
assert(str == "Number: 42, Pi: 3.14");

// Respects capacity limits
generic::fixed_string<10> small_str;
small_str.emplace_format("This is a very long string");
// Only first 10 characters are added
```

#### Comparison Operators

```cpp
friend constexpr bool operator==(const fixed_string<N> &l, std::string_view r)
template <size_t N_R>
friend constexpr bool operator==(const fixed_string<N> &l, const char (&r)[N_R])
friend constexpr bool operator==(const fixed_string<N> &l, const char *r)
```

Compares fixed_string with various string types.

**Examples:**
```cpp
generic::fixed_string<20> str{"Hello"};

// Compare with string_view
assert(str == std::string_view{"Hello"});
assert(str != std::string_view{"World"});

// Compare with string literal
assert(str == "Hello");
assert(str != "World");

// Compare with C string
const char* cstr = "Hello";
assert(str == cstr);

// Different lengths
assert(str != "Hello World");
```