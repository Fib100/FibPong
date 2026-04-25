#include "raylib.h"

void OnGameStart()
{
    
}

void LoadContent()
{
    
}

void UnloadContent()
{
    
}


void Update()
{
    if (IsKeyPressed(KEY_F1))
    {
        ToggleFullscreen();
    }
}

void Draw()
{
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 255});
    EndDrawing();
}

int main()
{
    bool shouldExitGameLoop = false;
    bool isExitRequested = false;

    InitWindow(800, 600, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    LoadContent();
    OnGameStart();

    while (!shouldExitGameLoop)
    {        
        Update();
        Draw();

        if (WindowShouldClose() || isExitRequested)
            shouldExitGameLoop = true;
    }

    UnloadContent();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
