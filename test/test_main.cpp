#include <algorithm>
#include <chrono>
#include <concepts>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <concurrent.hpp>

struct Foo {
    int foo;
};

struct Bar {
    std::string bar;
};

static_assert(std::derived_from<Concurrent<int>, std::atomic<int>>);
static_assert(std::derived_from<Concurrent<Foo>, std::atomic<Foo>>);
static_assert(!std::derived_from<Concurrent<Bar>, std::atomic<Bar>>);

int main()
{
    using namespace std::literals;

    auto str = Concurrent<std::string> { "hello" };
    auto threads = std::vector<std::thread> {};
    auto mutex = std::mutex {};
    const auto size = std::max(4u, std::thread::hardware_concurrency());

    threads.reserve(size);

    for (auto start = 0u, last = size - 1; start < last; ++start) {
        threads.emplace_back([&str, &mutex, start] {
            std::this_thread::sleep_for(100ns * start);
            const auto read = str.load();

            std::lock_guard lock { mutex };
            std::cout << start << "\t: " << read << '\n';
        });
    }

    threads.emplace_back([&str] {
        str.store("world");
    });

    for (auto& thread : threads)
        thread.join();

    return static_cast<int>(str.load() != "world");
}
