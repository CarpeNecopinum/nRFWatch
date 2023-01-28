#pragma once

#include <optional>
#include <coroutine>
#include <cstdint>
#include <exception>
#include <iostream>

struct ContinuationCondition
{
    uint32_t at_millis = -1;
    uint8_t inputs_mask = 0;

    bool should_resume() const;
};

struct Task
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        promise_type();

        ContinuationCondition mCondition;
        std::exception_ptr mException;

        Task get_return_object()
        {
            return {handle_type::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { mException = std::current_exception(); }
        std::suspend_always yield_value(ContinuationCondition from)
        {
            mCondition = from;
            return {};
        }
        void return_void() {}
    };

    handle_type mHandle;
    bool mEnabled = false;

    Task(handle_type h)
        : mHandle(h)
    {
    }
    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&other) noexcept : mHandle(other.mHandle), mEnabled(other.mEnabled)
    {
        other.mHandle = nullptr;
    }
    Task &operator=(Task &&other) noexcept
    {
        std::swap(mHandle, other.mHandle);
        mEnabled = other.mEnabled;
        return *this;
    }
    ~Task()
    {
        if (mHandle)
            mHandle.destroy();
    }

    ContinuationCondition maybe_resume()
    {
        if (mHandle.promise().mCondition.should_resume())
            resume();
        return mHandle.promise().mCondition;
    }

    void resume()
    {
        mHandle();
        if (mHandle.promise().mException)
            std::rethrow_exception(mHandle.promise().mException);
    }

    ContinuationCondition condition() const
    {
        return mHandle.promise().mCondition;
    }

    static ContinuationCondition resume_at(uint32_t millis);
    static ContinuationCondition resume_asap();
    static ContinuationCondition resume_in(uint32_t _millis);
};