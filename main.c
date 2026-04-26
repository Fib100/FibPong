#include "raylib.h"
#include "raymath.h"
#include <time.h>
#include <math.h>

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

// Balancing values
const unsigned int winMax = 1;
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
    ball.position = (Vector2) { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    ball.direction.x = 1.0f;
    ball.direction.y = -1.0f;
    ball.direction = Vector2Normalize(ball.direction);
    ball.direction.x *= GetRandomSign();
    ball.speed = 0.0f;
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

    InitWindow(screenWidth, screenHeight, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    SetRandomSeed((unsigned int)time(nullptr));

    while (!shouldExitGameLoop)
    {
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
            }
        }

        // Wait a few seconds at start
        if (startTimerIsRunning && gameOverState == 0)
        {
            startTimer -= dt;
            if (startTimer <= 0.0f)
            {
                startTimerIsRunning = false;
                startTimer = 0.0f;
                
                ball.speed = ballSpeedMax / 2.0f;
            }
        }

        // Left paddle movement
        if (IsKeyDown(KEY_W))
        {
            Vector2 leftPaddlePos = (Vector2){ leftPaddle.rect.x, leftPaddle.rect.y };
            leftPaddlePos = MoveGameObject(leftPaddlePos, (Vector2){ 0.0f, -1.0f }, leftPaddle.speed, dt);
            leftPaddle.rect.x = leftPaddlePos.x;
            leftPaddle.rect.y = leftPaddlePos.y;

            if (leftPaddle.rect.y < 20.0f)
            {
                leftPaddle.rect.y = 20.0f;
            }
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

        // Right paddle movement
        if (IsKeyDown(KEY_UP))
        {
            Vector2 rightPaddlePos = (Vector2){ rightPaddle.rect.x, rightPaddle.rect.y };
            rightPaddlePos = MoveGameObject(rightPaddlePos, (Vector2){ 0.0f, -1.0f }, rightPaddle.speed, dt);
            rightPaddle.rect.x = rightPaddlePos.x;
            rightPaddle.rect.y = rightPaddlePos.y;

            if (rightPaddle.rect.y < 20.0f)
            {
                rightPaddle.rect.y = 20.0f;
            }
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
        }

        // Ball boounce - bottom
        if (ball.position.y > (float)screenHeight)
        {
            ball.position.y = (float)screenHeight;
            ball.direction.y = -1.0f;
        }

        // Ball boounce - left paddle
        if (CheckCollisionCircleRec(ball.position, ball.radius, leftPaddle.rect))
        {
            ball.direction.x = 1.0f;
            ball.speed = ballSpeedMax;
        }

        // Ball boounce - right paddle
        if (CheckCollisionCircleRec(ball.position, ball.radius, rightPaddle.rect))
        {
            ball.direction.x = -1.0f;
            ball.speed = ballSpeedMax;
        }

        // Ball movement
        ball.direction = Vector2Normalize(ball.direction);
        ball.position = MoveGameObject(ball.position, ball.direction, ball.speed, dt);

        // Scoring - left paddle loses
        if (ball.position.x < -32.0f)
        {
            rightScore += 1;
            ResetBall();
        }

        // Scoring - right paddle loses
        if (ball.position.x > (float)screenWidth + 32.0f)
        {
            leftScore += 1;
            ResetBall();
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
            DrawText(TextFormat("%02i", leftScore), screenWidth / 2 - 112, 20, 48, primaryColor);
            DrawText(TextFormat("%02i", rightScore), (screenWidth / 2) + 64, 20, 48, primaryColor);

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

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
