#ifndef CONCURRENT_HPP_
#define CONCURRENT_HPP_

#include <atomic>
#include <shared_mutex>
#include <type_traits>
#include <utility>

template <typename T>
concept LockFree
    = std::is_trivially_copyable_v<T>
    && std::is_copy_constructible_v<T>
    && std::is_move_constructible_v<T>
    && std::is_copy_assignable_v<T>
    && std::is_move_assignable_v<T>
    && std::atomic<T>::is_always_lock_free;

template <typename T>
concept NotLockFree = !LockFree<T>;

template <typename>
class Concurrent;

template <LockFree T>
class Concurrent<T> : public std::atomic<T> {
public:
    using std::atomic<T>::atomic;
};

template <NotLockFree T>
class Concurrent<T> {
public:
    static constexpr bool is_always_lock_free = false;

    template <typename... Args>
    Concurrent(Args&&... args)
        : object_ { std::forward<Args>(args)... }
    {
    }

    [[nodiscard]] T operator=(T desired)
    {
        store(desired);

        return desired;
    }

    [[nodiscard]] constexpr bool is_lock_free() const noexcept
    {
        return false;
    }

    void store(T desired)
    {
        std::lock_guard lock { mutex_ };

        object_ = std::move(desired);
    }

    [[nodiscard]] T load() const
    {
        std::shared_lock lock { mutex_ };

        return object_;
    }

    [[nodiscard]] operator T() const
    {
        return load();
    }

    [[nodiscard]] T exchange(T desired)
    {
        std::lock_guard lock { mutex_ };

        return std::exchange(object_, std::move(desired));
    }

private:
    mutable std::shared_mutex mutex_;
    T object_;
};

#endif // CONCURRENT_HPP_
