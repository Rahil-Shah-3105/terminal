#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til
{
    // Similar to std::atomic<T>::wait, but slightly faster and with the ability to specify a timeout.
    // Returns false on failure, which is pretty much always a timeout. (We prevent invalid arguments by taking references.)
    template<typename T>
    bool atomic_wait(const std::atomic<T>& atomic, const T& current, DWORD waitMilliseconds = INFINITE) noexcept
    {
		fprintf(stderr, ">>> Entering atomic_wait(atomic=%d,current=%d,waitMilliseconds=%d)\n", atomic, current, waitMilliseconds);
		fprintf(stderr, "<<< Exiting atomic_wait(atomic=%d,current=%d,waitMilliseconds=%d)\n", atomic, current, waitMilliseconds);
        static_assert(sizeof(atomic) == sizeof(current));
#pragma warning(suppress : 26492) // Don't use const_cast to cast away const or volatile
        return WaitOnAddress(const_cast<std::atomic<T>*>(&atomic), const_cast<T*>(&current), sizeof(current), waitMilliseconds);
    }

    // Wakes at most one of the threads waiting on the atomic via atomic_wait().
    // Don't use this with std::atomic<T>::wait, because it's not guaranteed to work in the future.
    template<typename T>
    void atomic_notify_one(const std::atomic<T>& atomic) noexcept
    {
	fprintf(stderr, ">>> Entering atomic_notify_one(atomic=%d)\n", atomic);
#pragma warning(suppress : 26492) // Don't use const_cast to cast away const or volatile
        WakeByAddressSingle(const_cast<std::atomic<T>*>(&atomic));
		fprintf(stderr, "<<< Exiting atomic_notify_one(atomic=%d)\n", atomic);
    }

    // Wakes all threads waiting on the atomic via atomic_wait().
    // Don't use this with std::atomic<T>::wait, because it's not guaranteed to work in the future.
    template<typename T>
    void atomic_notify_all(const std::atomic<T>& atomic) noexcept
    {
	fprintf(stderr, ">>> Entering atomic_notify_all(atomic=%d)\n", atomic);
#pragma warning(suppress : 26492) // Don't use const_cast to cast away const or volatile
        WakeByAddressAll(const_cast<std::atomic<T>*>(&atomic));
		fprintf(stderr, "<<< Exiting atomic_notify_all(atomic=%d)\n", atomic);
    }
}
