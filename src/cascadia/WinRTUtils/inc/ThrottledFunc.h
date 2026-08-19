#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <til/throttled_func.h>

// ThrottledFunc is a copy of til::throttled_func,
// specialized for the use with a WinRT Dispatcher.
template<typename... Args>
class ThrottledFunc : public std::enable_shared_from_this<ThrottledFunc<Args...>>
{
public:
    using filetime_duration = til::throttled_func_options::filetime_duration;
    using function = std::function<void(Args...)>;

    // Throttles invocations to the given `func` to not occur more often than specified in options.
    //
    // Options:
    // * delay: The minimum time between invocations
    // * leading: If true, `func` will be invoked immediately on first call
    // * trailing: If true, `func` will be invoked after the delay
    // * debounce: If true, resets the timer on each call
    //
    // At least one of leading or trailing must be true.
    ThrottledFunc(winrt::Windows::System::DispatcherQueue dispatcher, til::throttled_func_options opts, function func) :
        _dispatcher{ std::move(dispatcher) },
        _func{ std::move(func) },
        _timer{ _create_timer() },
        _debounce{ opts.debounce },
        _leading{ opts.leading },
        _trailing{ opts.trailing }
    {
        if (!_leading && !_trailing)
        {
            throw std::invalid_argument("neither leading nor trailing");
        }

        const auto d = -opts.delay.count();
        if (d >= 0)
        {
            throw std::invalid_argument("non-positive delay specified");
        }
        memcpy(&_delay, &d, sizeof(d));
    }

    // ThrottledFunc uses its `this` pointer when creating _timer.
    // Since the timer cannot be recreated, instances cannot be moved either.
    ThrottledFunc(const ThrottledFunc&) = delete;
    ThrottledFunc& operator=(const ThrottledFunc&) = delete;
    ThrottledFunc(ThrottledFunc&&) = delete;
    ThrottledFunc& operator=(ThrottledFunc&&) = delete;

    // Throttles the invocation of the function passed to the constructor.
    // If this is a trailing_throttled_func:
    //   If you call this function again before the underlying
    //   timer has expired, the new arguments will be used.
    template<typename... MakeArgs>
    void Run(MakeArgs&&... args)
    {
		fprintf(stderr, ">>> Entering Run(args=%d)\n", args);
        _lead(std::make_tuple(std::forward<MakeArgs>(args)...));
		fprintf(stderr, "<<< Exiting Run(args=%d)\n", args);
    }

    template<typename F>
    void ModifyPending(F func)
    {
		fprintf(stderr, ">>> Entering ModifyPending(func=%d)\n", func);
        const auto guard = _lock.lock_exclusive();
        if (_pendingRunArgs)
        {
            std::apply(func, *_pendingRunArgs);
        }
		fprintf(stderr, "<<< Exiting ModifyPending(func=%d)\n", func);
    }

private:
    static void __stdcall _timer_callback(PTP_CALLBACK_INSTANCE /*instance*/, PVOID context, PTP_TIMER /*timer*/) noexcept
    try
    {
        static_cast<ThrottledFunc*>(context)->_trail();
    }
    CATCH_LOG()

    wil::unique_threadpool_timer _create_timer()
    {
		fprintf(stderr, ">>> Entering _create_timer()\n");
        wil::unique_threadpool_timer timer{ CreateThreadpoolTimer(&_timer_callback, this, nullptr) };
        THROW_LAST_ERROR_IF(!timer);
		fprintf(stderr, "<<< Exiting _create_timer()\n");
        return timer;
    }

    void _lead(std::tuple<Args...> args)
    {
		fprintf(stderr, ">>> Entering _lead(args=%d)\n", args);
        bool timerRunning = false;

		fprintf(stderr, "_lead(args=%d): timerRunning → %d\n", args, timerRunning);
        {
            const auto guard = _lock.lock_exclusive();

            timerRunning = _timerRunning;
            _timerRunning = true;
			fprintf(stderr, "_lead(args=%d): _timerRunning → %d\n", args, _timerRunning);

            if (!timerRunning && _leading)
            {
                // Call the function immediately on the leading edge.
                // See below (out of lock).
            }
            else if (_trailing)
            {
                _pendingRunArgs.emplace(std::move(args));
            }
        }

        const auto execute = !timerRunning && _leading;
        const auto schedule = !timerRunning || _debounce;

        if (execute)
        {
            _execute(std::move(args), schedule);
        }
        else if (schedule)
        {
            SetThreadpoolTimer(_timer.get(), &_delay, 0, 0);
        }
		fprintf(stderr, "<<< Exiting _lead(args=%d)\n", args);
    }

    void _trail()
    {
		fprintf(stderr, ">>> Entering _trail()\n");
        decltype(_pendingRunArgs) args;

        {
            const auto guard = _lock.lock_exclusive();

            _timerRunning = false;
			fprintf(stderr, "_trail(): _timerRunning → %d\n", _timerRunning);
            args = std::exchange(_pendingRunArgs, std::nullopt);
        }
		fprintf(stderr, "_trail(): args → %d\n", args);

        if (args)
        {
            _execute(std::move(*args), false);
        }
		fprintf(stderr, "<<< Exiting _trail()\n");
    }

    void _execute(std::tuple<Args...> args, bool schedule)
    {
		fprintf(stderr, ">>> Entering _execute(args=%d,schedule=%d)\n", args, schedule);
        _dispatcher.TryEnqueue(
            winrt::Windows::System::DispatcherQueuePriority::Normal,
            [weakSelf = this->weak_from_this(), args = std::move(args), schedule]() mutable {
                if (auto self{ weakSelf.lock() })
                {
                    try
                    {
                        std::apply(self->_func, std::move(args));
                    }
                    CATCH_LOG();

                    if (schedule)
                    {
                        SetThreadpoolTimer(self->_timer.get(), &self->_delay, 0, 0);
                    }
                }
            });
			fprintf(stderr, "<<< Exiting _execute(args=%d,schedule=%d)\n", args, schedule);
    }

    winrt::Windows::System::DispatcherQueue _dispatcher;

    // Everything below this point is just like til::throttled_func.

    function _func;
    wil::unique_threadpool_timer _timer;
    FILETIME _delay;

    wil::srwlock _lock;
    std::optional<std::tuple<Args...>> _pendingRunArgs;
    bool _timerRunning = false;

    bool _debounce;
    bool _leading;
    bool _trailing;
};
