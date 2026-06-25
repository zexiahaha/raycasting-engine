#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int worldMap[MAP_WIDTH][MAP_HEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct Player
{
    double posX, posY;
    double dirX, dirY;
    double planeX, planeY;
} Player;

double CastRay(const Player* p, double rayDirX, double rayDirY, int* hitSide, int* hitMapX, int* hitMapY, double* wallX);

uint32_t* GenerateBrickTexture(int texWidth, int texHeight);
uint32_t* GenerateFloorTexture(int texWidth, int texHeight);

void UpdateFrame();


Player player = {
    .posX = 22.0,
    .posY = 12.0,
    .dirX = -1,
    .dirY = 0,
    .planeX = 0,
    .planeY = 0.66
};

double moveSpeed = 0.05;
double rotSpeed = 0.003;
int texWidth = 64;
int texHeight = 64;

Color (*screen)[SCREEN_WIDTH];

Image img;
Texture2D tex;

uint32_t* brickTex;
uint32_t* floorTex;
uint32_t* spriteTex;

double zBuffer[SCREEN_WIDTH];

#define MAX_SPRITES 100

typedef struct Sprite{
    double x, y;
    int alive;
} Sprite;

Sprite sprites[MAX_SPRITES];
int spriteCount = 0;

int spriteOrder[MAX_SPRITES];
double spriteDistance[MAX_SPRITES];

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib basic window");
    DisableCursor();
    SetTargetFPS(60);

    sprites[0] = (Sprite){.x = 20.5, .y = 11.5, .alive = 1};
    sprites[1] = (Sprite){.x = 18.5, .y = 4.5, .alive = 1};
    sprites[2] = (Sprite){.x = 10.5, .y = 8.5, .alive = 1};

    spriteCount = 3;

    screen = malloc(sizeof(Color) * SCREEN_HEIGHT * SCREEN_WIDTH);
    img = (Image){
        .data = screen,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    tex = LoadTextureFromImage(img);
    brickTex = GenerateBrickTexture(texWidth, texHeight);
    floorTex = GenerateFloorTexture(texWidth, texHeight);
    spriteTex = GenerateSpriteTexture(texWidth, texHeight);


    #ifdef PLATFORM_WEB
        emscripten_set_main_loop(UpdateFrame, 0, 1);
    #else
        while (!WindowShouldClose()) {
            UpdateFrame();
        }
    #endif
    
    UnloadTexture(tex);
    CloseWindow();
    free(screen);
    free(brickTex);
    free(floorTex);
    free(spriteTex);
    return 0;
}

