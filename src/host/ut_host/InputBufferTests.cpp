// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"
#include "CommonState.hpp"

#include "../interactivity/inc/ServiceLocator.hpp"
#include "../types/inc/IInputEvent.hpp"

using namespace WEX::Logging;
using Microsoft::Console::Interactivity::ServiceLocator;

class InputBufferTests
{
    TEST_CLASS(InputBufferTests);

    std::unique_ptr<CommonState> m_state;

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
	std::cerr << "TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << "): m_state → " << m_state << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    {
        m_state = std::make_unique<CommonState>();
        m_state->InitEvents();
        return true;
    }

	std::cerr << ">>> Entering TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        return true;
    }

	std::cerr << ">>> Entering TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        WI_ClearFlag(gci.Flags, CONSOLE_OUTPUT_SUSPENDED);
        return true;
    }

    static const size_t RECORD_INSERT_COUNT = 12;

    INPUT_RECORD MakeKeyEvent(BOOL bKeyDown,
                              WORD wRepeatCount,
                              WORD wVirtualKeyCode,
                              WORD wVirtualScanCode,
                              WCHAR UnicodeChar,
                              DWORD dwControlKeyState)
    {
        INPUT_RECORD retval;
        retval.EventType = KEY_EVENT;
        retval.Event.KeyEvent.bKeyDown = bKeyDown;
        retval.Event.KeyEvent.wRepeatCount = wRepeatCount;
        retval.Event.KeyEvent.wVirtualKeyCode = wVirtualKeyCode;
        retval.Event.KeyEvent.wVirtualScanCode = wVirtualScanCode;
        retval.Event.KeyEvent.uChar.UnicodeChar = UnicodeChar;
        retval.Event.KeyEvent.dwControlKeyState = dwControlKeyState;
        return retval;
    }

	std::cerr << ">>> Entering TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << ")" << std::endl;
	std::cerr << "TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << "): record2.EventType → " << record2.EventType << std::endl;
    TEST_METHOD(CanGetNumberOfReadyEvents)
    {
		std::cerr << ">>> Entering TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << ")" << std::endl;
        InputBuffer inputBuffer;
        auto record = MakeKeyEvent(true, 1, L'a', 0, L'a', 0);
		std::cerr << "TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << "): record → " << record << std::endl;
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(record), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 1u);
        // add another event, check again
        INPUT_RECORD record2;
        record2.EventType = MENU_EVENT;
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(record2), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 2u);
    }

	std::cerr << ">>> Entering TEST_METHOD(CanInsertIntoInputBufferIndividually=" << CanInsertIntoInputBufferIndividually << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << ")" << std::endl;
    TEST_METHOD(CanInsertIntoInputBufferIndividually)
    {
		std::cerr << ">>> Entering TEST_METHOD(CanInsertIntoInputBufferIndividually=" << CanInsertIntoInputBufferIndividually << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(CanGetNumberOfReadyEvents=" << CanGetNumberOfReadyEvents << ")" << std::endl;
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            INPUT_RECORD record;
            record.EventType = MENU_EVENT;
            VERIFY_IS_GREATER_THAN(inputBuffer.Write(record), 0u);
            VERIFY_ARE_EQUAL(record, inputBuffer._storage.back());
        }
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);
    }

	std::cerr << ">>> Entering TEST_METHOD(CanBulkInsertIntoInputBuffer=" << CanBulkInsertIntoInputBuffer << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CanInsertIntoInputBufferIndividually=" << CanInsertIntoInputBufferIndividually << ")" << std::endl;
    TEST_METHOD(CanBulkInsertIntoInputBuffer)
    {
		std::cerr << ">>> Entering TEST_METHOD(CanBulkInsertIntoInputBuffer=" << CanBulkInsertIntoInputBuffer << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(CanInsertIntoInputBufferIndividually=" << CanInsertIntoInputBufferIndividually << ")" << std::endl;
        InputEventQueue events;
        INPUT_RECORD record;
        record.EventType = MENU_EVENT;
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            events.push_back(record);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(events), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);
        // verify that the events are the same in storage
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(inputBuffer._storage[i], record);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CanBulkInsertIntoInputBuffer=" << CanBulkInsertIntoInputBuffer << ")" << std::endl;
	std::cerr << "TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << "): mouseRecord.EventType → " << mouseRecord.EventType << std::endl;
	std::cerr << "TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << "): keyRecord.EventType → " << keyRecord.EventType << std::endl;
    TEST_METHOD(InputBufferCoalescesMouseEvents)
    {
		std::cerr << ">>> Entering TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(CanBulkInsertIntoInputBuffer=" << CanBulkInsertIntoInputBuffer << ")" << std::endl;

        INPUT_RECORD mouseRecord;
        mouseRecord.EventType = MOUSE_EVENT;
        mouseRecord.Event.MouseEvent.dwEventFlags = MOUSE_MOVED;

        // add a bunch of mouse event records
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            mouseRecord.Event.MouseEvent.dwMousePosition.X = static_cast<SHORT>(i + 1);
            mouseRecord.Event.MouseEvent.dwMousePosition.Y = static_cast<SHORT>(i + 1) * 2;
            VERIFY_IS_GREATER_THAN(inputBuffer.Write(mouseRecord), 0u);
        }

        // check that they coalesced
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 1u);
        // check that the mouse position is being updated correctly
        const auto& pMouseEvent = inputBuffer._storage.front().Event.MouseEvent;
        VERIFY_ARE_EQUAL(pMouseEvent.dwMousePosition.X, static_cast<SHORT>(RECORD_INSERT_COUNT));
        VERIFY_ARE_EQUAL(pMouseEvent.dwMousePosition.Y, static_cast<SHORT>(RECORD_INSERT_COUNT * 2));

        // add a key event and another mouse event to make sure that
        // an event between two mouse events stopped the coalescing.
        INPUT_RECORD keyRecord;
        keyRecord.EventType = KEY_EVENT;
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(keyRecord), 0u);
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(mouseRecord), 0u);

        // verify
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 3u);
    }

	std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceBulkMouseEvents=" << InputBufferDoesNotCoalesceBulkMouseEvents << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << ")" << std::endl;
    TEST_METHOD(InputBufferDoesNotCoalesceBulkMouseEvents)
    {
		std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceBulkMouseEvents=" << InputBufferDoesNotCoalesceBulkMouseEvents << ")" << std::endl;
        Log::Comment(L"The input buffer should not coalesce mouse events if more than one event is sent at a time");
		std::cerr << "<<< Exiting TEST_METHOD(InputBufferCoalescesMouseEvents=" << InputBufferCoalescesMouseEvents << ")" << std::endl;

        InputBuffer inputBuffer;
        INPUT_RECORD mouseRecords[RECORD_INSERT_COUNT];
        InputEventQueue events;

        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            mouseRecords[i].EventType = MOUSE_EVENT;
            mouseRecords[i].Event.MouseEvent.dwEventFlags = MOUSE_MOVED;
            events.push_back(mouseRecords[i]);
        }
        // send one mouse event to possibly coalesce into later
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(mouseRecords[0]), 0u);
        // write the others in bulk
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(events), 0u);
        // no events should have been coalesced
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT + 1);
        // check that the events stored match those inserted
        VERIFY_ARE_EQUAL(inputBuffer._storage.front(), mouseRecords[0]);
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(inputBuffer._storage[i + 1], mouseRecords[i]);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(InputBufferCoalescesKeyEvents=" << InputBufferCoalescesKeyEvents << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceBulkMouseEvents=" << InputBufferDoesNotCoalesceBulkMouseEvents << ")" << std::endl;
    TEST_METHOD(InputBufferCoalescesKeyEvents)
    {
		std::cerr << ">>> Entering TEST_METHOD(InputBufferCoalescesKeyEvents=" << InputBufferCoalescesKeyEvents << ")" << std::endl;
        Log::Comment(L"The input buffer should coalesce identical key events if they are send one at a time");
		std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceBulkMouseEvents=" << InputBufferDoesNotCoalesceBulkMouseEvents << ")" << std::endl;

        InputBuffer inputBuffer;
        auto record = MakeKeyEvent(true, 1, L'a', 0, L'a', 0);
		std::cerr << "TEST_METHOD(InputBufferCoalescesKeyEvents=" << InputBufferCoalescesKeyEvents << "): record → " << record << std::endl;

        // send a bunch of identical events
        inputBuffer.Flush();
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_IS_GREATER_THAN(inputBuffer.Write(record), 0u);
        }

        // all events should have been coalesced into one
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 1u);

        // the single event should have a repeat count for each
        // coalesced event
        InputEventQueue outEvents;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents, 1, true, false, false, false));

        VERIFY_IS_FALSE(outEvents.empty());
        const auto& pKeyEvent = outEvents.front().Event.KeyEvent;
        VERIFY_ARE_EQUAL(pKeyEvent.wRepeatCount, RECORD_INSERT_COUNT);
    }

	std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceBulkKeyEvents=" << InputBufferDoesNotCoalesceBulkKeyEvents << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(InputBufferCoalescesKeyEvents=" << InputBufferCoalescesKeyEvents << ")" << std::endl;
    TEST_METHOD(InputBufferDoesNotCoalesceBulkKeyEvents)
    {
		std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceBulkKeyEvents=" << InputBufferDoesNotCoalesceBulkKeyEvents << ")" << std::endl;
        Log::Comment(L"The input buffer should not coalesce key events if more than one event is sent at a time");
		std::cerr << "<<< Exiting TEST_METHOD(InputBufferCoalescesKeyEvents=" << InputBufferCoalescesKeyEvents << ")" << std::endl;

        InputBuffer inputBuffer;
        INPUT_RECORD keyRecords[RECORD_INSERT_COUNT];
        InputEventQueue events;

        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            keyRecords[i] = MakeKeyEvent(true, 1, L'a', 0, L'a', 0);
            events.push_back(keyRecords[i]);
        }
        inputBuffer.Flush();
        // send one key event to possibly coalesce into later
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(keyRecords[0]), 0u);
        // write the others in bulk
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(events), 0u);
        // no events should have been coalesced
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT + 1);
        // check that the events stored match those inserted
        VERIFY_ARE_EQUAL(inputBuffer._storage.front(), keyRecords[0]);
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(inputBuffer._storage[i + 1], keyRecords[i]);
        }
    }

	std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceSurrogatePairs=" << InputBufferDoesNotCoalesceSurrogatePairs << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceBulkKeyEvents=" << InputBufferDoesNotCoalesceBulkKeyEvents << ")" << std::endl;
    TEST_METHOD(InputBufferDoesNotCoalesceSurrogatePairs)
    {
		std::cerr << ">>> Entering TEST_METHOD(InputBufferDoesNotCoalesceSurrogatePairs=" << InputBufferDoesNotCoalesceSurrogatePairs << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceBulkKeyEvents=" << InputBufferDoesNotCoalesceBulkKeyEvents << ")" << std::endl;

        // U+1F44D thumbs up emoji
        inputBuffer.Write(MakeKeyEvent(true, 1, 0xD83D, 0, 0xD83D, 0));
        inputBuffer.Write(MakeKeyEvent(true, 1, 0xDC4D, 0, 0xDC4D, 0));

        // Should not coalesce despite otherwise matching perfectly.
        inputBuffer.Write(MakeKeyEvent(true, 1, 0xDC4D, 0, 0xDC4D, 0));

        // The events shouldn't be coalesced
        VERIFY_ARE_EQUAL(3u, inputBuffer.GetNumberOfReadyEvents());
    }

	std::cerr << ">>> Entering TEST_METHOD(CanFlushAllOutput=" << CanFlushAllOutput << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceSurrogatePairs=" << InputBufferDoesNotCoalesceSurrogatePairs << ")" << std::endl;
    TEST_METHOD(CanFlushAllOutput)
    {
		std::cerr << ">>> Entering TEST_METHOD(CanFlushAllOutput=" << CanFlushAllOutput << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(InputBufferDoesNotCoalesceSurrogatePairs=" << InputBufferDoesNotCoalesceSurrogatePairs << ")" << std::endl;
        InputEventQueue events;

        // put some events in the buffer so we can remove them
        INPUT_RECORD record;
        record.EventType = MENU_EVENT;
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            events.push_back(record);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(events), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);

        // remove them
        inputBuffer.Flush();
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);
    }

	std::cerr << ">>> Entering TEST_METHOD(CanFlushAllButKeys=" << CanFlushAllButKeys << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CanFlushAllOutput=" << CanFlushAllOutput << ")" << std::endl;
    TEST_METHOD(CanFlushAllButKeys)
    {
		std::cerr << ">>> Entering TEST_METHOD(CanFlushAllButKeys=" << CanFlushAllButKeys << ")" << std::endl;
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(CanFlushAllOutput=" << CanFlushAllOutput << ")" << std::endl;
        INPUT_RECORD records[RECORD_INSERT_COUNT] = { 0 };
        InputEventQueue inEvents;

        // create alternating mouse and key events
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            records[i].EventType = (i % 2 == 0) ? MENU_EVENT : KEY_EVENT;
            inEvents.push_back(records[i]);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inEvents), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);

        // remove them
        inputBuffer.FlushAllButKeys();
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT / 2);

        // make sure that the non key events were the ones removed
        InputEventQueue outEvents;
        auto amountToRead = RECORD_INSERT_COUNT / 2;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           false,
                                           false,
                                           false,
                                           false));
        VERIFY_ARE_EQUAL(amountToRead, outEvents.size());

        for (size_t i = 0; i < outEvents.size(); ++i)
        {
            VERIFY_ARE_EQUAL(outEvents[i].EventType, KEY_EVENT);
        }
    }

	std::cerr << "<<< Exiting TEST_METHOD(CanFlushAllButKeys=" << CanFlushAllButKeys << ")" << std::endl;
    TEST_METHOD(CanReadInput)
    {
        InputBuffer inputBuffer;
		std::cerr << "<<< Exiting TEST_METHOD(CanFlushAllButKeys=" << CanFlushAllButKeys << ")" << std::endl;
        INPUT_RECORD records[RECORD_INSERT_COUNT];
        InputEventQueue inEvents;

        // write some input records
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            records[i] = MakeKeyEvent(TRUE, 1, static_cast<WCHAR>(L'A' + i), 0, static_cast<WCHAR>(L'A' + i), 0);
            inEvents.push_back(records[i]);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inEvents), 0u);

        // read them back out
        InputEventQueue outEvents;
        auto amountToRead = RECORD_INSERT_COUNT;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           false,
                                           false,
                                           false,
                                           false));
        VERIFY_ARE_EQUAL(amountToRead, outEvents.size());
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);
        for (size_t i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(records[i], outEvents[i]);
        }
    }

    TEST_METHOD(CanPeekAtEvents)
    {
        InputBuffer inputBuffer;

        // add some events so that we have something to peek at
        INPUT_RECORD records[RECORD_INSERT_COUNT];
        InputEventQueue inEvents;
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            records[i] = MakeKeyEvent(TRUE, 1, static_cast<WCHAR>(L'A' + i), 0, static_cast<WCHAR>(L'A' + i), 0);
            inEvents.push_back(records[i]);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inEvents), 0u);

        // peek at events
        InputEventQueue outEvents;
        auto amountToRead = RECORD_INSERT_COUNT;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           true,
                                           false,
                                           false,
                                           false));

        VERIFY_ARE_EQUAL(amountToRead, outEvents.size());
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(records[i], outEvents[i]);
        }
    }

    TEST_METHOD(EmptyingBufferDuringReadSetsResetWaitEvent)
    {
        Log::Comment(L"hInputEvent should be reset if a read to the buffer completely empties it");

        InputBuffer inputBuffer;

        // add some events so that we have something to stick in front of
        INPUT_RECORD records[RECORD_INSERT_COUNT];
        InputEventQueue inEvents;
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            records[i] = MakeKeyEvent(TRUE, 1, static_cast<WCHAR>(L'A' + i), 0, static_cast<WCHAR>(L'A' + i), 0);
            inEvents.push_back(records[i]);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inEvents), 0u);

        auto& waitEvent = ServiceLocator::LocateGlobals().hInputEvent;
        waitEvent.SetEvent();

        // read one record, hInputEvent should still be signaled
        InputEventQueue outEvents;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           1,
                                           false,
                                           false,
                                           true,
                                           false));
        VERIFY_ARE_EQUAL(outEvents.size(), 1u);
        VERIFY_IS_TRUE(waitEvent.is_signaled());

        // read the rest, hInputEvent should be reset
        waitEvent.SetEvent();
        outEvents.clear();
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           RECORD_INSERT_COUNT - 1,
                                           false,
                                           false,
                                           true,
                                           false));
        VERIFY_ARE_EQUAL(outEvents.size(), RECORD_INSERT_COUNT - 1);
        VERIFY_IS_FALSE(waitEvent.is_signaled());
    }

	std::cerr << "TEST_METHOD(ReadingDbcsCharsPadsOutputArray=" << ReadingDbcsCharsPadsOutputArray << "): codepage → " << codepage << std::endl;
    TEST_METHOD(ReadingDbcsCharsPadsOutputArray)
    {
        Log::Comment(L"During a non-unicode read, the input buffer should count twice for each dbcs key event");

        auto& codepage = ServiceLocator::LocateGlobals().getConsoleInformation().CP;
        const auto restoreCP = wil::scope_exit([&, previous = codepage]() {
            codepage = previous;
        });

        codepage = CP_JAPANESE;

        // write a mouse event, key event, dbcs key event, mouse event
        InputBuffer inputBuffer;

        std::array<INPUT_RECORD, 4> inRecords{};
        inRecords[0].EventType = MOUSE_EVENT;
        inRecords[1] = MakeKeyEvent(TRUE, 1, L'A', 0, L'A', 0);
        inRecords[2] = MakeKeyEvent(TRUE, 1, 0x3042, 0, 0x3042, 0); // U+3042 hiragana A
        inRecords[3].EventType = MOUSE_EVENT;

        std::array<INPUT_RECORD, 5> outRecordsExpected{};
        outRecordsExpected[0].EventType = MOUSE_EVENT;
        outRecordsExpected[1] = MakeKeyEvent(TRUE, 1, L'A', 0, L'A', 0);
        outRecordsExpected[2] = MakeKeyEvent(TRUE, 1, 0x3042, 0, 0x82, 0);
        outRecordsExpected[3] = MakeKeyEvent(TRUE, 1, 0x3042, 0, 0xa0, 0);
        outRecordsExpected[4].EventType = MOUSE_EVENT;

        inputBuffer.Flush();
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inRecords), 0u);

        // read them out non-unicode style and compare
        InputEventQueue outEvents;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           outRecordsExpected.size(),
                                           false,
                                           false,
                                           false,
                                           false));
        VERIFY_ARE_EQUAL(outEvents.size(), outRecordsExpected.size());
        for (size_t i = 0; i < outEvents.size(); ++i)
        {
            VERIFY_ARE_EQUAL(outEvents[i], outRecordsExpected[i]);
        }
    }

	std::cerr << "TEST_METHOD(CanPrependEvents=" << CanPrependEvents << "): eventsWritten → " << eventsWritten << std::endl;
    TEST_METHOD(CanPrependEvents)
    {
        InputBuffer inputBuffer;

        // add some events so that we have something to stick in front of
        INPUT_RECORD records[RECORD_INSERT_COUNT];
        InputEventQueue inEvents;
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            records[i] = MakeKeyEvent(TRUE, 1, static_cast<WCHAR>(L'A' + i), 0, static_cast<WCHAR>(L'A' + i), 0);
            inEvents.push_back(records[i]);
        }
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(inEvents), 0u);

        // prepend some other events
        inEvents.clear();
        INPUT_RECORD prependRecords[RECORD_INSERT_COUNT];
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            prependRecords[i] = MakeKeyEvent(TRUE, 1, static_cast<WCHAR>(L'a' + i), 0, static_cast<WCHAR>(L'a' + i), 0);
            inEvents.push_back(prependRecords[i]);
        }
        auto eventsWritten = inputBuffer.Prepend(inEvents);
        VERIFY_ARE_EQUAL(eventsWritten, RECORD_INSERT_COUNT);

        // grab the first set of events and ensure they match prependRecords
        InputEventQueue outEvents;
        auto amountToRead = RECORD_INSERT_COUNT;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           false,
                                           false,
                                           false,
                                           false));
        VERIFY_ARE_EQUAL(amountToRead, outEvents.size());
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), RECORD_INSERT_COUNT);
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(prependRecords[i], outEvents[i]);
        }

        outEvents.clear();
        // verify the rest of the records
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           false,
                                           false,
                                           false,
                                           false));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);
        VERIFY_ARE_EQUAL(amountToRead, outEvents.size());
        for (unsigned int i = 0; i < RECORD_INSERT_COUNT; ++i)
        {
            VERIFY_ARE_EQUAL(records[i], outEvents[i]);
        }
    }

	std::cerr << "TEST_METHOD(CanReinitializeInputBuffer=" << CanReinitializeInputBuffer << "): inputBuffer.InputMode → " << inputBuffer.InputMode << std::endl;
    TEST_METHOD(CanReinitializeInputBuffer)
    {
        InputBuffer inputBuffer;
        auto originalInputMode = inputBuffer.InputMode;

        // change the buffer's state a bit
        INPUT_RECORD record;
        record.EventType = MENU_EVENT;
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(record), 0u);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 1u);
        inputBuffer.InputMode = 0x0;
        inputBuffer.ReinitializeInputBuffer();

        // check that the changes were reverted
        VERIFY_ARE_EQUAL(originalInputMode, inputBuffer.InputMode);
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);
    }

	std::cerr << "TEST_METHOD(HandleConsoleSuspensionEventsRemovesPauseKeys=" << HandleConsoleSuspensionEventsRemovesPauseKeys << "): pauseRecord → " << pauseRecord << std::endl;
	std::cerr << "TEST_METHOD(HandleConsoleSuspensionEventsRemovesPauseKeys=" << HandleConsoleSuspensionEventsRemovesPauseKeys << "): unpauseRecord → " << unpauseRecord << std::endl;
    TEST_METHOD(HandleConsoleSuspensionEventsRemovesPauseKeys)
    {
        const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        InputBuffer inputBuffer;
        auto pauseRecord = MakeKeyEvent(true, 1, VK_PAUSE, 0, 0, 0);

        // make sure we aren't currently paused and have an empty buffer
        VERIFY_IS_FALSE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);

        VERIFY_ARE_EQUAL(inputBuffer.Write(pauseRecord), 0u);

        // we should now be paused and the input record should be discarded
        VERIFY_IS_TRUE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);

        // the next key press should unpause us but be discarded
        auto unpauseRecord = MakeKeyEvent(true, 1, L'a', 0, L'a', 0);
        VERIFY_ARE_EQUAL(inputBuffer.Write(unpauseRecord), 0u);

        VERIFY_IS_FALSE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);
    }

	std::cerr << "TEST_METHOD(SystemKeysDontUnpauseConsole=" << SystemKeysDontUnpauseConsole << "): pauseRecord → " << pauseRecord << std::endl;
	std::cerr << "TEST_METHOD(SystemKeysDontUnpauseConsole=" << SystemKeysDontUnpauseConsole << "): systemRecord → " << systemRecord << std::endl;
    TEST_METHOD(SystemKeysDontUnpauseConsole)
    {
        const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
        InputBuffer inputBuffer;
        auto pauseRecord = MakeKeyEvent(true, 1, VK_PAUSE, 0, 0, 0);

        // make sure we aren't currently paused and have an empty buffer
        VERIFY_IS_FALSE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);

        // pause the screen
        VERIFY_ARE_EQUAL(inputBuffer.Write(pauseRecord), 0u);

        // we should now be paused and the input record should be discarded
        VERIFY_IS_TRUE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 0u);

        // sending a system key event should not stop the pause and
        // the record should be stored in the input buffer
        auto systemRecord = MakeKeyEvent(true, 1, VK_CONTROL, 0, 0, 0);
        VERIFY_IS_GREATER_THAN(inputBuffer.Write(systemRecord), 0u);

        VERIFY_IS_TRUE(WI_IsFlagSet(gci.Flags, CONSOLE_OUTPUT_SUSPENDED));
        VERIFY_ARE_EQUAL(inputBuffer.GetNumberOfReadyEvents(), 1u);

        InputEventQueue outEvents;
        size_t amountToRead = 2;
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           amountToRead,
                                           true,
                                           false,
                                           false,
                                           false));
    }

    TEST_METHOD(StreamReadingDeCoalesces)
    {
        InputBuffer inputBuffer;
        const WORD repeatCount = 5;
        auto record = MakeKeyEvent(true, repeatCount, L'a', 0, L'a', 0);
		std::cerr << "TEST_METHOD(StreamReadingDeCoalesces=" << StreamReadingDeCoalesces << "): record → " << record << std::endl;
        InputEventQueue outEvents;

        VERIFY_ARE_EQUAL(inputBuffer.Write(record), 1u);
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           1,
                                           false,
                                           false,
                                           true,
                                           true));
        VERIFY_ARE_EQUAL(outEvents.size(), 1u);
        VERIFY_ARE_EQUAL(inputBuffer._storage.size(), 1u);
        VERIFY_ARE_EQUAL(inputBuffer._storage.front().Event.KeyEvent.wRepeatCount, repeatCount - 1);
        VERIFY_ARE_EQUAL(outEvents.front().Event.KeyEvent.wRepeatCount, 1u);
    }

    TEST_METHOD(StreamPeekingDeCoalesces)
    {
        InputBuffer inputBuffer;
        const WORD repeatCount = 5;
        auto record = MakeKeyEvent(true, repeatCount, L'a', 0, L'a', 0);
		std::cerr << "TEST_METHOD(StreamPeekingDeCoalesces=" << StreamPeekingDeCoalesces << "): record → " << record << std::endl;
        InputEventQueue outEvents;

        VERIFY_ARE_EQUAL(inputBuffer.Write(record), 1u);
        VERIFY_NT_SUCCESS(inputBuffer.Read(outEvents,
                                           1,
                                           true,
                                           false,
                                           true,
                                           true));
        VERIFY_ARE_EQUAL(outEvents.size(), 1u);
        VERIFY_ARE_EQUAL(inputBuffer._storage.size(), 1u);
        VERIFY_ARE_EQUAL(inputBuffer._storage.front().Event.KeyEvent.wRepeatCount, repeatCount);
        VERIFY_ARE_EQUAL(outEvents.front().Event.KeyEvent.wRepeatCount, 1u);
    }
};
