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

Texture2D texture;
Texture2D textureFlip;
Texture2D textureFlip2;
Texture2D textureFlip3;




class Plane {
    public:          
        Vector2 position;
        Vector2 velocity; 
        double planeAngle = 0.0f;
        void CreatePlane(Vector2 startPos)
        {
            this->position = startPos;
            this->velocity.x = 5;
            this->velocity.y = 0;
            
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
static void GetMovement(); 
static void UpdatePlane(Plane& plane);
static void DrawGame();  


double DegToRad(double x)
{
    return ((x * 3.14)/180);
}


int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "if you see this, gimme a job");

    InitGame();
    // Image image = LoadImage("Untitled design.png");     // Loaded in CPU memory (RAM)
    // texture = LoadTextureFromImage(image); 
    texture = LoadTexture("pfiddyone2.png");
    textureFlip = LoadTexture("pfiddyone2flip.png");
    textureFlip2 = LoadTexture("pfiddyone2flip2.png");
    textureFlip3 = LoadTexture("pfiddyone2flip3.png");

    
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
            GetMovement();
            UpdatePlane(myPlane);
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

void GetMovement()
{
    if(myPlane.planeAngle >= 360)
    {
        myPlane.planeAngle = std::fmod(myPlane.planeAngle, 360);
    }
    if(myPlane.planeAngle < 0)
    {
        myPlane.planeAngle = 360 + myPlane.planeAngle;
    }
    if (IsKeyDown(KEY_W)) 
    {
        if(myPlane.planeAngle == 90)
            std::cout << "90" << '\n';
        else if(myPlane.planeAngle < 90 || myPlane.planeAngle >= 270)
            myPlane.planeAngle += 1;
        else
           myPlane.planeAngle -= 1; 
    }
    if (IsKeyDown(KEY_A)) 
    {
        if(myPlane.planeAngle == 180)
            std::cout << "180" << '\n';
        else if(myPlane.planeAngle < 180)
            myPlane.planeAngle += 1;
        else
           myPlane.planeAngle -= 1; 
    }
    if (IsKeyDown(KEY_S)) 
    {
        if(myPlane.planeAngle == 270)
            std::cout << "270" << '\n';
        else if(myPlane.planeAngle <= 90 || myPlane.planeAngle > 270)
            myPlane.planeAngle -= 1;
        else
           myPlane.planeAngle += 1; 
    }
    if (IsKeyDown(KEY_D)) 
    {
        if(myPlane.planeAngle == 0)
            std::cout << "0" << '\n';
        else if(myPlane.planeAngle <= 180)
            myPlane.planeAngle -= 1;
        else
           myPlane.planeAngle += 1; 
    }
}


void UpdatePlane(Plane& plane)
{
    plane.position.x += (cosf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
    plane.position.y -= (sinf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
}



void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        Rectangle source = {
            0.0f,                // X position in texture
            0.0f,                // Y position in texture
            (float)texture.width,// Width to sample
            (float)texture.height// Height to sample
        };

        // Define where to draw it on screen and how big
        Rectangle dest = {
            myPlane.position.x, 
            myPlane.position.y,
            texture.width * .3f,
            texture.height * .3f
            // 400.0f,    // X position on screen
            // 300.0f,    // Y position on screen
            // 100.0f,    // Width to draw
            // 100.0f     // Height to draw
        };

        // Define origin point (for rotation/scaling)
        Vector2 origin = {
            dest.width/2,
            dest.height/2
        };

        if (!gameOver)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);

            std::string text = "Angle: -" + std::to_string(myPlane.planeAngle);
            DrawText(text.c_str(), 20, 20, 20, BLACK); 

            DrawRectangle(myPlane.position.x, myPlane.position.y, 100, 20, GRAY);

            //DrawTexture(texture, myPlane.position.x, myPlane.position.y, WHITE);
            // if(myPlane.planeAngle <= 90 || myPlane.planeAngle > 270)
            //     DrawTextureEx(texture, {myPlane.position.x - 50, myPlane.position.y}, -myPlane.planeAngle, .3, WHITE);  // Draw a Texture2D with extended parameters
            // else
            // {           
            //     //DrawTextureEx(textureFlip, myPlane.position, -myPlane.planeAngle, .3, WHITE);
            //     //DrawTextureEx(textureFlip2, {myPlane.position.x, myPlane.position.y + 50}, -myPlane.planeAngle, .3, WHITE);
            //     DrawTextureEx(textureFlip3, {myPlane.position.x - 50, myPlane.position.y}, -myPlane.planeAngle, .3, WHITE);
            // }

            if(myPlane.planeAngle <= 90 || myPlane.planeAngle > 270)
                DrawTexturePro(texture, source, dest, origin, -myPlane.planeAngle, WHITE);  // Draw a Texture2D with extended parameters
            else
            {           
                DrawTexturePro(textureFlip3, source, dest, origin, -myPlane.planeAngle, WHITE);
            }//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);



            
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
        }
        else 
        {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }
    EndDrawing();
}



          