#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

double CastRay(const Player* p, double rayDirX, double rayDirY, int* hitSide, int* hitMapX, int* hitMapY);

int main()
{
    Player player;
    player.posX = 22.0;
    player.posY = 12.0;
    player.dirX = -1;
    player.dirY = 0;
    player.planeX = 0;
    player.planeY = 0.66;

    double moveSpeed = 0.05;
    double rotSpeed = 0.003;

    Color (*screen)[SCREEN_WIDTH] = malloc(sizeof(Color) * SCREEN_HEIGHT * SCREEN_WIDTH);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib basic window");

    DisableCursor();

    Image img = (Image){
        .data = screen,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    Texture2D tex = LoadTextureFromImage(img);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
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

        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            double cameraX = 2.0 * x / (double)SCREEN_WIDTH - 1.0;
            double rayDirX = player.dirX + player.planeX * cameraX;
            double rayDirY = player.dirY + player.planeY * cameraX;

            int hitSide, hitMapX, hitMapY;
            double perpWallDist = CastRay(&player, rayDirX, rayDirY, &hitSide, &hitMapX, &hitMapY);

            int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
            int drawStart = (SCREEN_HEIGHT - lineHeight) / 2;
            if (drawStart < 0) drawStart = 0;

            int drawEnd = lineHeight + drawStart;
            if (drawEnd > SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

            for (int y = 0; y < SCREEN_HEIGHT; y++)
            {
                if (y < drawStart)
                {
                    screen[y][x] = (Color){48, 48, 48, 255};
                }
                else if (y > drawEnd)
                {
                    screen[y][x] = (Color){96, 96, 96, 255};
                }
                else
                {
                    Color color;
                    switch(worldMap[hitMapX][hitMapY])
                    {
                        case 1: color = (Color){255, 0, 0, 255}; break;
                        case 2: color = (Color){0, 255, 0, 255}; break;
                        case 3: color = (Color){0, 0, 255, 255}; break;
                        case 4: color = (Color){255, 255, 255, 255}; break;
                        case 5: color = (Color){255, 255, 0, 255}; break;
                        default: color = (Color){128, 128, 128, 255}; break;
                    }

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
    
    UnloadTexture(tex);
    CloseWindow();
    free(screen);
    return 0;
}

double CastRay(const Player* p, double rayDirX, double rayDirY, int* hitSide, int* hitMapX, int* hitMapY)
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

    return perpWallDist;
}