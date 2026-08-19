#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

// Par for the course, the XAML timer class is "self-referential". Releasing all references
// to an instance will not stop the timer. Only calling Stop() explicitly will achieve that.
struct SafeDispatcherTimer
{
    SafeDispatcherTimer() = default;
    SafeDispatcherTimer(SafeDispatcherTimer const&) = delete;
    SafeDispatcherTimer& operator=(SafeDispatcherTimer const&) = delete;
    SafeDispatcherTimer(SafeDispatcherTimer&&) = delete;
    SafeDispatcherTimer& operator=(SafeDispatcherTimer&&) = delete;

    ~SafeDispatcherTimer()
    {
        Destroy();
    }

    explicit operator bool() const noexcept
    {
		fprintf(stderr, ">>> Entering bool()\n");
        fprintf(stderr, "<<< Exiting bool()\n");
        return _timer != nullptr;
    }

    winrt::Windows::Foundation::TimeSpan Interval()
    {
		fprintf(stderr, ">>> Entering Interval()\n");
        fprintf(stderr, "<<< Exiting Interval()\n");
        return _getTimer().Interval();
    }

    void Interval(winrt::Windows::Foundation::TimeSpan const& value)
    {
        _getTimer().Interval(value);
    }

    bool IsEnabled()
    {
		fprintf(stderr, ">>> Entering IsEnabled()\n");
        fprintf(stderr, "<<< Exiting IsEnabled()\n");
        return _timer && _timer.IsEnabled();
    }

    void Tick(winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const& handler)
    {
		fprintf(stderr, ">>> Entering Tick(handler=%d)\n", handler);
        auto& timer = _getTimer();
        if (_token)
        {
            timer.Tick(_token);
        }
        _token = timer.Tick(handler);
		fprintf(stderr, "Tick(handler=%d): _token → %d\n", handler, _token);
    fprintf(stderr, "Tick(handler=%d): _token → %d\n", handler, _token);
    fprintf(stderr, "<<< Exiting Tick(handler=%d)\n", handler);
    }

    void Start()
    {
		fprintf(stderr, ">>> Entering Start()\n");
        _getTimer().Start();
		fprintf(stderr, "<<< Exiting Start()\n");
    }

    void Stop() const
    {
		fprintf(stderr, ">>> Entering Stop()\n");
        if (_timer)
        {
            _timer.Stop();
        }
		fprintf(stderr, "<<< Exiting Stop()\n");
    }

    void Destroy()
    {
		fprintf(stderr, ">>> Entering Destroy()\n");
        if (!_timer)
        {
            return;
        }

        _timer.Stop();
        if (_token)
        {
            _timer.Tick(_token);
        }
        _timer = nullptr;
		fprintf(stderr, "Destroy(): _timer → %d\n", _timer);
        _token = {};
		fprintf(stderr, "Destroy(): _token → %d\n", _token);
    fprintf(stderr, "<<< Exiting Destroy()\n");
    }

private:
    ::winrt::Windows::UI::Xaml::DispatcherTimer& _getTimer()
    {
        if (!_timer)
        {
            _timer = ::winrt::Windows::UI::Xaml::DispatcherTimer{};
        }
        return _timer;
    }

    ::winrt::Windows::UI::Xaml::DispatcherTimer _timer{ nullptr };
    winrt::event_token _token;
};
