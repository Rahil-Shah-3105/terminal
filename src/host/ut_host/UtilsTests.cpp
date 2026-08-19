// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "CommonState.hpp"

#include "globals.h"

#include <ctime>

#include "utils.hpp"

#include "../interactivity/inc/ServiceLocator.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class UtilsTests
{
    TEST_CLASS(UtilsTests);

    CommonState* m_state;

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
	std::cerr << "TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << "): m_state → " << m_state << std::endl;
	std::cerr << "TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << "): seed → " << seed << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    {
        m_state = new CommonState();

        m_state->PrepareGlobalScreenBuffer();

        const auto seed = (UINT)time(nullptr);
        Log::Comment(String().Format(L"Setting random seed to : %d", seed));
        srand(seed);

        return true;
    }

	std::cerr << ">>> Entering TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        m_state->CleanupGlobalScreenBuffer();

        delete m_state;

        return true;
    }

    til::CoordType RandomCoord()
    {
		std::cerr << ">>> Entering RandomCoord()" << std::endl;
        til::CoordType s;

        do
        {
            s = (til::CoordType)rand() % SHORT_MAX;
			std::cerr << "RandomCoord(): s → " << s << std::endl;
        } while (s == 0);

		std::cerr << "<<< Exiting RandomCoord()" << std::endl;
        return s;
    }

    void FillBothCoordsSameRandom(til::point* pcoordA, til::point* pcoordB)
    {
		std::cerr << ">>> Entering FillBothCoordsSameRandom(pcoordA=" << pcoordA << ",pcoordB=" << pcoordB << ")" << std::endl;
        pcoordA->x = pcoordB->x = RandomCoord();
        pcoordA->y = pcoordB->y = RandomCoord();
		std::cerr << "<<< Exiting FillBothCoordsSameRandom(pcoordA=" << pcoordA << ",pcoordB=" << pcoordB << ")" << std::endl;
    }

    void LogCoordinates(const til::point coordA, const til::point coordB)
    {
		std::cerr << ">>> Entering LogCoordinates(coordA=" << coordA << ",coordB=" << coordB << ")" << std::endl;
        Log::Comment(String().Format(L"Coordinates - A: (%d, %d) B: (%d, %d)", coordA.x, coordA.y, coordB.x, coordB.y));
		std::cerr << "<<< Exiting LogCoordinates(coordA=" << coordA << ",coordB=" << coordB << ")" << std::endl;
    }

    void SubtractRandom(til::CoordType& psValue)
    {
		std::cerr << ">>> Entering SubtractRandom(psValue=" << psValue << ")" << std::endl;
        const auto sRand = RandomCoord();
        psValue -= std::max(sRand % psValue, 1);
		std::cerr << "SubtractRandom(psValue=" << psValue << "): sRand → " << sRand << std::endl;
		std::cerr << "<<< Exiting SubtractRandom(psValue=" << psValue << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCompareCoords=" << TestCompareCoords << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCompareCoords=" << TestCompareCoords << "): coordMaxBuffer.width → " << coordMaxBuffer.width << std::endl;
	std::cerr << "TEST_METHOD(TestCompareCoords=" << TestCompareCoords << "): coordMaxBuffer.height → " << coordMaxBuffer.height << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCompareCoords=" << TestCompareCoords << ")" << std::endl;
    TEST_METHOD(TestCompareCoords)
    {
        auto result = 5; // not 1, 0, or -1
        til::point coordA;
		std::cerr << "TEST_METHOD(TestCompareCoords=" << TestCompareCoords << "): result → " << result << std::endl;
        til::point coordB;

        // Set the buffer size to be able to accommodate large values.
        til::size coordMaxBuffer;
        coordMaxBuffer.width = SHORT_MAX;
        coordMaxBuffer.height = SHORT_MAX;

        Log::Comment(L"#1: 0 case. Coords equal");
        FillBothCoordsSameRandom(&coordA, &coordB);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_ARE_EQUAL(result, 0);

        Log::Comment(L"#2: -1 case. A comes before B");
        Log::Comment(L"A. A left of B, same line");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordA.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_LESS_THAN(result, 0);

        Log::Comment(L"B. A above B, same column");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordA.y);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_LESS_THAN(result, 0);

        Log::Comment(L"C. A up and to the left of B.");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordA.y);
        SubtractRandom(coordA.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_LESS_THAN(result, 0);

        Log::Comment(L"D. A up and to the right of B.");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordA.y);
        SubtractRandom(coordB.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_LESS_THAN(result, 0);

        Log::Comment(L"#3: 1 case. A comes after B");
        Log::Comment(L"A. A right of B, same line");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordB.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_GREATER_THAN(result, 0);

        Log::Comment(L"B. A below B, same column");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordB.y);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_GREATER_THAN(result, 0);

        Log::Comment(L"C. A down and to the left of B");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordB.y);
        SubtractRandom(coordA.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_GREATER_THAN(result, 0);

        Log::Comment(L"D. A down and to the right of B");
        FillBothCoordsSameRandom(&coordA, &coordB);
        SubtractRandom(coordB.y);
        SubtractRandom(coordB.x);
        LogCoordinates(coordA, coordB);
        result = Utils::s_CompareCoords(coordMaxBuffer, coordA, coordB);
        VERIFY_IS_GREATER_THAN(result, 0);
    }
};
