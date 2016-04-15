#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <iomanip>

#include "asm_highlighting.h"

#include "..\scintilla\include\Scintilla.h"
#include "..\scintilla\include\SciLexer.h"

#include "fasm.h"

#define IDM_COMPILE 5

static BYTE memBuff[0x10000];
static HWND	hWinInput, hWinOutput;
static HMENU hMenu, hMenubar;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void PrintHex(PBYTE buff, int len, std::stringstream& ss)
{
	ss << std::uppercase;

	for (int i = 0; i < len; ++i)
	{
		ss << "0x" << std::hex << std::setfill('0') << std::setw(2);
		ss << (int)buff[i];
        if (i < len-1)
		    ss << ", ";
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    char szAppName[50];
	auto ver = fasm_GetVersion();
	_snprintf(szAppName, sizeof(szAppName), "FASM v%u.%u", LOWORD(ver), HIWORD(ver));

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = LoadIcon(NULL, IDI_WINLOGO);
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = szAppName;

    Scintilla_RegisterClasses(hInstance);
    
    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName, szAppName,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        NULL, NULL,
        hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);


    while (GetMessage(&msg, NULL, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
    return msg.wParam;
}

void CreateOutputWindow(HWND hWnd, HINSTANCE hInst)
{
    hWinOutput = CreateWindowEx(0, "Scintilla", NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
        0, 0, 0, 0, hWnd, (HMENU)2, hInst, NULL);

	SendMessage(hWinOutput, SCI_SETLEXER,        SCLEX_ASM,    NULL);
	SendMessage(hWinOutput, SCI_SETSTYLEBITS,    5,            NULL);
    SendMessage(hWinOutput, SCI_SETMARGINWIDTHN, 0,              30);
	SendMessage(hWinOutput, SCI_SETTABWIDTH,     4,            NULL);
	SendMessage(hWinOutput, SCI_SETCARETFORE,    NULL,         NULL);
	SendMessage(hWinOutput, SCI_STYLECLEARALL,   NULL,         NULL);
    SendMessage(hWinOutput, SCI_SETWRAPMODE,     SC_WRAP_WORD, NULL);

    for (const auto& val : AsmSyntaxStyle) {
        SendMessage(hWinOutput, SCI_STYLESETFORE, val.Style, val.Forecolor);
        SendMessage(hWinOutput, SCI_STYLESETBACK, val.Style, val.Bgcolor);
        SendMessage(hWinOutput, SCI_STYLESETSIZE, val.Style, val.Size);
        SendMessage(hWinOutput, SCI_STYLESETFONT, val.Style, reinterpret_cast<LPARAM>(val.Font));
        SendMessage(hWinOutput, SCI_STYLESETBOLD, val.Style, val.Bold);
    }
}

void CreateASMEditor(HWND hWnd, HINSTANCE hInst)
{
	hWinInput = CreateWindowEx(0, "Scintilla", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
		0, 0, 0, 0, hWnd, (HMENU)1, hInst, NULL);

	SendMessage(hWinInput, SCI_SETLEXER,        SCLEX_ASM,  NULL);
	SendMessage(hWinInput, SCI_SETSTYLEBITS,    5,          NULL);
    SendMessage(hWinInput, SCI_SETMARGINWIDTHN, 0,            30);
	SendMessage(hWinInput, SCI_SETTABWIDTH,     4,          NULL);
	SendMessage(hWinInput, SCI_SETCARETFORE,    NULL,       NULL);
	SendMessage(hWinInput, SCI_STYLECLEARALL,   NULL,       NULL);

    for (int i = 0; i < _countof(AsmKeyWords); ++i) {
        SendMessage(hWinInput, SCI_SETKEYWORDS, i, (LPARAM)AsmKeyWords[i]);
    }

    for (const auto& val : AsmSyntaxStyle) {
        SendMessage(hWinInput, SCI_STYLESETFORE, val.Style, val.Forecolor);
        SendMessage(hWinInput, SCI_STYLESETBACK, val.Style, val.Bgcolor);
        SendMessage(hWinInput, SCI_STYLESETSIZE, val.Style, val.Size);
        SendMessage(hWinInput, SCI_STYLESETFONT, val.Style, reinterpret_cast<LPARAM>(val.Font));
        SendMessage(hWinInput, SCI_STYLESETBOLD, val.Style, val.Bold);
    }

#if _DEBUG
    char t[] =
        "use32 \n"
        "org 0\n"
        "mov eax, 12312\n"
        ";mov eax, 12312\n"
        "db 'a'\n"
        "nop\n"
        "mov dword [eax+2], ecx\n"
        ;
    SendMessage(hWinInput, SCI_SETTEXT, sizeof(t), (LPARAM)t);
#endif
}

void AddMenus(HWND hwnd) {
    hMenubar = CreateMenu();
    hMenu = CreateMenu();

    AppendMenuW(hMenubar, MF_POPUP, IDM_COMPILE, L"&Assemble (F5)");
    SetMenu(hwnd, hMenubar);
}

void FasmCompile()
{
    int len = SendMessage(hWinInput, SCI_GETTEXTLENGTH, NULL, NULL);
    if (len > 0)
    {
        char* buff = new char[len + 1];
        SendMessage(hWinInput, SCI_GETTEXT, len + 1, (LPARAM)buff);

		fasm_Assemble(buff, memBuff, sizeof(memBuff), 100, 0);
        FASM_STATE* state = reinterpret_cast<FASM_STATE*>(memBuff);

		std::stringstream ss;
		
        if (state->condition == FasmCondition::OK)
        {
			PrintHex(state->output_data, state->output_length, ss);
        }
		else if (state->condition == FasmCondition::FERROR)
		{
			ss << "Error: " << FasmErrorToString(state->error_code) << " at line " << state->error_data->line_number;
		}
		else
		{
			ss << "Error: " << FasmConditionToString(state->condition);
		}

		std::string output = ss.str();
		SendMessage(hWinOutput, SCI_SETTEXT, output.length(), (LPARAM)output.c_str());

        delete buff;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInst;
    RECT rect;
    static HCURSOR 	hCursor;
    static BOOL	bSplitterMoving;
    static DWORD dwSplitterPos;

    switch (uMsg)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam)->hInstance;
#pragma warning(push)
#pragma warning (disable:4302)
        hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS));
#pragma warning (pop)

        AddMenus(hWnd);
		CreateASMEditor(hWnd, hInst);
        CreateOutputWindow(hWnd, hInst);
        
        bSplitterMoving = FALSE;
        dwSplitterPos = 130;
        return 0;


    case WM_SIZE:
        if ((wParam != SIZE_MINIMIZED) && (HIWORD(lParam) < dwSplitterPos))
            dwSplitterPos = HIWORD(lParam) - 10;

        // Adjust the children's size and position
        MoveWindow(hWinInput,  0, 0, LOWORD(lParam), dwSplitterPos - 1, TRUE);
        MoveWindow(hWinOutput, 0, dwSplitterPos + 2, LOWORD(lParam), HIWORD(lParam) - dwSplitterPos - 2, TRUE);
        return 0;


    case WM_MOUSEMOVE:
        if (HIWORD(lParam) > 10) // do not allow above this mark
        {
            SetCursor(hCursor);
            if ((wParam == MK_LBUTTON) && bSplitterMoving)
            {
                GetClientRect(hWnd, &rect);
                if (HIWORD(lParam) > rect.bottom)
                    return 0;

                dwSplitterPos = HIWORD(lParam);
                SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));
            }
        }
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_F5) {
            FasmCompile();
        } break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case IDM_COMPILE: {
                FasmCompile();
            }break;
        }break;
    case WM_LBUTTONDOWN:
        SetCursor(hCursor);
        bSplitterMoving = TRUE;
        SetCapture(hWnd);
        return 0;

    case SCN_KEY: {
        if (wParam == VK_F5) {
        }
    } break;

    case WM_LBUTTONUP:
        ReleaseCapture();
        bSplitterMoving = FALSE;
        return 0;


    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
