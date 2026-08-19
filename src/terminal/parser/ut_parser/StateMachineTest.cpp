// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "stateMachine.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace Microsoft
{
    namespace Console
    {
        namespace VirtualTerminal
        {
            class StateMachineTest;
            class TestStateMachineEngine;
        };
    };
};

using namespace Microsoft::Console::VirtualTerminal;

class Microsoft::Console::VirtualTerminal::TestStateMachineEngine : public IStateMachineEngine
{
public:
    void ResetTestState()
    {
		std::cerr << ">>> Entering ResetTestState()" << std::endl;
        printed.clear();
        passedThrough.clear();
        executed.clear();
        csiId = 0;
		std::cerr << "ResetTestState(): csiId → " << csiId << std::endl;
        csiParams.clear();
        dcsId = 0;
		std::cerr << "ResetTestState(): dcsId → " << dcsId << std::endl;
        dcsParams.clear();
        dcsDataString.clear();
		std::cerr << "<<< Exiting ResetTestState()" << std::endl;
    }

    void UnknownSequence() noexcept override
    {
	std::cerr << ">>> Entering UnknownSequence()" << std::endl;
	std::cerr << "<<< Exiting UnknownSequence()" << std::endl;
    }

    bool EncounteredWin32InputModeSequence() const noexcept override
    {
		std::cerr << ">>> Entering EncounteredWin32InputModeSequence()" << std::endl;
		std::cerr << "<<< Exiting EncounteredWin32InputModeSequence()" << std::endl;
        return false;
    }

    bool ActionExecute(const wchar_t wch) override
    {
		std::cerr << ">>> Entering ActionExecute(wch=" << wch << ")" << std::endl;
        executed += wch;
		std::cerr << "<<< Exiting ActionExecute(wch=" << wch << ")" << std::endl;
        return true;
    };

    bool ActionExecuteFromEscape(const wchar_t /* wch */) override { return true; };
    bool ActionPrint(const wchar_t /* wch */) override { return true; };
    bool ActionPrintString(const std::wstring_view string) override
    {
		std::cerr << ">>> Entering ActionPrintString(string=" << string << ")" << std::endl;
        printed += string;
		std::cerr << "<<< Exiting ActionPrintString(string=" << string << ")" << std::endl;
        return true;
    };

    bool ActionPassThroughString(const std::wstring_view string) override
    {
		std::cerr << ">>> Entering ActionPassThroughString(string=" << string << ")" << std::endl;
        passedThrough += string;
		std::cerr << "<<< Exiting ActionPassThroughString(string=" << string << ")" << std::endl;
        return true;
    };

    bool ActionEscDispatch(const VTID /* id */) override { return true; };

    bool ActionVt52EscDispatch(const VTID /*id*/, const VTParameters /*parameters*/) override { return true; };

    bool ActionOscDispatch(const size_t /* parameter */, const std::wstring_view /* string */) override
    {
		std::cerr << ">>> Entering ActionOscDispatch()" << std::endl;
        if (pfnFlushToTerminal)
        {
            pfnFlushToTerminal();
		std::cerr << "<<< Exiting ActionOscDispatch()" << std::endl;
            return true;
        }
        return true;
    };

    bool ActionSs3Dispatch(const wchar_t /* wch */, const VTParameters /* parameters */) override { return true; };

    // ActionCsiDispatch is the only method that's actually implemented.
    bool ActionCsiDispatch(const VTID id, const VTParameters parameters) override
    {
		std::cerr << ">>> Entering ActionCsiDispatch(id=" << id << ",parameters=" << parameters << ")" << std::endl;
        // If flush to terminal is registered for a test, then use it.
        if (pfnFlushToTerminal)
        {
            pfnFlushToTerminal();
            return true;
        }
        else
        {
            csiId = id;
			std::cerr << "ActionCsiDispatch(id=" << id << ",parameters=" << parameters << "): csiId → " << csiId << std::endl;
            for (size_t i = 0; i < parameters.size(); i++)
            {
                csiParams.push_back(parameters.at(i).value_or(0));
            }
			std::cerr << "<<< Exiting ActionCsiDispatch(id=" << id << ",parameters=" << parameters << ")" << std::endl;
            return true;
        }
    }

