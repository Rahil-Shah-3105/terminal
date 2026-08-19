// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include <wextestclass.h>
#include "../../inc/consoletaeftemplates.hpp"

#include "stateMachine.hpp"
#include "OutputStateMachineEngine.hpp"

#include "ascii.hpp"

using namespace Microsoft::Console::VirtualTerminal;

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace Microsoft
{
    namespace Console
    {
        namespace VirtualTerminal
        {
            class OutputEngineTest;
        }
    }
}

// From VT100.net...
// 9999-10000 is the classic boundary for most parsers parameter values.
// 16383-16384 is the boundary for DECSR commands according to EK-VT520-RM section 4.3.3.2
// 32767-32768 is our boundary SHORT_MAX for the Windows console
#define PARAM_VALUES L"{0, 1, 2, 1000, 9999, 10000, 16383, 16384, 32767, 32768, 50000, 999999999}"

class DummyDispatch final : public TermDispatch
{
public:
    virtual void Print(const wchar_t /*wchPrintable*/) override
    {
	std::cerr << ">>> Entering Print()" << std::endl;
	std::cerr << "<<< Exiting Print()" << std::endl;
	std::cerr << ">>> Entering Print(wchPrintable=" << wchPrintable << ")" << std::endl;
	std::cerr << "<<< Exiting Print(wchPrintable=" << wchPrintable << ")" << std::endl;
    }

    virtual void PrintString(const std::wstring_view /*string*/) override
    {
	std::cerr << ">>> Entering PrintString()" << std::endl;
	std::cerr << "<<< Exiting PrintString()" << std::endl;
	std::cerr << ">>> Entering PrintString(string=" << string << ")" << std::endl;
	std::cerr << "<<< Exiting PrintString(string=" << string << ")" << std::endl;
    }
};

class Microsoft::Console::VirtualTerminal::OutputEngineTest final
{
    TEST_CLASS(OutputEngineTest);

	std::cerr << ">>> Entering TEST_METHOD(TestEscapePath=" << TestEscapePath << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiCursorMovementWithoutValues=" << TestCsiCursorMovementWithoutValues << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestAnsiMode=" << TestAnsiMode << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestMultipleModes=" << TestMultipleModes << ")" << std::endl;
    TEST_METHOD(TestEscapePath)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestEscapePath=" << TestEscapePath << ")" << std::endl;
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:uiTest", L"{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17}") // one value for each type of state test below.
        END_TEST_METHOD_PROPERTIES()

		std::cerr << ">>> Entering END_TEST_METHOD_PROPERTIES()" << std::endl;
        size_t uiTest;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiTest", uiTest));
        auto dispatch = std::make_unique<DummyDispatch>();
		std::cerr << "END_TEST_METHOD_PROPERTIES(): dispatch → " << dispatch << std::endl;
		std::cerr << "END_TEST_METHOD_PROPERTIES(): engine → " << engine << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        auto expectedEscapeState = StateMachine::VTStates::Escape;
		std::cerr << "END_TEST_METHOD_PROPERTIES(): expectedEscapeState → " << expectedEscapeState << std::endl;

        switch (uiTest)
        {
        case 0:
        {
            Log::Comment(L"Escape from Ground.");
            mach._state = StateMachine::VTStates::Ground;
            break;
        }
		std::cerr << "END_TEST_METHOD_PROPERTIES(): mach._state → " << mach._state << std::endl;
        case 1:
        {
            Log::Comment(L"Escape from Escape.");
            mach._state = StateMachine::VTStates::Escape;
            break;
        }
        case 2:
        {
            Log::Comment(L"Escape from Escape Intermediate");
            mach._state = StateMachine::VTStates::EscapeIntermediate;
            break;
        }
        case 3:
        {
            Log::Comment(L"Escape from CsiEntry");
            mach._state = StateMachine::VTStates::CsiEntry;
            break;
        }
        case 4:
        {
            Log::Comment(L"Escape from CsiIgnore");
            mach._state = StateMachine::VTStates::CsiIgnore;
            break;
        }
        case 5:
        {
            Log::Comment(L"Escape from CsiParam");
            mach._state = StateMachine::VTStates::CsiParam;
            break;
        }
        case 6:
        {
            Log::Comment(L"Escape from CsiIntermediate");
            mach._state = StateMachine::VTStates::CsiIntermediate;
            break;
        }
        // The OscParam and OscString states shouldn't escape out after an ESC.
        // They enter the OscTermination state to wait for the `\` char of the
        // string terminator, without which the OSC operation won't be executed.
        case 7:
        {
            Log::Comment(L"Escape from OscParam");
            mach._state = StateMachine::VTStates::OscParam;
            expectedEscapeState = StateMachine::VTStates::OscTermination;
            break;
        }
        case 8:
        {
            Log::Comment(L"Escape from OscString");
            mach._state = StateMachine::VTStates::OscString;
            expectedEscapeState = StateMachine::VTStates::OscTermination;
            break;
        }
        case 9:
        {
            Log::Comment(L"Escape from OscTermination");
            mach._state = StateMachine::VTStates::OscTermination;
            break;
        }
        case 10:
        {
            Log::Comment(L"Escape from Ss3Entry");
            mach._state = StateMachine::VTStates::Ss3Entry;
            break;
        }
        case 11:
        {
            Log::Comment(L"Escape from Ss3Param");
            mach._state = StateMachine::VTStates::Ss3Param;
            break;
        }
        case 12:
        {
            Log::Comment(L"Escape from DcsEntry");
            mach._state = StateMachine::VTStates::DcsEntry;
            break;
        }
        case 13:
        {
            Log::Comment(L"Escape from DcsIgnore");
            mach._state = StateMachine::VTStates::DcsIgnore;
            break;
        }
        case 14:
        {
            Log::Comment(L"Escape from DcsIntermediate");
            mach._state = StateMachine::VTStates::DcsIntermediate;
            break;
        }
        case 15:
        {
            Log::Comment(L"Escape from DcsParam");
            mach._state = StateMachine::VTStates::DcsParam;
            break;
        }
        case 16:
        {
            Log::Comment(L"Escape from DcsPassThrough");
            mach._state = StateMachine::VTStates::DcsPassThrough;
            mach._dcsStringHandler = [](const auto) { return true; };
            break;
        }
        case 17:
        {
            Log::Comment(L"Escape from SosPmApcString");
            mach._state = StateMachine::VTStates::SosPmApcString;
            break;
        }
		std::cerr << "<<< Exiting END_TEST_METHOD_PROPERTIES()" << std::endl;
        }

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(expectedEscapeState, mach._state);
		std::cerr << "<<< Exiting TEST_METHOD(TestCsiCursorMovementWithoutValues=" << TestCsiCursorMovementWithoutValues << ")" << std::endl;
		std::cerr << ">>> Entering TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << ")" << std::endl;
    }

	std::cerr << "<<< Exiting TEST_METHOD(TestOscColorTableReset=" << TestOscColorTableReset << ")" << std::endl;
	std::cerr << ">>> Entering TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << "): engine → " << engine << std::endl;
    TEST_METHOD(TestEscapeImmediatePath)
    {
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::EscapeIntermediate);
        mach.ProcessCharacter(L'(');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::EscapeIntermediate);
        mach.ProcessCharacter(L')');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::EscapeIntermediate);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::EscapeIntermediate);
        mach.ProcessCharacter(L'6');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << ")" << std::endl;
    TEST_METHOD(TestEscapeThenC0Path)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestEscapeImmediatePath=" << TestEscapeImmediatePath << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        // When we see a C0 control char in the escape state, the Output engine
        // should execute it, without interrupting the sequence it's currently
        // processing
        mach.ProcessCharacter(L'\x03');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
		std::cerr << "<<< Exiting TEST_METHOD(TestEscapeThenC0Path=" << TestEscapeThenC0Path << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(TestGroundPrint=" << TestGroundPrint << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestGroundPrint=" << TestGroundPrint << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestGroundPrint=" << TestGroundPrint << ")" << std::endl;
    TEST_METHOD(TestGroundPrint)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestGroundPrint=" << TestGroundPrint << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestGroundPrint=" << TestGroundPrint << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(L'a');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
		std::cerr << "<<< Exiting TEST_METHOD(TestGroundPrint=" << TestGroundPrint << ")" << std::endl;
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiEntry=" << TestCsiEntry << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiEntry=" << TestCsiEntry << "): engine → " << engine << std::endl;
    TEST_METHOD(TestCsiEntry)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiEntry=" << TestCsiEntry << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiEntry=" << TestCsiEntry << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiEntry=" << TestCsiEntry << ")" << std::endl;
    TEST_METHOD(TestC1CsiEntry)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiEntry=" << TestCsiEntry << ")" << std::endl;
        // Enable the acceptance of C1 control codes in the state machine.
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(L'\x9b');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << ")" << std::endl;
    TEST_METHOD(TestCsiImmediate)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestC1CsiEntry=" << TestC1CsiEntry << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'$');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIntermediate);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIntermediate);
        mach.ProcessCharacter(L'%');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIntermediate);
        mach.ProcessCharacter(L'v');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiParam=" << TestCsiParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiParam=" << TestCsiParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << ")" << std::endl;
    TEST_METHOD(TestCsiParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiParam=" << TestCsiParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiParam=" << TestCsiParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiImmediate=" << TestCsiImmediate << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'2');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'8');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        VERIFY_ARE_EQUAL(mach._parameters.size(), 4u);
        VERIFY_IS_FALSE(mach._parameters.at(0).has_value());
        VERIFY_ARE_EQUAL(mach._parameters.at(1), 324);
        VERIFY_IS_FALSE(mach._parameters.at(2).has_value());
        VERIFY_ARE_EQUAL(mach._parameters.at(3), 8);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiParam=" << TestCsiParam << ")" << std::endl;
    TEST_METHOD(TestCsiMaxParamCount)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiParam=" << TestCsiParam << ")" << std::endl;
        Log::Comment(L"Output a sequence with 100 parameters");
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        for (size_t i = 0; i < 100; i++)
        {
            if (i > 0)
            {
                mach.ProcessCharacter(L';');
                VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
            }
            mach.ProcessCharacter(L'0' + i % 10);
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        }
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        Log::Comment(L"Only MAX_PARAMETER_COUNT (32) parameters should be stored");
        VERIFY_ARE_EQUAL(mach._parameters.size(), MAX_PARAMETER_COUNT);
        for (size_t i = 0; i < MAX_PARAMETER_COUNT; i++)
        {
            VERIFY_IS_TRUE(mach._parameters.at(i).has_value());
            VERIFY_ARE_EQUAL(mach._parameters.at(i).value(), gsl::narrow_cast<VTInt>(i % 10));
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << ")" << std::endl;
    TEST_METHOD(TestLeadingZeroCsiParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiMaxParamCount=" << TestCsiMaxParamCount << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        for (auto i = 0; i < 50; i++) // Any number of leading zeros should be supported
        {
            mach.ProcessCharacter(L'0');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        }
        for (auto i = 0; i < 5; i++) // We're only expecting to be able to keep 5 digits max
        {
            mach.ProcessCharacter((wchar_t)(L'1' + i));
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        }
        VERIFY_ARE_EQUAL(mach._parameters.back(), 12345);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << ")" << std::endl;
    TEST_METHOD(TestCsiSubParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroCsiParam=" << TestLeadingZeroCsiParam << ")" << std::endl;
        // "\e[:3;9:5::8J"
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'9');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L'5');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L'8');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        VERIFY_ARE_EQUAL(mach._parameters.size(), 2u);
        VERIFY_IS_FALSE(mach._parameters.at(0).has_value());
        VERIFY_ARE_EQUAL(mach._parameters.at(1), 9);

        VERIFY_ARE_EQUAL(mach._subParameters.size(), 4u);
        VERIFY_ARE_EQUAL(mach._subParameters.at(0), 3);
        VERIFY_ARE_EQUAL(mach._subParameters.at(1), 5);
        VERIFY_IS_FALSE(mach._subParameters.at(2).has_value());
        VERIFY_ARE_EQUAL(mach._subParameters.at(3), 8);

        VERIFY_ARE_EQUAL(mach._subParameterRanges.at(0).first, 0);
        VERIFY_ARE_EQUAL(mach._subParameterRanges.at(0).second, 1);
        VERIFY_ARE_EQUAL(mach._subParameterRanges.at(1).first, 1);
        VERIFY_ARE_EQUAL(mach._subParameterRanges.at(1).second, 4);

        VERIFY_ARE_EQUAL(mach._subParameterRanges.size(), mach._parameters.size());
        VERIFY_IS_TRUE(
            (mach._subParameterRanges.back().second == mach._subParameters.size() - 1) // lastIndex
            || (mach._subParameterRanges.back().second == mach._subParameters.size()) // or lastIndex + 1
        );
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << "): firstRange → " << firstRange << std::endl;
	std::cerr << "TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << "): secondRange → " << secondRange << std::endl;
    TEST_METHOD(TestCsiMaxSubParamCount)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiSubParam=" << TestCsiSubParam << ")" << std::endl;
        Log::Comment(L"Output two parameters with 100 sub parameters each");
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        for (size_t nParam = 0; nParam < 2; nParam++)
        {
            mach.ProcessCharacter(L'3');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
            for (size_t i = 0; i < 100; i++)
            {
                mach.ProcessCharacter(L':');
                VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
                mach.ProcessCharacter(L'0' + i % 10);
                VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
            }
            Log::Comment(L"Receiving 100 sub parameters should set the overflow flag");
            VERIFY_IS_TRUE(mach._subParameterLimitOverflowed);
            mach.ProcessCharacter(L';');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
            VERIFY_IS_FALSE(mach._subParameterLimitOverflowed);
        }
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        Log::Comment(L"Only MAX_SUBPARAMETER_COUNT (6) sub parameters should be stored for each parameter");
        VERIFY_ARE_EQUAL(mach._subParameters.size(), 12u);

        // Verify that first 6 sub parameters are stored for each parameter.
        // subParameters = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5 }
        for (size_t i = 0; i < 12; i++)
        {
            VERIFY_IS_TRUE(mach._subParameters.at(i).has_value());
            VERIFY_ARE_EQUAL(mach._subParameters.at(i).value(), gsl::narrow_cast<VTInt>(i % 6));
        }

        auto firstRange = mach._subParameterRanges.at(0);
        auto secondRange = mach._subParameterRanges.at(1);
        VERIFY_ARE_EQUAL(firstRange.first, 0);
        VERIFY_ARE_EQUAL(firstRange.second, 6);
        VERIFY_ARE_EQUAL(secondRange.first, 6);
        VERIFY_ARE_EQUAL(secondRange.second, 12);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << ")" << std::endl;
    TEST_METHOD(TestLeadingZeroCsiSubParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiMaxSubParamCount=" << TestCsiMaxSubParamCount << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        for (auto i = 0; i < 50; i++) // Any number of leading zeros should be supported
        {
            mach.ProcessCharacter(L'0');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        }
        for (auto i = 0; i < 5; i++) // We're only expecting to be able to keep 5 digits max
        {
            mach.ProcessCharacter((wchar_t)(L'1' + i));
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiSubParam);
        }
        VERIFY_ARE_EQUAL(mach._subParameters.back(), 12345);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << ")" << std::endl;
    TEST_METHOD(TestCsiIgnore)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroCsiSubParam=" << TestLeadingZeroCsiSubParam << ")" << std::endl;
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'=');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIgnore);
        mach.ProcessCharacter(L'8');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIgnore);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'[');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIntermediate);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIgnore);
        mach.ProcessCharacter(L'8');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiIgnore);
        mach.ProcessCharacter(L'J');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestC1Osc=" << TestC1Osc << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestC1Osc=" << TestC1Osc << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << ")" << std::endl;
    TEST_METHOD(TestC1Osc)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestC1Osc=" << TestC1Osc << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestC1Osc=" << TestC1Osc << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestCsiIgnore=" << TestCsiIgnore << ")" << std::endl;
        // Enable the acceptance of C1 control codes in the state machine.
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(L'\x9d');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestC1Osc=" << TestC1Osc << ")" << std::endl;
    TEST_METHOD(TestOscStringSimple)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestC1Osc=" << TestC1Osc << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L'0');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'o');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'e');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L' ');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L't');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'e');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'x');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L't');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L'0');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'o');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'e');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L' ');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L't');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'e');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'x');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L't');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscTermination);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestLongOscString=" << TestLongOscString << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLongOscString=" << TestLongOscString << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << ")" << std::endl;
    TEST_METHOD(TestLongOscString)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestLongOscString=" << TestLongOscString << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestLongOscString=" << TestLongOscString << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestOscStringSimple=" << TestOscStringSimple << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L'0');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L';');
        for (auto i = 0; i < 260u; i++) // The buffer is only 256 long, so any longer value should work :P
        {
            mach.ProcessCharacter(L's');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        }
        VERIFY_ARE_EQUAL(mach._oscString.size(), 260u);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestLongOscString=" << TestLongOscString << ")" << std::endl;
    TEST_METHOD(NormalTestOscParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestLongOscString=" << TestLongOscString << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        for (auto i = 0; i < 5; i++) // We're only expecting to be able to keep 5 digits max
        {
            mach.ProcessCharacter((wchar_t)(L'1' + i));
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        }
        VERIFY_ARE_EQUAL(mach._oscParameter, 12345);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << ")" << std::endl;
    TEST_METHOD(TestLeadingZeroOscParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(NormalTestOscParam=" << NormalTestOscParam << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        for (auto i = 0; i < 50; i++) // Any number of leading zeros should be supported
        {
            mach.ProcessCharacter(L'0');
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        }
        for (auto i = 0; i < 5; i++) // We're only expecting to be able to keep 5 digits max
        {
            mach.ProcessCharacter((wchar_t)(L'1' + i));
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        }
        VERIFY_ARE_EQUAL(mach._oscParameter, 12345);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestLongOscParam=" << TestLongOscParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLongOscParam=" << TestLongOscParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestLongOscParam=" << TestLongOscParam << "): sizeMax → " << sizeMax << std::endl;
	std::cerr << "TEST_METHOD(TestLongOscParam=" << TestLongOscParam << "): sizeMaxStr → " << sizeMaxStr << std::endl;
    TEST_METHOD(TestLongOscParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestLongOscParam=" << TestLongOscParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestLongOscParam=" << TestLongOscParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestLeadingZeroOscParam=" << TestLeadingZeroOscParam << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        constexpr auto sizeMax = std::numeric_limits<size_t>::max();
        const auto sizeMaxStr = wil::str_printf<std::wstring>(L"%zu", sizeMax);
        for (auto& wch : sizeMaxStr)
        {
            mach.ProcessCharacter(wch);
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        }
        VERIFY_ARE_EQUAL(mach._oscParameter, MAX_PARAMETER_VALUE);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        for (const auto& wch : sizeMaxStr)
        {
            mach.ProcessCharacter(wch);
            VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        }
        VERIFY_ARE_EQUAL(mach._oscParameter, MAX_PARAMETER_VALUE);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::BEL);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestLongOscParam=" << TestLongOscParam << ")" << std::endl;
    TEST_METHOD(TestOscStringInvalidTermination)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestLongOscParam=" << TestLongOscParam << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscTermination);
        mach.ProcessCharacter(L'['); // This is not a string terminator.
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsEntry=" << TestDcsEntry << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsEntry=" << TestDcsEntry << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << ")" << std::endl;
    TEST_METHOD(TestDcsEntry)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsEntry=" << TestDcsEntry << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsEntry=" << TestDcsEntry << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestOscStringInvalidTermination=" << TestOscStringInvalidTermination << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsEntry=" << TestDcsEntry << ")" << std::endl;
    TEST_METHOD(TestC1DcsEntry)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsEntry=" << TestDcsEntry << ")" << std::endl;
        // Enable the acceptance of C1 control codes in the state machine.
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);

        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(L'\x90');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << ")" << std::endl;
    TEST_METHOD(TestDcsImmediate)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestC1DcsEntry=" << TestC1DcsEntry << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L' ');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIntermediate);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIntermediate);
        mach.ProcessCharacter(L'%');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIntermediate);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << ")" << std::endl;
    TEST_METHOD(TestDcsIgnore)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsImmediate=" << TestDcsImmediate << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L':');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsParam=" << TestDcsParam << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsParam=" << TestDcsParam << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << ")" << std::endl;
    TEST_METHOD(TestDcsParam)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsParam=" << TestDcsParam << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsParam=" << TestDcsParam << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsIgnore=" << TestDcsIgnore << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L'2');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);
        mach.ProcessCharacter(L'8');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsParam);

        VERIFY_ARE_EQUAL(mach._parameters.size(), 4u);
        VERIFY_IS_FALSE(mach._parameters.at(0).has_value());
        VERIFY_ARE_EQUAL(mach._parameters.at(1), 324);
        VERIFY_IS_FALSE(mach._parameters.at(2).has_value());
        VERIFY_ARE_EQUAL(mach._parameters.at(3), 8);

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsParam=" << TestDcsParam << ")" << std::endl;
    TEST_METHOD(TestDcsIntermediateAndPassThrough)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsParam=" << TestDcsParam << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L' ');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIntermediate);
        mach.ProcessCharacter(L'x');
        // Note that without a dispatcher the pass through data is instead ignored.
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << ")" << std::endl;
    TEST_METHOD(TestDcsLongStringPassThrough)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsIntermediateAndPassThrough=" << TestDcsIntermediateAndPassThrough << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L'q');
        // Note that without a dispatcher the pass through state is instead ignored.
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'N');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'N');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'N');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << ")" << std::endl;
    TEST_METHOD(TestDcsInvalidTermination)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsLongStringPassThrough=" << TestDcsLongStringPassThrough << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L'q');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'['); // This is not a string terminator.
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiEntry);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::CsiParam);
        mach.ProcessCharacter(L'm');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << ")" << std::endl;
    TEST_METHOD(TestSosPmApcString)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestDcsInvalidTermination=" << TestDcsInvalidTermination << ")" << std::endl;
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'X');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'2');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'^');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'4');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'_');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'5');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'6');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'\\');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestC1StringTerminator=" << TestC1StringTerminator << ")" << std::endl;
	std::cerr << "TEST_METHOD(TestC1StringTerminator=" << TestC1StringTerminator << "): engine → " << engine << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(TestC1StringTerminator=" << TestC1StringTerminator << ")" << std::endl;
    TEST_METHOD(TestC1StringTerminator)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestC1StringTerminator=" << TestC1StringTerminator << ")" << std::endl;
        auto dispatch = std::make_unique<DummyDispatch>();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
		std::cerr << "TEST_METHOD(TestC1StringTerminator=" << TestC1StringTerminator << "): dispatch → " << dispatch << std::endl;
        StateMachine mach(std::move(engine));

		std::cerr << "<<< Exiting TEST_METHOD(TestSosPmApcString=" << TestSosPmApcString << ")" << std::endl;
        // Enable the acceptance of C1 control codes in the state machine.
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);

        // C1 ST should terminate OSC string.
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L']');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscParam);
        mach.ProcessCharacter(L';');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L's');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::OscString);
        mach.ProcessCharacter(L'\x9c');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        // C1 ST should terminate DCS passthrough string.
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'P');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsEntry);
        mach.ProcessCharacter(L'q');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'#');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::DcsIgnore);
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'\x9c');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        // C1 ST should terminate SOS/PM/APC string.
        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'X');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'1');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'\x9c');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'^');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'2');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'\x9c');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);

        mach.ProcessCharacter(AsciiChars::ESC);
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Escape);
        mach.ProcessCharacter(L'_');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'3');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::SosPmApcString);
        mach.ProcessCharacter(L'\x9c');
        VERIFY_ARE_EQUAL(mach._state, StateMachine::VTStates::Ground);
    }
};

