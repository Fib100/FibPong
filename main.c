#include "raylib.h"
#include "raymath.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define NUM_SOUNDS 7

struct Paddle
{
    Rectangle rect;
    float speed;
    Color color;
};

struct Ball
{
    Vector2 position;
    float speed;
    float radius;
    Vector2 direction;
    Color color;
};

const int screenWidth = 1280;
const int screenHeight = 720;
struct Paddle leftPaddle = { 0 };
struct Paddle rightPaddle = { 0 };
struct Ball ball = { 0 };
Color primaryColor = (Color){ 251, 191, 0, 255 };
float startTimer = 0.0f;
bool startTimerIsRunning = false;
unsigned int bounceCount = 1;
unsigned int numSpeedIncreases = 0;

// Balancing values
const unsigned int winMax = 7;
const float ballSpeedMax = 800.0f;
const float startTimeMax = 3.0f;

// Returns a value of 1 or -1 ranodmly
int GetRandomSign()
{
    int result = 1;
    int randVal = GetRandomValue(-10, 9);

    if (randVal < 0)
    {
        result = -1;
    }

    return result;
}

// Resets ball after scoring
void ResetBall()
{
    ball.position = (Vector2) { (float)screenWidth / 2.0f, (float)GetRandomValue(20, screenHeight - 20) };
    ball.direction.x = 1.0f;
    ball.direction.y = -1.0f;
    ball.direction = Vector2Normalize(ball.direction);
    ball.direction.x *= GetRandomSign();
    ball.direction.y *= GetRandomSign();
    ball.speed = 0.0f;
    bounceCount = 1;
    numSpeedIncreases = 0;
    startTimer = startTimeMax;
    startTimerIsRunning = true;
}

// Returns the new position after moving
Vector2 MoveGameObject(Vector2 currentPosition, Vector2 direction, float speed, float dt)
{
    return Vector2Add(currentPosition, Vector2Scale(direction, speed * dt));
}

