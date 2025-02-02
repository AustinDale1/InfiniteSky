#include "raylib.h"

#include <iostream>
#include <algorithm>
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
#include <iomanip>


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


static std::vector<Rectangle> planeImages;
class Plane;
static std::vector<Plane> enemyPlanes;

class Plane {
    public:          
        Vector2 position;
        Vector2 velocity; 
        double planeAngle = 0.0f;
        bool isCrashed = false;
        bool isShooting = false;
        std::chrono::time_point<std::chrono::system_clock> start, end, ct;
        std::chrono::time_point<std::chrono::system_clock> newStart, newEnd;
        std::chrono::duration<double> dur{5.0};
        Rectangle planeImage;
        int index;
        bool him = false;
        int health = 20;
        bool init = false;
        void CreatePlane(Vector2 startPos, int counter)
        {
            index = counter;
            this->position = startPos;
            this->velocity.x = 5;
            this->velocity.y = 0;
            planeImage = {
                    this->position.x, 
                    this->position.y,
                    texture.width * .1f,
                    texture.height * .1f
                };
            planeImages.emplace_back(planeImage);
        }

    Plane() { 
    }
};

class Bullet {
   public:  
        Vector2 position;        
        double angle = 0.0f;
        bool isDone = false;
        Plane shotBy;
        
    Bullet(Vector2 pos, double planeAngle, Plane fired) {
        position = pos;
        angle = planeAngle;
        shotBy = fired;
    } 
};

Plane myPlane;
Plane plane2;
static std::vector<Bullet> bulletsInAir;
int counter = 0;
int score = 0;


static Plane planes[10] = {myPlane};
static void InitGame(void);
static void RestartGame(void);
static void UpdateDrawFrame(); 
static void UpdateGame();  
static void GetMovement(); 
static void UpdatePlane(Plane& plane);
static void UpdateEnemyPlane(Plane& plane);
static void DrawGame();
static Vector2 RotatePoint(Vector2& point, Vector2& center, float angle);
static Rectangle RotateRectangle(Rectangle& rect, float angle);


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
    myPlane.CreatePlane({SCREEN_WIDTH/2, SCREEN_HEIGHT/2}, counter);
    myPlane.him = true;
    counter++;
    plane2.CreatePlane({SCREEN_WIDTH/2, (SCREEN_HEIGHT/2) - 100}, counter);
    enemyPlanes.emplace_back(plane2);
    

}

void RestartGame(void)
{
    myPlane.position = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    myPlane.health = 20;
    myPlane.isShooting = false;
    myPlane.init = false;
    myPlane.isCrashed = false;
    myPlane.planeAngle = 0.0f;
    for(Plane& ep : enemyPlanes)
    {
        ep.position = {SCREEN_WIDTH/2, (SCREEN_HEIGHT/2) - 100};
        ep.health = 20;
        ep.isShooting = false;
        ep.init = false;
        ep.isCrashed = false;
        ep.planeAngle = 0.0f;
    }
    score = 0;
    bulletsInAir.clear();

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
            for(Plane& ep : enemyPlanes)
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
            RestartGame();
            std::cout << "restarting game" << '\n';
            gameOver = false;
        }
    }
}

int bulletCount = 0;
bool initialized = false;

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
    if(!myPlane.isCrashed)
    {
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
    }
    // if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    // {
        
    //     if(!myPlane.isShooting)
    //     {        
    //         bulletCount = 0;
    //         bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
    //         bulletCount++;
    //         myPlane.isShooting = true;
    //         myPlane.start = std::chrono::system_clock::now();
    //         myPlane.ct = std::chrono::system_clock::now();
    //     }
    //     std::cout << "count " << bulletCount <<'\n';
    // }
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        // std::cout << "oy " << myPlane.dur.count() << '\n';
        if(myPlane.dur > std::chrono::seconds(0))
        {
            if(!myPlane.isShooting)
            {
                myPlane.newStart = std::chrono::system_clock::now();
                bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
                myPlane.isShooting = true;

            } else
            {
                bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
                myPlane.newEnd = std::chrono::system_clock::now();
                
                myPlane.dur = myPlane.dur - (std::chrono::system_clock::now() - myPlane.newStart);
            }
        }
    } else {
        if(myPlane.isShooting)
        {
            myPlane.isShooting = false;
            myPlane.newEnd = std::chrono::system_clock::now();
            myPlane.init = true;
            // std::cout << "Do we ever get here?";
        }
        // std::cout << initialized << '\n';
        if(myPlane.dur <= std::chrono::seconds(5) && myPlane.init)
        {
            // std::cout << "here? or here" ;
            //std::string frickml = std::to_string((std::chrono::system_clock::now() - myPlane.newEnd).count());
            // std::cout << "huh " << (std::chrono::system_clock::now() - myPlane.newEnd).count();
            myPlane.dur = myPlane.dur + (std::chrono::system_clock::now() - myPlane.newEnd);
        }  
    }
    
    // if(myPlane.isShooting && bulletCount < 5 && ((std::chrono::system_clock::now() - myPlane.ct) > std::chrono::milliseconds(50)))
    // {
    //     std::cout << "in alt" << '\n';
    //     bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
    //     myPlane.ct = std::chrono::system_clock::now();
    //     bulletCount++;
    // }
    // myPlane.end = std::chrono::system_clock::now();
    // if( (myPlane.end-myPlane.start) > std::chrono::seconds(2))
    // {
    //     myPlane.isShooting = false;
    // }
}


