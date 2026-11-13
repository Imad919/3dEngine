#include <windows.h>
#include <cmath>


void GameRender(HDC hdc);
void GameUpdate();
void GameHandleKeyDown(WPARAM key);


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

struct wireframe_cuboid {
    vec3d rotation_centre;
    double anglex;
    double angley;
    double anglez;

    vec3d cuboid_centre;
    double length;
    double width;
    double height;
};

struct observer {
    vec3d location;
    double projection_scale;
};


vec2d give_3d_to_2d_coordinates(vec3d point3d, observer p)
{
    double scale = p.projection_scale;

    double dx = point3d.x - p.location.x;
    double dy = point3d.y - p.location.y;
    double dz = point3d.z - p.location.z;

    if (dz <= 10.0)
    {
        return {-123456, -123456}; // arbitrary coordinates used in the before the drawline calls
    }
    return {
        660.0 + scale * dx / dz,
        350.0 - scale * dy / dz
    };
}

vec2d rotated_point_coordinates (vec2d v, vec2d centre, double angle)
{
    double dx = v.x - centre.x;
    double dy = v.y - centre.y;

    double sin = std::sin(angle * 3.141592653589793 / 180.0);
    double cos = std::cos(angle * 3.141592653589793 / 180.0);

    double new_dx = dx * cos - dy * sin;
    double new_dy = dx * sin + dy * cos;

    return {centre.x + new_dx , centre.y + new_dy};
};

vec3d rotate_point_in_3d_coordinates (vec3d v, vec3d centre, double ax, double ay, double az)
{
    //rotation around x axis
    vec2d in_yz {v.y, v.z}; 
    vec2d centre_yz {centre.y, centre.z};
    vec2d coordinates_in_yz = rotated_point_coordinates(in_yz, centre_yz, ax); //after rotation 

    //rotation around y axis
    vec2d in_xz {v.x, coordinates_in_yz.y}; // .y is the z
    vec2d centre_xz {centre.x, centre.z};
    vec2d coordinates_in_xz = rotated_point_coordinates(in_xz, centre_xz, -ay); // '-' because i inverted the y coordinate

    //rotation around z axis
    vec2d in_xy {coordinates_in_xz.x, coordinates_in_yz.x};
    vec2d centre_xy = {centre.x, centre.y};
    vec2d coordinates_in_xy = rotated_point_coordinates(in_xy, centre_xy, az);
    vec3d point_after_xyz_rotation {coordinates_in_xy.x, coordinates_in_xy.y, coordinates_in_xz.y};

    return point_after_xyz_rotation;
}

void draw_wireframe_cuboid (HDC hdc, wireframe_cuboid c, observer p)
{
    double left   = c.cuboid_centre.x - c.length / 2.0;
    double right  = c.cuboid_centre.x + c.length / 2.0;

    double bottom = c.cuboid_centre.y - c.width / 2.0;
    double top    = c.cuboid_centre.y + c.width / 2.0;

    double front  = c.cuboid_centre.z - c.height / 2.0;
    double back   = c.cuboid_centre.z + c.height / 2.0;

    vec3d vertices[8] = {
    // Front face
    {left,  bottom, front}, // 0
    {right, bottom, front}, // 1
    {right, top,    front}, // 2
    {left,  top,    front}, // 3

    // Back face
    {left,  bottom, back},  // 4
    {right, bottom, back},  // 5
    {right, top,    back},  // 6
    {left,  top,    back}   // 7
    };

    int edges[12][2] = {
    // Front face
    {0, 1}, {1, 2}, {2, 3}, {3, 0},

    // Back face
    {4, 5}, {5, 6}, {6, 7}, {7, 4},

    // Connecting edges
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    // Store the projected vertices here
    vec2d projected_vertices[8];

    // Rotate and project all 8 vertices
    for (int i = 0; i < 8; i++) {

        vec3d rotated = rotate_point_in_3d_coordinates(
            vertices[i],
            c.rotation_centre,
            c.anglex, c.angley, c.anglez
        );

        projected_vertices[i] = give_3d_to_2d_coordinates(rotated, p);
    }

    // Draw all 12 edges
    for (int i = 0; i < 12; i++) {

        int start = edges[i][0];
        int end = edges[i][1];
        
        if (projected_vertices[start].x == -123456 ||
            projected_vertices[end].x == -123456)
        {
            continue;
        }

        DrawLine(hdc,
            static_cast<int>(projected_vertices[start].x),
            static_cast<int>(projected_vertices[start].y),
            static_cast<int>(projected_vertices[end].x),
            static_cast<int>(projected_vertices[end].y),
            RGB(0, 255, 0),
            3);
    }
}

void rotate_wireframe_cuboid (wireframe_cuboid& a, double r_around_x, double r_around_y, double r_around_z) // '&' so that the function does not modify a copy
{
        a.anglex += r_around_x;
        a.angley += r_around_y;
        a.anglez += r_around_z;

        // just to keep the angles small (trig funcs work with larger ones too)
        if (a.anglex >= 360.0) a.anglex -= 360.0;
        if (a.angley >= 360.0) a.angley -= 360.0;
        if (a.anglez >= 360.0) a.anglez -= 360.0;
}

void move_observer(observer& camera, WPARAM key)
{
    switch (key)
    {
        case 'W':
            camera.location.z += 10;
            break;

        case 'A':
            camera.location.x -= 10;
            break;

        case 'S':
            camera.location.z -= 10;
            break;

        case 'D':
            camera.location.x += 10;
            break;

        case VK_UP:
            camera.location.y += 10;
            break;

        case VK_DOWN:
            camera.location.y -= 10;
            break;
    }
}


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

        GameRender(hdc);

        EndPaint(hwnd, &ps);

        return 0;
    }
    if (msg == WM_TIMER)
    {
        GameUpdate();

        InvalidateRect(hwnd, NULL, FALSE); // InvalidateRect() tells Windows that your window needs repainting
        return 0;
    }   
    if (msg == WM_KEYDOWN){
    GameHandleKeyDown(wParam);
    return 0;
}
    return DefWindowProcW(hwnd, msg, wParam, lParam);
};


int RunWindowManager(){
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