#include "raylib.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cstdlib> 
#include <string>
#include <cmath>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif




constexpr int SCREEN_WIDTH = 2400;
constexpr int SCREEN_HEIGHT = 1350;
constexpr float GRAVITY = 9.81f;
constexpr int BULLET_INITIAL_SPEED = 823;
constexpr float SCALE_FACTOR = 100.0;
constexpr float TIME_STEP = 1.0f / 60.0f;

static bool gameOver = false;
static bool pause = false;

class Plane {
    public:          
        Vector2 position;
        Vector2 velocity; 
        void CreatePlane(Vector2 startPos)
        {
            this->position = startPos;
        }
        void DestroyPlane()
        {
            
        }

    Plane() { 
    }
};
Plane myPlane;

static Plane planes[10] = {myPlane};
static void InitGame(void);   
static void UpdateDrawFrame(); 
static void UpdateGame();   
static void UpdatePlane(Plane plane);
static void DrawGame();  


double DegToRad(double x)
{
    return ((x * 3.14)/180);
}


int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "if you see this, gimme a job");

    InitGame();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }

    CloseWindow();

    return 0;
}




void InitGame(void)
{
    std::cout << "In init" << '\n';
    myPlane.CreatePlane({SCREEN_WIDTH/2, SCREEN_HEIGHT/2});
}

void UpdateDrawFrame()
{
    UpdateGame();
    DrawGame();
}

void UpdateGame()
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            std::cout << "restarting game" << '\n';
            gameOver = false;
        }
    }
}

void updatePlane(Plane plane)
{
    
}



void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);

            DrawRectangle(myPlane.position.x, myPlane.position.y, 100, 20, GRAY);
            
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
        }
        else 
        {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }
    EndDrawing();
}



          