int main()
{
    bool shouldExitGameLoop = false;
    bool isExitRequested = false;
    bool hasSceneInitialized = false;
    unsigned int gameOverState = 0; // 0 = playing, 1 = left wins, 2 = right wins
    float dt = 0.0f;
    unsigned int leftScore = 0;
    unsigned int rightScore = 0;
    int lock = 0;

    InitWindow(screenWidth, screenHeight, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    SetRandomSeed((unsigned int)time(nullptr));

    Music bgMusic = LoadMusicStream("assets/midnight_drive.ogg");
    bgMusic.looping = true;
    SetMusicVolume(bgMusic, 0.4f);
    PlayMusicStream(bgMusic);

    Sound sounds[NUM_SOUNDS];
    sounds[0] = LoadSound("assets/ballHitPaddle.wav");
    sounds[1] = LoadSound("assets/ballHitWall.wav");
    sounds[2] = LoadSound("assets/countdown1.wav");
    sounds[3] = LoadSound("assets/countdown2.wav");
    sounds[4] = LoadSound("assets/lose.wav");
    sounds[5] = LoadSound("assets/restart.wav");
    sounds[6] = LoadSound("assets/ballStart.wav");

    while (!shouldExitGameLoop)
    {
        // Update music stream buffers
        UpdateMusicStream (bgMusic);

        // Initialize scene
        if (!hasSceneInitialized)
        {
            hasSceneInitialized = true;

            leftPaddle =
                (struct Paddle){ .rect = (Rectangle){ .x = 100.0f, .y = 360.0f - 54.0f, .width = 12.0f, .height = 140.0f },
                .speed = 600.0f,
                .color = primaryColor};
                
            rightPaddle =
                (struct Paddle){ .rect = (Rectangle){ .x = (float)screenWidth - 100.0f, .y = 360.0f - 54.0f, .width = 12.0f, .height = 140.0f },
                .speed = 600.0f,
                .color = primaryColor};
                
            ball = (struct Ball){ .position = (Vector2){ .x = (float)screenWidth / 2.0f, .y = (float)screenHeight / 2.0f },
                .speed = 0.0f, .radius = 12.0f,
                .direction = (Vector2){ .x = 1.0f, .y = -1.0f },
                .color = primaryColor};
                
            ResetBall();
        }

        // = Update =============================================================
        dt = GetFrameTime();

        if (IsKeyPressed(KEY_F1))
        {
            ToggleFullscreen();
        }

        // Wait after game over to show who wins
        if (startTimerIsRunning && gameOverState != 0)
        {
            startTimer -= dt;
            if (startTimer <= 0.0f)
            {
                startTimerIsRunning = false;
                startTimer = 0.0f;
                
                gameOverState = 0;
                startTimer = startTimeMax;
                startTimerIsRunning = true;
                PlaySound(sounds[5]);
                if (!IsMusicStreamPlaying(bgMusic))
                {
                    PlayMusicStream(bgMusic);
                }
            }
        }

        // Wait a few seconds at start
        if (startTimerIsRunning && gameOverState == 0)
        {
            startTimer -= dt;

            // Play countdown timer
            if (startTimer <= (startTimeMax) / 3 * 2 && lock == 0)
            {
                PlaySound(sounds[2]);
                lock += 1;
            }

            if (startTimer <= (startTimeMax) / 3 && lock == 1)
            {
                PlaySound(sounds[2]);
                lock += 1;
            }

            // Timer has expired
            if (startTimer <= 0.0f)
            {
                startTimerIsRunning = false;
                startTimer = 0.0f;
                lock = 0;
                
                ball.speed = ballSpeedMax / 2.0f;

                PlaySound(sounds[3]);
                PlaySound(sounds[6]);
            }
        }

        // Left paddle movement
        if (IsKeyDown(KEY_W))
        {
            Vector2 leftPaddlePos = (Vector2){ leftPaddle.rect.x, leftPaddle.rect.y };
            leftPaddlePos = MoveGameObject(leftPaddlePos, (Vector2){ 0.0f, -1.0f }, leftPaddle.speed, dt);
            leftPaddle.rect.x = leftPaddlePos.x;
            leftPaddle.rect.y = leftPaddlePos.y;

            leftPaddle.rect.y = fmaxf(leftPaddle.rect.y, 20.0f);
        }

        if (IsKeyDown(KEY_S))
        {
            Vector2 leftPaddlePos = (Vector2){ leftPaddle.rect.x, leftPaddle.rect.y };
            leftPaddlePos = MoveGameObject(leftPaddlePos, (Vector2){ 0.0f, 1.0f }, leftPaddle.speed, dt);
            leftPaddle.rect.x = leftPaddlePos.x;
            leftPaddle.rect.y = leftPaddlePos.y;

            if ((leftPaddle.rect.y + leftPaddle.rect.height) > (float)screenHeight - 20.0f)
            {
                leftPaddle.rect.y = ((float)screenHeight - 20.0f) - leftPaddle.rect.height;
            }
        }

        // Right paddle movementd
        if (IsKeyDown(KEY_UP))
        {
            Vector2 rightPaddlePos = (Vector2){ rightPaddle.rect.x, rightPaddle.rect.y };
            rightPaddlePos = MoveGameObject(rightPaddlePos, (Vector2){ 0.0f, -1.0f }, rightPaddle.speed, dt);
            rightPaddle.rect.x = rightPaddlePos.x;
            rightPaddle.rect.y = rightPaddlePos.y;

            rightPaddle.rect.y = fmaxf(rightPaddle.rect.y, 20.0f);
        }

        if (IsKeyDown(KEY_DOWN))
        {
            Vector2 rightPaddlePos = (Vector2){ rightPaddle.rect.x, rightPaddle.rect.y };
            rightPaddlePos = MoveGameObject(rightPaddlePos, (Vector2){ 0.0f, 1.0f }, rightPaddle.speed, dt);
            rightPaddle.rect.x = rightPaddlePos.x;
            rightPaddle.rect.y = rightPaddlePos.y;

            if ((rightPaddle.rect.y + rightPaddle.rect.height) > (float)screenHeight - 20.0f)
            {
                rightPaddle.rect.y = ((float)screenHeight - 20.0f) - rightPaddle.rect.height;
            }
        }

        // Ball boounce - top
        if (ball.position.y < 0.0f)
        {
            ball.position.y = 0.0f;
            ball.direction.y = 1.0f;
            PlaySound(sounds[1]);
        }

        // Ball boounce - bottom
        if (ball.position.y > (float)screenHeight)
        {
            ball.position.y = (float)screenHeight;
            ball.direction.y = -1.0f;
            PlaySound(sounds[1]);
        }

        // Ball boounce - left paddle
        if (CheckCollisionCircleRec(ball.position, ball.radius, leftPaddle.rect))
        {
            ball.direction.x = 1.0f;
            ball.speed = ballSpeedMax;
            bounceCount += 1;
            PlaySound(sounds[0]);
        }

        // Ball boounce - right paddle
        if (CheckCollisionCircleRec(ball.position, ball.radius, rightPaddle.rect))
        {
            ball.direction.x = -1.0f;
            ball.speed = ballSpeedMax;
            bounceCount += 1;
            PlaySound(sounds[0]);
        }

        if (bounceCount % 10 == 0)
        {
            numSpeedIncreases += 1;
            bounceCount = 1;
        }

        // Ball movement
        ball.direction = Vector2Normalize(ball.direction);
        ball.position = MoveGameObject(ball.position, ball.direction, ball.speed + (float)(numSpeedIncreases * 150), dt);

        // Scoring - left paddle loses
        if (ball.position.x < -32.0f)
        {
            rightScore += 1;
            ResetBall();
            PlaySound(sounds[4]);
        }

        // Scoring - right paddle loses
        if (ball.position.x > (float)screenWidth + 32.0f)
        {
            leftScore += 1;
            ResetBall();
            PlaySound(sounds[4]);
        }

        // Check game over state
        if (leftScore >= winMax)
        {
            gameOverState = 1;
            startTimerIsRunning = false;
            leftScore = 0;
            rightScore = 0;
        }

        if (rightScore >= winMax)
        {
            gameOverState = 2;
            startTimerIsRunning = false;
            leftScore = 0;
            rightScore = 0;
        }

        if (gameOverState != 0 && !startTimerIsRunning)
        {
            StopMusicStream(bgMusic);
            startTimer = startTimeMax * 2.0f;
            startTimerIsRunning = true;
        }

        // = Draw =============================================================
        BeginDrawing();
            ClearBackground((Color){ 20, 20, 20, 255});

            // Draw BG
            DrawLineDashed((Vector2){ .x = (float)screenWidth / 2.0f, .y = 0.0f }, (Vector2){ .x = (float)screenWidth / 2.0f, (float)screenHeight }, 32, 16, primaryColor);
            DrawLineDashed((Vector2){ .x = ((float)screenWidth / 2.0f) + 1.0f, .y = 0.0f }, (Vector2){ .x = (float)screenWidth / 2.0f, (float)screenHeight }, 32, 16, primaryColor);
            DrawLineEx((Vector2){ 0.0f, 2.0f }, (Vector2){ (float)screenWidth, 2.0f }, 4.0f, primaryColor);
            DrawLineEx((Vector2){ 0.0f, (float)screenHeight - 4.0f }, (Vector2){ (float)screenWidth, (float)screenHeight - 4.0f }, 4.0f, primaryColor);
            
            // Draw paddles
            DrawRectangleRec(leftPaddle.rect, leftPaddle.color);
            DrawRectangleRec(rightPaddle.rect, rightPaddle.color);

            // Draw ball
            DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color);

            // Draw score
            char buffer[16];
            sprintf_s(buffer, 16, "%02i", leftScore);
            DrawText(buffer, screenWidth / 2 - 112, 20, 48, primaryColor);
            memset(buffer, 0, 16);
            sprintf_s(buffer, 16, "%02i", rightScore);
            DrawText(buffer, (screenWidth / 2) + 64, 20, 48, primaryColor);

            if (gameOverState == 1)
            {
                DrawText("Left wins!", (screenWidth / 2) - 120, 200, 48, primaryColor);
            }

            if (gameOverState == 2)
            {
                DrawText("Right wins!", (screenWidth / 2) - 120, 200, 48, primaryColor);
            }

        EndDrawing();

        if (WindowShouldClose() || isExitRequested)
            shouldExitGameLoop = true;
    }

    for (int i = 0; i < NUM_SOUNDS; i++)
    {
        UnloadSound(sounds[i]);
    }
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