void UpdatePlane(Plane& plane)
{
    if(plane.isCrashed && plane.position.y > SCREEN_HEIGHT)
    {
        gameOver = true;
    }
    if(myPlane.planeAngle >= 360)
    {
        myPlane.planeAngle = std::fmod(myPlane.planeAngle, 360);
    }
    if(myPlane.planeAngle < 0)
    {
        myPlane.planeAngle = 360 + myPlane.planeAngle;
    }
    if(!plane.isCrashed)
    {
        plane.position.x += (cosf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
        plane.position.y -= (sinf(DegToRad(myPlane.planeAngle)) * plane.velocity.x);
    } else {
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

    }
}

double angle = 0;
double x = 1;
int bulCount = 0;
void UpdateEnemyPlane(Plane& plane)
{
    if(plane.planeAngle >= 360)
    {
        plane.planeAngle = std::fmod(plane.planeAngle, 360);
    }
    if(plane.planeAngle < 0)
    {
        plane.planeAngle = 360 + plane.planeAngle;
    }
    if(plane.isCrashed && plane.position.y > SCREEN_HEIGHT)
    {
        plane.isCrashed = false;
        plane.health = 20;
        plane.position = {-200, 100};
    }
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
    if(angle - plane.planeAngle <= 5 && angle - plane.planeAngle >= 0)
    {
        if(plane.dur > std::chrono::seconds(0))
        {
            if(!plane.isShooting)
            {
                plane.newStart = std::chrono::system_clock::now();
                // std::cout << "Started" << '\n';
                bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle, plane));
                plane.isShooting = true;
            } else
            {
                // if(bulletCount % 10 == 0)
                // {
                    // std::cout << "End" << '\n';
                    bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle, plane));
                    plane.newEnd = std::chrono::system_clock::now();
                    // std::cout << "1 " << plane.dur.count() << '\n';
                    plane.dur = plane.dur - (std::chrono::system_clock::now() - plane.newStart);
                    // std::cout << "1 " << plane.dur.count() << '\n';
                // }
                bulletCount++;

            }
        }
    } else 
    {
        if(plane.isShooting)
        {
            if(std::chrono::system_clock::now() - plane.newStart > std::chrono::seconds(1)) {
                std::cout << "Do you stop shooting? " << '\n';
                plane.isShooting = false;
                plane.newEnd = std::chrono::system_clock::now();
                plane.init = true;
            } else {
                std::cout << "Do you go here often" << '\n';
            }
        }
        if(plane.dur <= std::chrono::seconds(5) && plane.init)//This used to be if(plane.dur <= std::chrono::seconds(5) && initialized)??  dunno what that's about
        {
            plane.dur = plane.dur + (std::chrono::system_clock::now() - plane.newEnd);
        }  
    }





    // if(!plane.isShooting)
    // {        
    //     if(angle - plane.planeAngle <= 5 && angle - plane.planeAngle >= 0 && plane.dur > std::chrono::seconds(0))
    //     {
    //         myPlane.newStart = std::chrono::system_clock::now();
    //         bulCount = 0;
    //         bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle, plane));
    //         bulCount++;
    //         plane.isShooting = true;
    //         plane.start = std::chrono::system_clock::now();
    //         plane.ct = std::chrono::system_clock::now();
    //     } else{
    //         myPlane.newEnd = std::chrono::system_clock::now();
    //     }
    // }

    // if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    // {
    //     // std::cout << "oy " << myPlane.dur.count() << '\n';
    //     if(myPlane.dur > std::chrono::seconds(0))
    //     {
    //         if(!myPlane.isShooting)
    //         {
    //             myPlane.newStart = std::chrono::system_clock::now();
    //             bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
    //             myPlane.isShooting = true;

    //         } else
    //         {
    //             bulletsInAir.emplace_back(Bullet(myPlane.position, myPlane.planeAngle, myPlane));
    //             myPlane.newEnd = std::chrono::system_clock::now();
                
    //             myPlane.dur = myPlane.dur - (std::chrono::system_clock::now() - myPlane.newStart);
    //         }
    //     }
    // } else {
    //     if(myPlane.isShooting)
    //     {
    //         myPlane.isShooting = false;
    //         myPlane.newEnd = std::chrono::system_clock::now();
    //         initialized = true;
    //         // std::cout << "Do we ever get here?";
    //     }
    //     // std::cout << initialized << '\n';
    //     if(myPlane.dur <= std::chrono::seconds(5) && initialized)
    //     {
    //         // std::cout << "here? or here" ;
    //         //std::string frickml = std::to_string((std::chrono::system_clock::now() - myPlane.newEnd).count());
    //         // std::cout << "huh " << (std::chrono::system_clock::now() - myPlane.newEnd).count();
    //         myPlane.dur = myPlane.dur + (std::chrono::system_clock::now() - myPlane.newEnd);
    //     }  
    // }
   
    // if(plane.isShooting && bulCount < 5 && ((std::chrono::system_clock::now() - plane.ct) > std::chrono::milliseconds(50)))
    // {
    //     std::cout << "in alt" << '\n';
    //     bulletsInAir.emplace_back(Bullet(plane.position, plane.planeAngle, plane));
    //     plane.ct = std::chrono::system_clock::now();
    //     bulCount++;
    // }
    // plane.end = std::chrono::system_clock::now();
    // if( (plane.end-plane.start) > std::chrono::seconds(2))
    // {
    //     plane.isShooting = false;
    // }

}

