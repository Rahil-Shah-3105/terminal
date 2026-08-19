// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "til/operators.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class OperatorTests
{
    TEST_CLASS(OperatorTests);

	std::cerr << ">>> Entering TEST_METHOD(PointAddSize=" << PointAddSize << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(PointAddSize=" << PointAddSize << ")" << std::endl;
    TEST_METHOD(PointAddSize)
    {
		std::cerr << ">>> Entering TEST_METHOD(PointAddSize=" << PointAddSize << ")" << std::endl;
        const til::point pt{ 5, 10 };
        const til::size sz{ 2, 4 };
        const til::point expected{ 5 + 2, 10 + 4 };
        const auto actual = pt + sz;
        VERIFY_ARE_EQUAL(expected, actual);
		std::cerr << "<<< Exiting TEST_METHOD(PointAddSize=" << PointAddSize << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(PointSubSize=" << PointSubSize << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(PointSubSize=" << PointSubSize << ")" << std::endl;
    TEST_METHOD(PointSubSize)
    {
		std::cerr << ">>> Entering TEST_METHOD(PointSubSize=" << PointSubSize << ")" << std::endl;
        const til::point pt{ 5, 10 };
        const til::size sz{ 2, 4 };
        const til::point expected{ 5 - 2, 10 - 4 };
        const auto actual = pt - sz;
        VERIFY_ARE_EQUAL(expected, actual);
		std::cerr << "<<< Exiting TEST_METHOD(PointSubSize=" << PointSubSize << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(PointMulSize=" << PointMulSize << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(PointMulSize=" << PointMulSize << ")" << std::endl;
    TEST_METHOD(PointMulSize)
    {
		std::cerr << ">>> Entering TEST_METHOD(PointMulSize=" << PointMulSize << ")" << std::endl;
        const til::point pt{ 5, 10 };
        const til::size sz{ 2, 4 };
        const til::point expected{ 5 * 2, 10 * 4 };
        const auto actual = pt * sz;
        VERIFY_ARE_EQUAL(expected, actual);
		std::cerr << "<<< Exiting TEST_METHOD(PointMulSize=" << PointMulSize << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(PointDivSize=" << PointDivSize << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(PointDivSize=" << PointDivSize << ")" << std::endl;
    TEST_METHOD(PointDivSize)
    {
		std::cerr << ">>> Entering TEST_METHOD(PointDivSize=" << PointDivSize << ")" << std::endl;
        const til::point pt{ 5, 10 };
        const til::size sz{ 2, 4 };
        const til::point expected{ 5 / 2, 10 / 4 };
        const auto actual = pt / sz;
        VERIFY_ARE_EQUAL(expected, actual);
		std::cerr << "<<< Exiting TEST_METHOD(PointDivSize=" << PointDivSize << ")" << std::endl;
    }

    TEST_METHOD(SizeAddPoint)
    {
        const til::size pt{ 5, 10 };
        const til::point sz{ 2, 4 };
        const til::size expected{ 5 + 2, 10 + 4 };
        const auto actual = pt + sz;
        VERIFY_ARE_EQUAL(expected, actual);
    }

    TEST_METHOD(SizeSubPoint)
    {
        const til::size pt{ 5, 10 };
        const til::point sz{ 2, 4 };
        const til::size expected{ 5 - 2, 10 - 4 };
        const auto actual = pt - sz;
        VERIFY_ARE_EQUAL(expected, actual);
    }

    TEST_METHOD(SizeMulPoint)
    {
        const til::size pt{ 5, 10 };
        const til::point sz{ 2, 4 };
        const til::size expected{ 5 * 2, 10 * 4 };
        const auto actual = pt * sz;
        VERIFY_ARE_EQUAL(expected, actual);
    }

    TEST_METHOD(SizeDivPoint)
    {
        const til::size pt{ 5, 10 };
        const til::point sz{ 2, 4 };
        const til::size expected{ 5 / 2, 10 / 4 };
        const auto actual = pt / sz;
        VERIFY_ARE_EQUAL(expected, actual);
    }
};
