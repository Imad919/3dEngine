// run using '.\Window_Manager.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ Window_Manager.cpp -o Window_Manager.exe -lgdi32 -luser32


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


// The CUBE:

//name points:

//front face verticies
double bottom_left_front_x = 500.0;
double bottom_left_front_y = 200.0;
double bottom_left_front_z = 500.0;

double bottom_right_front_x = 800.0;
double bottom_right_front_y = 200.0;
double bottom_right_front_z = 500.0;

double top_left_front_x = 500.0;
double top_left_front_y = 500.0;
double top_left_front_z = 500.0;

double top_right_front_x = 800.0;
double top_right_front_y = 500.0;
double top_right_front_z = 500.0;

// back face verticies
double bottom_left_back_x = 500.0;
double bottom_left_back_y = 200.0;
double bottom_left_back_z = 800.0;

double bottom_right_back_x = 800.0;
double bottom_right_back_y = 200.0;
double bottom_right_back_z = 800.0;

double top_left_back_x = 500.0;
double top_left_back_y = 500.0;
double top_left_back_z = 800.0;

double top_right_back_x = 800.0;
double top_right_back_y = 500.0;
double top_right_back_z = 800.0;


//front face lines vertecies:

vec3d bottom_left_front_vertex_3d {bottom_left_front_x, bottom_left_front_y, bottom_left_front_z};
vec3d top_left_front_vertex_3d {top_left_front_x, top_left_front_y, top_left_front_z};
vec3d bottom_right_front_vertex_3d {bottom_right_front_x, bottom_right_front_y, bottom_right_front_z};
vec3d top_right_front_vertex_3d {top_right_front_x, top_right_front_y, top_right_front_z};

//back face lines vertecies:

vec3d bottom_left_back_vertex_3d {bottom_left_back_x, bottom_left_back_y, bottom_left_back_z};
vec3d top_left_back_vertex_3d {top_left_back_x, top_left_back_y, top_left_back_z};
vec3d bottom_right_back_vertex_3d {bottom_right_back_x, bottom_right_back_y, bottom_right_back_z};
vec3d top_right_back_vertex_3d {top_right_back_x, top_right_back_y, top_right_back_z};


// convert all verticies to 2d

//front face lines 2d verticies:

vec2d bottom_left_front_vertex_2d = project_3d_to_2d(bottom_left_front_vertex_3d);
vec2d top_left_front_vertex_2d = project_3d_to_2d(top_left_front_vertex_3d);

vec2d bottom_right_front_vertex_2d = project_3d_to_2d(bottom_right_front_vertex_3d);
vec2d top_right_front_vertex_2d = project_3d_to_2d(top_right_front_vertex_3d);

//back face lines 2d verticies:

vec2d bottom_left_back_vertex_2d = project_3d_to_2d(bottom_left_back_vertex_3d);
vec2d top_left_back_vertex_2d = project_3d_to_2d(top_left_back_vertex_3d);

vec2d bottom_right_back_vertex_2d = project_3d_to_2d(bottom_right_back_vertex_3d);
vec2d top_right_back_vertex_2d = project_3d_to_2d(top_right_back_vertex_3d);


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

            // Store all 8 projected cube vertices
            vec2d cube_vertices[8] = {
                bottom_left_front_vertex_2d,   // 0
                bottom_right_front_vertex_2d,  // 1
                top_right_front_vertex_2d,     // 2
                top_left_front_vertex_2d,      // 3

                bottom_left_back_vertex_2d,    // 4
                bottom_right_back_vertex_2d,   // 5
                top_right_back_vertex_2d,      // 6
                top_left_back_vertex_2d        // 7
            };

            // Define the 12 connections
            int cube_edges[12][2] = {

                // Front face
                {0, 1},
                {1, 2},
                {2, 3},
                {3, 0},

                // Back face
                {4, 5},
                {5, 6},
                {6, 7},
                {7, 4},

                // Connect front and back faces
                {0, 4},
                {1, 5},
                {2, 6},
                {3, 7}
            };

            // Draw all 12 edges
            for (int i = 0; i < 12; i++) {

                int start = cube_edges[i][0];
                int end = cube_edges[i][1];

                DrawLine(hdc,
                    static_cast<int>(cube_vertices[start].x),
                    static_cast<int>(cube_vertices[start].y),
                    static_cast<int>(cube_vertices[end].x),
                    static_cast<int>(cube_vertices[end].y),
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

    std::cout << "Started...";

    SetTimer(hwnd, 1, 16, NULL);

    // Keeps the window open
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }; 
    
    return 0;
}