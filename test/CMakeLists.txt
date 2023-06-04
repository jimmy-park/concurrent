find_package(Threads REQUIRED)

add_executable(main test_main.cpp)
target_link_libraries(main PRIVATE
    Threads::Threads
    concurrent::concurrent
)
add_test(test_main main)
