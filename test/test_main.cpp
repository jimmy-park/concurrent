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
    static constexpr auto size = 10;

    Concurrent<std::string> concurrent_string;
    std::vector<std::thread> threads;

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
