#include <iostream>
#include <raylib.h>

using namespace std;

Color Green = Color{ 38, 185, 154, 255};
Color Dark_Green = Color{ 20, 160, 133, 255};
Color Light_Green = Color{ 129, 204, 184, 255};
Color Yellow = Color{ 243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;
int framerate = 144;

int frames_since_score = 0;
float speedIncrease = 0.25;


class Ball{
    public:
        float x, y;
        float speed_x, speed_y;
        int radius;
        
        void draw(){
            DrawCircle(x, y, radius, Yellow);
        }

        void Update(Sound goalSound) {
            x += speed_x;
            y += speed_y;
            if (y + radius >= GetScreenHeight() || y - radius <= 0)
            {
                speed_y *= -1;
            }
            if (x + radius >= GetScreenWidth())
            {
                player_score++;
                Reset(goalSound);
            }
            if (x - radius <= 0)
            {
                cpu_score++;
                Reset(goalSound);
            }
        }
        void Reset(Sound goalSound) {
            PlaySound(goalSound);
            x = GetScreenWidth() / 2;
            y = GetScreenHeight() / 2;
            frames_since_score = 0;
        }
};

class Paddle {
    public:
        float x, y;
        float width, height;
        float speed;
        void draw() 
        {
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
        }
        void Update() {
            if (IsKeyDown(KEY_W) && y > 15) {
                y -= speed;
            }
            if (IsKeyDown(KEY_S) && y + height < GetScreenHeight() - 15) {
                y += speed;
            }
        }
};

class CpuPaddle: public Paddle{
    public:
        int minimum_distance;
        void Update(int ball_y) {

            if (y + height / 2 > ball_y + minimum_distance&& y > 15) {
                y -= speed;
            }
            if (y + height / 2 < ball_y  - minimum_distance&& y + height < GetScreenHeight() - 15) {
                y += speed;
            }
        }
};

Ball ball;
Paddle player;
CpuPaddle cpu;


int main () {
    cout << "starting the game" << endl;
    const int screen_Width = 1280;
    const int screen_Height = 720;
    InitWindow(screen_Width, screen_Height, "Pong");
    InitAudioDevice();
    SetTargetFPS(framerate);

    Sound hitSound = LoadSound("assets/hitsound.wav");
    Sound goalSound = LoadSound("assets/goal.wav");
    
    SetSoundVolume(hitSound, 0.5f);
    SetSoundVolume(goalSound, 0.25f);

    ball.radius = 20;
    ball.x = screen_Width / 2;
    ball.y = screen_Height / 2;
    ball.speed_x = 5;
    ball.speed_y = 5;
    ball.draw();

    player.width = 25;
    player.height = 120;
    player.x = 10;
    player.y = screen_Height / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = screen_Width - 10 - cpu.width;
    cpu.y = screen_Height / 2 - cpu.height / 2;
    cpu.speed = 5;
    cpu.minimum_distance = 40;
    
    

    // Main game loop
    while(WindowShouldClose() == false) {

        BeginDrawing();

        // Update
        ball.Update(goalSound);
        player.Update();
        cpu.Update(ball.y);
    
        // freeze the ball for second after scoring
        if (frames_since_score < framerate )
        {
            ball.speed_x = 0;
            ball.speed_y = 0;
        } else if (frames_since_score  == framerate)
        {
            int choices[] = {1, -1};
            ball.speed_x = 5 * choices[GetRandomValue(0, 1)];
            ball.speed_y = 5 * choices[GetRandomValue(0, 1)];
        } 
        
        frames_since_score++;
        // Check for collision with player paddle
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speed_x *= -1;
            if (ball.speed_x > 0)
            {
                ball.speed_x += speedIncrease;
            }
            else
            {
                ball.speed_x -= speedIncrease;
            }
            if (ball.speed_y > 0)
            {
                ball.speed_y += speedIncrease;
            }
            else
            {
                ball.speed_y -= speedIncrease;
            }
            PlaySound(hitSound);
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x *= -1;
            if (ball.speed_x > 0)
            {
                ball.speed_x += speedIncrease;
            }
            else
            {
                ball.speed_x -= speedIncrease;
            }
            if (ball.speed_y > 0)
            {
                ball.speed_y += speedIncrease;
            }
            else
            {
                ball.speed_y -= speedIncrease;
            }
            PlaySound(hitSound);
        }

        // Draw
        ClearBackground(Dark_Green);
        DrawRectangle(screen_Width / 2, 0, screen_Width/ 2, screen_Height, Green);
        DrawCircle(screen_Width / 2, screen_Height / 2, 100, Light_Green);
        DrawLine(screen_Width / 2, 0, screen_Width / 2, screen_Height, WHITE);
        ball.draw();
        player.draw();
        cpu.draw();

        DrawText(TextFormat("%i", player_score), screen_Width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", cpu_score), 3 *  screen_Width / 4 - 20 , 20, 80, WHITE);
        DrawText(TextFormat("Ball Speed: %.1f", abs(ball.speed_x) ), screen_Width / 2 - 60  , 20, 30, WHITE);
        EndDrawing();

    } 
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}