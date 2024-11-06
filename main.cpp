#include "raylib.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cstdlib> 
#include <string>
#include <cmath>
#include <chrono>  
#include <vector>
#include <thread>

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
        bool isCrashed = false;
        bool isShooting = false;
        std::chrono::time_point<std::chrono::system_clock> start, end, ct;
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

class Bullet {
   public:  
        Vector2 position;        
        double angle = 0.0f;
        bool isDone = false;
    Bullet(Vector2 pos, double planeAngle) {
        position = pos;
        angle = planeAngle;
    } 
};

static std::vector<Rectangle> planeImages;
class EnemyPlane;
static std::vector<EnemyPlane> enemyPlanes;

class EnemyPlane {
    public:
        Vector2 position;
            Vector2 velocity; 
            double planeAngle = 0.0f;
            bool isCrashed = false;
            bool isShooting = false;
            std::chrono::time_point<std::chrono::system_clock> start, end, ct;
            void CreatePlane(Vector2 startPos)
            {
                this->position = startPos;
                this->velocity.x = 5;
                this->velocity.y = 0;
                Rectangle dest2 = {
                    this->position.x, 
                    this->position.y,
                    texture.width * .1f,
                    texture.height * .1f
                };
                std::cout << "On creation " << dest2.width << "\n\n\n\n\n\n\n";
                planeImages.emplace_back(dest2);
                
            }
            void DestroyPlane()
            {
                
            }
    EnemyPlane() { 
    }
};

Plane myPlane;
EnemyPlane plane2;
static std::vector<Bullet> bulletsInAir;


static Plane planes[10] = {myPlane};
static void InitGame(void);   
static void UpdateDrawFrame(); 
static void UpdateGame();  
static void GetMovement(); 
static void UpdatePlane(Plane& plane);
static void UpdateEnemyPlane(EnemyPlane& plane);
static void DrawGame();  


double DegToRad(double x)
{
    return ((x * 3.14)/180);
}


int main(void)
{
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "if you see this, gimme a job");
    texture = LoadTexture("pfiddyone2.png");
    textureFlip = LoadTexture("pfiddyone2flip.png");
    textureFlip2 = LoadTexture("pfiddyone2flip2.png");
    textureFlip3 = LoadTexture("pfiddyone2flip3.png");
    InitGame();
    // Image image = LoadImage("Untitled design.png");     // Loaded in CPU memory (RAM)
    // texture = LoadTextureFromImage(image); 


    
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
    plane2.CreatePlane({SCREEN_WIDTH/2, (SCREEN_HEIGHT/2) - 100});
    enemyPlanes.emplace_back(plane2);
    

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
            for(EnemyPlane& ep : enemyPlanes)
            {
                UpdateEnemyPlane(ep);
            }
            // UpdateEnemyPlane(plane2);
            int i = 0;
            for (Bullet& bullet : bulletsInAir) {
                if(!bullet.isDone)
                {
                    if(bullet.position.x > SCREEN_WIDTH || bullet.position.x < 0 || bullet.position.y > SCREEN_HEIGHT || bullet.position.y < 0)
                    {
                        bullet.isDone = true;
                    }
                    bullet.position.x += (cosf(DegToRad(bullet.angle)) * 8);
                    bullet.position.y -= (sinf(DegToRad(bullet.angle)) * 8);
                } else
                {
                    bulletsInAir.erase(bulletsInAir.begin() + i);
                }
                i++;
            }
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

int bulletCount = 0;

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
            myPlane.planeAngle += 3;
        else
           myPlane.planeAngle -= 3; 
    }
    if (IsKeyDown(KEY_A)) 
    {
        if(myPlane.planeAngle == 180)
            std::cout << "180" << '\n';
        else if(myPlane.planeAngle < 180)
            myPlane.planeAngle += 3;
        else
           myPlane.planeAngle -= 3; 
    }
    if (IsKeyDown(KEY_S)) 
    {
        if(myPlane.planeAngle == 270)
            std::cout << "270" << '\n';
        else if(myPlane.planeAngle <= 90 || myPlane.planeAngle > 270)
            myPlane.planeAngle -= 3;
        else
           myPlane.planeAngle += 3; 
    }
    if (IsKeyDown(KEY_D)) 
    {
        if(myPlane.planeAngle == 0)
            std::cout << "0" << '\n';
        else if(myPlane.planeAngle <= 180)
            myPlane.planeAngle -= 3;
        else
           myPlane.planeAngle += 3; 
    }
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        
        if(!myPlane.isShooting)
        {        
            bulletCount = 0;
            bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle));
            bulletCount++;
            myPlane.isShooting = true;
            myPlane.start = std::chrono::system_clock::now();
            myPlane.ct = std::chrono::system_clock::now();
        }
        std::cout << "count " << bulletCount <<'\n';
    }
    
    if(myPlane.isShooting && bulletCount < 4 && ((std::chrono::system_clock::now() - myPlane.ct) > std::chrono::milliseconds(50)))
    {
        std::cout << "in alt" << '\n';
        bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle));
        myPlane.ct = std::chrono::system_clock::now();
        bulletCount++;
    }
    myPlane.end = std::chrono::system_clock::now();
    if( (myPlane.end-myPlane.start) > std::chrono::seconds(2))
    {
        myPlane.isShooting = false;
    }
}


