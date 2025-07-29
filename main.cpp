
#include <deque>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <bits/ostream.tcc>
using namespace std;

//Colors
Color Red = {255,0,0,255};
Color Green = {173,204,96,255};
Color DarkGreen = {43,51,25,255};

int CellSize = 30;
int CellCount = 25;
int Offset = 75;

double LastUpdateTime = 0;

bool EventTriggered(double interval)
{
    double CurrentTime = GetTime();
    if (CurrentTime - LastUpdateTime >= interval)
    {
        LastUpdateTime = CurrentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 Element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i],Element))
        {
            return true;
        }
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> Body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};

    Vector2 Direction = {1,0};

    bool AddSegment = false;


    void Draw()
    {
        for (unsigned int i = 0; i < Body.size(); i++)
        {
            float X = Body[i].x;
            float Y = Body[i].y;
            Rectangle Segment = Rectangle{Offset + X * CellSize,Offset + Y * CellSize,(float)CellSize,(float)CellSize};
            DrawRectangleRounded(Segment, 0.5,6,DarkGreen);
        }
    }

    void Update()
    {
        Body.push_front(Vector2Add(Body[0],Direction));
        if (AddSegment)
        {
            AddSegment = false;
        }
        else
        {
            Body.pop_back();
        }
    }
    void Reset()
    {
        Body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        Direction = {1,0};
    }
};

class FoodBase
{

public:

    Vector2 Pos;
    Texture2D Tex;

    FoodBase(deque<Vector2> SnakeBody)
    {
        Image Img = LoadImage("Graphics/food.png");
        Tex = LoadTextureFromImage(Img);
        UnloadImage(Img);
        Pos = GenerateRandomPos(SnakeBody);
    }

    ~FoodBase()
    {
        UnloadTexture(Tex);
    };

    void Draw()
    {
       DrawTexture(Tex,Offset +Pos.x * CellSize,Offset +Pos.y * CellSize,Red);
    }

    Vector2 GenerateRandomCell()
    {
        float X = GetRandomValue(0,CellCount -1);
        float Y = GetRandomValue(0,CellCount -1);
        return Vector2{X,Y};
    }
    Vector2 GenerateRandomPos(deque<Vector2> SnakeBody)
    {
        Vector2 CheckPos;
        do {
            CheckPos = GenerateRandomCell();
        } while (ElementInDeque(CheckPos, SnakeBody));
        return CheckPos;
    }

};

class Game
{
public:
    Snake snake = Snake();
    FoodBase food = FoodBase(snake.Body);
    bool Running = true;
    float Difficulty = 0.2f;
    int Score = 0;
    Sound EatSound;
    Sound WallSound;

    Game()
    {
        InitAudioDevice();
        EatSound = LoadSound("Sounds/eat.mp3");
        WallSound = LoadSound("Sounds/wall.mp3");
    }
    ~Game()
    {
        UnloadSound(EatSound);
        UnloadSound(WallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }
    void Update()
    {
        if (Running)
        {
            snake.Update();
            CheckCollisionWithEdges();
            CheckCollisionWithFood();
            CheckCollisionWithTail();
        }
    }
    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.Body[0],food.Pos))
        {
            food.Pos = food.GenerateRandomPos(snake.Body);
            snake.AddSegment = true;
            Score++;
            Difficulty *= 0.95f; // 5% faster each time
            Difficulty = fmaxf(Difficulty, 0.05f); // cap the speed
            PlaySound(EatSound);
        }
    }
    void CheckCollisionWithTail()
    {
        deque<Vector2> Tail = snake.Body;
        Tail.pop_front();
        if (ElementInDeque(snake.Body[0],Tail))
        {
            GameOver();

        }
    }
    void CheckCollisionWithEdges()
    {
        if (snake.Body[0].x == CellCount || snake.Body[0].x == -1)
        {
            GameOver();
        }
        if (snake.Body[0].y == CellCount || snake.Body[0].y == -1)
        {
            GameOver();
        }
    }
    void GameOver()
    {
        PlaySound(WallSound);
        Running = false;
    }
    void GameReset()
    {
        snake.Reset();
        food.Pos = food.GenerateRandomPos(snake.Body);
        Score = 0;
        Difficulty = 0.2f;
    }
};
int main()
{
    cout << "Current working directory: " << GetWorkingDirectory() << endl;

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);
    cout << "Hello World!" << endl;
    InitWindow(2*Offset + CellSize*CellCount,2*Offset +CellSize*CellCount, "Retro Snake");
    SetTargetFPS(60);

    Game SuperGame = Game();
    Vector2 NewDirection = SuperGame.snake.Direction;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        //Inputs:
        if (IsKeyDown(KEY_W) && SuperGame.snake.Direction.y != 1)
        {
            NewDirection = {0,-1};
        }
        else if (IsKeyDown(KEY_S) && SuperGame.snake.Direction.y != -1)
        {
            NewDirection = {0,1};
        }
        else if (IsKeyDown(KEY_A) && SuperGame.snake.Direction.x != 1)
        {
            NewDirection = {-1,0};
        }
        else if (IsKeyDown(KEY_D) && SuperGame.snake.Direction.x != -1)
        {
            NewDirection = {1,0};
        }
        if (IsKeyDown(KEY_SPACE) && !SuperGame.Running)
        {
            SuperGame.GameReset();
            NewDirection = SuperGame.snake.Direction;
            SuperGame.Running = true;
        }

        if (EventTriggered(SuperGame.Difficulty))
        {
            SuperGame.snake.Direction = NewDirection;
            SuperGame.Update();
        }
        // Drawing
        ClearBackground(Green);

        for (int i = 0; i <= CellCount; i++) {
            DrawLine(Offset+i * CellSize, Offset+0, Offset+i * CellSize, Offset+CellSize * CellCount, DarkGreen);
            DrawLine(Offset+0, Offset+i * CellSize, Offset+CellSize * CellCount, Offset+i * CellSize, DarkGreen);
        }

        DrawRectangleLinesEx(Rectangle{(float)Offset-5, (float)Offset-5,(float)CellSize*CellCount+10,(float)CellSize*CellCount+10},5,DarkGreen);


        DrawFPS(10,0);
        DrawText(TextFormat("Direction X: %0.f",SuperGame.snake.Direction.x), SuperGame.snake.Body[0].x*CellSize+Offset+100, SuperGame.snake.Body[0].y*CellSize+Offset+30,20, WHITE);
        DrawText(TextFormat("Direction Y: %0.f",SuperGame.snake.Direction.y), SuperGame.snake.Body[0].x*CellSize+Offset+100, SuperGame.snake.Body[0].y*CellSize+Offset+50,20, WHITE);

        if (SuperGame.Running)
        {
            DrawText("SUPEEEEEEEEEEEEEEER SNAAAAAAAAAAAKE!",20, 20,30, DarkGreen);
            DrawText(TextFormat("Score: %i",SuperGame.Score),CellSize*CellCount+10-Offset, 45,30, DarkGreen);
        }
        else
        {
            DrawText("PRESS SPACE TO RESTART!",20, 20,30, Red);
            DrawText( "Hi :)",CellSize*CellCount+Offset + 5, 35,30, VIOLET);
        }

        SuperGame.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}