#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>

// 窗口大小
const int WIDTH = 800;
const int HEIGHT = 450;

// 地面高度
const int GROUND_Y = 360;

// 角色
int playerX = 120;
int playerY = GROUND_Y - 50;
int playerW = 45;
int playerH = 50;

// 跳跃相关
bool isJumping = false;
double velocityY = 0;
double gravity = 0.8;
double jumpPower = -15;

// 障碍物
int obstacleX = WIDTH;
int obstacleY = GROUND_Y - 45;
int obstacleW = 35;
int obstacleH = 45;
int obstacleSpeed = 7;

// 游戏状态
int score = 0;
bool gameOver = false;

// 重新开始游戏
void resetGame()
{
    playerY = GROUND_Y - playerH;
    isJumping = false;
    velocityY = 0;

    obstacleX = WIDTH;
    score = 0;
    gameOver = false;
}

// 判断碰撞
bool checkCollision()
{
    if (playerX < obstacleX + obstacleW &&
        playerX + playerW > obstacleX &&
        playerY < obstacleY + obstacleH &&
        playerY + playerH > obstacleY)
    {
        return true;
    }
    return false;
}

// 绘制游戏画面
void drawGame()
{
    cleardevice();

    // 背景
    setbkcolor(RGB(220, 240, 255));
    cleardevice();

    // 地面
    setlinecolor(BLACK);
    line(0, GROUND_Y, WIDTH, GROUND_Y);

    // 角色
    setfillcolor(RGB(255, 170, 80));
    solidrectangle(playerX, playerY, playerX + playerW, playerY + playerH);

    // 角色眼睛，先让它像个小动物
    setfillcolor(BLACK);
    solidcircle(playerX + 30, playerY + 15, 4);

    // 障碍物
    setfillcolor(RGB(60, 60, 60));
    solidrectangle(obstacleX, obstacleY, obstacleX + obstacleW, obstacleY + obstacleH);

    // 分数
    settextcolor(BLACK);
    settextstyle(24, 0, L"微软雅黑");

    wchar_t scoreText[50];
    swprintf_s(scoreText, L"分数：%d", score);
    outtextxy(20, 20, scoreText);

    outtextxy(20, 50, L"空格跳跃，R重新开始");

    // 游戏结束提示
    if (gameOver)
    {
        settextstyle(40, 0, L"微软雅黑");
        outtextxy(300, 170, L"游戏结束");

        settextstyle(24, 0, L"微软雅黑");
        outtextxy(275, 230, L"按 R 重新开始");
    }
}

// 更新游戏逻辑
void updateGame()
{
    if (gameOver)
    {
        return;
    }

    // 跳跃物理
    if (isJumping)
    {
        playerY += (int)velocityY;
        velocityY += gravity;

        if (playerY >= GROUND_Y - playerH)
        {
            playerY = GROUND_Y - playerH;
            isJumping = false;
            velocityY = 0;
        }
    }

    // 障碍物移动
    obstacleX -= obstacleSpeed;

    if (obstacleX + obstacleW < 0)
    {
        obstacleX = WIDTH;
        score += 10;
    }

    // 碰撞检测
    if (checkCollision())
    {
        gameOver = true;
    }
}

// 处理按键
void handleInput()
{
    // 空格键跳跃
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        if (!isJumping && !gameOver)
        {
            isJumping = true;
            velocityY = jumpPower;
        }
    }

    // R 键重新开始
    if (GetAsyncKeyState('R') & 0x8000)
    {
        resetGame();
    }
}

int main()
{
    initgraph(WIDTH, HEIGHT);

    setbkcolor(RGB(220, 240, 255));
    cleardevice();

    BeginBatchDraw();

    while (true)
    {
        handleInput();
        updateGame();
        drawGame();

        FlushBatchDraw();
        Sleep(16);
    }

    EndBatchDraw();
    closegraph();

    return 0;
}