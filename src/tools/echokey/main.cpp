// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#define DEFINE_CONSOLEV2_PROPERTIES

// System headers
#include <windows.h>

// Standard C++ library
#include <cwchar>
#include <cstdlib>
#include <cstdio>

#include <string>
using namespace std;

// WIL
#define WIL_SUPPORT_BITOPERATION_PASCAL_NAMES
#include <wil/Common.h>
#include <wil/Result.h>

bool gVtInput = false;
bool gVtOutput = true;
bool gWindowInput = false;
bool gUseAltBuffer = false;
bool gUseAscii = false;

bool gExitRequested = false;

HANDLE g_hOut = INVALID_HANDLE_VALUE;
HANDLE g_hIn = INVALID_HANDLE_VALUE;

static const char CTRL_D = 0x4;

void csi(string seq)
{
	std::cerr << ">>> Entering csi(seq=" << seq << ")" << std::endl;
    if (!gVtOutput)
    {
        return;
    }
    string fullSeq = "\x1b[";
	std::cerr << "csi(seq=" << seq << "): fullSeq → " << fullSeq << std::endl;
    fullSeq += seq;
    printf(fullSeq.c_str());
	std::cerr << "<<< Exiting csi(seq=" << seq << ")" << std::endl;
}

void useAltBuffer()
{
	std::cerr << ">>> Entering useAltBuffer()" << std::endl;
    csi("?1049h");
	std::cerr << "<<< Exiting useAltBuffer()" << std::endl;
}

void useMainBuffer()
{
	std::cerr << ">>> Entering useMainBuffer()" << std::endl;
    csi("?1049l");
	std::cerr << "<<< Exiting useMainBuffer()" << std::endl;
}

