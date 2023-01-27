#pragma once

#include <optional>
#include <coroutine>
#include <cstdint>
#include <exception>
#include <iostream>

struct ContinuationCondition
{
    enum class Type
    {
        Immediately,
        Millis
    };
    Type type;

    union
    {
        uint32_t millis;
    } mData;

    static ContinuationCondition Immediately()
    {
        return ContinuationCondition{Type::Immediately};
    }
    static ContinuationCondition Millis(uint32_t millis)
    {
        return ContinuationCondition{Type::Millis, millis};
    }

    bool should_resume() const;

    uint32_t next_resume() const
    {
        switch (type)
        {
        case Type::Immediately:
            return 0;
        case Type::Millis:
            return mData.millis;
        default:
            return 0;
        }
    }
};

struct Task
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        ContinuationCondition mCondition = ContinuationCondition::Immediately();
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
            mCondition = from; // caching the result in promise
            return {};
        }
        void return_void() {}
    };

    handle_type mHandle;
    uint32_t mNextResume;
    bool mEnabled = false;

    Task(handle_type h)
        : mHandle(h)
    {
    }
    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&other) noexcept : mHandle(other.mHandle), mNextResume(other.mNextResume), mEnabled(other.mEnabled)
    {
        other.mHandle = nullptr;
    }
    Task &operator=(Task &&other) noexcept
    {
        std::swap(mHandle, other.mHandle);
        mNextResume = other.mNextResume;
        mEnabled = other.mEnabled;
        return *this;
    }
    ~Task()
    {
        if (mHandle)
            mHandle.destroy();
    }

    uint32_t maybe_resume()
    {
        // if (mHandle.promise().mValue <= millis())

        if (mHandle.promise().mCondition.should_resume())
            resume();
        // mHandle();
        // return mHandle.promise().mCondition.next_resume();
        return mNextResume;
    }

    void resume()
    {
        mHandle();
        if (mHandle.promise().mException)
            std::rethrow_exception(mHandle.promise().mException);
        mNextResume = mHandle.promise().mCondition.next_resume();
    }

    static ContinuationCondition resume_at(uint32_t millis);
    static ContinuationCondition resume_asap();
    static ContinuationCondition resume_in(uint32_t _millis);
};