class StatefulDispatch final : public TermDispatch
{
public:
    virtual void Print(const wchar_t wchPrintable) override
    {
        _printString += wchPrintable;
    }

    virtual void PrintString(const std::wstring_view string) override
    {
        _printString += string;
    }

    StatefulDispatch() :
        _cursorDistance{ 0 },
        _line{ 0 },
        _column{ 0 },
        _cursorUp{ false },
        _cursorDown{ false },
        _cursorBackward{ false },
        _cursorForward{ false },
        _cursorNextLine{ false },
        _cursorPreviousLine{ false },
        _cursorHorizontalPositionAbsolute{ false },
        _verticalLinePositionAbsolute{ false },
        _horizontalPositionRelative{ false },
        _verticalPositionRelative{ false },
        _cursorPosition{ false },
        _cursorSave{ false },
        _cursorLoad{ false },
        _eraseDisplay{ false },
        _eraseLine{ false },
        _insertCharacter{ false },
        _deleteCharacter{ false },
        _eraseType{ (DispatchTypes::EraseType)-1 },
        _eraseTypes{},
        _setGraphics{ false },
        _statusReportType{ (DispatchTypes::StatusType)-1 },
        _deviceStatusReport{ false },
        _deviceAttributes{ false },
        _secondaryDeviceAttributes{ false },
        _tertiaryDeviceAttributes{ false },
        _vt52DeviceAttributes{ false },
        _requestTerminalParameters{ false },
        _reportingPermission{ (DispatchTypes::ReportingPermission)-1 },
        _modeType{ (DispatchTypes::ModeParams)-1 },
        _modeTypes{},
        _modeEnabled{ false },
        _warningBell{ false },
        _carriageReturn{ false },
        _lineFeed{ false },
        _lineFeedType{ (DispatchTypes::LineFeedType)-1 },
        _reverseLineFeed{ false },
        _setWindowTitle{ false },
        _forwardTab{ false },
        _numTabs{ 0 },
        _tabClear{ false },
        _tabClearTypes{},
        _xtermResourcesChanged{},
        _xtermResourceValues{},
        _hyperlinkMode{ false },
        _options{ s_cMaxOptions, static_cast<DispatchTypes::GraphicsOptions>(s_uiGraphicsCleared) }, // fill with cleared option
        _colorTable{},
        _setColorTableEntry{ false },
        _resetAllColors{ false }
    {
    }

    void ClearState()
    {
		std::cerr << ">>> Entering ClearState()" << std::endl;
        StatefulDispatch dispatch;
        *this = dispatch;
		std::cerr << "<<< Exiting ClearState()" << std::endl;
    }