void RenderSprites()
{
    for (int i = 0; i < spriteCount; i++)
    {
        spriteOrder[i] = i;
        spriteDistance[i] = (player.posX - sprites[i],x) * (player.posX - sprites[i],x)
                            + (player.posY - sprites[i].y) * (player.posY - sprites[i].y);
    }

    for (int i = 0; i < spriteCount - 1; i++)
    {
        for (int j = 0; j < spriteCount - 1 - i; j++)
        {
            if (spriteDistance[spriteOrder[j]] < spriteDistance[spriteOrder[j + 1]])
            {
                int temp = spriteOrder[j];
                spriteOrder[j] = spriteOrder[j + 1];
                spriteOrder[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < spriteCount; i++)
    {
        int s = spriteOrder[i];

        double spriteX = sprites[s].x - player.posX;
        double spriteY = sprites[s].y - player.posY;

        
    }
}

void UpdateFrame()
{
    double dx = 0, dy = 0;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        dx += player.dirX;
        dy += player.dirY;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        dx -= player.dirX;
        dy -= player.dirY;
    } 
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        dx -= player.planeX;
        dy -= player.planeY;
    } 
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        dx += player.planeX;
        dy += player.planeY;
    }

    if (dx != 0 || dy != 0)
    {
        double len = sqrt(dx * dx + dy * dy);
        dx = dx / len;
        dy = dy / len;
    }

    double newX = player.posX + dx * moveSpeed;
    double newY = player.posY + dy * moveSpeed;

    if (worldMap[(int)newX][(int)player.posY] == 0)
    {
        player.posX = newX;
    }
    if (worldMap[(int)player.posX][(int)newY] == 0)
    {
        player.posY = newY;
    }

    double rotAngle = -GetMouseDelta().x * rotSpeed;
    double oldDirX = player.dirX;
    player.dirX = player.dirX * cos(rotAngle) - player.dirY * sin(rotAngle);
    player.dirY = oldDirX * sin(rotAngle) + player.dirY * cos(rotAngle);

    double oldPlaneX = player.planeX;
    player.planeX = player.planeX * cos(rotAngle) - player.planeY * sin(rotAngle);
    player.planeY = oldPlaneX * sin(rotAngle) + player.planeY * cos(rotAngle);

    double rayDirLeftX = player.dirX + player.planeX * (-1.0);
    double rayDirLeftY = player.dirY + player.planeY * (-1.0);
    double rayDirRightX = player.dirX + player.planeX * (+1.0);
    double rayDirRightY = player.dirY + player.planeY * (+1.0);

    for (int y = SCREEN_HEIGHT / 2 + 1; y < SCREEN_HEIGHT; y++)
    {
        double rowDistance = 0.5 * SCREEN_HEIGHT / (y - SCREEN_HEIGHT / 2);

        double floorStepX = rowDistance * (rayDirRightX - rayDirLeftX) / SCREEN_WIDTH;
        double floorStepY = rowDistance * (rayDirRightY - rayDirLeftY) / SCREEN_WIDTH;

        double floorX = player.posX + rowDistance * rayDirLeftX;
        double floorY = player.posY + rowDistance * rayDirLeftY;

        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int texX = (int)(floorX * texWidth) & (texWidth - 1);
            int texY = (int)(floorY * texHeight) & (texHeight - 1);
            screen[y][x] = GetColor(floorTex[texY * texWidth + texX]);

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    for (int y = SCREEN_HEIGHT / 2 - 1; y > 0; y--)
    {
        double rowDistance = 0.5 * SCREEN_HEIGHT / (SCREEN_HEIGHT / 2 - y);

        double floorStepX = rowDistance * (rayDirRightX - rayDirLeftX) / SCREEN_WIDTH;
        double floorStepY = rowDistance * (rayDirRightY - rayDirLeftY) / SCREEN_WIDTH;

        double floorX = player.posX + rowDistance * rayDirLeftX;
        double floorY = player.posY + rowDistance * rayDirLeftY;

        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int texX = (int)(floorX * texWidth) & (texWidth - 1);
            int texY = (int)(floorY * texHeight) & (texHeight - 1);
            screen[y][x] = GetColor(floorTex[texY * texWidth + texX]);

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }



    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        double cameraX = 2.0 * x / (double)SCREEN_WIDTH - 1.0;
        double rayDirX = player.dirX + player.planeX * cameraX;
        double rayDirY = player.dirY + player.planeY * cameraX;

        int hitSide, hitMapX, hitMapY;
        double wallX;
        double perpWallDist = CastRay(&player, rayDirX, rayDirY, &hitSide, &hitMapX, &hitMapY, &wallX);

        zBuffer[x] = perpWallDist;

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = (SCREEN_HEIGHT - lineHeight) / 2;
        if (drawStart < 0) drawStart = 0;

        int drawEnd = lineHeight + drawStart;
        if (drawEnd > SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        int texX = (int)(wallX * texWidth);
        double step = (double)texHeight / lineHeight;
        double texPos = (drawStart - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;

        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            if (y < drawStart)
            {
                // screen[y][x] = (Color){48, 48, 48, 255};
            }
            else if (y > drawEnd)
            {
                // screen[y][x] = (Color){96, 96, 96, 255};
            }
            else
            {
                Color color;
                // switch(worldMap[hitMapX][hitMapY])
                // {
                //     case 1: color = (Color){255, 0, 0, 255}; break;
                //     case 2: color = (Color){0, 255, 0, 255}; break;
                //     case 3: color = (Color){0, 0, 255, 255}; break;
                //     case 4: color = (Color){255, 255, 255, 255}; break;
                //     case 5: color = (Color){255, 255, 0, 255}; break;
                //     default: color = (Color){128, 128, 128, 255}; break;
                // }

                int texY = (int)texPos & (texHeight - 1);
                texPos += step;

                color = GetColor(brickTex[texY * texWidth + texX]);

                if (hitSide == 1)
                {
                    color.r /= 2;
                    color.g /= 2;
                    color.b /= 2;
                }

                screen[y][x] = color;
            }
        }
    }

    UpdateTexture(tex, *screen);
    BeginDrawing();
    DrawTexture(tex, 0, 0, WHITE);
    EndDrawing();
}

double CastRay(const Player* p, double rayDirX, double rayDirY, int* hitSide, int* hitMapX, int* hitMapY, double* wallX)
{
    int mapX = (int)p->posX;
    int mapY = (int)p->posY;

    double sideDistX;
    double sideDistY;

    double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

    int stepX;
    int stepY;

    int hit = 0;

    double perpWallDist;

    if (rayDirX < 0)
    {
        stepX = -1;
        sideDistX = (p->posX - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0 - p->posX) * deltaDistX;
    }

    if (rayDirY < 0)
    {
        stepY = -1;
        sideDistY = (p->posY - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0 - p->posY) * deltaDistY;
    }

    while (hit == 0)
    {
        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            *hitSide = 0;
        }
        else
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            *hitSide = 1;
        }

        if (worldMap[mapX][mapY] > 0)
        {
            hit = 1;
        }
    }

    if (*hitSide == 0)
    {
        perpWallDist = sideDistX - deltaDistX;
    }
    else
    {
        perpWallDist = sideDistY - deltaDistY;
    }

    *hitMapX = mapX;
    *hitMapY = mapY;

    if (*hitSide == 0)
    {
        *wallX = p->posY + perpWallDist * rayDirY;
    }
    else
    {
        *wallX = p->posX + perpWallDist *rayDirX;
    }
    *wallX -= floor(*wallX);

    return perpWallDist;
}

