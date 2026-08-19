// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "til/point.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

// Ensure the "safety" of til::point::as_win32_point
static_assert(
    sizeof(til::point) == sizeof(POINT) &&
    alignof(til::point) == alignof(POINT) &&
    offsetof(til::point, x) == offsetof(POINT, x) &&
    offsetof(til::point, y) == offsetof(POINT, y));

class PointTests
{
    TEST_CLASS(PointTests);

	std::cerr << ">>> Entering TEST_METHOD(DefaultConstruct=" << DefaultConstruct << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(DefaultConstruct=" << DefaultConstruct << ")" << std::endl;
    TEST_METHOD(DefaultConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(DefaultConstruct=" << DefaultConstruct << ")" << std::endl;
        const til::point pt;
        VERIFY_ARE_EQUAL(0, pt.x);
        VERIFY_ARE_EQUAL(0, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(DefaultConstruct=" << DefaultConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(RawConstruct=" << RawConstruct << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(RawConstruct=" << RawConstruct << ")" << std::endl;
    TEST_METHOD(RawConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(RawConstruct=" << RawConstruct << ")" << std::endl;
        const til::point pt{ 5, 10 };
        VERIFY_ARE_EQUAL(5, pt.x);
        VERIFY_ARE_EQUAL(10, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(RawConstruct=" << RawConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(RawFloatingConstruct=" << RawFloatingConstruct << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(RawFloatingConstruct=" << RawFloatingConstruct << ")" << std::endl;
    TEST_METHOD(RawFloatingConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(RawFloatingConstruct=" << RawFloatingConstruct << ")" << std::endl;
        const til::point pt{ til::math::rounding, 3.2f, 7.6f };
        VERIFY_ARE_EQUAL(3, pt.x);
        VERIFY_ARE_EQUAL(8, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(RawFloatingConstruct=" << RawFloatingConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(SignedConstruct=" << SignedConstruct << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(SignedConstruct=" << SignedConstruct << ")" << std::endl;
    TEST_METHOD(SignedConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(SignedConstruct=" << SignedConstruct << ")" << std::endl;
        const auto x = -5;
        const auto y = -10;

        const til::point pt{ x, y };
        VERIFY_ARE_EQUAL(x, pt.x);
        VERIFY_ARE_EQUAL(y, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(SignedConstruct=" << SignedConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(CoordConstruct=" << CoordConstruct << ")" << std::endl;
	std::cerr << "TEST_METHOD(CoordConstruct=" << CoordConstruct << "): pt → " << pt << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CoordConstruct=" << CoordConstruct << ")" << std::endl;
    TEST_METHOD(CoordConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(CoordConstruct=" << CoordConstruct << ")" << std::endl;
        COORD coord{ -5, 10 };

        const auto pt = til::wrap_coord(coord);
        VERIFY_ARE_EQUAL(coord.X, pt.x);
        VERIFY_ARE_EQUAL(coord.Y, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(CoordConstruct=" << CoordConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(PointConstruct=" << PointConstruct << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(PointConstruct=" << PointConstruct << ")" << std::endl;
    TEST_METHOD(PointConstruct)
    {
		std::cerr << ">>> Entering TEST_METHOD(PointConstruct=" << PointConstruct << ")" << std::endl;
        POINT point{ 5, -10 };

        const til::point pt{ point };
        VERIFY_ARE_EQUAL(point.x, pt.x);
        VERIFY_ARE_EQUAL(point.y, pt.y);
		std::cerr << "<<< Exiting TEST_METHOD(PointConstruct=" << PointConstruct << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(Equality=" << Equality << ")" << std::endl;
    TEST_METHOD(Equality)
    {
		std::cerr << ">>> Entering TEST_METHOD(Equality=" << Equality << ")" << std::endl;
        Log::Comment(L"Equal.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 == s2);
        }

        Log::Comment(L"Left Width changed.");
        {
            const til::point s1{ 4, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_FALSE(s1 == s2);
        }

        Log::Comment(L"Right Width changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 6, 10 };
            VERIFY_IS_FALSE(s1 == s2);
        }

        Log::Comment(L"Left Height changed.");
        {
            const til::point s1{ 5, 9 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_FALSE(s1 == s2);
        }

        Log::Comment(L"Right Height changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 11 };
            VERIFY_IS_FALSE(s1 == s2);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(Inequality=" << Inequality << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Equality=" << Equality << ")" << std::endl;
    TEST_METHOD(Inequality)
    {
		std::cerr << ">>> Entering TEST_METHOD(Inequality=" << Inequality << ")" << std::endl;
        Log::Comment(L"Equal.");
		std::cerr << "<<< Exiting TEST_METHOD(Equality=" << Equality << ")" << std::endl;
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_FALSE(s1 != s2);
        }

        Log::Comment(L"Left Width changed.");
        {
            const til::point s1{ 4, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 != s2);
        }

        Log::Comment(L"Right Width changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 6, 10 };
            VERIFY_IS_TRUE(s1 != s2);
        }

        Log::Comment(L"Left Height changed.");
        {
            const til::point s1{ 5, 9 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 != s2);
        }

        Log::Comment(L"Right Height changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 11 };
            VERIFY_IS_TRUE(s1 != s2);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(LessThanOrEqual=" << LessThanOrEqual << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Inequality=" << Inequality << ")" << std::endl;
    TEST_METHOD(LessThanOrEqual)
    {
		std::cerr << ">>> Entering TEST_METHOD(LessThanOrEqual=" << LessThanOrEqual << ")" << std::endl;
        Log::Comment(L"Equal.");
		std::cerr << "<<< Exiting TEST_METHOD(Inequality=" << Inequality << ")" << std::endl;
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 <= s2);
        }

        Log::Comment(L"Left Width changed.");
        {
            const til::point s1{ 4, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 <= s2);
        }

        Log::Comment(L"Right Width changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 6, 10 };
            VERIFY_IS_TRUE(s1 <= s2);
        }

        Log::Comment(L"Left Height changed.");
        {
            const til::point s1{ 5, 9 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 <= s2);
        }

        Log::Comment(L"Right Height changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 11 };
            VERIFY_IS_TRUE(s1 <= s2);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(GreaterThanOrEqual=" << GreaterThanOrEqual << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(LessThanOrEqual=" << LessThanOrEqual << ")" << std::endl;
    TEST_METHOD(GreaterThanOrEqual)
    {
		std::cerr << ">>> Entering TEST_METHOD(GreaterThanOrEqual=" << GreaterThanOrEqual << ")" << std::endl;
        Log::Comment(L"Equal.");
		std::cerr << "<<< Exiting TEST_METHOD(LessThanOrEqual=" << LessThanOrEqual << ")" << std::endl;
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_TRUE(s1 >= s2);
        }

        Log::Comment(L"Left Width changed.");
        {
            const til::point s1{ 4, 10 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_FALSE(s1 >= s2);
        }

        Log::Comment(L"Right Width changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 6, 10 };
            VERIFY_IS_FALSE(s1 >= s2);
        }

        Log::Comment(L"Left Height changed.");
        {
            const til::point s1{ 5, 9 };
            const til::point s2{ 5, 10 };
            VERIFY_IS_FALSE(s1 >= s2);
        }

        Log::Comment(L"Right Height changed.");
        {
            const til::point s1{ 5, 10 };
            const til::point s2{ 5, 11 };
            VERIFY_IS_FALSE(s1 >= s2);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(Boolean=" << Boolean << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(GreaterThanOrEqual=" << GreaterThanOrEqual << ")" << std::endl;
	std::cerr << "TEST_METHOD(Boolean=" << Boolean << "): actual → " << actual << std::endl;
    TEST_METHOD(Boolean)
    {
		std::cerr << ">>> Entering TEST_METHOD(Boolean=" << Boolean << ")" << std::endl;
        SetVerifyOutput verifyOutputScope{ VerifyOutputSettings::LogOnlyFailures };
		std::cerr << "<<< Exiting TEST_METHOD(GreaterThanOrEqual=" << GreaterThanOrEqual << ")" << std::endl;

        static constexpr til::CoordType values[] = { til::CoordTypeMin, -1, 0, 1, til::CoordTypeMax };

        for (const auto x : values)
        {
            for (const auto y : values)
            {
                const til::point p{ x, y };
                const auto expected = x >= 0 && y >= 0;
                const auto actual = static_cast<bool>(p);
                VERIFY_ARE_EQUAL(expected, actual);
            }
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(Addition=" << Addition << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Boolean=" << Boolean << ")" << std::endl;
	std::cerr << "TEST_METHOD(Addition=" << Addition << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(Addition)
    {
		std::cerr << ">>> Entering TEST_METHOD(Addition=" << Addition << ")" << std::endl;
        Log::Comment(L"Addition of two things that should be in bounds.");
		std::cerr << "<<< Exiting TEST_METHOD(Boolean=" << Boolean << ")" << std::endl;
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x + pt2.x, pt.y + pt2.y };

            VERIFY_ARE_EQUAL(expected, pt + pt2);
        }

        Log::Comment(L"Addition results in value that is too large (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                pt + pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Addition results in value that is too large (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                pt + pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(AdditionInplace=" << AdditionInplace << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Addition=" << Addition << ")" << std::endl;
	std::cerr << "TEST_METHOD(AdditionInplace=" << AdditionInplace << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(AdditionInplace)
    {
        Log::Comment(L"Addition of two things that should be in bounds.");
		std::cerr << "<<< Exiting TEST_METHOD(Addition=" << Addition << ")" << std::endl;
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x + pt2.x, pt.y + pt2.y };

            auto actual = pt;
            actual += pt2;

            VERIFY_ARE_EQUAL(expected, actual);
        }

        Log::Comment(L"Addition results in value that is too large (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                auto actual = pt;
                actual += pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Addition results in value that is too large (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                auto actual = pt;
                actual += pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "<<< Exiting TEST_METHOD(AdditionInplace=" << AdditionInplace << ")" << std::endl;
	std::cerr << "TEST_METHOD(Subtraction=" << Subtraction << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(Subtraction)
    {
        Log::Comment(L"Subtraction of two things that should be in bounds.");
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x - pt2.x, pt.y - pt2.y };

            VERIFY_ARE_EQUAL(expected, pt - pt2);
        }

        Log::Comment(L"Subtraction results in value that is too small (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ -2, -2 };

            auto fn = [&]() {
                pt2 - pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Subtraction results in value that is too small (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ -2, -2 };

            auto fn = [&]() {
                pt2 - pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "TEST_METHOD(SubtractionInplace=" << SubtractionInplace << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(SubtractionInplace)
    {
        Log::Comment(L"Subtraction of two things that should be in bounds.");
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x - pt2.x, pt.y - pt2.y };

            auto actual = pt;
            actual -= pt2;

            VERIFY_ARE_EQUAL(expected, actual);
        }

        Log::Comment(L"Subtraction results in value that is too small (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ -2, -2 };

            auto fn = [&]() {
                auto actual = pt2;
                actual -= pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Subtraction results in value that is too small (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ -2, -2 };

            auto fn = [&]() {
                auto actual = pt2;
                actual -= pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "TEST_METHOD(Multiplication=" << Multiplication << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(Multiplication)
    {
        Log::Comment(L"Multiplication of two things that should be in bounds.");
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x * pt2.x, pt.y * pt2.y };

            VERIFY_ARE_EQUAL(expected, pt * pt2);
        }

        Log::Comment(L"Multiplication results in value that is too large (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 10, 10 };

            auto fn = [&]() {
                pt* pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Multiplication results in value that is too large (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ 10, 10 };

            auto fn = [&]() {
                pt* pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "TEST_METHOD(MultiplicationInplace=" << MultiplicationInplace << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(MultiplicationInplace)
    {
        Log::Comment(L"Multiplication of two things that should be in bounds.");
        {
            const til::point pt{ 5, 10 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x * pt2.x, pt.y * pt2.y };

            auto actual = pt;
            actual *= pt2;

            VERIFY_ARE_EQUAL(expected, actual);
        }

        Log::Comment(L"Multiplication results in value that is too large (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 10, 10 };

            auto fn = [&]() {
                auto actual = pt;
                actual *= pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Multiplication results in value that is too large (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const til::point pt2{ 10, 10 };

            auto fn = [&]() {
                auto actual = pt;
                actual *= pt2;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "TEST_METHOD(Division=" << Division << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(Division)
    {
        Log::Comment(L"Division of two things that should be in bounds.");
        {
            const til::point pt{ 555, 510 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x / pt2.x, pt.y / pt2.y };

            VERIFY_ARE_EQUAL(expected, pt / pt2);
        }

        Log::Comment(L"Division by zero");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                pt2 / pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

	std::cerr << "TEST_METHOD(DivisionInplace=" << DivisionInplace << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(DivisionInplace)
    {
        Log::Comment(L"Division of two things that should be in bounds.");
        {
            const til::point pt{ 555, 510 };
            const til::point pt2{ 23, 47 };

            const til::point expected{ pt.x / pt2.x, pt.y / pt2.y };
            auto actual = pt;
            actual /= pt2;

            VERIFY_ARE_EQUAL(expected, actual);
        }

        Log::Comment(L"Division by zero");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const til::point pt2{ 1, 1 };

            auto fn = [&]() {
                auto actual = pt2;
                actual /= pt;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

    TEST_METHOD(XCast)
    {
        const til::point pt{ 5, 10 };
        VERIFY_ARE_EQUAL(static_cast<SHORT>(pt.x), pt.narrow_x<SHORT>());
    }

    TEST_METHOD(YCast)
    {
        const til::point pt{ 5, 10 };
        VERIFY_ARE_EQUAL(static_cast<SHORT>(pt.x), pt.narrow_x<SHORT>());
    }

	std::cerr << "TEST_METHOD(CastToPoint=" << CastToPoint << "): val → " << val << std::endl;
	std::cerr << "TEST_METHOD(CastToPoint=" << CastToPoint << "): y → " << y << std::endl;
	std::cerr << "TEST_METHOD(CastToPoint=" << CastToPoint << "): overflowExpected → " << overflowExpected << std::endl;
    TEST_METHOD(CastToPoint)
    {
        Log::Comment(L"Typical situation.");
        {
            const til::point pt{ 5, 10 };
            auto val = pt.to_win32_point();
            VERIFY_ARE_EQUAL(5, val.x);
            VERIFY_ARE_EQUAL(10, val.y);
        }

        Log::Comment(L"Fit max x into POINT (may overflow).");
        {
            constexpr auto x = std::numeric_limits<til::CoordType>().max();
            const auto y = 10;
            const til::point pt{ x, y };

            // On some platforms, til::CoordType will fit inside x/y
            const auto overflowExpected = x > std::numeric_limits<decltype(POINT::x)>().max();

            if (overflowExpected)
            {
                auto fn = [&]() {
                    auto val = pt.to_win32_point();
                };

                VERIFY_THROWS(fn(), gsl::narrowing_error);
            }
            else
            {
                auto val = pt.to_win32_point();
                VERIFY_ARE_EQUAL(x, val.x);
            }
        }

        Log::Comment(L"Fit max y into POINT (may overflow).");
        {
            constexpr auto y = std::numeric_limits<til::CoordType>().max();
            const auto x = 10;
            const til::point pt{ x, y };

            // On some platforms, til::CoordType will fit inside x/y
            const auto overflowExpected = y > std::numeric_limits<decltype(POINT::y)>().max();

            if (overflowExpected)
            {
                auto fn = [&]() {
                    auto val = pt.to_win32_point();
                };

                VERIFY_THROWS(fn(), gsl::narrowing_error);
            }
            else
            {
                auto val = pt.to_win32_point();
                VERIFY_ARE_EQUAL(y, val.y);
            }
        }
    }

	std::cerr << "TEST_METHOD(CastToD2D1Point2F=" << CastToD2D1Point2F << "): val → " << val << std::endl;
    TEST_METHOD(CastToD2D1Point2F)
    {
        Log::Comment(L"Typical situation.");
        {
            const til::point pt{ 5, 10 };
            auto val = pt.to_d2d_point();
            VERIFY_ARE_EQUAL(5, val.x);
            VERIFY_ARE_EQUAL(10, val.y);
        }

        // All til::CoordTypes fit into a float, so there's no exception tests.
    }

	std::cerr << "TEST_METHOD(Scaling=" << Scaling << "): bigSize → " << bigSize << std::endl;
    TEST_METHOD(Scaling)
    {
        Log::Comment(L"Multiplication of two things that should be in bounds.");
        {
            const til::point pt{ 5, 10 };
            const auto scale = 23;

            const til::point expected{ pt.x * scale, pt.y * scale };

            VERIFY_ARE_EQUAL(expected, pt * scale);
        }

        Log::Comment(L"Multiplication results in value that is too large (x).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ bigSize, static_cast<til::CoordType>(0) };
            const auto scale = 10;

            auto fn = [&]() {
                pt* scale;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Multiplication results in value that is too large (y).");
        {
            constexpr auto bigSize = std::numeric_limits<til::CoordType>().max();
            const til::point pt{ static_cast<til::CoordType>(0), bigSize };
            const auto scale = 10;

            auto fn = [&]() {
                pt* scale;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }

        Log::Comment(L"Division of two things that should be in bounds.");
        {
            const til::point pt{ 555, 510 };
            const auto scale = 23;

            const til::point expected{ pt.x / scale, pt.y / scale };

            VERIFY_ARE_EQUAL(expected, pt / scale);
        }

        Log::Comment(L"Division by zero");
        {
            const til::point pt{ 1, 1 };
            const auto scale = 0;

            auto fn = [&]() {
                pt / scale;
            };

            VERIFY_THROWS(fn(), gsl::narrowing_error);
        }
    }

    TEST_METHOD(CastFromFloatWithMathTypes)
    {
        Log::Comment(L"Ceiling");
        {
            {
                til::point converted{ til::math::ceiling, 1.f, 2.f };
                VERIFY_ARE_EQUAL((til::point{ 1, 2 }), converted);
            }
            {
                til::point converted{ til::math::ceiling, 1.6f, 2.4f };
                VERIFY_ARE_EQUAL((til::point{ 2, 3 }), converted);
            }
            {
                til::point converted{ til::math::ceiling, 3., 4. };
                VERIFY_ARE_EQUAL((til::point{ 3, 4 }), converted);
            }
            {
                til::point converted{ til::math::ceiling, 3.6, 4.4 };
                VERIFY_ARE_EQUAL((til::point{ 4, 5 }), converted);
            }
        }

        Log::Comment(L"Flooring");
        {
            {
                til::point converted{ til::math::flooring, 1.f, 2.f };
                VERIFY_ARE_EQUAL((til::point{ 1, 2 }), converted);
            }
            {
                til::point converted{ til::math::flooring, 1.6f, 2.4f };
                VERIFY_ARE_EQUAL((til::point{ 1, 2 }), converted);
            }
            {
                til::point converted{ til::math::flooring, 3., 4. };
                VERIFY_ARE_EQUAL((til::point{ 3, 4 }), converted);
            }
            {
                til::point converted{ til::math::flooring, 3.6, 4.4 };
                VERIFY_ARE_EQUAL((til::point{ 3, 4 }), converted);
            }
        }

        Log::Comment(L"Rounding");
        {
            {
                til::point converted{ til::math::rounding, 1.f, 2.f };
                VERIFY_ARE_EQUAL((til::point{ 1, 2 }), converted);
            }
            {
                til::point converted{ til::math::rounding, 1.6f, 2.4f };
                VERIFY_ARE_EQUAL((til::point{ 2, 2 }), converted);
            }
            {
                til::point converted{ til::math::rounding, 3., 4. };
                VERIFY_ARE_EQUAL((til::point{ 3, 4 }), converted);
            }
            {
                til::point converted{ til::math::rounding, 3.6, 4.4 };
                VERIFY_ARE_EQUAL((til::point{ 4, 4 }), converted);
            }
        }
    }
};
