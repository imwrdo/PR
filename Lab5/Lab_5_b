#include <windows.h>
#include <tchar.h>

#define ID_TEXT_EDIT 101
#define ID_BUTTON_CUT 102
#define ID_BUTTON_COPY 103
#define ID_BUTTON_PASTE 104
#define ID_BUTTON_CLEAR 105

UINT CF_CUSTOM_TEXT = RegisterClipboardFormat(TEXT("rozproszki"));

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreate(HWND);
void HandleClear(HWND);
void HandleCut(HWND);
void HandleCopy(HWND);
void HandlePaste(HWND);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = TEXT("WindowClass");
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, TEXT("Call to RegisterClassEx failed!"), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

    
    HWND hWnd = CreateWindow(TEXT("WindowClass"), TEXT("Clipboard Communication"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, TEXT("Call to CreateWindow failed!"), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

  
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        OnCreate(hWnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BUTTON_CUT:
            HandleCopy(hWnd);
            HandleCut(hWnd);
            break;
        case ID_BUTTON_COPY:
            HandleCopy(hWnd);
            break;
        case ID_BUTTON_PASTE:
            HandlePaste(hWnd);
            break;
        case ID_BUTTON_CLEAR:
            HandleClear(hWnd);
            break;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        MessageBox(hWnd, TEXT("Received WM_CLOSE message!"), TEXT("Information"), MB_OK);
        Sleep(1000);
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnCreate(HWND hwnd)
{
    CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
        10, 10, 300, 100, hwnd, (HMENU)ID_TEXT_EDIT, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("CUT"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 120, 70, 30, hwnd, (HMENU)ID_BUTTON_CUT, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("COPY"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        90, 120, 70, 30, hwnd, (HMENU)ID_BUTTON_COPY, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("PASTE"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        170, 120, 70, 30, hwnd, (HMENU)ID_BUTTON_PASTE, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("CLEAR"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 120, 70, 30, hwnd, (HMENU)ID_BUTTON_CLEAR, NULL, NULL);
}

void HandleClear(HWND hwnd)
{
    HWND tekst = GetDlgItem(hwnd, ID_TEXT_EDIT);
    if (tekst != NULL)
    {
        SetWindowText(tekst, TEXT(""));
    }
}

void HandleCut(HWND hwnd)
{
    HWND tekst = GetDlgItem(hwnd, ID_TEXT_EDIT);
    if (tekst != NULL)
    {
        SendMessage(tekst, WM_CUT, 0, 0);
        TCHAR buffer[100];
        GetWindowText(tekst, buffer, 100);
        MessageBox(hwnd, buffer, TEXT("Information"), MB_OK);
        SetWindowText(tekst, TEXT(""));
    }
}

void HandleCopy(HWND hwnd)
{
    HWND tekst = GetDlgItem(hwnd, ID_TEXT_EDIT);
    if (tekst != NULL)
    {
        int nTextLength = GetWindowTextLength(tekst);
        if (nTextLength > 0)
        {
            char* pszText = (char*)GlobalAlloc(GPTR, nTextLength + 1);
            GetWindowTextA(tekst, pszText, nTextLength + 1);

            if (OpenClipboard(hwnd))
            {
                EmptyClipboard();
                HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, nTextLength + 1);
                if (hClipboardData != NULL)
                {
                    char* pClipboardData = (char*)GlobalLock(hClipboardData);
                    if (pClipboardData != NULL)
                    {
                        strcpy_s(pClipboardData, nTextLength + 1, pszText);
                        GlobalUnlock(hClipboardData);
                        SetClipboardData(CF_CUSTOM_TEXT, hClipboardData);
                    }
                }
                CloseClipboard();
            }

            GlobalFree(pszText);
        }
    }
}


void HandlePaste(HWND hwnd)
{
    HWND tekst = GetDlgItem(hwnd, ID_TEXT_EDIT);
    if (tekst != NULL)
    {
        if (OpenClipboard(hwnd))
        {
            HANDLE hClipboardData = GetClipboardData(CF_CUSTOM_TEXT);
            if (hClipboardData != NULL)
            {
                char* pClipboardData = (char*)GlobalLock(hClipboardData);
                if (pClipboardData != NULL)
                {
                    SetWindowTextA(tekst, pClipboardData);
                    GlobalUnlock(hClipboardData);
                }
            }
            CloseClipboard();
        }
    }
}
