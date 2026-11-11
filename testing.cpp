// run using '.\testing.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ testing.cpp -o testing.exe -lgdi32 -luser32

#include <windows.h>
#include <iostream>


void DrawLine(HDC hdc, int x1, int y1, int x2, int y2,
    COLORREF color, int thickness)
    {
    HPEN pen = CreatePen(PS_SOLID, thickness, color);

    // stores old pen and selects current one
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    // selects old pen again
    SelectObject(hdc, oldPen);
    
    DeleteObject(pen);
    }

// Makes the entire window black
void ClearWindow(HWND hwnd, HDC hdc)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

int lineX = 20;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) {
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        ClearWindow(hwnd, hdc);

        DrawLine(hdc, lineX, 200, lineX, 400, RGB(255, 255, 255), 3);

        EndPaint(hwnd, &ps);

        return 0;
    }
    if (msg == WM_TIMER)
    {
        lineX += 2;
        InvalidateRect(hwnd, NULL, FALSE); // InvalidateRect() tells Windows that your window needs repainting
        return 0;
    }   
    return DefWindowProcW(hwnd, msg, wParam, lParam);
};

int main(){
    WNDCLASSW wclass = {0};
    wclass.lpszClassName = L"window class";

    wclass.lpfnWndProc = WindowProc;

    RegisterClassW(&wclass);

    HWND hwnd = CreateWindowW(L"window class",
        L"Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        NULL,
        NULL);

    ShowWindow(hwnd, SW_SHOW);

    std::cout << "started \n";

    int nothing;
    std::cout << "enter any number to clear window: ";
    std::cin >> nothing;

    SetTimer(hwnd, 1, 16, NULL);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }; 
    
    std::cout << "ended \n";

    return 0;
}