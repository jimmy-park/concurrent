# Concurrent\<T\>

[![CI](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml/badge.svg)](https://github.com/jimmy-park/concurrent/actions/workflows/ci.yaml)

C++20 concurrent wrapper class

## Features

- Provide the same interface as `std::atomic<T>` (load, store, exchange, ...)
- Equal to `std::atomic<T>` when `T` could be a lock-free atomic object
- Implement thread-safe operations using `std::shared_mutex` for non-atomic objects

## Usage

```cpp
#include <concurrent.hpp>

int main()
{
    Concurrent<int> i { 42 }; // This is actually an empty derived class of std::atomic<int>
    Concurrent<std::string> str { "Read and write operations are thread-safe" };

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
