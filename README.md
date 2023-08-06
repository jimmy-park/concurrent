# Concurrent\<T\>

[![CI](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml/badge.svg)](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml)
[![CodeQL](https://github.com/jimmy-park/concurrent/actions/workflows/codeql.yaml/badge.svg)](https://github.com/jimmy-park/concurrent/actions/workflows/codeql.yaml)

C++20 concurrent wrapper class

## Features

- Equal to `std::atomic<T>` when `T` could be a lock-free atomic object
- Implement thread-safe operations using `std::shared_mutex` for non-atomic objects
  - Provide the same interface as `std::atomic` (load, store, exchange, ...)
  - `release()` : Release the managed object
  - `apply_shared(Callable)` : Call `lock_shared()` before invoking the Callable object
  - `apply_exclusive(Callable)` : Call `lock()` before invoking the Callable object

## CMake Options

| Option               | Default | Description                       |
| ---                  | ---     | ---                               |
| `CONCURRENT_COMPILE` | `OFF`   | Build as a static/shared library  |
| `CONCURRENT_INSTALL` | `OFF`   | Install headers and CMake targets |

## Usage

### Build

```sh
# List all presets
cmake --list-presets all

# Use a configure preset
cmake --preset windows

# Use a build preset
# <configure-preset>-[clean|install]
cmake --build --preset windows

# Use a test preset
ctest --preset windows

# Use a build preset for install
# equal to `cmake --build --preset windows --target install`
cmake --build --preset windows-install
```

### Integration

Require CMake 3.23+ due to `target_sources(FILE_SET)`

```CMake
include(FetchContent)

FetchContent_Declare(
    concurrent
    URL https://github.com/jimmy-park/concurrent/archive/main.tar.gz
)
FetchContent_MakeAvailable(concurrent)

# If you're using CPM.cmake
# CPMAddPackage(
#     NAME concurrent
#     URL https://github.com/jimmy-park/concurrent/archive/main.tar.gz
# )

add_executable(main main.cpp)
target_link_libraries(main PRIVATE concurrent::concurrent)
```

## Example

```cpp
#include <concurrent.hpp>

int main()
{
    // This is actually an empty derived class of std::atomic<int>
    Concurrent<int> concurrent_int { 0 };
    concurrent_int.fetch_add(42);

    // Read and write operations are thread-safe
    Concurrent<std::string> concurrent_string;
    std::vector<std::thread> threads;

    static constexpr auto size = 10;
    threads.reserve(size);

    for (auto start = 0, last = size; start < last; ++start) {
        threads.emplace_back([&concurrent_string, start] {
            concurrent_string.apply_exclusive([start](auto& str) {
                str += "thread ";
                str += static_cast<char>('0' + start);
                str += '\n';
            });
        });
    }

    for (auto& thread : threads)
        thread.join();

    concurrent_string.apply_shared([](const auto& str) {
        std::cout << str << std::endl;
    });

    return 0;
}
```