    void CursorUp(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorUp(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorUp = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorUp(uiDistance=" << uiDistance << "): _cursorUp → " << _cursorUp << std::endl;
		std::cerr << "CursorUp(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting CursorUp(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorDown(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorDown(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorDown = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorDown(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "CursorDown(uiDistance=" << uiDistance << "): _cursorDown → " << _cursorDown << std::endl;
		std::cerr << "<<< Exiting CursorDown(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorBackward(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorBackward(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorBackward = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorBackward(uiDistance=" << uiDistance << "): _cursorBackward → " << _cursorBackward << std::endl;
		std::cerr << "CursorBackward(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting CursorBackward(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorForward(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorForward(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorForward = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorForward(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "CursorForward(uiDistance=" << uiDistance << "): _cursorForward → " << _cursorForward << std::endl;
		std::cerr << "<<< Exiting CursorForward(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorNextLine(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorNextLine(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorNextLine = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorNextLine(uiDistance=" << uiDistance << "): _cursorNextLine → " << _cursorNextLine << std::endl;
		std::cerr << "CursorNextLine(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting CursorNextLine(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorPrevLine(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering CursorPrevLine(uiDistance=" << uiDistance << ")" << std::endl;
        _cursorPreviousLine = true;
        _cursorDistance = uiDistance;
		std::cerr << "CursorPrevLine(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "CursorPrevLine(uiDistance=" << uiDistance << "): _cursorPreviousLine → " << _cursorPreviousLine << std::endl;
		std::cerr << "<<< Exiting CursorPrevLine(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorHorizontalPositionAbsolute(const VTInt uiPosition) noexcept override
    {
		std::cerr << ">>> Entering CursorHorizontalPositionAbsolute(uiPosition=" << uiPosition << ")" << std::endl;
        _cursorHorizontalPositionAbsolute = true;
        _cursorDistance = uiPosition;
		std::cerr << "CursorHorizontalPositionAbsolute(uiPosition=" << uiPosition << "): _cursorHorizontalPositionAbsolute → " << _cursorHorizontalPositionAbsolute << std::endl;
		std::cerr << "CursorHorizontalPositionAbsolute(uiPosition=" << uiPosition << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting CursorHorizontalPositionAbsolute(uiPosition=" << uiPosition << ")" << std::endl;
    }

    void VerticalLinePositionAbsolute(const VTInt uiPosition) noexcept override
    {
		std::cerr << ">>> Entering VerticalLinePositionAbsolute(uiPosition=" << uiPosition << ")" << std::endl;
        _verticalLinePositionAbsolute = true;
        _cursorDistance = uiPosition;
		std::cerr << "VerticalLinePositionAbsolute(uiPosition=" << uiPosition << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "VerticalLinePositionAbsolute(uiPosition=" << uiPosition << "): _verticalLinePositionAbsolute → " << _verticalLinePositionAbsolute << std::endl;
		std::cerr << "<<< Exiting VerticalLinePositionAbsolute(uiPosition=" << uiPosition << ")" << std::endl;
    }

    void HorizontalPositionRelative(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering HorizontalPositionRelative(uiDistance=" << uiDistance << ")" << std::endl;
        _horizontalPositionRelative = true;
        _cursorDistance = uiDistance;
		std::cerr << "HorizontalPositionRelative(uiDistance=" << uiDistance << "): _horizontalPositionRelative → " << _horizontalPositionRelative << std::endl;
		std::cerr << "HorizontalPositionRelative(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting HorizontalPositionRelative(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void VerticalPositionRelative(const VTInt uiDistance) noexcept override
    {
		std::cerr << ">>> Entering VerticalPositionRelative(uiDistance=" << uiDistance << ")" << std::endl;
        _verticalPositionRelative = true;
        _cursorDistance = uiDistance;
		std::cerr << "VerticalPositionRelative(uiDistance=" << uiDistance << "): _verticalPositionRelative → " << _verticalPositionRelative << std::endl;
		std::cerr << "VerticalPositionRelative(uiDistance=" << uiDistance << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting VerticalPositionRelative(uiDistance=" << uiDistance << ")" << std::endl;
    }

    void CursorPosition(const VTInt uiLine, const VTInt uiColumn) noexcept override
    {
		std::cerr << ">>> Entering CursorPosition(uiLine=" << uiLine << ",uiColumn=" << uiColumn << ")" << std::endl;
        _cursorPosition = true;
        _line = uiLine;
		std::cerr << "CursorPosition(uiLine=" << uiLine << ",uiColumn=" << uiColumn << "): _line → " << _line << std::endl;
		std::cerr << "CursorPosition(uiLine=" << uiLine << ",uiColumn=" << uiColumn << "): _column → " << _column << std::endl;
		std::cerr << "CursorPosition(uiLine=" << uiLine << ",uiColumn=" << uiColumn << "): _cursorPosition → " << _cursorPosition << std::endl;
        _column = uiColumn;
		std::cerr << "<<< Exiting CursorPosition(uiLine=" << uiLine << ",uiColumn=" << uiColumn << ")" << std::endl;
    }

    void CursorSaveState() noexcept override
    {
		std::cerr << ">>> Entering CursorSaveState()" << std::endl;
		std::cerr << "CursorSaveState(): _cursorSave → " << _cursorSave << std::endl;
        _cursorSave = true;
		std::cerr << "<<< Exiting CursorSaveState()" << std::endl;
    }

    void CursorRestoreState() noexcept override
    {
		std::cerr << ">>> Entering CursorRestoreState()" << std::endl;
		std::cerr << "CursorRestoreState(): _cursorLoad → " << _cursorLoad << std::endl;
        _cursorLoad = true;
		std::cerr << "<<< Exiting CursorRestoreState()" << std::endl;
    }

    void EraseInDisplay(const DispatchTypes::EraseType eraseType) noexcept override
    {
		std::cerr << ">>> Entering EraseInDisplay(eraseType=" << eraseType << ")" << std::endl;
        _eraseDisplay = true;
        _eraseType = eraseType;
		std::cerr << "EraseInDisplay(eraseType=" << eraseType << "): _eraseType → " << _eraseType << std::endl;
		std::cerr << "EraseInDisplay(eraseType=" << eraseType << "): _eraseDisplay → " << _eraseDisplay << std::endl;
        _eraseTypes.push_back(eraseType);
		std::cerr << "<<< Exiting EraseInDisplay(eraseType=" << eraseType << ")" << std::endl;
    }

    void EraseInLine(const DispatchTypes::EraseType eraseType) noexcept override
    {
		std::cerr << ">>> Entering EraseInLine(eraseType=" << eraseType << ")" << std::endl;
        _eraseLine = true;
        _eraseType = eraseType;
		std::cerr << "EraseInLine(eraseType=" << eraseType << "): _eraseType → " << _eraseType << std::endl;
		std::cerr << "EraseInLine(eraseType=" << eraseType << "): _eraseLine → " << _eraseLine << std::endl;
        _eraseTypes.push_back(eraseType);
		std::cerr << "<<< Exiting EraseInLine(eraseType=" << eraseType << ")" << std::endl;
    }

    void InsertCharacter(const VTInt uiCount) noexcept override
    {
		std::cerr << ">>> Entering InsertCharacter(uiCount=" << uiCount << ")" << std::endl;
        _insertCharacter = true;
        _cursorDistance = uiCount;
		std::cerr << "InsertCharacter(uiCount=" << uiCount << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "InsertCharacter(uiCount=" << uiCount << "): _insertCharacter → " << _insertCharacter << std::endl;
		std::cerr << "<<< Exiting InsertCharacter(uiCount=" << uiCount << ")" << std::endl;
    }

    void DeleteCharacter(const VTInt uiCount) noexcept override
    {
		std::cerr << ">>> Entering DeleteCharacter(uiCount=" << uiCount << ")" << std::endl;
        _deleteCharacter = true;
        _cursorDistance = uiCount;
		std::cerr << "DeleteCharacter(uiCount=" << uiCount << "): _deleteCharacter → " << _deleteCharacter << std::endl;
		std::cerr << "DeleteCharacter(uiCount=" << uiCount << "): _cursorDistance → " << _cursorDistance << std::endl;
		std::cerr << "<<< Exiting DeleteCharacter(uiCount=" << uiCount << ")" << std::endl;
    }

    void SetGraphicsRendition(const VTParameters options) noexcept override
    {
		std::cerr << ">>> Entering SetGraphicsRendition(options=" << options << ")" << std::endl;
        _options.clear();
        for (size_t i = 0; i < options.size(); i++)
        {
            _options.push_back(options.at(i));
        }
        _setGraphics = true;
		std::cerr << "SetGraphicsRendition(options=" << options << "): _setGraphics → " << _setGraphics << std::endl;
		std::cerr << "<<< Exiting SetGraphicsRendition(options=" << options << ")" << std::endl;
    }

    void DeviceStatusReport(const DispatchTypes::StatusType statusType, const VTParameter /*id*/) noexcept override
    {
		std::cerr << ">>> Entering DeviceStatusReport(statusType=" << statusType << ")" << std::endl;
        _deviceStatusReport = true;
        _statusReportType = statusType;
		std::cerr << "DeviceStatusReport(statusType=" << statusType << "): _deviceStatusReport → " << _deviceStatusReport << std::endl;
		std::cerr << "DeviceStatusReport(statusType=" << statusType << "): _statusReportType → " << _statusReportType << std::endl;
		std::cerr << "<<< Exiting DeviceStatusReport(statusType=" << statusType << ")" << std::endl;
    }

    void DeviceAttributes() noexcept override
    {
		std::cerr << ">>> Entering DeviceAttributes()" << std::endl;
		std::cerr << "DeviceAttributes(): _deviceAttributes → " << _deviceAttributes << std::endl;
        _deviceAttributes = true;
		std::cerr << "<<< Exiting DeviceAttributes()" << std::endl;
    }

    void SecondaryDeviceAttributes() noexcept override
    {
		std::cerr << ">>> Entering SecondaryDeviceAttributes()" << std::endl;
		std::cerr << "SecondaryDeviceAttributes(): _secondaryDeviceAttributes → " << _secondaryDeviceAttributes << std::endl;
        _secondaryDeviceAttributes = true;
		std::cerr << "<<< Exiting SecondaryDeviceAttributes()" << std::endl;
    }

    void TertiaryDeviceAttributes() noexcept override
    {
		std::cerr << ">>> Entering TertiaryDeviceAttributes()" << std::endl;
		std::cerr << "TertiaryDeviceAttributes(): _tertiaryDeviceAttributes → " << _tertiaryDeviceAttributes << std::endl;
        _tertiaryDeviceAttributes = true;
		std::cerr << "<<< Exiting TertiaryDeviceAttributes()" << std::endl;
    }

    void Vt52DeviceAttributes() noexcept override
    {
		std::cerr << ">>> Entering Vt52DeviceAttributes()" << std::endl;
		std::cerr << "Vt52DeviceAttributes(): _vt52DeviceAttributes → " << _vt52DeviceAttributes << std::endl;
        _vt52DeviceAttributes = true;
		std::cerr << "<<< Exiting Vt52DeviceAttributes()" << std::endl;
    }

    void RequestTerminalParameters(const DispatchTypes::ReportingPermission permission) noexcept override
    {
		std::cerr << ">>> Entering RequestTerminalParameters(permission=" << permission << ")" << std::endl;
        _requestTerminalParameters = true;
        _reportingPermission = permission;
		std::cerr << "RequestTerminalParameters(permission=" << permission << "): _reportingPermission → " << _reportingPermission << std::endl;
		std::cerr << "RequestTerminalParameters(permission=" << permission << "): _requestTerminalParameters → " << _requestTerminalParameters << std::endl;
		std::cerr << "<<< Exiting RequestTerminalParameters(permission=" << permission << ")" << std::endl;
    }

    void _ModeParamsHelper(_In_ DispatchTypes::ModeParams const param, const bool fEnable)
    {
		std::cerr << ">>> Entering _ModeParamsHelper(param=" << param << ",fEnable=" << fEnable << ")" << std::endl;
        _modeType = param;
        _modeTypes.push_back(param);
		std::cerr << "_ModeParamsHelper(param=" << param << ",fEnable=" << fEnable << "): _modeType → " << _modeType << std::endl;
		std::cerr << "_ModeParamsHelper(param=" << param << ",fEnable=" << fEnable << "): _modeEnabled → " << _modeEnabled << std::endl;
        _modeEnabled = fEnable;
		std::cerr << "<<< Exiting _ModeParamsHelper(param=" << param << ",fEnable=" << fEnable << ")" << std::endl;
    }

    void SetMode(const DispatchTypes::ModeParams param) noexcept override
    {
		std::cerr << ">>> Entering SetMode(param=" << param << ")" << std::endl;
        _ModeParamsHelper(param, true);
		std::cerr << "<<< Exiting SetMode(param=" << param << ")" << std::endl;
    }

    void ResetMode(const DispatchTypes::ModeParams param) noexcept override
    {
		std::cerr << ">>> Entering ResetMode(param=" << param << ")" << std::endl;
        _ModeParamsHelper(param, false);
		std::cerr << "<<< Exiting ResetMode(param=" << param << ")" << std::endl;
    }

    void SetAnsiMode(const bool ansiMode) noexcept override
    {
		std::cerr << ">>> Entering SetAnsiMode(ansiMode=" << ansiMode << ")" << std::endl;
        _ModeParamsHelper(DispatchTypes::DECANM_AnsiMode, ansiMode);
		std::cerr << "<<< Exiting SetAnsiMode(ansiMode=" << ansiMode << ")" << std::endl;
    }

    void WarningBell() noexcept override
    {
		std::cerr << ">>> Entering WarningBell()" << std::endl;
		std::cerr << "WarningBell(): _warningBell → " << _warningBell << std::endl;
        _warningBell = true;
		std::cerr << "<<< Exiting WarningBell()" << std::endl;
    }

    void CarriageReturn() noexcept override
    {
		std::cerr << ">>> Entering CarriageReturn()" << std::endl;
		std::cerr << "CarriageReturn(): _carriageReturn → " << _carriageReturn << std::endl;
        _carriageReturn = true;
		std::cerr << "<<< Exiting CarriageReturn()" << std::endl;
    }

    void LineFeed(const DispatchTypes::LineFeedType lineFeedType) noexcept override
    {
		std::cerr << ">>> Entering LineFeed(lineFeedType=" << lineFeedType << ")" << std::endl;
        _lineFeed = true;
        _lineFeedType = lineFeedType;
		std::cerr << "LineFeed(lineFeedType=" << lineFeedType << "): _lineFeed → " << _lineFeed << std::endl;
		std::cerr << "LineFeed(lineFeedType=" << lineFeedType << "): _lineFeedType → " << _lineFeedType << std::endl;
		std::cerr << "<<< Exiting LineFeed(lineFeedType=" << lineFeedType << ")" << std::endl;
    }

    void ReverseLineFeed() noexcept override
    {
		std::cerr << ">>> Entering ReverseLineFeed()" << std::endl;
		std::cerr << "ReverseLineFeed(): _reverseLineFeed → " << _reverseLineFeed << std::endl;
        _reverseLineFeed = true;
		std::cerr << "<<< Exiting ReverseLineFeed()" << std::endl;
    }

    void SetWindowTitle(std::wstring_view title) override
    {
		std::cerr << ">>> Entering SetWindowTitle(title=" << title << ")" << std::endl;
        _setWindowTitle = true;
        _setWindowTitleText = title;
		std::cerr << "SetWindowTitle(title=" << title << "): _setWindowTitleText → " << _setWindowTitleText << std::endl;
		std::cerr << "SetWindowTitle(title=" << title << "): _setWindowTitle → " << _setWindowTitle << std::endl;
		std::cerr << "<<< Exiting SetWindowTitle(title=" << title << ")" << std::endl;
    }

    void ForwardTab(const VTInt numTabs) noexcept override
    {
		std::cerr << ">>> Entering ForwardTab(numTabs=" << numTabs << ")" << std::endl;
        _forwardTab = true;
        _numTabs = numTabs;
		std::cerr << "ForwardTab(numTabs=" << numTabs << "): _forwardTab → " << _forwardTab << std::endl;
		std::cerr << "ForwardTab(numTabs=" << numTabs << "): _numTabs → " << _numTabs << std::endl;
		std::cerr << "<<< Exiting ForwardTab(numTabs=" << numTabs << ")" << std::endl;
    }

    void TabClear(const DispatchTypes::TabClearType clearType) noexcept override
    {
		std::cerr << ">>> Entering TabClear(clearType=" << clearType << ")" << std::endl;
        _tabClear = true;
        _tabClearTypes.push_back(clearType);
		std::cerr << "TabClear(clearType=" << clearType << "): _tabClear → " << _tabClear << std::endl;
		std::cerr << "<<< Exiting TabClear(clearType=" << clearType << ")" << std::endl;
    }

    void SetColorTableEntry(const size_t tableIndex, const COLORREF color) noexcept override
    {
		std::cerr << ">>> Entering SetColorTableEntry(tableIndex=" << tableIndex << ",color=" << color << ")" << std::endl;
        _setColorTableEntry = true;
        _colorTable.at(tableIndex) = color;
		std::cerr << "SetColorTableEntry(tableIndex=" << tableIndex << ",color=" << color << "): _setColorTableEntry → " << _setColorTableEntry << std::endl;
		std::cerr << "<<< Exiting SetColorTableEntry(tableIndex=" << tableIndex << ",color=" << color << ")" << std::endl;
    }

    void RequestColorTableEntry(const size_t tableIndex) noexcept override
    {
		std::cerr << ">>> Entering RequestColorTableEntry(tableIndex=" << tableIndex << ")" << std::endl;
        _colorTableEntriesRequested.push_back(tableIndex);
		std::cerr << "<<< Exiting RequestColorTableEntry(tableIndex=" << tableIndex << ")" << std::endl;
    }

    void ResetColorTable() noexcept override
    {
		std::cerr << ">>> Entering ResetColorTable()" << std::endl;
		std::cerr << "ResetColorTable(): _resetAllColors → " << _resetAllColors << std::endl;
        _resetAllColors = true;
		std::cerr << "<<< Exiting ResetColorTable()" << std::endl;
    }

    void ResetColorTableEntry(const size_t tableIndex) noexcept override
    {
		std::cerr << ">>> Entering ResetColorTableEntry(tableIndex=" << tableIndex << ")" << std::endl;
        _colorTableEntriesReset.push_back(tableIndex);
		std::cerr << "<<< Exiting ResetColorTableEntry(tableIndex=" << tableIndex << ")" << std::endl;
    }

    void SetXtermColorResource(const size_t resource, const DWORD color) override
    {
		std::cerr << ">>> Entering SetXtermColorResource(resource=" << resource << ",color=" << color << ")" << std::endl;
        _xtermResourcesChanged.push_back(resource);
        _xtermResourceValues.push_back(color);
		std::cerr << "<<< Exiting SetXtermColorResource(resource=" << resource << ",color=" << color << ")" << std::endl;
    }

    void RequestXtermColorResource(const size_t resource) override
    {
		std::cerr << ">>> Entering RequestXtermColorResource(resource=" << resource << ")" << std::endl;
        _xtermResourcesRequested.push_back(resource);
		std::cerr << "<<< Exiting RequestXtermColorResource(resource=" << resource << ")" << std::endl;
    }

    void ResetXtermColorResource(const size_t resource) override
    {
		std::cerr << ">>> Entering ResetXtermColorResource(resource=" << resource << ")" << std::endl;
        _xtermResourcesReset.push_back(resource);
		std::cerr << "<<< Exiting ResetXtermColorResource(resource=" << resource << ")" << std::endl;
    }

    void SetClipboard(wil::zwstring_view content) noexcept override
    {
		std::cerr << ">>> Entering SetClipboard(content=" << content << ")" << std::endl;
		std::cerr << "SetClipboard(content=" << content << "): _copyContent → " << _copyContent << std::endl;
        _copyContent = content;
		std::cerr << "<<< Exiting SetClipboard(content=" << content << ")" << std::endl;
    }

    void AddHyperlink(std::wstring_view uri, std::wstring_view params) noexcept override
    {
		std::cerr << ">>> Entering AddHyperlink(uri=" << uri << ",params=" << params << ")" << std::endl;
        _hyperlinkMode = true;
        _uri = uri;
		std::cerr << "AddHyperlink(uri=" << uri << ",params=" << params << "): _uri → " << _uri << std::endl;
		std::cerr << "AddHyperlink(uri=" << uri << ",params=" << params << "): _hyperlinkMode → " << _hyperlinkMode << std::endl;
        if (!params.empty())
        {
            _customId = params;
			std::cerr << "AddHyperlink(uri=" << uri << ",params=" << params << "): _customId → " << _customId << std::endl;
        }
		std::cerr << "<<< Exiting AddHyperlink(uri=" << uri << ",params=" << params << ")" << std::endl;
    }

    void EndHyperlink() noexcept override
    {
		std::cerr << ">>> Entering EndHyperlink()" << std::endl;
        _hyperlinkMode = false;
        _uri.clear();
		std::cerr << "EndHyperlink(): _hyperlinkMode → " << _hyperlinkMode << std::endl;
        _customId.clear();
		std::cerr << "<<< Exiting EndHyperlink()" << std::endl;
    }

    void DoConEmuAction(const std::wstring_view /*string*/) noexcept override
    {
	std::cerr << ">>> Entering DoConEmuAction()" << std::endl;
	std::cerr << "<<< Exiting DoConEmuAction()" << std::endl;
    }

    std::wstring _printString;
    size_t _cursorDistance;
    size_t _line;
    size_t _column;
    bool _cursorUp;
    bool _cursorDown;
    bool _cursorBackward;
    bool _cursorForward;
    bool _cursorNextLine;
    bool _cursorPreviousLine;
    bool _cursorHorizontalPositionAbsolute;
    bool _verticalLinePositionAbsolute;
    bool _horizontalPositionRelative;
    bool _verticalPositionRelative;
    bool _cursorPosition;
    bool _cursorSave;
    bool _cursorLoad;
    bool _eraseDisplay;
    bool _eraseLine;
    bool _insertCharacter;
    bool _deleteCharacter;
    DispatchTypes::EraseType _eraseType;
    std::vector<DispatchTypes::EraseType> _eraseTypes;
    bool _setGraphics;
    DispatchTypes::StatusType _statusReportType;
    bool _deviceStatusReport;
    bool _deviceAttributes;
    bool _secondaryDeviceAttributes;
    bool _tertiaryDeviceAttributes;
    bool _vt52DeviceAttributes;
    bool _requestTerminalParameters;
    DispatchTypes::ReportingPermission _reportingPermission;
    DispatchTypes::ModeParams _modeType;
    std::vector<DispatchTypes::ModeParams> _modeTypes;
    bool _modeEnabled;
    bool _warningBell;
    bool _carriageReturn;
    bool _lineFeed;
    DispatchTypes::LineFeedType _lineFeedType;
    bool _reverseLineFeed;
    bool _setWindowTitle;
    std::wstring _setWindowTitleText;
    bool _forwardTab;
    size_t _numTabs;
    bool _tabClear;
    std::vector<DispatchTypes::TabClearType> _tabClearTypes;
    std::vector<size_t> _xtermResourcesChanged;
    std::vector<DWORD> _xtermResourceValues;
    std::vector<size_t> _xtermResourcesRequested;
    std::vector<size_t> _xtermResourcesReset;
    bool _setColorTableEntry;
    std::vector<size_t> _colorTableEntriesRequested;
    bool _resetAllColors;
    std::vector<size_t> _colorTableEntriesReset;
    bool _hyperlinkMode;
    std::wstring _copyContent;
    std::wstring _uri;
    std::wstring _customId;

    static const size_t s_cMaxOptions = 16;
    static const size_t s_uiGraphicsCleared = UINT_MAX;
    static const size_t XTERM_COLOR_TABLE_SIZE = 256;
    std::vector<DispatchTypes::GraphicsOptions> _options;
    std::array<COLORREF, XTERM_COLOR_TABLE_SIZE> _colorTable;
};

class StateMachineExternalTest final
{
    TEST_CLASS(StateMachineExternalTest);

    void InsertNumberToMachine(StateMachine* const pMachine, size_t number)
    {
		std::cerr << ">>> Entering InsertNumberToMachine(pMachine=" << pMachine << ",number=" << number << ")" << std::endl;
        static const size_t cchBufferMax = 20;

        wchar_t pwszDistance[cchBufferMax];
        auto cchDistance = swprintf_s(pwszDistance, cchBufferMax, L"%zu", number);
		std::cerr << "InsertNumberToMachine(pMachine=" << pMachine << ",number=" << number << "): cchDistance → " << cchDistance << std::endl;

        if (cchDistance > 0 && cchDistance < cchBufferMax)
        {
            for (auto i = 0; i < cchDistance; i++)
            {
                pMachine->ProcessCharacter(pwszDistance[i]);
            }
        }
		std::cerr << "<<< Exiting InsertNumberToMachine(pMachine=" << pMachine << ",number=" << number << ")" << std::endl;
    }

    void ApplyParameterBoundary(size_t* uiExpected, size_t uiGiven)
    {
		std::cerr << ">>> Entering ApplyParameterBoundary(uiExpected=" << uiExpected << ",uiGiven=" << uiGiven << ")" << std::endl;
        // 0 and 1 should be 1. Use the preset value.
        // 1-MAX_PARAMETER_VALUE should be what we set.
        // > MAX_PARAMETER_VALUE should be MAX_PARAMETER_VALUE.
        if (uiGiven <= 1)
        {
            *uiExpected = 1u;
        }
        else if (uiGiven > 1 && uiGiven <= MAX_PARAMETER_VALUE)
        {
            *uiExpected = uiGiven;
        }
        else if (uiGiven > MAX_PARAMETER_VALUE)
        {
            *uiExpected = MAX_PARAMETER_VALUE;
        }
		std::cerr << "<<< Exiting ApplyParameterBoundary(uiExpected=" << uiExpected << ",uiGiven=" << uiGiven << ")" << std::endl;
    }

    void TestCsiCursorMovement(const wchar_t wchCommand,
                               const size_t uiDistance,
                               const bool fUseDistance,
                               const bool fAddExtraParam,
                               const bool* const pfFlag,
                               StateMachine& mach,
                               StatefulDispatch& dispatch)
    {
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');

        if (fUseDistance)
        {
            InsertNumberToMachine(&mach, uiDistance);

            // Extraneous parameters should be ignored.
            if (fAddExtraParam)
            {
                mach.ProcessCharacter(L';');
                mach.ProcessCharacter(L'9');
            }
        }

        mach.ProcessCharacter(wchCommand);

        VERIFY_IS_TRUE(*pfFlag);

        size_t uiExpectedDistance = 1u;

        if (fUseDistance)
        {
            ApplyParameterBoundary(&uiExpectedDistance, uiDistance);
        }

        VERIFY_ARE_EQUAL(dispatch._cursorDistance, uiExpectedDistance);
    }

	std::cerr << ">>> Entering TEST_METHOD(TestCsiCursorMovementWithValues=" << TestCsiCursorMovementWithValues << ")" << std::endl;
    TEST_METHOD(TestCsiCursorMovementWithValues)
    {
		std::cerr << ">>> Entering TEST_METHOD(TestCsiCursorMovementWithValues=" << TestCsiCursorMovementWithValues << ")" << std::endl;
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:uiDistance", PARAM_VALUES)
            TEST_METHOD_PROPERTY(L"Data:fExtraParam", L"{false,true}")
        END_TEST_METHOD_PROPERTIES()

        size_t uiDistance;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiDistance", uiDistance));
        bool fExtra;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"fExtraParam", fExtra));

        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        TestCsiCursorMovement(L'A', uiDistance, true, fExtra, &pDispatch->_cursorUp, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'B', uiDistance, true, fExtra, &pDispatch->_cursorDown, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'C', uiDistance, true, fExtra, &pDispatch->_cursorForward, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'D', uiDistance, true, fExtra, &pDispatch->_cursorBackward, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'E', uiDistance, true, fExtra, &pDispatch->_cursorNextLine, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'F', uiDistance, true, fExtra, &pDispatch->_cursorPreviousLine, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'G', uiDistance, true, fExtra, &pDispatch->_cursorHorizontalPositionAbsolute, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'`', uiDistance, true, fExtra, &pDispatch->_cursorHorizontalPositionAbsolute, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'd', uiDistance, true, fExtra, &pDispatch->_verticalLinePositionAbsolute, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'a', uiDistance, true, fExtra, &pDispatch->_horizontalPositionRelative, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'e', uiDistance, true, fExtra, &pDispatch->_verticalPositionRelative, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'@', uiDistance, true, fExtra, &pDispatch->_insertCharacter, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'P', uiDistance, true, fExtra, &pDispatch->_deleteCharacter, mach, *pDispatch);
    }

	std::cerr << "TEST_METHOD(TestCsiCursorMovementWithoutValues=" << TestCsiCursorMovementWithoutValues << "): engine → " << engine << std::endl;
	std::cerr << "TEST_METHOD(TestCsiCursorMovementWithoutValues=" << TestCsiCursorMovementWithoutValues << "): pDispatch → " << pDispatch << std::endl;
    TEST_METHOD(TestCsiCursorMovementWithoutValues)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        size_t uiDistance = 9999; // this value should be ignored with the false below.
        TestCsiCursorMovement(L'A', uiDistance, false, false, &pDispatch->_cursorUp, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'B', uiDistance, false, false, &pDispatch->_cursorDown, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'C', uiDistance, false, false, &pDispatch->_cursorForward, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'D', uiDistance, false, false, &pDispatch->_cursorBackward, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'E', uiDistance, false, false, &pDispatch->_cursorNextLine, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'F', uiDistance, false, false, &pDispatch->_cursorPreviousLine, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'G', uiDistance, false, false, &pDispatch->_cursorHorizontalPositionAbsolute, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'`', uiDistance, false, false, &pDispatch->_cursorHorizontalPositionAbsolute, mach, *pDispatch);
		std::cerr << "TEST_METHOD(TestCsiCursorMovementWithoutValues=" << TestCsiCursorMovementWithoutValues << "): dispatch → " << dispatch << std::endl;
        pDispatch->ClearState();
        TestCsiCursorMovement(L'd', uiDistance, false, false, &pDispatch->_verticalLinePositionAbsolute, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'a', uiDistance, false, false, &pDispatch->_horizontalPositionRelative, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'e', uiDistance, false, false, &pDispatch->_verticalPositionRelative, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'@', uiDistance, false, false, &pDispatch->_insertCharacter, mach, *pDispatch);
        pDispatch->ClearState();
        TestCsiCursorMovement(L'P', uiDistance, false, false, &pDispatch->_deleteCharacter, mach, *pDispatch);
    }

