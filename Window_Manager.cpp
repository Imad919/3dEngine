// run using '.\Window_Manager.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ Window_Manager.cpp -o Window_Manager.exe -lgdi32 -luser32
// ***fix the connection by using variables***

#include <windows.h>
#include <iostream> //for debugging
#include <cmath>


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

void DrawPoint(HDC hdc, int x, int y, COLORREF color, int thickness)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HBRUSH brush = CreateSolidBrush(color);

    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    int left = x - thickness / 2;
    int top = y - thickness / 2;

    Ellipse(hdc, left, top, left + thickness, top + thickness);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(pen);
    DeleteObject(brush);
}

// Makes the entire window black
void ClearWindow(HWND hwnd, HDC hdc)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

struct vec2d {
    double x, y;
};

struct vec3d {
    double x, y, z;
};

vec3d observer {660, 350, 0};

vec2d project_3d_to_2d(vec3d point3d)
{
    double scale = 400.0;

    double dx = point3d.x - observer.x;
    double dy = point3d.y - observer.y;
    double dz = point3d.z - observer.z;

    return {
        660.0 + scale * dx / dz,
        350.0 - scale * dy / dz
    };
}

vec2d rotate_point (vec2d v, vec2d centre, double angle)
{
    double dx = v.x - centre.x;
    double dy = v.y - centre.y;

    double sin = std::sin(angle * 3.141592653589793 / 180.0);
    double cos = std::cos(angle * 3.141592653589793 / 180.0);

    double new_dx = dx * cos - dy * sin;
    double new_dy = dx * sin + dy * cos;

    return {centre.x + new_dx , centre.y + new_dy};
};

//THE TWO WALLS

//all have same length in 2d
// Shared coordinates
const double left_x = 300.0;
const double right_x = 1020.0;
const double middle_x = 660.0;

const double bottom_y = 100.0;
const double top_y = 600.0;

const double left_z = 700.0;
const double right_z = 700.0;
const double middle_z = 500.0;

// Line left
vec3d line_point1 {left_x, bottom_y, left_z};
vec3d line_point2 {left_x, top_y, left_z};

// Line right
vec3d line2_point1 {right_x, bottom_y, right_z};
vec3d line2_point2 {right_x, top_y, right_z};

// Line middle
vec3d line3_point1 {middle_x, bottom_y, middle_z};
vec3d line3_point2 {middle_x, top_y, middle_z};

// Bottom left to bottom middle
vec3d line4_point1 {left_x, bottom_y, left_z};
vec3d line4_point2 {middle_x, bottom_y, middle_z};

// Top left to top middle
vec3d line5_point1 {left_x, top_y, left_z};
vec3d line5_point2 {middle_x, top_y, middle_z};

// Top right to top middle
vec3d line6_point1 {right_x, top_y, right_z};
vec3d line6_point2 {middle_x, top_y, middle_z};

// Bottom right to bottom middle
vec3d line7_point1 {right_x, bottom_y, right_z};
vec3d line7_point2 {middle_x, bottom_y, middle_z};

// Project all endpoints into 2D

vec2d line_point1_2d = project_3d_to_2d(line_point1);
vec2d line_point2_2d = project_3d_to_2d(line_point2);

vec2d line2_point1_2d = project_3d_to_2d(line2_point1);
vec2d line2_point2_2d = project_3d_to_2d(line2_point2);

vec2d line3_point1_2d = project_3d_to_2d(line3_point1);
vec2d line3_point2_2d = project_3d_to_2d(line3_point2);

vec2d line4_point1_2d = project_3d_to_2d(line4_point1);
vec2d line4_point2_2d = project_3d_to_2d(line4_point2);

vec2d line5_point1_2d = project_3d_to_2d(line5_point1);
vec2d line5_point2_2d = project_3d_to_2d(line5_point2);

vec2d line6_point1_2d = project_3d_to_2d(line6_point1);
vec2d line6_point2_2d = project_3d_to_2d(line6_point2);

vec2d line7_point1_2d = project_3d_to_2d(line7_point1);
vec2d line7_point2_2d = project_3d_to_2d(line7_point2);

/*
//building spinning square
double square_angle = 0.0;
vec2d square_centre{600.0,300.0};
vec2d square_points[4] = { //an array of vec2ds
    {500.0, 400.0},
    {700.0, 400.0},
    {700.0, 200.0},
    {500.0, 200.0}
};
vec2d rotated_square_points[4]; // given value immediately in main
*/

// Custom window procedure (for everything except msg == WM_DESTROY)
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
        
        /*
        for (int i = 0; i < 4; i++) {

        int next = (i + 1) % 4; // 1, 2, 3 then 0

        DrawLine(hdc,
            static_cast<int>(rotated_square_points[i].x),
            static_cast<int>(rotated_square_points[i].y),
            static_cast<int>(rotated_square_points[next].x),
            static_cast<int>(rotated_square_points[next].y),
            RGB(0, 255, 0),
            3);
        }
        */

        vec2d projected_lines[7][2] = {
            {line_point1_2d, line_point2_2d},
            {line2_point1_2d, line2_point2_2d},
            {line3_point1_2d, line3_point2_2d},
            {line4_point1_2d, line4_point2_2d},
            {line5_point1_2d, line5_point2_2d},
            {line6_point1_2d, line6_point2_2d},
            {line7_point1_2d, line7_point2_2d}
        };

        for (int i = 0; i < 7; i++) {
            DrawLine(hdc,
                static_cast<int>(projected_lines[i][0].x),
                static_cast<int>(projected_lines[i][0].y),
                static_cast<int>(projected_lines[i][1].x),
                static_cast<int>(projected_lines[i][1].y),
                RGB(0, 255, 0),
                3);
        }

        EndPaint(hwnd, &ps);

    return 0;
    }
    if (msg == WM_TIMER)
    {
        /*
        //for square
        square_angle += 3;
        // just to keep the angle small (trig funcs work with larger ones too)
        if (square_angle >= 360.0) {
            square_angle -= 360.0;
        }

        for (int i = 0; i < 4; i++) {
            rotated_square_points[i] =
                rotate_point(square_points[i], square_centre, square_angle);
        }
        */

        InvalidateRect(hwnd, NULL, FALSE); // InvalidateRect() tells Windows that your window needs repainting
        return 0;
    }   
    return DefWindowProcW(hwnd, msg, wParam, lParam);
};


int main(){
    // sets all struct members to null
    WNDCLASSW wc = {0}; 

    // L is important because this member needs a wide char
    wc.lpszClassName = L"Window class"; 

    // using our own window procedure instead of the default one 
    // otherwise the program does not restart unless the terminal is killed 
    wc.lpfnWndProc = WindowProc; 

    RegisterClassW(&wc);

    // HWND is a datatype
    HWND hwnd = CreateWindowW(L"Window class",
        L"Spinning line",
        WS_OVERLAPPEDWINDOW,
        120,
        60,
        1320,
        700,
        NULL,
        NULL,
        NULL,
        NULL);
    
    ShowWindow(hwnd, SW_SHOW);

    /*
    for (int i = 0; i < 4; i++) { // for safe measure (not needed)
    rotated_square_points[i] =
        rotate_point(square_points[i], square_centre, square_angle);
    }
    */

    std::cout << "Started...";

    SetTimer(hwnd, 1, 16, NULL);

    // Keeps the window open
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }; 
    
    return 0;
};