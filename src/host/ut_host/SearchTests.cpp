// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "CommonState.hpp"

#include "../buffer/out/search.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;
using Microsoft::Console::Interactivity::ServiceLocator;

class SearchTests
{
    TEST_CLASS(SearchTests);

    CommonState* m_state{ nullptr };

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
	std::cerr << "TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << "): m_state → " << m_state << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    {
        wil::unique_hmodule icu{ LoadLibraryExW(L"icu.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32) };
        if (!icu)
        {
            Log::Result(TestResults::Skipped, L"ICU is not present");
            return true;
        }

        m_state = new CommonState();
        m_state->PrepareGlobalScreenBuffer();
        return true;
    }

	std::cerr << ">>> Entering TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        if (m_state)
        {
            m_state->CleanupGlobalScreenBuffer();
            delete m_state;
        }
        return true;
    }

	std::cerr << ">>> Entering TEST_METHOD_SETUP(MethodSetup=" << MethodSetup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD_SETUP(MethodSetup=" << MethodSetup << ")" << std::endl;
    TEST_METHOD_SETUP(MethodSetup)
    {
        m_state->PrepareNewTextBufferInfo();
        m_state->FillTextBuffer();

        return true;
    }

	std::cerr << ">>> Entering TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        m_state->CleanupNewTextBufferInfo();
        Selection::Instance().ClearSelection();
        return true;
    }

    static void DoFoundChecks(Search& s, til::point coordStartExpected, til::CoordType lineDelta, bool reverse)
    {
		std::cerr << ">>> Entering DoFoundChecks(s=" << s << ",coordStartExpected=" << coordStartExpected << ",lineDelta=" << lineDelta << ",reverse=" << reverse << ")" << std::endl;
        const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        auto coordEndExpected = coordStartExpected;
        coordEndExpected.x += 2;

        VERIFY_IS_TRUE(s.SelectCurrent());
        VERIFY_ARE_EQUAL(coordStartExpected, gci.renderData.GetSelectionAnchor());
        VERIFY_ARE_EQUAL(coordEndExpected, gci.renderData.GetSelectionEnd());

        coordStartExpected.y += lineDelta;
        coordEndExpected.y += lineDelta;
        s.FindNext(reverse);

        VERIFY_IS_TRUE(s.SelectCurrent());
        VERIFY_ARE_EQUAL(coordStartExpected, gci.renderData.GetSelectionAnchor());
        VERIFY_ARE_EQUAL(coordEndExpected, gci.renderData.GetSelectionEnd());

        coordStartExpected.y += lineDelta;
        coordEndExpected.y += lineDelta;
        s.FindNext(reverse);

        VERIFY_IS_TRUE(s.SelectCurrent());
        VERIFY_ARE_EQUAL(coordStartExpected, gci.renderData.GetSelectionAnchor());
        VERIFY_ARE_EQUAL(coordEndExpected, gci.renderData.GetSelectionEnd());

        coordStartExpected.y += lineDelta;
        coordEndExpected.y += lineDelta;
        s.FindNext(reverse);

        VERIFY_IS_TRUE(s.SelectCurrent());
        VERIFY_ARE_EQUAL(coordStartExpected, gci.renderData.GetSelectionAnchor());
        VERIFY_ARE_EQUAL(coordEndExpected, gci.renderData.GetSelectionEnd());
		std::cerr << "<<< Exiting DoFoundChecks(s=" << s << ",coordStartExpected=" << coordStartExpected << ",lineDelta=" << lineDelta << ",reverse=" << reverse << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(ForwardCaseSensitive=" << ForwardCaseSensitive << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseSensitive=" << ForwardCaseSensitive << ")" << std::endl;
    TEST_METHOD(ForwardCaseSensitive)
    {
		std::cerr << ">>> Entering TEST_METHOD(ForwardCaseSensitive=" << ForwardCaseSensitive << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        Search s;
        s.Reset(gci.renderData, L"AB", SearchFlag::None, false);
        DoFoundChecks(s, {}, 1, false);
		std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseSensitive=" << ForwardCaseSensitive << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(ForwardCaseSensitiveJapanese=" << ForwardCaseSensitiveJapanese << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseSensitiveJapanese=" << ForwardCaseSensitiveJapanese << ")" << std::endl;
    TEST_METHOD(ForwardCaseSensitiveJapanese)
    {
		std::cerr << ">>> Entering TEST_METHOD(ForwardCaseSensitiveJapanese=" << ForwardCaseSensitiveJapanese << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"\x304b", SearchFlag::None, false);
        DoFoundChecks(s, { 2, 0 }, 1, false);
		std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseSensitiveJapanese=" << ForwardCaseSensitiveJapanese << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(ForwardCaseInsensitive=" << ForwardCaseInsensitive << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseInsensitive=" << ForwardCaseInsensitive << ")" << std::endl;
    TEST_METHOD(ForwardCaseInsensitive)
    {
		std::cerr << ">>> Entering TEST_METHOD(ForwardCaseInsensitive=" << ForwardCaseInsensitive << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        Search s;
        s.Reset(gci.renderData, L"ab", SearchFlag::CaseInsensitive, false);
        DoFoundChecks(s, {}, 1, false);
		std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseInsensitive=" << ForwardCaseInsensitive << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(ForwardCaseInsensitiveJapanese=" << ForwardCaseInsensitiveJapanese << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseInsensitiveJapanese=" << ForwardCaseInsensitiveJapanese << ")" << std::endl;
    TEST_METHOD(ForwardCaseInsensitiveJapanese)
    {
		std::cerr << ">>> Entering TEST_METHOD(ForwardCaseInsensitiveJapanese=" << ForwardCaseInsensitiveJapanese << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"\x304b", SearchFlag::CaseInsensitive, false);
        DoFoundChecks(s, { 2, 0 }, 1, false);
		std::cerr << "<<< Exiting TEST_METHOD(ForwardCaseInsensitiveJapanese=" << ForwardCaseInsensitiveJapanese << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(BackwardCaseSensitive=" << BackwardCaseSensitive << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(BackwardCaseSensitive=" << BackwardCaseSensitive << ")" << std::endl;
    TEST_METHOD(BackwardCaseSensitive)
    {
		std::cerr << ">>> Entering TEST_METHOD(BackwardCaseSensitive=" << BackwardCaseSensitive << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"AB", SearchFlag::None, true);
        DoFoundChecks(s, { 0, 3 }, -1, true);
		std::cerr << "<<< Exiting TEST_METHOD(BackwardCaseSensitive=" << BackwardCaseSensitive << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(BackwardCaseSensitiveJapanese=" << BackwardCaseSensitiveJapanese << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(BackwardCaseSensitiveJapanese=" << BackwardCaseSensitiveJapanese << ")" << std::endl;
    TEST_METHOD(BackwardCaseSensitiveJapanese)
    {
		std::cerr << ">>> Entering TEST_METHOD(BackwardCaseSensitiveJapanese=" << BackwardCaseSensitiveJapanese << ")" << std::endl;
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"\x304b", SearchFlag::None, true);
        DoFoundChecks(s, { 2, 3 }, -1, true);
		std::cerr << "<<< Exiting TEST_METHOD(BackwardCaseSensitiveJapanese=" << BackwardCaseSensitiveJapanese << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(BackwardCaseInsensitive=" << BackwardCaseInsensitive << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(BackwardCaseInsensitive=" << BackwardCaseInsensitive << ")" << std::endl;
    TEST_METHOD(BackwardCaseInsensitive)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"ab", SearchFlag::CaseInsensitive, true);
        DoFoundChecks(s, { 0, 3 }, -1, true);
    }

    TEST_METHOD(BackwardCaseInsensitiveJapanese)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        s.Reset(gci.renderData, L"\x304b", SearchFlag::CaseInsensitive, true);
        DoFoundChecks(s, { 2, 3 }, -1, true);
    }

    TEST_METHOD(ForwardCaseSensitiveRegex)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        Search s;
        s.Reset(gci.renderData, L"[BA]{2}", SearchFlag::RegularExpression, false);
        DoFoundChecks(s, {}, 1, false);
    }

    TEST_METHOD(ForwardCaseSensitiveRegexJapanese)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        // N.B. this is not a literal U+30xx, but a regex escape sequence \x{30xx}
        s.Reset(gci.renderData, LR"-([\x{3041}-\x{304c}])-", SearchFlag::RegularExpression, false);
        DoFoundChecks(s, { 2, 0 }, 1, false);
    }

    TEST_METHOD(ForwardCaseInsensitiveRegex)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        Search s;
        s.Reset(gci.renderData, L"ab", SearchFlag::CaseInsensitive | SearchFlag::RegularExpression, false);
        DoFoundChecks(s, {}, 1, false);
    }

    TEST_METHOD(ForwardCaseInsensitiveRegexJapanese)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        Search s;
        // N.B. this is not a literal U+30xx, but a regex escape sequence \x{30xx}
        s.Reset(gci.renderData, LR"-([\x{3041}-\x{304c}])-", SearchFlag::CaseInsensitive | SearchFlag::RegularExpression, false);
        DoFoundChecks(s, { 2, 0 }, 1, false);
    }

    TEST_METHOD(ForwardCaseSensitiveRegexWithCaseInsensitiveFlag)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

        Search s;
        s.Reset(gci.renderData, L"(?i)ab", SearchFlag::RegularExpression, false);
        DoFoundChecks(s, {}, 1, false);
    }
};
