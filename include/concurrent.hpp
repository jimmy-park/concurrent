#ifndef CONCURRENT_HPP_
#define CONCURRENT_HPP_

#include <atomic>
#include <concepts>
#include <functional>
#include <shared_mutex>
#include <type_traits>
#include <utility>

namespace detail {

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

} // namespace detail

template <typename>
class Concurrent;

template <detail::LockFree T>
class Concurrent<T> : public std::atomic<T> {
public:
    using std::atomic<T>::atomic;
};

template <detail::NotLockFree T>
class Concurrent<T> {
public:
    static constexpr bool is_always_lock_free = false;

    template <typename... Args>
    Concurrent(Args&&... args)
        noexcept(std::is_nothrow_default_constructible_v<std::shared_mutex> && std::is_nothrow_constructible_v<T, Args&&...>)
        : object_ { std::forward<Args>(args)... }
    {
    }

    ~Concurrent() noexcept(std::destructible<std::shared_mutex> && std::destructible<T>) = default;
    Concurrent(Concurrent&) = delete;
    Concurrent(const Concurrent&) = delete;
    Concurrent(Concurrent&&) = delete;
    Concurrent& operator=(Concurrent&) = delete;
    Concurrent& operator=(const Concurrent&) = delete;
    Concurrent& operator=(Concurrent&&) = delete;

    [[nodiscard]] T operator=(T desired)
        noexcept(noexcept(store(desired)) && std::is_nothrow_move_constructible_v<T>)
    {
        store(desired);

        return desired;
    }

    [[nodiscard]] constexpr bool is_lock_free() const noexcept
    {
        return false;
    }

    void store(T desired)
        noexcept(is_nothrow_lockable && std::is_nothrow_move_assignable_v<T>)
    {
        std::lock_guard lock { mutex_ };

        object_ = std::move(desired);
    }

    [[nodiscard]] T load() const
        noexcept(is_nothrow_lockable && std::is_nothrow_copy_constructible_v<T>)
    {
        std::shared_lock lock { mutex_ };

        return object_;
    }

    [[nodiscard]] operator T() const noexcept(noexcept(load()))
    {
        return load();
    }

    [[nodiscard]] T exchange(T desired)
        noexcept(is_nothrow_lockable && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
    {
        std::lock_guard lock { mutex_ };

        return std::exchange(object_, std::move(desired));
    }

    [[nodiscard]] T release()
        noexcept(is_nothrow_lockable && std::is_nothrow_move_constructible_v<T>)
    {
        std::lock_guard lock { mutex_ };

        return std::move(object_);
    }

    template <std::invocable<const T&> Fn>
    auto apply_shared(Fn&& func) const
        noexcept(is_nothrow_lockable && std::is_nothrow_invocable_v<Fn, const T&>)
    {
        using Result = std::invoke_result_t<Fn, const T&>;
        static_assert(!std::is_reference_v<Result> && !std::is_pointer_v<Result>);

        std::shared_lock lock { mutex_ };

        return std::invoke(std::forward<Fn>(func), object_);
    }

    template <std::invocable<T&> Fn>
    auto apply_exclusive(Fn&& func)
        noexcept(is_nothrow_lockable && std::is_nothrow_invocable_v<Fn, T&>)
    {
        using Result = std::invoke_result_t<Fn, T&>;
        static_assert(!std::is_reference_v<Result> && !std::is_pointer_v<Result>);

        std::lock_guard lock { mutex_ };

        return std::invoke(std::forward<Fn>(func), object_);
    }

private:
    static constexpr bool is_nothrow_lockable = requires(std::shared_mutex mutex) {
        { mutex.lock()          } noexcept;
        { mutex.lock_shared()   } noexcept;
        { mutex.unlock()        } noexcept;
        { mutex.unlock_shared() } noexcept;
    };

    mutable std::shared_mutex mutex_;
    T object_;
};

#endif // CONCURRENT_HPP_
