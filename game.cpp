// run using '.\game.exe' by first 'cd .\3dEngine' and not simple run button (it deletes the exe)
// To build: g++ -mconsole Game.cpp 3dRenderer.cpp -o Game.exe -lgdi32 -luser32



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

    double length;
    double width;
    double height;
};

struct observer {
    vec3d location;
    double projection_scale;
    double yaw;
    double pitch;
};

struct world {
    double border_x;
    double border_y;
    double border_z;
};


// ENGINE FUNCTION DECLARATIONS

int RunWindowManager();

void draw_wireframe_cuboid(
    HDC hdc,
    wireframe_cuboid& c,
    double cuboid_centre_x, 
    double cuboid_centre_y, 
    double cuboid_centre_z,
    double rotation_centre_x, 
    double rotation_centre_y, 
    double rotation_centre_z,
    observer p,
    world w
);

void rotate_wireframe_cuboid(wireframe_cuboid& a, double r_around_x, double r_around_y, double r_around_z);

void move_observer(observer& camera, WPARAM key);

void rotate_camera(observer& camera, WPARAM key);


// GAME OBJECTS AND VARIABLES

observer player {
    {0, 0, 0},
    400,
    0.0,
    0.0
};

world w {
    40000,
    40000,
    40000
};

wireframe_cuboid cuboid1 {
    {1000.0, -300.0, 1000.0},
    0.0, 0.0, 0.0,
    300.0, 300.0, 300.0
};


// GAME FUNCTIONS

// Called every time the engine repaints
void GameRender(HDC hdc)
{
    for (int i = -3000; i < 3000; i += 1000) {
        for (int j =-3000; j < 3000; j += 1000){
            draw_wireframe_cuboid(hdc, cuboid1, i, -300, j, 650, 350, 650, player, w);
        }
    }
}

// Called every timer update
void GameUpdate()
{
    rotate_wireframe_cuboid(cuboid1, 0.0, 1.0, 0.0);
}

// Called when a key is pressed
void GameHandleKeyDown(WPARAM key)
{
    move_observer(player, key); // using w a s d space f (moves relative to the camera)
    rotate_camera(player, key); // using arrow keys
}


// MAIN

int main()
{
    RunWindowManager();
    return 0;
}