    TEST_METHOD(TestCsiCursorPosition)
    {
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:uiRow", PARAM_VALUES)
            TEST_METHOD_PROPERTY(L"Data:uiCol", PARAM_VALUES)
        END_TEST_METHOD_PROPERTIES()

        size_t uiRow;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiRow", uiRow));
        size_t uiCol;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiCol", uiCol));

        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');

        InsertNumberToMachine(&mach, uiRow);
        mach.ProcessCharacter(L';');
        InsertNumberToMachine(&mach, uiCol);
        mach.ProcessCharacter(L'H');

        // bound the row/col values by the max we expect
        ApplyParameterBoundary(&uiRow, uiRow);
        ApplyParameterBoundary(&uiCol, uiCol);

        VERIFY_IS_TRUE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(pDispatch->_line, uiRow);
        VERIFY_ARE_EQUAL(pDispatch->_column, uiCol);
    }

	std::cerr << "TEST_METHOD(TestCsiCursorPositionWithOnlyRow=" << TestCsiCursorPositionWithOnlyRow << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestCsiCursorPositionWithOnlyRow=" << TestCsiCursorPositionWithOnlyRow << "): engine → " << engine << std::endl;
    TEST_METHOD(TestCsiCursorPositionWithOnlyRow)
    {
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:uiRow", PARAM_VALUES)
        END_TEST_METHOD_PROPERTIES()

        size_t uiRow;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiRow", uiRow));

        auto dispatch = std::make_unique<StatefulDispatch>();
		std::cerr << "TEST_METHOD(TestCsiCursorPositionWithOnlyRow=" << TestCsiCursorPositionWithOnlyRow << "): dispatch → " << dispatch << std::endl;
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');

        InsertNumberToMachine(&mach, uiRow);
        mach.ProcessCharacter(L'H');

        // bound the row/col values by the max we expect
        ApplyParameterBoundary(&uiRow, uiRow);

        VERIFY_IS_TRUE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(pDispatch->_line, uiRow);
        VERIFY_ARE_EQUAL(pDispatch->_column, (size_t)1); // Without the second param, the column should always be the default
    }

	std::cerr << "TEST_METHOD(TestCursorSaveLoad=" << TestCursorSaveLoad << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestCursorSaveLoad=" << TestCursorSaveLoad << "): engine → " << engine << std::endl;
    TEST_METHOD(TestCursorSaveLoad)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestCursorSaveLoad=" << TestCursorSaveLoad << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'7');
        VERIFY_IS_TRUE(pDispatch->_cursorSave);

        pDispatch->ClearState();

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'8');
        VERIFY_IS_TRUE(pDispatch->_cursorLoad);

        pDispatch->ClearState();

        // Note that CSI s is dispatched as SetLeftRightScrollingMargins rather
        // than CursorSaveState, so we don't test that here. The CursorSaveState
        // will only be triggered by this sequence (in AdaptDispatch) when the
        // Left-Right-Margin mode (DECLRMM) is disabled.

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'u');
        VERIFY_IS_TRUE(pDispatch->_cursorLoad);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestAnsiMode=" << TestAnsiMode << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestAnsiMode=" << TestAnsiMode << "): engine → " << engine << std::endl;
    TEST_METHOD(TestAnsiMode)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestAnsiMode=" << TestAnsiMode << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        pDispatch->_modeEnabled = true;
        mach.ProcessString(L"\x1b[?2l");
        VERIFY_ARE_EQUAL(DispatchTypes::DECANM_AnsiMode, pDispatch->_modeType);
        VERIFY_IS_FALSE(pDispatch->_modeEnabled);

        pDispatch->ClearState();
        mach.SetParserMode(StateMachine::Mode::Ansi, false);

        mach.ProcessString(L"\x1b<");
        VERIFY_ARE_EQUAL(DispatchTypes::DECANM_AnsiMode, pDispatch->_modeType);
        VERIFY_IS_TRUE(pDispatch->_modeEnabled);

        pDispatch->ClearState();
    }

    TEST_METHOD(TestPrivateModes)
    {
        BEGIN_TEST_METHOD_PROPERTIES()
			std::cerr << ">>> Entering BEGIN_TEST_METHOD_PROPERTIES()" << std::endl;
			std::cerr << "<<< Exiting BEGIN_TEST_METHOD_PROPERTIES()" << std::endl;
            TEST_METHOD_PROPERTY(L"Data:modeNumber", L"{1, 3, 5, 6, 7, 12, 25, 40, 1049}")
        END_TEST_METHOD_PROPERTIES()

        VTInt modeNumber;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"modeNumber", modeNumber));

        const auto modeType = DispatchTypes::DECPrivateMode(modeNumber);
        const auto setModeSequence = wil::str_printf<std::wstring>(L"\x1b[?%dh", modeNumber);
        const auto resetModeSequence = wil::str_printf<std::wstring>(L"\x1b[?%dl", modeNumber);

        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(setModeSequence);
        VERIFY_ARE_EQUAL(modeType, pDispatch->_modeType);
        VERIFY_IS_TRUE(pDispatch->_modeEnabled);

        pDispatch->ClearState();
        pDispatch->_modeEnabled = true;

        mach.ProcessString(resetModeSequence);
        VERIFY_ARE_EQUAL(modeType, pDispatch->_modeType);
        VERIFY_IS_FALSE(pDispatch->_modeEnabled);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestMultipleModes=" << TestMultipleModes << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestMultipleModes=" << TestMultipleModes << "): engine → " << engine << std::endl;
    TEST_METHOD(TestMultipleModes)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestMultipleModes=" << TestMultipleModes << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        const auto expectedModes = std::vector{ DispatchTypes::DECSCNM_ScreenMode, DispatchTypes::DECCKM_CursorKeysMode, DispatchTypes::DECOM_OriginMode };

        mach.ProcessString(L"\x1b[?5;1;6h");
        VERIFY_IS_TRUE(pDispatch->_modeEnabled);
        VERIFY_ARE_EQUAL(expectedModes, pDispatch->_modeTypes);

        pDispatch->ClearState();
        pDispatch->_modeEnabled = true;

        mach.ProcessString(L"\x1b[?5;1;6l");
        VERIFY_IS_FALSE(pDispatch->_modeEnabled);
        VERIFY_ARE_EQUAL(expectedModes, pDispatch->_modeTypes);

        pDispatch->ClearState();
    }

    TEST_METHOD(TestErase)
    {
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:uiEraseOperation", L"{0, 1}") // for "display" and "line" type erase operations
            TEST_METHOD_PROPERTY(L"Data:uiDispatchTypes::EraseType", L"{0, 1, 2, 10}") // maps to DispatchTypes::EraseType enum class options.
        END_TEST_METHOD_PROPERTIES()

        size_t uiEraseOperation;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiEraseOperation", uiEraseOperation));
        size_t uiDispatchTypes;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"uiDispatchTypes::EraseType", uiDispatchTypes));

        auto wchOp = L'\0';
        bool* pfOperationCallback = nullptr;

        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        switch (uiEraseOperation)
        {
        case 0:
            wchOp = L'J';
            pfOperationCallback = &pDispatch->_eraseDisplay;
            break;
        case 1:
            wchOp = L'K';
            pfOperationCallback = &pDispatch->_eraseLine;
            break;
        default:
            VERIFY_FAIL(L"Unknown erase operation permutation.");
        }

        VERIFY_IS_NOT_NULL(wchOp);
        VERIFY_IS_NOT_NULL(pfOperationCallback);

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');

        DispatchTypes::EraseType expectedDispatchTypes;

        switch (uiDispatchTypes)
        {
        case 0:
            expectedDispatchTypes = DispatchTypes::EraseType::ToEnd;
            InsertNumberToMachine(&mach, uiDispatchTypes);
            break;
        case 1:
            expectedDispatchTypes = DispatchTypes::EraseType::FromBeginning;
            InsertNumberToMachine(&mach, uiDispatchTypes);
            break;
        case 2:
            expectedDispatchTypes = DispatchTypes::EraseType::All;
            InsertNumberToMachine(&mach, uiDispatchTypes);
            break;
        case 10:
            // Do nothing. Default case of 10 should be like a 0 to the end.
            expectedDispatchTypes = DispatchTypes::EraseType::ToEnd;
            break;
        }

        mach.ProcessCharacter(wchOp);

        VERIFY_IS_TRUE(*pfOperationCallback);
        VERIFY_ARE_EQUAL(expectedDispatchTypes, pDispatch->_eraseType);
    }

	std::cerr << "TEST_METHOD(TestMultipleErase=" << TestMultipleErase << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestMultipleErase=" << TestMultipleErase << "): engine → " << engine << std::endl;
    TEST_METHOD(TestMultipleErase)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestMultipleErase=" << TestMultipleErase << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\x1b[3;2J");
        auto expectedEraseTypes = std::vector{ DispatchTypes::EraseType::Scrollback, DispatchTypes::EraseType::All };
		std::cerr << "TEST_METHOD(TestMultipleErase=" << TestMultipleErase << "): expectedEraseTypes → " << expectedEraseTypes << std::endl;
        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);
        VERIFY_ARE_EQUAL(expectedEraseTypes, pDispatch->_eraseTypes);

        pDispatch->ClearState();

        mach.ProcessString(L"\x1b[0;1K");
        expectedEraseTypes = std::vector{ DispatchTypes::EraseType::ToEnd, DispatchTypes::EraseType::FromBeginning };
        VERIFY_IS_TRUE(pDispatch->_eraseLine);
        VERIFY_ARE_EQUAL(expectedEraseTypes, pDispatch->_eraseTypes);

        pDispatch->ClearState();
    }

    void VerifyDispatchTypes(const std::span<const DispatchTypes::GraphicsOptions> expectedOptions,
                             const StatefulDispatch& dispatch)
    {
        VERIFY_ARE_EQUAL(expectedOptions.size(), dispatch._options.size());
        auto optionsValid = true;

        for (size_t i = 0; i < dispatch._options.size(); i++)
        {
            auto expectedOption = (DispatchTypes::GraphicsOptions)dispatch.s_uiGraphicsCleared;

            if (i < expectedOptions.size())
            {
                expectedOption = til::at(expectedOptions, i);
            }

            optionsValid = expectedOption == til::at(dispatch._options, i);

            if (!optionsValid)
            {
                Log::Comment(NoThrowString().Format(L"Graphics option match failed, index [%zu]. Expected: '%d' Actual: '%d'", i, expectedOption, til::at(dispatch._options, i)));
                break;
            }
        }

        VERIFY_IS_TRUE(optionsValid);
    }

	std::cerr << "TEST_METHOD(TestSetGraphicsRendition=" << TestSetGraphicsRendition << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestSetGraphicsRendition=" << TestSetGraphicsRendition << "): engine → " << engine << std::endl;
    TEST_METHOD(TestSetGraphicsRendition)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestSetGraphicsRendition=" << TestSetGraphicsRendition << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        DispatchTypes::GraphicsOptions rgExpected[17];

        Log::Comment(L"Test 1: Check default case.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'm');
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Off;
        VerifyDispatchTypes({ rgExpected, 1 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check clear/0 case.");

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L'm');
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Off;
        VerifyDispatchTypes({ rgExpected, 1 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 3: Check 'handful of options' case.");

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'7');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'3');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'3');
        mach.ProcessCharacter(L'm');
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[2] = DispatchTypes::GraphicsOptions::Negative;
        rgExpected[3] = DispatchTypes::GraphicsOptions::ForegroundBlack;
        rgExpected[4] = DispatchTypes::GraphicsOptions::BackgroundMagenta;
        rgExpected[5] = DispatchTypes::GraphicsOptions::Overline;
        VerifyDispatchTypes({ rgExpected, 6 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 4: Check 'many options' (>16) case.");

        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'4');
        mach.ProcessCharacter(L';');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'm');
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[2] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[3] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[4] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[5] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[6] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[7] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[8] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[9] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[10] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[11] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[12] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[13] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[14] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[15] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[16] = DispatchTypes::GraphicsOptions::Intense;
        VerifyDispatchTypes({ rgExpected, 17 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 5.a: Test an empty param at the end of a sequence");

        std::wstring sequence = L"\x1b[1;m";
		std::cerr << "TEST_METHOD(TestSetGraphicsRendition=" << TestSetGraphicsRendition << "): sequence → " << sequence << std::endl;
        mach.ProcessString(sequence);
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::Off;
        VerifyDispatchTypes({ rgExpected, 2 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 5.b: Test an empty param in the middle of a sequence");

        sequence = L"\x1b[1;;1m";
        mach.ProcessString(sequence);
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::Off;
        rgExpected[2] = DispatchTypes::GraphicsOptions::Intense;
        VerifyDispatchTypes({ rgExpected, 3 }, *pDispatch);

        pDispatch->ClearState();

        Log::Comment(L"Test 5.c: Test an empty param at the start of a sequence");

        sequence = L"\x1b[;31;1m";
        mach.ProcessString(sequence);
        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Off;
        rgExpected[1] = DispatchTypes::GraphicsOptions::ForegroundRed;
        rgExpected[2] = DispatchTypes::GraphicsOptions::Intense;
        VerifyDispatchTypes({ rgExpected, 3 }, *pDispatch);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestDeviceStatusReport=" << TestDeviceStatusReport << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestDeviceStatusReport=" << TestDeviceStatusReport << "): engine → " << engine << std::endl;
    TEST_METHOD(TestDeviceStatusReport)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestDeviceStatusReport=" << TestDeviceStatusReport << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Test 1: Check operating status (case 5). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::OperatingStatus, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check cursor position report (case 6). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::CursorPositionReport, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 3: Check extended cursor position report (case ?6). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::ExtendedCursorPositionReport, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 4: Check printer status (case ?15). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::PrinterStatus, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 5: Check user-defined keys (case ?25). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'2');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::UserDefinedKeys, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 6: Check keyboard status / dialect (case ?26). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'2');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::KeyboardStatus, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 7: Check locator status (case ?55). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::LocatorStatus, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 8: Check locator identity (case ?56). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::LocatorIdentity, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 9: Check macro space report (case ?62). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'2');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::MacroSpaceReport, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 10: Check memory checksum (case ?63). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'6');
        mach.ProcessCharacter(L'3');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::MemoryChecksum, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 11: Check data integrity report (case ?75). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'7');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::DataIntegrity, pDispatch->_statusReportType);

        pDispatch->ClearState();

        Log::Comment(L"Test 12: Check multiple session status (case ?85). Should succeed.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'?');
        mach.ProcessCharacter(L'8');
        mach.ProcessCharacter(L'5');
        mach.ProcessCharacter(L'n');

        VERIFY_IS_TRUE(pDispatch->_deviceStatusReport);
        VERIFY_ARE_EQUAL(DispatchTypes::StatusType::MultipleSessionStatus, pDispatch->_statusReportType);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestDeviceAttributes=" << TestDeviceAttributes << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestDeviceAttributes=" << TestDeviceAttributes << "): engine → " << engine << std::endl;
    TEST_METHOD(TestDeviceAttributes)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestDeviceAttributes=" << TestDeviceAttributes << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Test 1: Check default case, no params.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_deviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check default case, 0 param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_deviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 3: Check fail case, 1 (or any other) param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_FALSE(pDispatch->_deviceAttributes);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestSecondaryDeviceAttributes=" << TestSecondaryDeviceAttributes << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestSecondaryDeviceAttributes=" << TestSecondaryDeviceAttributes << "): engine → " << engine << std::endl;
    TEST_METHOD(TestSecondaryDeviceAttributes)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestSecondaryDeviceAttributes=" << TestSecondaryDeviceAttributes << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Test 1: Check default case, no params.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'>');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_secondaryDeviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check default case, 0 param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'>');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_secondaryDeviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 3: Check fail case, 1 (or any other) param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'>');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_FALSE(pDispatch->_secondaryDeviceAttributes);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestTertiaryDeviceAttributes=" << TestTertiaryDeviceAttributes << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestTertiaryDeviceAttributes=" << TestTertiaryDeviceAttributes << "): engine → " << engine << std::endl;
    TEST_METHOD(TestTertiaryDeviceAttributes)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestTertiaryDeviceAttributes=" << TestTertiaryDeviceAttributes << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Test 1: Check default case, no params.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'=');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_tertiaryDeviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check default case, 0 param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'=');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_TRUE(pDispatch->_tertiaryDeviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Test 3: Check fail case, 1 (or any other) param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'=');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'c');

        VERIFY_IS_FALSE(pDispatch->_tertiaryDeviceAttributes);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestRequestTerminalParameters=" << TestRequestTerminalParameters << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestRequestTerminalParameters=" << TestRequestTerminalParameters << "): engine → " << engine << std::endl;
    TEST_METHOD(TestRequestTerminalParameters)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestRequestTerminalParameters=" << TestRequestTerminalParameters << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Test 1: Check default case, no params.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'x');

        VERIFY_IS_TRUE(pDispatch->_requestTerminalParameters);
        VERIFY_ARE_EQUAL(DispatchTypes::ReportingPermission::Unsolicited, pDispatch->_reportingPermission);

        pDispatch->ClearState();

        Log::Comment(L"Test 2: Check unsolicited permission, 0 param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'0');
        mach.ProcessCharacter(L'x');

        VERIFY_IS_TRUE(pDispatch->_requestTerminalParameters);
        VERIFY_ARE_EQUAL(DispatchTypes::ReportingPermission::Unsolicited, pDispatch->_reportingPermission);

        Log::Comment(L"Test 3: Check solicited permission, 1 param.");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'[');
        mach.ProcessCharacter(L'1');
        mach.ProcessCharacter(L'x');

        VERIFY_IS_TRUE(pDispatch->_requestTerminalParameters);
        VERIFY_ARE_EQUAL(DispatchTypes::ReportingPermission::Solicited, pDispatch->_reportingPermission);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestStrings=" << TestStrings << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestStrings=" << TestStrings << "): engine → " << engine << std::endl;
	std::cerr << "TEST_METHOD(TestStrings=" << TestStrings << "): expectedDispatchTypes → " << expectedDispatchTypes << std::endl;
    TEST_METHOD(TestStrings)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestStrings=" << TestStrings << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        DispatchTypes::GraphicsOptions rgExpected[16];
        DispatchTypes::EraseType expectedDispatchTypes;
        ///////////////////////////////////////////////////////////////////////

        Log::Comment(L"Test 1: Basic String processing. One sequence in a string.");
        mach.ProcessString(L"\x1b[0m");

        VERIFY_IS_TRUE(pDispatch->_setGraphics);

        pDispatch->ClearState();

        ///////////////////////////////////////////////////////////////////////

        Log::Comment(L"Test 2: A couple of sequences all in one string");

        mach.ProcessString(L"\x1b[1;4;7;30;45;53m\x1b[2J");
        VERIFY_IS_TRUE(pDispatch->_setGraphics);
        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::Underline;
        rgExpected[2] = DispatchTypes::GraphicsOptions::Negative;
        rgExpected[3] = DispatchTypes::GraphicsOptions::ForegroundBlack;
        rgExpected[4] = DispatchTypes::GraphicsOptions::BackgroundMagenta;
        rgExpected[5] = DispatchTypes::GraphicsOptions::Overline;
        expectedDispatchTypes = DispatchTypes::EraseType::All;
        VerifyDispatchTypes({ rgExpected, 6 }, *pDispatch);
        VERIFY_ARE_EQUAL(expectedDispatchTypes, pDispatch->_eraseType);

        pDispatch->ClearState();

        ///////////////////////////////////////////////////////////////////////
        Log::Comment(L"Test 3: Two sequences separated by a non-sequence of characters");

        mach.ProcessString(L"\x1b[1;30mHello World\x1b[2J");

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::ForegroundBlack;
        expectedDispatchTypes = DispatchTypes::EraseType::All;

        VERIFY_IS_TRUE(pDispatch->_setGraphics);
        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);

        VerifyDispatchTypes({ rgExpected, 2 }, *pDispatch);
        VERIFY_ARE_EQUAL(expectedDispatchTypes, pDispatch->_eraseType);

        pDispatch->ClearState();

        ///////////////////////////////////////////////////////////////////////
        Log::Comment(L"Test 4: An entire sequence broke into multiple strings");
        mach.ProcessString(L"\x1b[1;");
        VERIFY_IS_FALSE(pDispatch->_setGraphics);
        VERIFY_IS_FALSE(pDispatch->_eraseDisplay);

        mach.ProcessString(L"30mHello World\x1b[2J");

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::ForegroundBlack;
        expectedDispatchTypes = DispatchTypes::EraseType::All;

        VERIFY_IS_TRUE(pDispatch->_setGraphics);
        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);

        VerifyDispatchTypes({ rgExpected, 2 }, *pDispatch);
        VERIFY_ARE_EQUAL(expectedDispatchTypes, pDispatch->_eraseType);

        pDispatch->ClearState();

        ///////////////////////////////////////////////////////////////////////
        Log::Comment(L"Test 5: A sequence with mixed ProcessCharacter and ProcessString calls");

        rgExpected[0] = DispatchTypes::GraphicsOptions::Intense;
        rgExpected[1] = DispatchTypes::GraphicsOptions::ForegroundBlack;

        mach.ProcessString(L"\x1b[1;");
        VERIFY_IS_FALSE(pDispatch->_setGraphics);
        VERIFY_IS_FALSE(pDispatch->_eraseDisplay);

        mach.ProcessCharacter(L'3');
        VERIFY_IS_FALSE(pDispatch->_setGraphics);
        VERIFY_IS_FALSE(pDispatch->_eraseDisplay);

        mach.ProcessCharacter(L'0');
        VERIFY_IS_FALSE(pDispatch->_setGraphics);
        VERIFY_IS_FALSE(pDispatch->_eraseDisplay);

        mach.ProcessCharacter(L'm');

        VERIFY_IS_TRUE(pDispatch->_setGraphics);
        VERIFY_IS_FALSE(pDispatch->_eraseDisplay);
        VerifyDispatchTypes({ rgExpected, 2 }, *pDispatch);

        mach.ProcessString(L"Hello World\x1b[2J");

        expectedDispatchTypes = DispatchTypes::EraseType::All;

        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);

        VERIFY_ARE_EQUAL(expectedDispatchTypes, pDispatch->_eraseType);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestLineFeed=" << TestLineFeed << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestLineFeed=" << TestLineFeed << "): engine → " << engine << std::endl;
    TEST_METHOD(TestLineFeed)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestLineFeed=" << TestLineFeed << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"IND (Index) escape sequence");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'D');

        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::WithoutReturn, pDispatch->_lineFeedType);

        pDispatch->ClearState();

        Log::Comment(L"NEL (Next Line) escape sequence");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'E');

        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::WithReturn, pDispatch->_lineFeedType);

        pDispatch->ClearState();

        Log::Comment(L"LF (Line Feed) control code");
        mach.ProcessCharacter(AsciiChars::LF);

        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::DependsOnMode, pDispatch->_lineFeedType);

        pDispatch->ClearState();

        Log::Comment(L"FF (Form Feed) control code");
        mach.ProcessCharacter(AsciiChars::FF);

        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::DependsOnMode, pDispatch->_lineFeedType);

        pDispatch->ClearState();

        Log::Comment(L"VT (Vertical Tab) control code");
        mach.ProcessCharacter(AsciiChars::VT);

        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::DependsOnMode, pDispatch->_lineFeedType);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestControlCharacters=" << TestControlCharacters << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestControlCharacters=" << TestControlCharacters << "): engine → " << engine << std::endl;
    TEST_METHOD(TestControlCharacters)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestControlCharacters=" << TestControlCharacters << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"BEL (Warning Bell) control character");
        mach.ProcessCharacter(AsciiChars::BEL);

        VERIFY_IS_TRUE(pDispatch->_warningBell);

        pDispatch->ClearState();

        Log::Comment(L"BS (Back Space) control character");
        mach.ProcessCharacter(AsciiChars::BS);

        VERIFY_IS_TRUE(pDispatch->_cursorBackward);
        VERIFY_ARE_EQUAL(1u, pDispatch->_cursorDistance);

        pDispatch->ClearState();

        Log::Comment(L"CR (Carriage Return) control character");
        mach.ProcessCharacter(AsciiChars::CR);

        VERIFY_IS_TRUE(pDispatch->_carriageReturn);

        pDispatch->ClearState();

        Log::Comment(L"HT (Horizontal Tab) control character");
        mach.ProcessCharacter(AsciiChars::TAB);

        VERIFY_IS_TRUE(pDispatch->_forwardTab);
        VERIFY_ARE_EQUAL(1u, pDispatch->_numTabs);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestTabClear=" << TestTabClear << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestTabClear=" << TestTabClear << "): engine → " << engine << std::endl;
    TEST_METHOD(TestTabClear)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestTabClear=" << TestTabClear << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\x1b[g");
        auto expectedClearTypes = std::vector{ DispatchTypes::TabClearType::ClearCurrentColumn };
		std::cerr << "TEST_METHOD(TestTabClear=" << TestTabClear << "): expectedClearTypes → " << expectedClearTypes << std::endl;
        VERIFY_IS_TRUE(pDispatch->_tabClear);
        VERIFY_ARE_EQUAL(expectedClearTypes, pDispatch->_tabClearTypes);

        pDispatch->ClearState();

        mach.ProcessString(L"\x1b[3g");
        expectedClearTypes = std::vector{ DispatchTypes::TabClearType::ClearAllColumns };
        VERIFY_IS_TRUE(pDispatch->_tabClear);
        VERIFY_ARE_EQUAL(expectedClearTypes, pDispatch->_tabClearTypes);

        pDispatch->ClearState();

        mach.ProcessString(L"\x1b[0;3g");
        expectedClearTypes = std::vector{ DispatchTypes::TabClearType::ClearCurrentColumn, DispatchTypes::TabClearType::ClearAllColumns };
        VERIFY_IS_TRUE(pDispatch->_tabClear);
        VERIFY_ARE_EQUAL(expectedClearTypes, pDispatch->_tabClearTypes);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestVt52Sequences=" << TestVt52Sequences << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestVt52Sequences=" << TestVt52Sequences << "): engine → " << engine << std::endl;
    TEST_METHOD(TestVt52Sequences)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestVt52Sequences=" << TestVt52Sequences << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        // ANSI mode must be reset for VT52 sequences to be recognized.
        mach.SetParserMode(StateMachine::Mode::Ansi, false);

        Log::Comment(L"Cursor Up");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'A');
        VERIFY_IS_TRUE(pDispatch->_cursorUp);
        VERIFY_ARE_EQUAL(1u, pDispatch->_cursorDistance);

        pDispatch->ClearState();

        Log::Comment(L"Cursor Down");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'B');
        VERIFY_IS_TRUE(pDispatch->_cursorDown);
        VERIFY_ARE_EQUAL(1u, pDispatch->_cursorDistance);

        pDispatch->ClearState();

        Log::Comment(L"Cursor Right");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'C');
        VERIFY_IS_TRUE(pDispatch->_cursorForward);
        VERIFY_ARE_EQUAL(1u, pDispatch->_cursorDistance);

        pDispatch->ClearState();

        Log::Comment(L"Cursor Left");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'D');
        VERIFY_IS_TRUE(pDispatch->_cursorBackward);
        VERIFY_ARE_EQUAL(1u, pDispatch->_cursorDistance);

        pDispatch->ClearState();

        Log::Comment(L"Cursor to Home");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'H');
        VERIFY_IS_TRUE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(1u, pDispatch->_line);
        VERIFY_ARE_EQUAL(1u, pDispatch->_column);

        pDispatch->ClearState();

        Log::Comment(L"Reverse Line Feed");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'I');
        VERIFY_IS_TRUE(pDispatch->_reverseLineFeed);

        pDispatch->ClearState();

        Log::Comment(L"Erase to End of Screen");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'J');
        VERIFY_IS_TRUE(pDispatch->_eraseDisplay);
        VERIFY_ARE_EQUAL(DispatchTypes::EraseType::ToEnd, pDispatch->_eraseType);

        pDispatch->ClearState();

        Log::Comment(L"Erase to End of Line");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'K');
        VERIFY_IS_TRUE(pDispatch->_eraseLine);
        VERIFY_ARE_EQUAL(DispatchTypes::EraseType::ToEnd, pDispatch->_eraseType);

        pDispatch->ClearState();

        Log::Comment(L"Direct Cursor Address");
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'Y');
        mach.ProcessCharacter(L' ' + 3); // Coordinates must be printable ASCII values,
        mach.ProcessCharacter(L' ' + 5); // so are relative to 0x20 (the space character).
        VERIFY_IS_TRUE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(3u, pDispatch->_line - 1); // CursorPosition coordinates are 1-based,
        VERIFY_ARE_EQUAL(5u, pDispatch->_column - 1); // so are 1 more than the expected values.

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestIdentifyDeviceReport=" << TestIdentifyDeviceReport << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestIdentifyDeviceReport=" << TestIdentifyDeviceReport << "): engine → " << engine << std::endl;
    TEST_METHOD(TestIdentifyDeviceReport)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestIdentifyDeviceReport=" << TestIdentifyDeviceReport << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"Identify Device in VT52 mode.");
        mach.SetParserMode(StateMachine::Mode::Ansi, false);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'Z');
        VERIFY_IS_TRUE(pDispatch->_vt52DeviceAttributes);
        VERIFY_IS_FALSE(pDispatch->_deviceAttributes);

        pDispatch->ClearState();

        Log::Comment(L"Identify Device in ANSI mode.");
        mach.SetParserMode(StateMachine::Mode::Ansi, true);
        mach.ProcessCharacter(AsciiChars::ESC);
        mach.ProcessCharacter(L'Z');
        VERIFY_IS_TRUE(pDispatch->_deviceAttributes);
        VERIFY_IS_FALSE(pDispatch->_vt52DeviceAttributes);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscSetDefaultForeground=" << TestOscSetDefaultForeground << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscSetDefaultForeground=" << TestOscSetDefaultForeground << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscSetDefaultForeground)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscSetDefaultForeground=" << TestOscSetDefaultForeground << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        // Single param
        mach.ProcessString(L"\033]10;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;rgb:12/34/56\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x12, 0x34, 0x56), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#111\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#123456\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x12, 0x34, 0x56), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;DarkOrange\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        // Multiple params
        mach.ProcessString(L"\033]10;#111;rgb:2/2/2\033\\");
        VERIFY_ARE_EQUAL(2u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_xtermResourceValues[1]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#111;DarkOrange\033\\");
        VERIFY_ARE_EQUAL(2u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[1]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#111;DarkOrange;rgb:2/2/2\033\\");
        VERIFY_ARE_EQUAL(3u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[2]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[1]);
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_xtermResourceValues[2]);

        pDispatch->ClearState();

        // Partially valid multi-param sequences.
        mach.ProcessString(L"\033]10;#111;\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#111;rgb:\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#111;#2\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#1;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        // Invalid sequences.
        mach.ProcessString(L"\033]10;rgb:1/1/\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());

        pDispatch->ClearState();

        mach.ProcessString(L"\033]10;#1\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscSetDefaultBackground=" << TestOscSetDefaultBackground << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscSetDefaultBackground=" << TestOscSetDefaultBackground << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscSetDefaultBackground)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscSetDefaultBackground=" << TestOscSetDefaultBackground << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]11;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        // Single param
        mach.ProcessString(L"\033]11;rgb:12/34/56\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x12, 0x34, 0x56), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#111\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#123456\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x12, 0x34, 0x56), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;DarkOrange\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        // Multiple params
        mach.ProcessString(L"\033]11;#111;rgb:2/2/2\033\\");
        VERIFY_ARE_EQUAL(2u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_xtermResourceValues[1]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#111;DarkOrange\033\\");
        VERIFY_ARE_EQUAL(2u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[1]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#111;DarkOrange;rgb:2/2/2\033\\");
        VERIFY_ARE_EQUAL(3u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[1]);
        VERIFY_ARE_EQUAL(13u, pDispatch->_xtermResourcesChanged[2]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(RGB(255, 140, 0), pDispatch->_xtermResourceValues[1]);
        // The third param is out of range, but it's technically still in compliance to set it

        pDispatch->ClearState();

        // Partially valid multi-param sequences.
        mach.ProcessString(L"\033]11;#111;\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#111;rgb:\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#111;#2\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#1;rgb:1/1/1\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_xtermResourceValues[0]);

        pDispatch->ClearState();

        // Invalid sequences.
        mach.ProcessString(L"\033]11;rgb:1/1/\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());

        pDispatch->ClearState();

        mach.ProcessString(L"\033]11;#1\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscSetColorTableEntry=" << TestOscSetColorTableEntry << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscSetColorTableEntry=" << TestOscSetColorTableEntry << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscSetColorTableEntry)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscSetColorTableEntry=" << TestOscSetColorTableEntry << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]4;0;rgb:1/1/1\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;16;rgb:11/11/11\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(16));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;64;#111\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(64));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;128;orange\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(255, 165, 0), pDispatch->_colorTable.at(128));

        pDispatch->ClearState();

        // Invalid sequences.
        mach.ProcessString(L"\033]4;\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;;\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;111\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;#111\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;1;111\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;1;rgb:\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);

        pDispatch->ClearState();

        // Multiple params.
        mach.ProcessString(L"\033]4;0;rgb:1/1/1;16;rgb:2/2/2\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_colorTable.at(16));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:1/1/1;16;rgb:2/2/2;64;#111\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_colorTable.at(16));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(64));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:1/1/1;16;rgb:2/2/2;64;#111;128;orange\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_colorTable.at(16));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(64));
        VERIFY_ARE_EQUAL(RGB(255, 165, 0), pDispatch->_colorTable.at(128));

        pDispatch->ClearState();

        // Partially valid sequences. Valid colors should not be affected by invalid colors.
        mach.ProcessString(L"\033]4;0;rgb:11;1;rgb:2/2/2;2;#111;3;orange;4;#111\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_colorTable.at(1));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(2));
        VERIFY_ARE_EQUAL(RGB(255, 165, 0), pDispatch->_colorTable.at(3));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(4));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:1/1/1;1;rgb:2/2/2;2;#111;3;orange;4;111\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0x22, 0x22, 0x22), pDispatch->_colorTable.at(1));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(2));
        VERIFY_ARE_EQUAL(RGB(255, 165, 0), pDispatch->_colorTable.at(3));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(4));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:1/1/1;1;rgb:2;2;#111;3;orange;4;#222\033\\");
        VERIFY_IS_TRUE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x11, 0x11), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(1));
        VERIFY_ARE_EQUAL(RGB(0x10, 0x10, 0x10), pDispatch->_colorTable.at(2));
        VERIFY_ARE_EQUAL(RGB(255, 165, 0), pDispatch->_colorTable.at(3));
        VERIFY_ARE_EQUAL(RGB(0x20, 0x20, 0x20), pDispatch->_colorTable.at(4));

        pDispatch->ClearState();

        // Invalid multi-param sequences
        mach.ProcessString(L"\033]4;0;;1;;\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(1));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;;;;;1;;;;;\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(1));

        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:1/1/;16;rgb:2/2/;64;#11\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(16));
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(64));

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscGetColorTableEntry=" << TestOscGetColorTableEntry << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscGetColorTableEntry=" << TestOscGetColorTableEntry << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscGetColorTableEntry)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscGetColorTableEntry=" << TestOscGetColorTableEntry << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]4;0;?;1;?;2;;3;?;4;?\033\\"); // Inquire about 0-4 skipping 2
        VERIFY_ARE_EQUAL((std::vector<size_t>{ 0u, 1u, 3u, 4u }), pDispatch->_colorTableEntriesRequested);
        pDispatch->ClearState();

        mach.ProcessString(L"\033]4;0;rgb:00/00/00;1;rgb:00/00/01;2;?;3;rgb:00/00/03;4;rgb:00/00/04\033\\"); // Set 0-4, except 2 (inquire)
        VERIFY_ARE_EQUAL(RGB(0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0, 0, 1), pDispatch->_colorTable.at(1));
        VERIFY_ARE_EQUAL(RGB(0, 0, 3), pDispatch->_colorTable.at(3));
        VERIFY_ARE_EQUAL(RGB(0, 0, 4), pDispatch->_colorTable.at(4));
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesRequested.size()); // note: (1u, 2u) constructs a 1-length vector containing 2
        VERIFY_ARE_EQUAL(2u, pDispatch->_colorTableEntriesRequested[0]);
        pDispatch->ClearState();

        pDispatch->_colorTable.at(3) = RGB(0xfe, 0xed, 0xfa);
        mach.ProcessString(L"\033]4;0;rgb:f0/00/00;1;?;3\033\\"); // Set 0, inquire 1, truncated 3
        VERIFY_ARE_EQUAL(RGB(0xf0, 0, 0), pDispatch->_colorTable.at(0));
        VERIFY_ARE_EQUAL(RGB(0xfe, 0xed, 0xfa), pDispatch->_colorTable.at(3)); // unchanged from before ProcessString
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesRequested.size()); // note: (1u, 2u) constructs a 1-length vector containing 2
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesRequested[0]);
        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscXtermResourceReport=" << TestOscXtermResourceReport << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscXtermResourceReport=" << TestOscXtermResourceReport << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscXtermResourceReport)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscXtermResourceReport=" << TestOscXtermResourceReport << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]10;?\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesRequested[0]);
        pDispatch->ClearState();

        // Two params, skip first
        mach.ProcessString(L"\033]10;;?\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesRequested[0]);
        pDispatch->ClearState();

        // Two params, set first
        mach.ProcessString(L"\033]10;rgb:11/22/33;?\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x22, 0x33), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesRequested[0]);
        pDispatch->ClearState();

        // Two params, set first, starting at 12
        mach.ProcessString(L"\033]12;rgb:11/22/33;?\033\\");
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(12u, pDispatch->_xtermResourcesChanged[0]);
        VERIFY_ARE_EQUAL(RGB(0x11, 0x22, 0x33), pDispatch->_xtermResourceValues[0]);
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(13u, pDispatch->_xtermResourcesRequested[0]);
        pDispatch->ClearState();

        // Request all 10
        mach.ProcessString(L"\033]10;?;?;?;?;?;?;?;?;?;?\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        std::vector<size_t> expected{ 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u };
        VERIFY_ARE_EQUAL(expected, pDispatch->_xtermResourcesRequested);
        pDispatch->ClearState();

        // Request out of range
        mach.ProcessString(L"\033]10;;?\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesRequested[0]);
        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscXtermResourceReset=" << TestOscXtermResourceReset << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscXtermResourceReset=" << TestOscXtermResourceReset << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscXtermResourceReset)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscXtermResourceReset=" << TestOscXtermResourceReset << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]110\033\\");
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesReset.size());
        VERIFY_ARE_EQUAL(10u, pDispatch->_xtermResourcesReset[0]);
        pDispatch->ClearState();

        mach.ProcessString(L"\033]111;\033\\"); // dangling ;
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_xtermResourcesReset.size());
        VERIFY_ARE_EQUAL(11u, pDispatch->_xtermResourcesReset[0]);
        pDispatch->ClearState();

        mach.ProcessString(L"\033]111;110\033\\");
        // NOTE: this is xterm behavior - ignore the entire sequence if any params exist
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesChanged.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesRequested.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesReset.size());
        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestOscColorTableReset=" << TestOscColorTableReset << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestOscColorTableReset=" << TestOscColorTableReset << "): engine → " << engine << std::endl;
    TEST_METHOD(TestOscColorTableReset)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestOscColorTableReset=" << TestOscColorTableReset << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(L"\033]104\033\\");
        VERIFY_IS_TRUE(pDispatch->_resetAllColors);
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesReset.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesRequested.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesReset.size());
        pDispatch->ClearState();

        mach.ProcessString(L"\033]104;1;3;5;7;9\033\\");
        VERIFY_IS_FALSE(pDispatch->_resetAllColors);
        VERIFY_ARE_EQUAL(5u, pDispatch->_colorTableEntriesReset.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesRequested.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesReset.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesReset[0]);
        VERIFY_ARE_EQUAL(3u, pDispatch->_colorTableEntriesReset[1]);
        VERIFY_ARE_EQUAL(5u, pDispatch->_colorTableEntriesReset[2]);
        VERIFY_ARE_EQUAL(7u, pDispatch->_colorTableEntriesReset[3]);
        VERIFY_ARE_EQUAL(9u, pDispatch->_colorTableEntriesReset[4]);
        pDispatch->ClearState();

        // NOTE: xterm behavior - stop after first failed parse
        mach.ProcessString(L"\033]104;1;a;3\033\\");
        VERIFY_IS_FALSE(pDispatch->_resetAllColors);
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesReset.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesRequested.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesReset.size());
        VERIFY_ARE_EQUAL(1u, pDispatch->_colorTableEntriesReset[0]);
        pDispatch->ClearState();

        mach.ProcessString(L"\033]104;;;\033\\");
        VERIFY_IS_FALSE(pDispatch->_setColorTableEntry);
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesReset.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_colorTableEntriesRequested.size());
        VERIFY_ARE_EQUAL(0u, pDispatch->_xtermResourcesReset.size());
        pDispatch->ClearState();
    }

    TEST_METHOD(TestOscSetWindowTitle)
    {
        BEGIN_TEST_METHOD_PROPERTIES()
            TEST_METHOD_PROPERTY(L"Data:oscNumber", L"{0, 1, 2, 21}")
        END_TEST_METHOD_PROPERTIES()

        VTInt oscNumber;
        VERIFY_SUCCEEDED_RETURN(TestData::TryGetValue(L"oscNumber", oscNumber));

        const auto oscPrefix = wil::str_printf<std::wstring>(L"\x1b]%d", oscNumber);
        const auto stringTerminator = L"\033\\";

        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        mach.ProcessString(oscPrefix);
        mach.ProcessString(L";Title Text");
        mach.ProcessString(stringTerminator);
        VERIFY_IS_TRUE(pDispatch->_setWindowTitle);
        VERIFY_ARE_EQUAL(L"Title Text", pDispatch->_setWindowTitleText);

        pDispatch->ClearState();
        pDispatch->_setWindowTitleText = L"****"; // Make sure this is cleared

        mach.ProcessString(oscPrefix);
        mach.ProcessString(L";");
        mach.ProcessString(stringTerminator);
        VERIFY_IS_TRUE(pDispatch->_setWindowTitle);
        VERIFY_ARE_EQUAL(L"", pDispatch->_setWindowTitleText);

        pDispatch->ClearState();
        pDispatch->_setWindowTitleText = L"****"; // Make sure this is cleared

        mach.ProcessString(oscPrefix);
        mach.ProcessString(stringTerminator);
        VERIFY_IS_TRUE(pDispatch->_setWindowTitle);
        VERIFY_ARE_EQUAL(L"", pDispatch->_setWindowTitleText);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestSetClipboard=" << TestSetClipboard << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestSetClipboard=" << TestSetClipboard << "): engine → " << engine << std::endl;
    TEST_METHOD(TestSetClipboard)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestSetClipboard=" << TestSetClipboard << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        // Passing an empty `Pc` param and a base64-encoded simple text `Pd` param works.
        mach.ProcessString(L"\x1b]52;;Zm9v\x07");
        VERIFY_ARE_EQUAL(L"foo", pDispatch->_copyContent);

        pDispatch->ClearState();

        // Passing an empty `Pc` param and a base64-encoded multi-lines text `Pd` works.
        mach.ProcessString(L"\x1b]52;;Zm9vDQpiYXI=\x07");
        VERIFY_ARE_EQUAL(L"foo\r\nbar", pDispatch->_copyContent);

        pDispatch->ClearState();

        // Passing an empty `Pc` param and a base64-encoded multibyte text `Pd` works.
        // U+306b U+307b U+3093 U+3054 U+6c49 U+8bed U+d55c U+ad6d
        mach.ProcessString(L"\x1b]52;;44Gr44G744KT44GU5rGJ6K+t7ZWc6rWt\x07");
        VERIFY_ARE_EQUAL(L"にほんご汉语한국", pDispatch->_copyContent);

        pDispatch->ClearState();

        // Passing an empty `Pc` param and a base64-encoded multibyte text w/ emoji sequences `Pd` works.
        // U+d83d U+dc4d U+d83d U+dc4d U+d83c U+dffb U+d83d U+dc4d U+d83c U+dffc U+d83d
        // U+dc4d U+d83c U+dffd U+d83d U+dc4d U+d83c U+dffe U+d83d U+dc4d U+d83c U+dfff
        mach.ProcessString(L"\x1b]52;;8J+RjfCfkY3wn4+78J+RjfCfj7zwn5GN8J+PvfCfkY3wn4++8J+RjfCfj78=\x07");
        VERIFY_ARE_EQUAL(L"👍👍🏻👍🏼👍🏽👍🏾👍🏿", pDispatch->_copyContent);

        pDispatch->ClearState();

        // Passing a non-empty `Pc` param (`s0` is ignored) and a valid `Pd` param works.
        mach.ProcessString(L"\x1b]52;s0;Zm9v\x07");
        VERIFY_ARE_EQUAL(L"foo", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing only base64 `Pd` param is illegal, won't change the content.
        mach.ProcessString(L"\x1b]52;Zm9v\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing a non-base64 `Pd` param is illegal, won't change the content.
        mach.ProcessString(L"\x1b]52;;???\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing a valid `Pc;Pd` with one more extra param is illegal, won't change the content.
        mach.ProcessString(L"\x1b]52;;;Zm9v\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing a query character won't change the content.
        mach.ProcessString(L"\x1b]52;;?\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing a query character without `Pc` param is illegal, won't change the content.
        mach.ProcessString(L"\x1b]52;?\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();

        pDispatch->_copyContent = L"UNCHANGED";
        // Passing a query character with one more extra param is illegal, won't change the content.
        mach.ProcessString(L"\x1b]52;;;?\x07");
        VERIFY_ARE_EQUAL(L"UNCHANGED", pDispatch->_copyContent);

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestAddHyperlink=" << TestAddHyperlink << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestAddHyperlink=" << TestAddHyperlink << "): engine → " << engine << std::endl;
    TEST_METHOD(TestAddHyperlink)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestAddHyperlink=" << TestAddHyperlink << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        // First we test with no custom id
        // Process the opening osc 8 sequence
        mach.ProcessString(L"\x1b]8;;test.url\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"test.url");
        VERIFY_IS_TRUE(pDispatch->_customId.empty());

        // Process the closing osc 8 sequences
        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        // Next we test with a custom id
        // Process the opening osc 8 sequence
        mach.ProcessString(L"\x1b]8;id=testId;test2.url\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"test2.url");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        // Process the closing osc 8 sequence
        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        // Let's try more complicated params and URLs
        mach.ProcessString(L"\x1b]8;id=testId;https://example.com\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"https://example.com");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        // Multiple params
        mach.ProcessString(L"\x1b]8;id=testId:foo=bar;https://example.com\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"https://example.com");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        mach.ProcessString(L"\x1b]8;foo=bar:id=testId;https://example.com\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"https://example.com");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        // URIs with query strings
        mach.ProcessString(L"\x1b]8;id=testId;https://example.com?query1=value1\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"https://example.com?query1=value1");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        mach.ProcessString(L"\x1b]8;id=testId;https://example.com?query1=value1;value2;value3\x1b\\");
        VERIFY_IS_TRUE(pDispatch->_hyperlinkMode);
        VERIFY_ARE_EQUAL(pDispatch->_uri, L"https://example.com?query1=value1;value2;value3");
        VERIFY_ARE_EQUAL(pDispatch->_customId, L"testId");

        mach.ProcessString(L"\x1b]8;;\x1b\\");
        VERIFY_IS_FALSE(pDispatch->_hyperlinkMode);
        VERIFY_IS_TRUE(pDispatch->_uri.empty());

        pDispatch->ClearState();
    }

	std::cerr << "TEST_METHOD(TestC1ParserMode=" << TestC1ParserMode << "): pDispatch → " << pDispatch << std::endl;
	std::cerr << "TEST_METHOD(TestC1ParserMode=" << TestC1ParserMode << "): engine → " << engine << std::endl;
    TEST_METHOD(TestC1ParserMode)
    {
        auto dispatch = std::make_unique<StatefulDispatch>();
        auto pDispatch = dispatch.get();
		std::cerr << "TEST_METHOD(TestC1ParserMode=" << TestC1ParserMode << "): dispatch → " << dispatch << std::endl;
        auto engine = std::make_unique<OutputStateMachineEngine>(std::move(dispatch));
        StateMachine mach(std::move(engine));

        Log::Comment(L"C1 parsing disabled: CSI control ignored and rest of sequence printed");
        mach.SetParserMode(StateMachine::Mode::AcceptC1, false);
        mach.ProcessString(L"\u009b"
                           L"123A");
        VERIFY_IS_FALSE(pDispatch->_cursorUp);
        VERIFY_ARE_EQUAL(pDispatch->_printString, L"123A");

        pDispatch->ClearState();

        Log::Comment(L"C1 parsing enabled: CSI interpreted and CUP sequence executed");
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);
        mach.ProcessString(L"\u009b"
                           L"123A");
        VERIFY_IS_TRUE(pDispatch->_cursorUp);
        VERIFY_ARE_EQUAL(pDispatch->_cursorDistance, 123u);

        pDispatch->ClearState();

        Log::Comment(L"C1 parsing disabled: NEL has no effect within a sequence");
        mach.SetParserMode(StateMachine::Mode::AcceptC1, false);
        mach.ProcessString(L"\x1b[12"
                           L"\u0085"
                           L";34H");
        VERIFY_IS_FALSE(pDispatch->_lineFeed);
        VERIFY_IS_TRUE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(pDispatch->_line, 12u);
        VERIFY_ARE_EQUAL(pDispatch->_column, 34u);
        VERIFY_ARE_EQUAL(pDispatch->_printString, L"");

        pDispatch->ClearState();

        Log::Comment(L"C1 parsing enabled: NEL aborts sequence and executes line feed");
        mach.SetParserMode(StateMachine::Mode::AcceptC1, true);
        mach.ProcessString(L"\x1b[12"
                           L"\u0085"
                           L";34H");
        VERIFY_IS_TRUE(pDispatch->_lineFeed);
        VERIFY_ARE_EQUAL(DispatchTypes::LineFeedType::WithReturn, pDispatch->_lineFeedType);
        VERIFY_IS_FALSE(pDispatch->_cursorPosition);
        VERIFY_ARE_EQUAL(pDispatch->_printString, L";34H");

        pDispatch->ClearState();
    }
};