void UpdatePlane(Plane& plane)
{
    plane.position.x += (cosf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
    plane.position.y -= (sinf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
}

double angle = 0;
double x = 1;
int bulCount = 0;
void UpdateEnemyPlane(EnemyPlane& plane)
{
    if(!plane.isCrashed) {
        angle = atanf((myPlane.position.x - plane.position.x)/(-myPlane.position.y + plane.position.y));
        angle = (angle * 180)/3.14;
        if(myPlane.position.x - plane.position.x > 0)
        {
            if(-myPlane.position.y + plane.position.y >= 0)
            {
                angle = 90 - angle;
            } else if(-myPlane.position.y + plane.position.y < 0)
            {
                angle = -(-270 + angle);
            } 
        } else if(myPlane.position.x - plane.position.x < 0)
        {
            if(-myPlane.position.y + plane.position.y > 0)
            {
                angle = -(-90 + angle);
            } else if(-myPlane.position.y + plane.position.y < 0)
            {
                angle = 270 - angle;
            } else{
                
            }
        } else{
            if(-myPlane.position.y + plane.position.y > 0)
            {
                angle = 90;
            } else
            {
                angle = 270;
            }
        }
        if(angle > plane.planeAngle)
        {
            plane.planeAngle += 2;
        } else if(angle < plane.planeAngle)
        {
            plane.planeAngle -= 2;
        }
        plane.position.x += (cosf(DegToRad(plane.planeAngle)) * plane.velocity.x);
        plane.position.y -= (sinf(DegToRad(plane.planeAngle)) * plane.velocity.x);
    }
    else {
        if(plane.planeAngle >= 90 && plane.planeAngle <= 270)
        {
            if(plane.planeAngle != 270)
            {
                plane.planeAngle++;
            }
        } else
        {
            plane.planeAngle --;
        }
        plane.position.x += (cosf(DegToRad(plane.planeAngle)) * plane.velocity.x);
        plane.position.y -= (sinf(DegToRad(plane.planeAngle)) * (plane.velocity.x));
        x = x + .01;

    }
    
    if(!plane.isShooting)
    {        
        if(angle - plane.planeAngle <= 5 && angle - plane.planeAngle >= 0)
        {
            bulCount = 0;
            bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle));
            bulCount++;
            plane.isShooting = true;
            plane.start = std::chrono::system_clock::now();
            plane.ct = std::chrono::system_clock::now();
        }
    }
   
    if(plane.isShooting && bulCount < 4 && ((std::chrono::system_clock::now() - plane.ct) > std::chrono::milliseconds(50)))
    {
        std::cout << "in alt" << '\n';
        bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle));
        plane.ct = std::chrono::system_clock::now();
        bulCount++;
    }
    plane.end = std::chrono::system_clock::now();
    if( (plane.end-plane.start) > std::chrono::seconds(2))
    {
        plane.isShooting = false;
    }

}



void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        Rectangle source = {
            0.0f,
            0.0f,
            (float)texture.width,
            (float)texture.height
        };

        // Define where to draw it on screen and how big
        Rectangle dest = {
            myPlane.position.x, 
            myPlane.position.y,
            texture.width * .1f,
            texture.height * .1f
        };

        // Rectangle dest2 = {
        //     plane2.position.x, 
        //     plane2.position.y,
        //     texture.width * .1f,
        //     texture.height * .1f
        // };


        Vector2 origin = {
            dest.width/2,
            dest.height/2
        };

        if (!gameOver)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);

            std::string text = "Angle: -" + std::to_string(myPlane.planeAngle);

            DrawText(text.c_str(), 20, 20, 20, BLACK); 
            text = "shoot pew pew";
            //DrawRectangle(myPlane.position.x, myPlane.position.y, 100, 20, GRAY);

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
            int x = 0;
        //             Rectangle dest2 = {
        //     plane2.position.x, 
        //     plane2.position.y,
        //     texture.width * .1f,
        //     texture.height * .1f
        // };
            for(EnemyPlane& ep : enemyPlanes)
            {
                planeImages[x].x = ep.position.x;
                planeImages[x].y = ep.position.y;
                if(ep.planeAngle <= 90 || ep.planeAngle > 270) {
                    DrawTexturePro(texture, source, planeImages[x], origin, -ep.planeAngle, PINK);  // Draw a Texture2D with extended parameters
                }
                else
                {           
                    DrawTexturePro(textureFlip3, source, planeImages[x], origin, -ep.planeAngle, PINK);
                }
                x++;
            }
            if(myPlane.isShooting)
            {
                DrawText(text.c_str(), 200, 20, 20, BLACK); 

            }
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
            text = std::to_string(angle);
            DrawText(text.c_str(), 400, 20, 20, BLACK); 
            text = std::to_string(myPlane.planeAngle);
            DrawText(text.c_str(), 600, 20, 20, BLACK); 
            for (Bullet bullet : bulletsInAir) {
                if(!bullet.isDone)
                {
                    int y = 0;
                    for(EnemyPlane& ep : enemyPlanes)
                    {
                        if(CheckCollisionCircleRec(bullet.position, 5.0, planeImages[y]))
                        {
                            text = "HITTHETARGET";
                            DrawText(text.c_str(), 200, 200, 20, BLACK);
                            bullet.isDone = true;
                            ep.isCrashed = true;
                        }
                        y++;
                    }
                    DrawCircleV(bullet.position, 5.0f, RED);
                }
            }
        }
        else 
        {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }
    EndDrawing();
}



          