    IStateMachineEngine::StringHandler ActionDcsDispatch(const VTID id, const VTParameters parameters) override
    {
		std::cerr << ">>> Entering ActionDcsDispatch(id=" << id << ",parameters=" << parameters << ")" << std::endl;
        dcsId = id;
        for (size_t i = 0; i < parameters.size(); i++)
        {
            dcsParams.push_back(parameters.at(i).value_or(0));
        }
        dcsDataString.clear();
		std::cerr << "<<< Exiting ActionDcsDispatch(id=" << id << ",parameters=" << parameters << ")" << std::endl;
        return [=](const auto ch) { dcsDataString += ch; return true; };
    }

    // These will only be populated if ActionCsiDispatch is called.
    uint64_t csiId = 0;
    std::vector<size_t> csiParams;

    // Flush function for pass-through test.
    std::function<bool()> pfnFlushToTerminal;

    // Passed through string.
    std::wstring passedThrough;

    // Printed string.
    std::wstring printed;

    // Executed string.
    std::wstring executed;

    // These will only be populated if ActionDcsDispatch is called.
    uint64_t dcsId = 0;
    std::vector<size_t> dcsParams;
    std::wstring dcsDataString;
};

class Microsoft::Console::VirtualTerminal::StateMachineTest
{
    TEST_CLASS(StateMachineTest);

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    {
        return true;
    }

	std::cerr << ">>> Entering TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        return true;
    }

    TEST_METHOD(TwoStateMachinesDoNotInterfereWithEachOther);

    TEST_METHOD(PassThroughUnhandled);
    TEST_METHOD(RunStorageBeforeEscape);
    TEST_METHOD(BulkTextPrint);
    TEST_METHOD(PassThroughUnhandledSplitAcrossWrites);

    TEST_METHOD(DcsDataStringsReceivedByHandler);

    TEST_METHOD(VtParameterSubspanTest);
};

void StateMachineTest::TwoStateMachinesDoNotInterfereWithEachOther()
{
	std::cerr << ">>> Entering StateMachineTest::TwoStateMachinesDoNotInterfereWithEachOther()" << std::endl;
    auto firstEnginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    const auto& firstEngine{ *firstEnginePtr.get() };
    StateMachine firstStateMachine{ std::move(firstEnginePtr) };

    auto secondEnginePtr{ std::make_unique<TestStateMachineEngine>() };
    const auto& secondEngine{ *secondEnginePtr.get() };
    StateMachine secondStateMachine{ std::move(secondEnginePtr) };

    firstStateMachine.ProcessString(L"\x1b[12"); // partial sequence
    secondStateMachine.ProcessString(L"\x1b[3C"); // full sequence on second parser
    firstStateMachine.ProcessString(L";34m"); // completion to previous partial sequence on first parser

    std::vector<size_t> expectedFirstCsi{ 12u, 34u };
    std::vector<size_t> expectedSecondCsi{ 3u };

    VERIFY_ARE_EQUAL(expectedFirstCsi, firstEngine.csiParams);
    VERIFY_ARE_EQUAL(expectedSecondCsi, secondEngine.csiParams);
	std::cerr << "<<< Exiting StateMachineTest::TwoStateMachinesDoNotInterfereWithEachOther()" << std::endl;
}

void StateMachineTest::PassThroughUnhandled()
{
	std::cerr << ">>> Entering StateMachineTest::PassThroughUnhandled()" << std::endl;
    auto enginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    auto& engine{ *enginePtr.get() };
    StateMachine machine{ std::move(enginePtr) };

    // Hook up the passthrough function.
    engine.pfnFlushToTerminal = std::bind(&StateMachine::FlushToTerminal, &machine);

    machine.ProcessString(L"\x1b[?999h 12345 Hello World");

    VERIFY_ARE_EQUAL(String(L"\x1b[?999h"), String(engine.passedThrough.c_str()));
    VERIFY_ARE_EQUAL(String(L" 12345 Hello World"), String(engine.printed.c_str()));
	std::cerr << "<<< Exiting StateMachineTest::PassThroughUnhandled()" << std::endl;
}

