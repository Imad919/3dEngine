// run using '.\game.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ -mconsole Game.cpp Window_Manager.cpp -o Game.exe -lgdi32 -luser32



#include <windows.h>


// SHARED STRUCTURES

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


// ENGINE FUNCTION DECLARATIONS

int RunWindowManager();

void draw_wireframe_cuboid(
    HDC hdc,
    wireframe_cuboid c,
    observer p
);

void rotate_wireframe_cuboid(wireframe_cuboid& a, double r_around_x, double r_around_y, double r_around_z);

void move_observer(observer& camera, WPARAM key);


// GAME OBJECTS

observer player {
    {660, 350, 0},
    400
};

wireframe_cuboid cuboid1 {
    {650.0, 350.0, 650.0},
    0.0, 0.0, 0.0,
    {650.0, 350.0, 650.0},
    300.0, 300.0, 300.0
};

wireframe_cuboid cuboid2 {
    {650.0, 350.0, -550.0},
    0.0, 0.0, 0.0,
    {650.0, 350.0, -550.0},
    300.0, 300.0, 300.0   
};


// GAME FUNCTIONS

// Called every time the engine repaints
void GameRender(HDC hdc)
{
    draw_wireframe_cuboid(hdc, cuboid1, player);
    draw_wireframe_cuboid(hdc, cuboid2, player);
}

// Called every timer update
void GameUpdate()
{
    rotate_wireframe_cuboid(cuboid1, 2.0, 1.0, 1.5);
    rotate_wireframe_cuboid(cuboid2, 0.0, 2.0, 0.0);
}

// Called when a key is pressed
void GameHandleKeyDown(WPARAM key)
{
    move_observer(player, key);
}


// MAIN

int main()
{
    RunWindowManager();
    return 0;
}
