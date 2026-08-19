// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class CoalesceTests
{
    TEST_CLASS(CoalesceTests);

    TEST_METHOD(CoalesceFirstValue);
    TEST_METHOD(CoalesceMiddleValue);
    TEST_METHOD(CoalesceDefaultValue);

    TEST_METHOD(CoalesceOrNotFirstValue);
    TEST_METHOD(CoalesceOrNotMiddleValue);
    TEST_METHOD(CoalesceOrNotDefaultValue);
    TEST_METHOD(CoalesceOrNotDefaultIsNullopt);
};

void CoalesceTests::CoalesceFirstValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceFirstValue()" << std::endl;
    auto result = til::coalesce_value(std::optional<int>(1),
                                      std::optional<int>(2),
                                      std::optional<int>(3),
                                      4);
    VERIFY_ARE_EQUAL(1, result);
	std::cerr << "<<< Exiting CoalesceTests::CoalesceFirstValue()" << std::endl;
}
void CoalesceTests::CoalesceMiddleValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceMiddleValue()" << std::endl;
    auto result = til::coalesce_value(std::optional<int>(std::nullopt),
                                      std::optional<int>(2),
                                      std::optional<int>(3),
                                      4);
    VERIFY_ARE_EQUAL(2, result);
	std::cerr << "<<< Exiting CoalesceTests::CoalesceMiddleValue()" << std::endl;
}
void CoalesceTests::CoalesceDefaultValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceDefaultValue()" << std::endl;
    auto result = til::coalesce_value(std::optional<int>(std::nullopt),
                                      std::optional<int>(std::nullopt),
                                      std::optional<int>(std::nullopt),
                                      4);
    VERIFY_ARE_EQUAL(4, result);
	std::cerr << "<<< Exiting CoalesceTests::CoalesceDefaultValue()" << std::endl;
}

void CoalesceTests::CoalesceOrNotFirstValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceOrNotFirstValue()" << std::endl;
    auto result = til::coalesce(std::optional<int>(1),
                                std::optional<int>(2),
                                std::optional<int>(3),
                                std::optional<int>(4));
    VERIFY_IS_TRUE(result.has_value());
    VERIFY_ARE_EQUAL(1, result.value());
	std::cerr << "<<< Exiting CoalesceTests::CoalesceOrNotFirstValue()" << std::endl;
}
void CoalesceTests::CoalesceOrNotMiddleValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceOrNotMiddleValue()" << std::endl;
    auto result = til::coalesce(std::optional<int>(std::nullopt),
                                std::optional<int>(2),
                                std::optional<int>(3),
                                std::optional<int>(4));
    VERIFY_IS_TRUE(result.has_value());
    VERIFY_ARE_EQUAL(2, result.value());
	std::cerr << "<<< Exiting CoalesceTests::CoalesceOrNotMiddleValue()" << std::endl;
}
void CoalesceTests::CoalesceOrNotDefaultValue()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceOrNotDefaultValue()" << std::endl;
    auto result = til::coalesce(std::optional<int>(std::nullopt),
                                std::optional<int>(std::nullopt),
                                std::optional<int>(std::nullopt),
                                std::optional<int>(4));
    VERIFY_IS_TRUE(result.has_value());
    VERIFY_ARE_EQUAL(4, result.value());
	std::cerr << "<<< Exiting CoalesceTests::CoalesceOrNotDefaultValue()" << std::endl;
}
void CoalesceTests::CoalesceOrNotDefaultIsNullopt()
{
	std::cerr << ">>> Entering CoalesceTests::CoalesceOrNotDefaultIsNullopt()" << std::endl;
    auto result = til::coalesce(std::optional<int>(std::nullopt),
                                std::optional<int>(std::nullopt),
                                std::optional<int>(std::nullopt),
                                std::optional<int>(std::nullopt));
    VERIFY_IS_FALSE(result.has_value());
	std::cerr << "<<< Exiting CoalesceTests::CoalesceOrNotDefaultIsNullopt()" << std::endl;
}
