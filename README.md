# Concurrent\<T\>

[![CI](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml/badge.svg)](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml)

C++20 concurrent wrapper class

## Features

- Equal to `std::atomic<T>` when `T` could be a lock-free atomic object
- Implement thread-safe operations using `std::shared_mutex` for non-atomic objects
  - Provide the same interface as `std::atomic<T>` (load, store, exchange, ...)
  - `apply_shared(Callable)` : Call `lock_shared()` before invoking the Callable object
  - `apply_exclusive(Callable)` : Call `lock()` before invoking the Callable object

## Usage

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

### CMake Integration

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