void toPrintableBufferA(char c, char* printBuffer, int* printCch)
{
	std::cerr << ">>> Entering toPrintableBufferA(c=" << c << ",printBuffer=" << printBuffer << ",printCch=" << printCch << ")" << std::endl;
    if (c == '\x1b')
    {
        printBuffer[0] = '^';
        printBuffer[1] = '[';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\x03')
    {
        printBuffer[0] = '^';
        printBuffer[1] = 'C';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\x0')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = '0';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\r')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'r';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\n')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'n';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\t')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 't';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\b')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'b';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else
    {
        printBuffer[0] = (char)c;
        printBuffer[1] = ' ';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
	std::cerr << "<<< Exiting toPrintableBufferA(c=" << c << ",printBuffer=" << printBuffer << ",printCch=" << printCch << ")" << std::endl;
}
void toPrintableBufferW(wchar_t c, wchar_t* printBuffer, int* printCch)
{
	std::cerr << ">>> Entering toPrintableBufferW(c=" << c << ",printBuffer=" << printBuffer << ",printCch=" << printCch << ")" << std::endl;
    if (c == L'\x1b')
    {
        printBuffer[0] = L'^';
        printBuffer[1] = L'[';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\x03')
    {
        printBuffer[0] = L'^';
        printBuffer[1] = L'C';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\x0')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'0';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\r')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'r';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\n')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'n';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\t')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L't';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\b')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'b';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else
    {
        printBuffer[0] = (wchar_t)c;
        printBuffer[1] = L' ';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
	std::cerr << "<<< Exiting toPrintableBufferW(c=" << c << ",printBuffer=" << printBuffer << ",printCch=" << printCch << ")" << std::endl;
}

void handleKeyEventA(KEY_EVENT_RECORD keyEvent)
{
	std::cerr << ">>> Entering handleKeyEventA(keyEvent=" << keyEvent << ")" << std::endl;
    char printBuffer[3];
    int printCch = 0;
    const char c = keyEvent.uChar.AsciiChar;
    toPrintableBufferA(c, printBuffer, &printCch);

    if (!keyEvent.bKeyDown)
    {
        // Print in grey
        csi("38;5;242m");
    }

    wprintf(L"Down: %d Repeat: %d KeyCode: 0x%x ScanCode: 0x%x Char: %hs (0x%x) KeyState: 0x%x\r\n",
            keyEvent.bKeyDown,
            keyEvent.wRepeatCount,
            keyEvent.wVirtualKeyCode,
            keyEvent.wVirtualScanCode,
            printBuffer,
            keyEvent.uChar.AsciiChar,
            keyEvent.dwControlKeyState);

    // restore colors
    csi("0m");

    // Die on Ctrl+D
    if (keyEvent.uChar.AsciiChar == CTRL_D)
    {
        gExitRequested = true;
		std::cerr << "handleKeyEventA(keyEvent=" << keyEvent << "): gExitRequested → " << gExitRequested << std::endl;
    }
	std::cerr << "<<< Exiting handleKeyEventA(keyEvent=" << keyEvent << ")" << std::endl;
}

void handleKeyEventW(KEY_EVENT_RECORD keyEvent)
{
	std::cerr << ">>> Entering handleKeyEventW(keyEvent=" << keyEvent << ")" << std::endl;
    wchar_t printBuffer[3];
    int printCch = 0;
    const wchar_t c = keyEvent.uChar.UnicodeChar;
    toPrintableBufferW(c, printBuffer, &printCch);

    if (!keyEvent.bKeyDown)
    {
        // Print in grey
        csi("38;5;242m");
    }

    wprintf(L"Down: %d Repeat: %d KeyCode: 0x%x ScanCode: 0x%x Char: %s (0x%x) KeyState: 0x%x\r\n",
            keyEvent.bKeyDown,
            keyEvent.wRepeatCount,
            keyEvent.wVirtualKeyCode,
            keyEvent.wVirtualScanCode,
            printBuffer,
            keyEvent.uChar.UnicodeChar,
            keyEvent.dwControlKeyState);

    // restore colors
    csi("0m");

    // Die on Ctrl+D
    if (c == CTRL_D)
    {
        gExitRequested = true;
		std::cerr << "handleKeyEventW(keyEvent=" << keyEvent << "): gExitRequested → " << gExitRequested << std::endl;
    }
	std::cerr << "<<< Exiting handleKeyEventW(keyEvent=" << keyEvent << ")" << std::endl;
}

void handleWindowEvent(WINDOW_BUFFER_SIZE_RECORD windowEvent)
{
	std::cerr << ">>> Entering handleWindowEvent(windowEvent=" << windowEvent << ")" << std::endl;
    SHORT bufferWidth = windowEvent.dwSize.X;
    SHORT bufferHeight = windowEvent.dwSize.Y;

    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { 0 };
    csbiex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    bool fSuccess = !!GetConsoleScreenBufferInfoEx(g_hOut, &csbiex);
	std::cerr << "handleWindowEvent(windowEvent=" << windowEvent << "): fSuccess → " << fSuccess << std::endl;
    if (fSuccess)
    {
        SMALL_RECT srViewport = csbiex.srWindow;

        unsigned short viewX = srViewport.Left;
        unsigned short viewY = srViewport.Top;
        unsigned short viewWidth = srViewport.Right - srViewport.Left + 1;
        unsigned short viewHeight = srViewport.Bottom - srViewport.Top + 1;
        wprintf(L"BufferSize: (%d,%d) Viewport:(x, y, w, h)=(%d,%d,%d,%d)\r\n",
                bufferWidth,
                bufferHeight,
                viewX,
                viewY,
                viewWidth,
                viewHeight);
    }
	std::cerr << "<<< Exiting handleWindowEvent(windowEvent=" << windowEvent << ")" << std::endl;
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        return true;
    }

    return false;
}

void usage()
{
	std::cerr << ">>> Entering usage()" << std::endl;
    wprintf(L"usage: echokey [options]\n");
    wprintf(L"options:\n");
    wprintf(L"\t-i: enable reading VT input mode.\n");
    wprintf(L"\t-o: disable VT output.\n");
    wprintf(L"\t-w: enable reading window events.\n");
    wprintf(L"\t-a: Use ReadConsoleInputA instead.\n");
    wprintf(L"\t--alt: run in the alt buffer. Cannot be combined with `-o`\n");
    wprintf(L"\t-?: print this help message\n");
	std::cerr << "<<< Exiting usage()" << std::endl;
}

int __cdecl wmain(int argc, wchar_t* argv[])
{
    gVtInput = false;
    gVtOutput = true;
    gWindowInput = false;
    gUseAltBuffer = false;
    gExitRequested = false;
    gUseAscii = false;

    for (int i = 1; i < argc; i++)
    {
        wstring arg = wstring(argv[i]);
        wprintf(L"arg=%s\n", arg.c_str());
        if (arg.compare(L"-i") == 0)
        {
            gVtInput = true;
            wprintf(L"Using VT Input\n");
        }
        else if (arg.compare(L"-w") == 0)
        {
            gVtInput = true;
            wprintf(L"Reading Window Input\n");
        }
        else if (arg.compare(L"--alt") == 0)
        {
            gUseAltBuffer = true;
            wprintf(L"Using Alt Buffer.\n");
        }
        else if (arg.compare(L"-o") == 0)
        {
            gVtOutput = false;
            wprintf(L"Disabling VT Output\n");
        }
        else if (arg.compare(L"-a") == 0)
        {
            gUseAscii = true;
            wprintf(L"Using ReadConsoleInputA\n");
        }
        else if (arg.compare(L"-?") == 0)
        {
            usage();
            exit(0);
        }
        else
        {
            wprintf(L"Didn't recognize arg `%s`\n", arg.c_str());
            usage();
            exit(0);
        }
    }

    g_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    g_hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD dwOutMode = 0;
    DWORD dwInMode = 0;
    GetConsoleMode(g_hOut, &dwOutMode);
    GetConsoleMode(g_hIn, &dwInMode);
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    const DWORD initialInMode = dwInMode;
    const DWORD initialOutMode = dwOutMode;

    if (gVtOutput)
    {
        dwOutMode = WI_SetAllFlags(dwOutMode, ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    }

    if (gVtInput)
    {
        dwInMode = WI_SetFlag(dwInMode, ENABLE_VIRTUAL_TERMINAL_INPUT);
    }

    if (gWindowInput)
    {
        dwInMode = WI_SetFlag(dwInMode, ENABLE_WINDOW_INPUT);
    }

    if (gUseAltBuffer && !gVtOutput)
    {
        wprintf(L"Specified `--alt` to use the alternate buffer with `-o`, which disables VT.  --alt requires VT output to be enabled.\n");
        Sleep(2000);
        exit(EXIT_FAILURE);
    }

    SetConsoleMode(g_hOut, dwOutMode);
    SetConsoleMode(g_hIn, dwInMode);

    if (gUseAltBuffer)
    {
        useAltBuffer();
    }

    wprintf(L"Start Mode (i/o):(0x%4x, 0x%4x)\n", initialInMode, initialOutMode);
    wprintf(L"New Mode   (i/o):(0x%4x, 0x%4x)\n", dwInMode, dwOutMode);
    wprintf(L"Press ^D to exit\n");

    while (!gExitRequested)
    {
        INPUT_RECORD rc;
        DWORD dwRead = 0;

        if (gUseAscii)
        {
            ReadConsoleInputA(g_hIn, &rc, 1, &dwRead);
        }
        else
        {
            ReadConsoleInputW(g_hIn, &rc, 1, &dwRead);
        }

        switch (rc.EventType)
        {
        case KEY_EVENT:
        {
            if (gUseAscii)
            {
                handleKeyEventA(rc.Event.KeyEvent);
            }
            else
            {
                handleKeyEventW(rc.Event.KeyEvent);
            }
            break;
        }
        case WINDOW_BUFFER_SIZE_EVENT:
        {
            handleWindowEvent(rc.Event.WindowBufferSizeEvent);
            break;
        }
        }
    }

    if (gUseAltBuffer)
    {
        useMainBuffer();
    }
    SetConsoleMode(g_hOut, initialOutMode);
    SetConsoleMode(g_hIn, initialInMode);

    exit(EXIT_FAILURE);
}
