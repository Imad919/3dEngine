// run using '.\Window_Manager.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ Window_Manager.cpp -o Window_Manager.exe -lgdi32 -luser32


#include <windows.h>
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

vec3d rotate_point_in_3d (vec3d v, vec3d centre, double ax, double ay, double az)
{
    //rotation around x axis
    vec2d in_yz {v.y, v.z}; 
    vec2d centre_yz {centre.y, centre.z};
    vec2d coordinates_in_yz = rotate_point(in_yz, centre_yz, ax); //after rotation 

    //rotation around y axis
    vec2d in_xz {v.x, coordinates_in_yz.y}; // .y is the z
    vec2d centre_xz {centre.x, centre.z};
    vec2d coordinates_in_xz = rotate_point(in_xz, centre_xz, -ay); // '-' because i inverted the y coordinate

    //rotation around z axis
    vec2d in_xy {coordinates_in_xz.x, coordinates_in_yz.x};
    vec2d centre_xy = {centre.x, centre.y};
    vec2d coordinates_in_xy = rotate_point(in_xy, centre_xy, az);
    vec3d point_after_xyz_rotation {coordinates_in_xy.x, coordinates_in_xy.y, coordinates_in_xz.y};

    return point_after_xyz_rotation;
};

// The CUBE:

//name angles
double anglex = 0.0;
double angley = 0.0;
double anglez = 0.0;

// Centre of the cube
vec3d cube_centre{650.0, 350.0, 650.0};

// The 8 original 3D vertices
vec3d cube_vertices[8] = {

    // Front face
    {500.0, 200.0, 500.0},  // 0 bottom left
    {800.0, 200.0, 500.0},  // 1 bottom right
    {800.0, 500.0, 500.0},  // 2 top right
    {500.0, 500.0, 500.0},  // 3 top left

    // Back face
    {500.0, 200.0, 800.0},  // 4 bottom left
    {800.0, 200.0, 800.0},  // 5 bottom right
    {800.0, 500.0, 800.0},  // 6 top right
    {500.0, 500.0, 800.0}   // 7 top left
};


// The 12 edges of the cube
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

    // Connecting edges
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7}
};


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

        // Store the projected vertices here
        vec2d projected_vertices[8];

        // Rotate and project all 8 vertices
        for (int i = 0; i < 8; i++) {

            vec3d rotated = rotate_point_in_3d(
                cube_vertices[i],
                cube_centre,
                anglex, angley, anglez
            );

            projected_vertices[i] = project_3d_to_2d(rotated);
        }

        // Draw all 12 edges
        for (int i = 0; i < 12; i++) {

            int start = cube_edges[i][0];
            int end = cube_edges[i][1];

            DrawLine(hdc,
                static_cast<int>(projected_vertices[start].x),
                static_cast<int>(projected_vertices[start].y),
                static_cast<int>(projected_vertices[end].x),
                static_cast<int>(projected_vertices[end].y),
                RGB(0, 255, 0),
                3);
        }

        EndPaint(hwnd, &ps);

        return 0;
    }
    if (msg == WM_TIMER)
    {
        //for rotating cube
        anglex += 1.5;
        angley += 2.5;
        anglez += 1.0;

        // just to keep the angles small (trig funcs work with larger ones too)
        if (anglex >= 360.0) anglex -= 360.0;
        if (angley >= 360.0) angley -= 360.0;
        if (anglez >= 360.0) anglez -= 360.0;

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
        L"Spinning Cube",
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

    SetTimer(hwnd, 1, 16, NULL);

    // Keeps the window open
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }; 
    
    return 0;
}