int counter2 = 0;

Vector2 RotatePoint(Vector2& point, Vector2& center, float angle) {
    // Convert angle to radians
    float rad = angle * DEG2RAD;
    
    // Translate point to origin
    float translatedX = point.x - center.x;
    float translatedY = point.y - center.y;
    
    // Rotate point
    float rotatedX = translatedX * cos(rad) - translatedY * sin(rad);
    float rotatedY = translatedX * sin(rad) + translatedY * cos(rad);
    
    // Translate point back
    return {
        rotatedX + center.x, 
        rotatedY + center.y
    };
}

Rectangle RotateRectangle(Rectangle& rect, float angle) {
    // Calculate rectangle center
    Vector2 center = {
        rect.x + rect.width / 2.0f, 
        rect.y + rect.height / 2.0f
    };
    
    // Define the four corners of the rectangle
    Vector2 topLeft = {rect.x, rect.y};
    Vector2 topRight = {rect.x + rect.width, rect.y};
    Vector2 bottomRight = {rect.x + rect.width, rect.y + rect.height};
    Vector2 bottomLeft = {rect.x, rect.y + rect.height};
    
    // Rotate each point around the center
    topLeft = RotatePoint(topLeft, center, angle);
    topRight = RotatePoint(topRight, center, angle);
    bottomRight = RotatePoint(bottomRight, center, angle);
    bottomLeft = RotatePoint(bottomLeft, center, angle);
    
    // Find new bounding box
    float minX = std::min({topLeft.x, topRight.x, bottomRight.x, bottomLeft.x});
    float maxX = std::max({topLeft.x, topRight.x, bottomRight.x, bottomLeft.x});
    float minY = std::min({topLeft.y, topRight.y, bottomRight.y, bottomLeft.y});
    float maxY = std::max({topLeft.y, topRight.y, bottomRight.y, bottomLeft.y});
    
    // Update rectangle
    rect.x = minX;
    rect.y = minY;
    rect.width = maxX - minX;
    rect.height = maxY - minY;
    return rect;
}

static int counterr = 0;

