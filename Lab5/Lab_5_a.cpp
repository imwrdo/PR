#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <stdio.h>


#define ID_CHECKBOX_MOVE 101
#define ID_CHECKBOX_SIZE 102
#define ID_CHECKBOX_PAINT 103
#define ID_BUTTON_CLEAR 104
#define ID_BUTTON_CLOSE 105

// Deklaracje funkcji
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnCreate(HWND hwnd);
void HandleClear(HWND hwnd);
void HandleCheckBox(HWND hwnd, WORD idCheckBox);

// Funkcja WinMain
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,       
    _In_opt_ HINSTANCE hPrevInstance,  
    _In_ LPSTR     lpCmdLine,      
    _In_ int       nCmdShow)      
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
        MessageBox(NULL,
            TEXT("Call to RegisterClassEx failed!"),
            TEXT("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    HWND hWnd = CreateWindow( 
        TEXT("WindowClass"),
        TEXT("Win32 Guided Tour Application"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        550, 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            TEXT("Call to CreateWindow failed!"),
            TEXT("Win32 Guided Tour"),
            NULL);

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
        case ID_BUTTON_CLEAR:
            HandleClear(hWnd);
            break;
        case ID_CHECKBOX_MOVE:
        case ID_CHECKBOX_SIZE:
        case ID_CHECKBOX_PAINT:
            if (HIWORD(wParam) == BN_CLICKED) {
                HandleCheckBox(hWnd, LOWORD(wParam));
            }
            break;
        case ID_BUTTON_CLOSE:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        default:
            break;
        }
        break;
    case WM_MOVE:
        if (IsDlgButtonChecked(hWnd, ID_CHECKBOX_MOVE)) {
            printf("Recieved WM_MOVE message\n");
        }
        break;
    case WM_SIZE:
        if (IsDlgButtonChecked(hWnd, ID_CHECKBOX_SIZE)) {
            printf("Recieved WM_SIZE message\n");
        }
        break;
    case WM_PAINT:
        if (IsDlgButtonChecked(hWnd, ID_CHECKBOX_PAINT)) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            printf("Received WM_PAINT message\n");
        }
        break;
    case WM_CLOSE:
        printf("Received WM_CLOSE message. Closing console...\n");
        Sleep(1000); 
        FreeConsole();
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        printf("Received WM_DESTROY message. Closing console...\n");
        Sleep(1000);
        FreeConsole();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void OnCreate(HWND hwnd) {
    CreateWindow(TEXT("BUTTON"), TEXT("WM_MOVE"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 10, 100, 30, hwnd, (HMENU)ID_CHECKBOX_MOVE, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("WM_SIZE"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        110, 10, 100, 30, hwnd, (HMENU)ID_CHECKBOX_SIZE, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("WM_PAINT"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        210, 10, 100, 30, hwnd, (HMENU)ID_CHECKBOX_PAINT, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("CLEAR"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        310, 10, 100, 30, hwnd, (HMENU)ID_BUTTON_CLEAR, NULL, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("CLOSE"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        410, 10, 100, 30, hwnd, (HMENU)ID_BUTTON_CLOSE, NULL, NULL);
    AllocConsole(); 
    AttachConsole(GetCurrentProcessId()); 
    freopen("CONOUT$", "w", stdout); 
    printf("Console initialized\n");
}



void HandleClear(HWND hwnd) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}


void HandleCheckBox(HWND hwnd, WORD idCheckBox) {

    BOOL checked = IsDlgButtonChecked(hwnd, idCheckBox);
 

    switch (idCheckBox) {
    case ID_CHECKBOX_MOVE:
        if (checked) printf("\nWM_MOVE checked\n\n");
        else if (!checked) printf("\nWM_MOVE unchecked\n\n");
        break;
    case ID_CHECKBOX_SIZE:
        if (checked) printf("\nWM_SIZE checked\n\n");
        else if (!checked) printf("\nWM_SIZE unchecked\n\n");
        break;
    case ID_CHECKBOX_PAINT:
        if (checked) printf("\nWM_PAINT checked\n\n");
        else if (!checked) printf("\nWM_PAINT unchecked\n\n");
        break;
    default:
        break;
    }
}