void StateMachineTest::RunStorageBeforeEscape()
{
	std::cerr << ">>> Entering StateMachineTest::RunStorageBeforeEscape()" << std::endl;
    auto enginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    auto& engine{ *enginePtr.get() };
    StateMachine machine{ std::move(enginePtr) };

    // Hook up the passthrough function.
    engine.pfnFlushToTerminal = std::bind(&StateMachine::FlushToTerminal, &machine);

    // Print a bunch of regular text to build up the run buffer before transitioning state.
    machine.ProcessString(L"12345 Hello World\x1b[?999h");

    // Then ensure the entire buffered run was printed all at once back to us.
    VERIFY_ARE_EQUAL(String(L"12345 Hello World"), String(engine.printed.c_str()));
    VERIFY_ARE_EQUAL(String(L"\x1b[?999h"), String(engine.passedThrough.c_str()));
	std::cerr << "<<< Exiting StateMachineTest::RunStorageBeforeEscape()" << std::endl;
}

void StateMachineTest::BulkTextPrint()
{
	std::cerr << ">>> Entering StateMachineTest::BulkTextPrint()" << std::endl;
    auto enginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    auto& engine{ *enginePtr.get() };
    StateMachine machine{ std::move(enginePtr) };

    // Print a bunch of regular text to build up the run buffer before transitioning state.
    machine.ProcessString(L"12345 Hello World");

    // Then ensure the entire buffered run was printed all at once back to us.
    VERIFY_ARE_EQUAL(String(L"12345 Hello World"), String(engine.printed.c_str()));
	std::cerr << "<<< Exiting StateMachineTest::BulkTextPrint()" << std::endl;
}

void StateMachineTest::PassThroughUnhandledSplitAcrossWrites()
{
	std::cerr << ">>> Entering StateMachineTest::PassThroughUnhandledSplitAcrossWrites()" << std::endl;
    auto enginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    auto& engine{ *enginePtr.get() };
    StateMachine machine{ std::move(enginePtr) };

    // Hook up the passthrough function.
    engine.pfnFlushToTerminal = std::bind(&StateMachine::FlushToTerminal, &machine);

    // Broken in two pieces (test case from GH#3081)
    machine.ProcessString(L"\x1b[?12");
    VERIFY_ARE_EQUAL(L"", engine.passedThrough); // nothing out yet
    VERIFY_ARE_EQUAL(L"", engine.printed);

    machine.ProcessString(L"34h");
    VERIFY_ARE_EQUAL(L"\x1b[?1234h", engine.passedThrough); // whole sequence out, no other output
    VERIFY_ARE_EQUAL(L"", engine.printed);

    engine.ResetTestState();

    // Three pieces
    machine.ProcessString(L"\x1b[?2");
    VERIFY_ARE_EQUAL(L"", engine.passedThrough); // nothing out yet
    VERIFY_ARE_EQUAL(L"", engine.printed);

    machine.ProcessString(L"34");
    VERIFY_ARE_EQUAL(L"", engine.passedThrough); // nothing out yet
    VERIFY_ARE_EQUAL(L"", engine.printed);

    machine.ProcessString(L"5h");
    VERIFY_ARE_EQUAL(L"\x1b[?2345h", engine.passedThrough); // whole sequence out, no other output
    VERIFY_ARE_EQUAL(L"", engine.printed);

    engine.ResetTestState();

    // Split during OSC terminator (test case from GH#3080)
    machine.ProcessString(L"\x1b]99;foo\x1b");
    VERIFY_ARE_EQUAL(L"", engine.passedThrough); // nothing out yet
    VERIFY_ARE_EQUAL(L"", engine.printed);

    machine.ProcessString(L"\\");
    VERIFY_ARE_EQUAL(L"\x1b]99;foo\x1b\\", engine.passedThrough);
    VERIFY_ARE_EQUAL(L"", engine.printed);
	std::cerr << "<<< Exiting StateMachineTest::PassThroughUnhandledSplitAcrossWrites()" << std::endl;
}

