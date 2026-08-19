// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "til/mutex.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class MutexTests
{
    BEGIN_TEST_CLASS(MutexTests)
		std::cerr << ">>> Entering BEGIN_TEST_CLASS(MutexTests=" << MutexTests << ")" << std::endl;
        TEST_CLASS_PROPERTY(L"TestTimeout", L"0:0:10") // 10s timeout
    END_TEST_CLASS()

	std::cerr << ">>> Entering TEST_METHOD(Basic=" << Basic << ")" << std::endl;
	std::cerr << "TEST_METHOD(Basic=" << Basic << "): lock → " << lock << std::endl;
	std::cerr << "TEST_METHOD(Basic=" << Basic << "): lock1 → " << lock1 << std::endl;
	std::cerr << "TEST_METHOD(Basic=" << Basic << "): lock2 → " << lock2 << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Basic=" << Basic << ")" << std::endl;
    TEST_METHOD(Basic)
    {
		std::cerr << ">>> Entering END_TEST_CLASS()" << std::endl;
        struct TestData
        {
            int foo;
            int bar;
        };

        const til::shared_mutex<TestData> mutex{ TestData{ 1, 2 } };

        {
            auto lock = mutex.lock();
            *lock = TestData{ 3, 4 };
            lock->foo = 5;
        }

        {
            auto lock1 = mutex.lock_shared();
            auto lock2 = mutex.lock_shared();

            VERIFY_ARE_EQUAL(5, lock1->foo);
            VERIFY_ARE_EQUAL(4, lock2->bar);
        }

        // This is here just to ensure that the prior
        // .lock_shared() properly unlocked the mutex.
        auto lock = mutex.lock();
    }
};