void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        Rectangle r = {
            200, 
            200,
            20,
            20
            };
            counterr++;
            // RotateRectangle(r, 1);
            //r.
            // DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            // DrawRectangleRec(RotateRectangle(r, counterr), GREEN);
            
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
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {135, 206, 255, 255});

            std::string text = "Score: " + std::to_string(score);

            DrawText(text.c_str(), 20, 20, 20, BLACK); 
            
            text = "Health: " + std::to_string(myPlane.health);
            DrawText(text.c_str(), 1800, 20, 20, BLACK); 
            text = "Bullets: " + std::to_string(myPlane.dur.count());
            DrawText(text.c_str(), 1800, 120, 20, BLACK);

            for(Plane loopEP : enemyPlanes) {
                if(loopEP.health > 0) {
                    text = "EnemyHealth: " + std::to_string(loopEP.health);
                    DrawText(text.c_str(), 2000, 20, 20, BLACK); 
                    text = "EnemyBullets: " + std::to_string(loopEP.dur.count());
                    DrawText(text.c_str(), 2000, 120, 20, BLACK); 

                    // text = "newstart: " + std::to_string(plane2.newStart);
                    // DrawText(text.c_str(), 2000, 220, 20, BLACK); 
                    // text = "newend: " + std::to_string(plane2.newEnd);
                    // DrawText(text.c_str(), 2000, 320, 20, BLACK); 
                    // text = "now: " + std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());
                    // DrawText(text.c_str(), 2000, 420, 20, BLACK); 
                }
            }
  


            text = "shoot pew pew";
            DrawRectangleRec((Rectangle){dest.x, dest.y, dest.width, dest.height}, RED);

            if(myPlane.planeAngle <= 90 || myPlane.planeAngle > 270)
                DrawTexturePro(texture, source, dest, origin, -myPlane.planeAngle, WHITE);  // Draw a Texture2D with extended parameters
            else
            {           
                DrawTexturePro(textureFlip3, source, dest, origin, -myPlane.planeAngle, WHITE);
            }
            int x = 0;

            for(Plane& ep : enemyPlanes)
            {
                DrawRectangleRec(planeImages[x], RED);
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
                text = std::to_string(static_cast<int>(ep.dur.count()));
            }

            DrawText(text.c_str(), 200, 20, 20, BLACK); 
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
            text = std::to_string(angle);
            DrawText(text.c_str(), 400, 20, 20, BLACK); 
            text = std::to_string(myPlane.planeAngle);
            DrawText(text.c_str(), 600, 20, 20, BLACK); 
            for (Bullet bullet : bulletsInAir) {
                if(!bullet.isDone)
                {
                    int y = 0;
                    for(Plane& ep : enemyPlanes)
                    {
                        if(CheckCollisionCircleRec(bullet.position, 5.0, planeImages[y]) && (bullet.shotBy.index != ep.index))
                        {
                            ep.health--;
                            if(bullet.shotBy.him && !ep.isCrashed && ep.health <= 0)
                            {
                                myPlane.health += 5;
                                score++;
                                ep.isCrashed = true;
                            }
                            text = "HITTHETARGET";
                            DrawText(text.c_str(), 200, 200, 20, BLACK);
                            bullet.isDone = true;

                            text = std::to_string(planes->planeAngle);
                        }
                        y++;
                    }
                    if(CheckCollisionCircleRec(bullet.position, 5.0, dest) && (bullet.shotBy.index != myPlane.index))
                    {
                        counter2++;
                        //std::cout << "ok " << bullet.shotBy.name << " " << myPlane.name << '\n';
                        myPlane.health--;
                        if(bullet.isDone)
                        {
                            gameOver = true;
                        }

                        if(myPlane.health <= 0)
                        {
                            myPlane.isCrashed = true;
                        }
                        // myPlane.isCrashed = true;
                        bullet.isDone = true;
                    }
                    DrawCircleV(bullet.position, 3.0f, GRAY);
                }
            }
            //text = myPlane.dur;
            //text = "maybe";
            text = std::to_string(myPlane.health);
            DrawText(text.c_str(), 800, 20, 20, BLACK); 
        }
        else 
        {
            // Rectangle r = {
            // 200, 
            // 200,
            // 20,
            // 20
            // };
            // counterr++;
            // std::cout << "IT GOT HERE " << '\n';
            // // RotateRectangle(r, 1);
            // //r.
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            // DrawRectangleRec(RotateRectangle(r, counter), GREEN);

        }
    EndDrawing();
}



          