void StateMachineTest::DcsDataStringsReceivedByHandler()
{
	std::cerr << ">>> Entering StateMachineTest::DcsDataStringsReceivedByHandler()" << std::endl;
    BEGIN_TEST_METHOD_PROPERTIES()
        TEST_METHOD_PROPERTY(L"Data:terminatorType", L"{ 0, 1, 2, 3 }")
    END_TEST_METHOD_PROPERTIES()

    size_t terminatorType;
    VERIFY_SUCCEEDED(TestData::TryGetValue(L"terminatorType", terminatorType));

    auto enginePtr{ std::make_unique<TestStateMachineEngine>() };
    // this dance is required because StateMachine presumes to take ownership of its engine.
    auto& engine{ *enginePtr.get() };
    StateMachine machine{ std::move(enginePtr) };

    uint64_t expectedCsiId = 0;
    std::wstring expectedExecuted = L"";

    std::wstring terminatorString;
    switch (terminatorType)
    {
    case 0:
        Log::Comment(L"Data string terminated with ST");
        terminatorString = L"\033\\";
        break;
    case 1:
        Log::Comment(L"Data string terminated with CSI sequence");
        terminatorString = L"\033[m";
        expectedCsiId = VTID(L'm');
        break;
    case 2:
        Log::Comment(L"Data string terminated with CAN");
        terminatorString = L"\030";
        expectedExecuted = L"\030";
        break;
    case 3:
        Log::Comment(L"Data string terminated with SUB");
        terminatorString = L"\032";
        expectedExecuted = L"\032";
        break;
    }

    // Output a DCS sequence terminated with the current test string
    machine.ProcessString(L"\033P1;2;3|data string");
    machine.ProcessString(terminatorString);
    machine.ProcessString(L"printed text");

    // Verify the sequence ID and parameters are received.
    VERIFY_ARE_EQUAL(VTID("|"), engine.dcsId);
    VERIFY_ARE_EQUAL(std::vector<size_t>({ 1, 2, 3 }), engine.dcsParams);

    // Verify that the data string is received (ESC terminated).
    VERIFY_ARE_EQUAL(L"data string\033", engine.dcsDataString);

    // Verify the characters following the sequence are printed.
    VERIFY_ARE_EQUAL(L"printed text", engine.printed);

    // Verify the CSI sequence was received (if expected).
    VERIFY_ARE_EQUAL(expectedCsiId, engine.csiId);

    // Verify the control characters were executed (if expected).
    VERIFY_ARE_EQUAL(expectedExecuted, engine.executed);
}

void StateMachineTest::VtParameterSubspanTest()
{
	std::cerr << ">>> Entering StateMachineTest::VtParameterSubspanTest()" << std::endl;
    const auto parameterList = std::vector<VTParameter>{ 12, 34, 56, 78 };
    const auto parameterSpan = VTParameters{ parameterList.data(), parameterList.size() };
	std::cerr << "StateMachineTest::VtParameterSubspanTest(): parameterSpan → " << parameterSpan << std::endl;

    {
        Log::Comment(L"Subspan from 0 gives all the parameters");
        const auto subspan = parameterSpan.subspan(0);
		std::cerr << "StateMachineTest::VtParameterSubspanTest(): subspan → " << subspan << std::endl;
        VERIFY_ARE_EQUAL(4u, subspan.size());
        VERIFY_ARE_EQUAL(12, subspan.at(0));
        VERIFY_ARE_EQUAL(34, subspan.at(1));
        VERIFY_ARE_EQUAL(56, subspan.at(2));
        VERIFY_ARE_EQUAL(78, subspan.at(3));
    }
    {
        Log::Comment(L"Subspan from 2 gives the last 2 parameters");
        const auto subspan = parameterSpan.subspan(2);
        VERIFY_ARE_EQUAL(2u, subspan.size());
        VERIFY_ARE_EQUAL(56, subspan.at(0));
        VERIFY_ARE_EQUAL(78, subspan.at(1));
    }
    {
        Log::Comment(L"Subspan at the end of the range gives 1 omitted value");
        const auto subspan = parameterSpan.subspan(4);
        VERIFY_ARE_EQUAL(1u, subspan.size());
        VERIFY_IS_FALSE(subspan.at(0).has_value());
    }
    {
        Log::Comment(L"Subspan past the end of the range gives 1 omitted value");
        const auto subspan = parameterSpan.subspan(6);
        VERIFY_ARE_EQUAL(1u, subspan.size());
        VERIFY_IS_FALSE(subspan.at(0).has_value());
    }
	std::cerr << "<<< Exiting StateMachineTest::VtParameterSubspanTest()" << std::endl;
}