uint32_t* GenerateBrickTexture(int texWidth, int texHeight)
{
    int size = texWidth * texHeight;
    uint32_t* tex = malloc(size * sizeof(uint32_t));

    for (int y = 0; y < texHeight; y++)
    {
        for (int x = 0; x < texWidth; x++)
        {
            int brickRow = y / 17;
            int shiftedX = x - 8;
            if (y % 17 == 16)
            {
                tex[y * texWidth + x] = ColorToInt((Color){0, 0, 0, 255});
            }
            else if ((brickRow % 2 == 0 && x % 17 == 16) || (brickRow % 2 != 0 && x % 17 == 7 && shiftedX >= 0))
            {
                tex[y * texWidth + x] = ColorToInt((Color){0, 0, 0, 255});
            }
            else
            {
                tex[y * texWidth + x] = ColorToInt((Color){192, 64, 64, 255});
            }
        }
    }

    return tex;
}

uint32_t* GenerateFloorTexture(int texWidth, int texHeight)
{
    int size = texWidth * texHeight;
    uint32_t* tex = malloc(size * sizeof(uint32_t));

    for (int y = 0; y < texHeight; y++)
    {
        for (int x = 0; x < texWidth; x++)
        {
            if ((x / 8 + y / 8) % 2 == 0)
            {
                tex[y* texWidth + x] = ColorToInt((Color){160, 160, 160, 255});
            } else {
                tex[y* texWidth + x] = ColorToInt((Color){96, 96, 96, 255});
            }
        }
    }

    return tex;
}

uint32_t GenerateSpriteTexture(int texWidth, int texHeight)
{
    int size = texWidth * texHeight;
    uint32_t* tex = malloc(size * sizeof(uint32_t));

    
    for (int y = 0; y < texHeight; y++)
    {
        for (int x = 0; x < texWidth; x++)
        {
            int cx = x - 32;
            int cy = y - 32;
            if (abs(cx) + abs(cy) < 28)
            {
                tex[y * texWidth + x] = ColorToInt((Color){220, 50, 50, 255});
            }
            else
            {
                tex[y * texWidth + x] = ColorToInt((Color){255, 0, 50, 255});
            }
        }
    }

    return tex;
}