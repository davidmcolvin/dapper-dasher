#include "raylib.h"
//Test
struct AnimData
{
  Rectangle rec;
  Vector2 pos;
  int frame;
  float updateTime;
  float runningTime;
};

bool isInGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
  //update running time
  data.runningTime += deltaTime;

  if (data.runningTime >= data.updateTime)
  {
      data.runningTime = 0.0;
      //update animation frame
      data.rec.x = data.frame * data.rec.width;
      data.frame ++;
      if (data.frame > maxFrame)
      {
          data.frame = 0;
      }
  }

  return data;
}

int main()
{

    int windowDimensions[2] = {512,380};

    InitWindow(windowDimensions[0],windowDimensions[1],"Dapper Dasher!");

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;

    const int sizeOfNebulae{6};

    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;

        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].pos.x = windowDimensions[0] + 300 * i;

        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
    }
    
    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    // pixels/second
    const int jumpVelocity{-600};
    const int nebulaVelocity{-200};

    int velocity{};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{};
    float mgX{};
    float fgX{};

    // accelration due to gravity (pixels/second)/second
    int gravity{1'000};

    bool isInAir = false;

    bool collision{false};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        const float dT = GetFrameTime();

        BeginDrawing();
        ClearBackground(WHITE);

        // draw the background
        bgX -= 20 * dT;

        if (bgX <= -background.width * 2)
        {
            bgX = 0;
        }

        // draw the midground
        mgX -= 40 * dT;

        if (mgX <= -midground.width * 2)
        {
            mgX = 0;
        }

        // draw the midground
        fgX -= 80 * dT;

        if (fgX <= -foreground.width * 2)
        {
            fgX = 0;
        }

        Vector2 bg1Pos {bgX,0.0};
        Vector2 bg2Pos {bgX + background.width * 2,0.0};

        Vector2 mg1Pos {mgX,0.0};
        Vector2 mg2Pos {mgX + midground.width * 2,0.0};

        Vector2 fg1Pos {fgX,0.0};
        Vector2 fg2Pos {fgX + foreground.width * 2,0.0};

        DrawTextureEx(background,bg1Pos,0,2.0,WHITE);
        DrawTextureEx(background,bg2Pos,0,2.0,WHITE);
        DrawTextureEx(midground,mg1Pos,0,2.0,WHITE);
        DrawTextureEx(midground,mg2Pos,0,2.0,WHITE);
        DrawTextureEx(foreground,fg1Pos,0,2.0,WHITE);
        DrawTextureEx(foreground,fg2Pos,0,2.0,WHITE);

        // ground check
        if (isInGround(scarfyData, windowDimensions[1]) )
        {
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        // check for jump
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVelocity;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebulaVelocity * dT;
        }
        
        finishLine += nebulaVelocity * dT;

        scarfyData.pos.y += velocity * dT;

        if (!isInAir)
        { 
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad
            };

            Rectangle scarfyRec {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }

        }

        if (collision)
        {
            DrawText("Game Over!",windowDimensions[0]/2-75,windowDimensions[1]/2,30,WHITE);
        }
        else if (finishLine + 300 <= scarfyData.pos.x)
        {
            DrawText("You Win!",windowDimensions[0]/2-65,windowDimensions[1]/2,30,WHITE);
        }
        else
        {
          DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
  
          for (int i = 0; i < sizeOfNebulae; i++)
          {
              DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
          }
        }

        EndDrawing();
    }
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    CloseWindow();
}