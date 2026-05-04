#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>

// =====================
// 窗口与地面
// =====================
const int WIDTH = 800;
const int HEIGHT = 450;
int groundY[6] = {
    390, // 模式1地面高度
    396, // 模式2地面高度
    404, // 模式3地面高度
    395, // 模式4地面高度
    401, // 模式5地面高度
    398 // 模式6地面高度
};

// =====================
// 游戏状态
// =====================
enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};

GameState gameState = MENU;

// =====================
// 当前模式
// =====================
int mode = 1;
int score = 0;

// =====================
// 角色
// =====================
int playerX = 120;
int playerY = groundY[mode - 1] - 50;
int playerW = 45;
int playerH = 50;

bool isJumping = false;
double velocityY = 0;
double gravity = 0.8;
double jumpPower = -15;

// =====================
// 角色图片
// =====================
IMAGE roleImgs[6];
IMAGE menuBg;
IMAGE modeBg[6];
// 六个模式对应的障碍物图片
IMAGE obstacleImgs[6];
// =====================
// 障碍物 1
// =====================
int obstacleX = WIDTH;
int obstacleY = groundY[mode - 1] - 45;
int obstacleW = 55;
int obstacleH = 55;
int obstacleSpeed = 7;

// =====================
// 障碍物 2，用于双障碍模式
// =====================
int obstacle2X = WIDTH + 350;
int obstacle2Y = groundY[mode - 1] - 35;
int obstacle2W = 55;
int obstacle2H = 55;
bool useSecondObstacle = false;

// =====================
// 护盾模式
// =====================
bool hasShield = false;

// =====================
// 函数声明
// =====================
void loadRoleImages();
void drawTransparentImage(int x, int y, IMAGE* img);
void resetGame();
void applyMode(int selectedMode);
void drawMenu();
void drawGame();
void updateGame();
void handleInput();
bool checkCollisionOne(int ox, int oy, int ow, int oh);
void handleCollision();





// =====================
// 加载 6 个角色图片
// =====================
void loadRoleImages()
{
    loadimage(&roleImgs[0], L"images/sionstart.jpg", 80, 80);
    loadimage(&roleImgs[1], L"images/rikustart.jpg", 80, 80);
    loadimage(&roleImgs[2], L"images/yushistart.jpg", 80, 80);
    loadimage(&roleImgs[3], L"images/jaeheestart.jpg", 80, 80);
    loadimage(&roleImgs[4], L"images/ryostart.jpg", 80, 80);
    loadimage(&roleImgs[5], L"images/sakuyastart.jpg", 80, 80);

    // 加载封面背景图，自动缩放到窗口大小
    loadimage(&menuBg, L"images/menu_bg.jpg", WIDTH, HEIGHT);
   
    // 六个模式背景
    loadimage(&modeBg[0], L"images/mode1_bg.png", WIDTH, HEIGHT);
    loadimage(&modeBg[1], L"images/mode2_bg.png", WIDTH, HEIGHT);
    loadimage(&modeBg[2], L"images/mode3_bg.png", WIDTH, HEIGHT);
    loadimage(&modeBg[3], L"images/mode4_bg.png", WIDTH, HEIGHT);
    loadimage(&modeBg[4], L"images/mode5_bg.png", WIDTH, HEIGHT);
    loadimage(&modeBg[5], L"images/mode6_bg.png", WIDTH, HEIGHT);

    // 六个模式障碍物图片
    loadimage(&obstacleImgs[0], L"images/obstacle1.png", 55, 55);
    loadimage(&obstacleImgs[1], L"images/obstacle2.png", 55, 55);
    loadimage(&obstacleImgs[2], L"images/obstacle3.png", 55, 55);
    loadimage(&obstacleImgs[3], L"images/obstacle4.png", 55, 55);
    loadimage(&obstacleImgs[4], L"images/obstacle5.png", 55, 55);
    loadimage(&obstacleImgs[5], L"images/obstacle6.png", 55, 55);
}


// =====================
// 粉色背景透明绘制函数
// 只把亮粉色 RGB(255, 0, 255) 附近的颜色当作透明
// 角色本身的白色脸不会消失
// =====================
void drawTransparentImage(int x, int y, IMAGE* img)
{
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(img);

    int imgW = img->getwidth();
    int imgH = img->getheight();
    int winW = getwidth();
    int winH = getheight();

    for (int iy = 0; iy < imgH; iy++)
    {
        for (int ix = 0; ix < imgW; ix++)
        {
            int drawX = x + ix;
            int drawY = y + iy;

            if (drawX < 0 || drawX >= winW || drawY < 0 || drawY >= winH)
            {
                continue;
            }

            DWORD color = src[iy * imgW + ix];

            int blue = color & 0xff;
            int green = (color >> 8) & 0xff;
            int red = (color >> 16) & 0xff;

            // 只把亮粉色背景当透明
            if (red > 180 && green < 100 && blue > 180)
            {
                continue;
            }

            dst[drawY * winW + drawX] = color;
        }
    }
}


// =====================
// 根据模式设置参数
// =====================
void applyMode(int selectedMode)
{
    mode = selectedMode;

    // 默认参数
    obstacleSpeed = 7;
    gravity = 0.8;
    jumpPower = -15;
    useSecondObstacle = false;
    hasShield = false;

    if (mode == 1)
    {
        // 普通模式
        obstacleSpeed = 7;
        gravity = 0.8;
        jumpPower = -15;
    }
    else if (mode == 2)
    {
        // 高速模式
        obstacleSpeed = 11;
        gravity = 0.8;
        jumpPower = -15;
    }
    else if (mode == 3)
    {
        // 低重力模式
        obstacleSpeed = 7;
        gravity = 0.45;
        jumpPower = -13;
    }
    else if (mode == 4)
    {
        // 双障碍模式
        obstacleSpeed = 8;
        gravity = 0.8;
        jumpPower = -15;
        useSecondObstacle = true;
    }
    else if (mode == 5)
    {
        // 护盾模式
        obstacleSpeed = 8;
        gravity = 0.8;
        jumpPower = -15;
        hasShield = true;
    }
    else if (mode == 6)
    {
        // 夜晚模式
        obstacleSpeed = 9;
        gravity = 0.8;
        jumpPower = -15;
    }
}


// =====================
// 重新开始游戏
// =====================
void resetGame()
{
    applyMode(mode);

    playerY = groundY[mode - 1] - playerH;
    isJumping = false;
    velocityY = 0;

    obstacleX = WIDTH;
    obstacle2X = WIDTH + 350;

    obstacleY = groundY[mode - 1] - obstacleH;
    obstacle2Y = groundY[mode - 1] - obstacle2H;

    score = 0;



    gameState = PLAYING;
}


// =====================
// 绘制开始菜单
// =====================
void drawMenu()
{
    cleardevice();

    // 绘制封面背景图
    putimage(0, 0, &menuBg);
    // 关键：文字背景透明，避免出现黑色长条
    setbkmode(TRANSPARENT);
   

    settextcolor(BLACK);
    settextstyle(38, 0, L"Comic Sans MS");
    outtextxy(225, 30, L"WISH DOLL GAME");

    settextstyle(22, 0, L"微软雅黑");
    outtextxy(230, 90, L"按数字键 1~6 选择角色和模式");

    // 六张角色图片，使用粉色透明绘制
    drawTransparentImage(80, 150, &roleImgs[0]);
    drawTransparentImage(200, 150, &roleImgs[1]);
    drawTransparentImage(320, 150, &roleImgs[2]);
    drawTransparentImage(440, 150, &roleImgs[3]);
    drawTransparentImage(560, 150, &roleImgs[4]);
    drawTransparentImage(680, 150, &roleImgs[5]);

    settextstyle(18, 0, L"Comic Sans MS");

    outtextxy(105, 240, L"1");
    outtextxy(225, 240, L"2");
    outtextxy(345, 240, L"3");
    outtextxy(465, 240, L"4");
    outtextxy(585, 240, L"5");
    outtextxy(705, 240, L"6");

    outtextxy(55, 285, L"SIONING");
    outtextxy(175, 285, L"KURI");
    outtextxy(295, 285, L"BUBBLE NYANG");
    outtextxy(415, 285, L"DANGTERI");
    outtextxy(535, 285, L"RYONRYON");
    outtextxy(655, 285, L"SAKUPANG");

    outtextxy(230, 390, L"游戏中：空格跳跃，R重新开始，ESC返回菜单");
}


// =====================
// 碰撞检测
// =====================
bool checkCollisionOne(int ox, int oy, int ow, int oh)
{
    if (playerX < ox + ow &&
        playerX + playerW > ox &&
        playerY < oy + oh &&
        playerY + playerH > oy)
    {
        return true;
    }

    return false;
}


// =====================
// 处理碰撞
// =====================
void handleCollision()
{
    bool collision = false;

    if (checkCollisionOne(obstacleX, obstacleY, obstacleW, obstacleH))
    {
        collision = true;
    }

    if (useSecondObstacle && checkCollisionOne(obstacle2X, obstacle2Y, obstacle2W, obstacle2H))
    {
        collision = true;
    }

    if (collision)
    {
        if (hasShield)
        {
            // 护盾抵挡一次碰撞
            hasShield = false;

            // 把障碍物移走，防止连续触发
            obstacleX = WIDTH;
            obstacle2X = WIDTH + 350;
        }
        else
        {
            gameState = GAME_OVER;
        }
    }
}


// =====================
// 更新游戏逻辑
// =====================
void updateGame()
{
    if (gameState != PLAYING)
    {
        return;
    }

    // 跳跃物理
    if (isJumping)
    {
        playerY += (int)velocityY;
        velocityY += gravity;

        if (playerY >= groundY[mode - 1] - playerH)
        {
            playerY = groundY[mode - 1] - playerH;
            isJumping = false;
            velocityY = 0;
        }
    }

    // 障碍物 1 移动
    obstacleX -= obstacleSpeed;

    if (obstacleX + obstacleW < 0)
    {
        obstacleX = WIDTH;
        score += 10;
    }

    // 障碍物 2 移动
    if (useSecondObstacle)
    {
        obstacle2X -= obstacleSpeed;

        if (obstacle2X + obstacle2W < 0)
        {
            obstacle2X = WIDTH + 250;
            score += 10;
        }
    }

    // 难度随分数增加
    if (score > 0 && score % 50 == 0)
    {
        obstacleSpeed = 7 + score / 50;

        if (mode == 2)
        {
            obstacleSpeed += 4;
        }

        if (mode == 6)
        {
            obstacleSpeed += 2;
        }
    }

    handleCollision();
}


// =====================
// 绘制游戏画面
// =====================
void drawGame()
{
    cleardevice();

    // 背景
    // 绘制当前模式背景图
    putimage(0, 0, &modeBg[mode - 1]);

    // 文字透明背景
    setbkmode(TRANSPARENT);

    // 第 6 模式如果背景偏暗，文字用白色；其他模式用黑色
    if (mode == 6)
    {
        settextcolor(WHITE);
        setlinecolor(WHITE);
    }
    else
    {
        settextcolor(BLACK);
        setlinecolor(BLACK);
    }

    // 地面
   // line(0, groundY[mode - 1], WIDTH, groundY[mode - 1]);

    // 角色图片，使用粉色透明绘制
    drawTransparentImage(playerX, playerY - 30, &roleImgs[mode - 1]);

    // 护盾显示
    if (hasShield)
    {
        setlinecolor(RGB(0, 200, 255));
        circle(playerX + playerW / 2 + 18, playerY + playerH / 2 - 5, 45);
    }

    // 障碍物 1
    drawTransparentImage(obstacleX, obstacleY, &obstacleImgs[mode - 1]);
    // 障碍物 2
    if (useSecondObstacle)
    {
        drawTransparentImage(obstacle2X, obstacle2Y, &obstacleImgs[mode - 1]);
    }

    // 分数和模式
    settextstyle(22, 0, L"Comic Sans MS");

    wchar_t scoreText[80];
    swprintf_s(scoreText, L"分数：%d", score);
    outtextxy(20, 20, scoreText);

    wchar_t modeText[80];

    if (mode == 1)
    {
        swprintf_s(modeText, L"当前模式：SION VER.");
    }
    else if (mode == 2)
    {
        swprintf_s(modeText, L"当前模式：KURI VER.");
    }
    else if (mode == 3)
    {
        swprintf_s(modeText, L"当前模式：BUBBLE NYANG VER.");
    }
    else if (mode == 4)
    {
        swprintf_s(modeText, L"当前模式：DANGTERI VER.");
    }
    else if (mode == 5)
    {
        swprintf_s(modeText, L"当前模式：RYONRYON VER.");
    }
    else if (mode == 6)
    {
        swprintf_s(modeText, L"当前模式：SAKUPANG VER.");
    }

    outtextxy(20, 50, modeText);

    outtextxy(20, 80, L"空格跳跃，R重新开始，ESC返回菜单");

    if (mode == 5)
    {
        if (hasShield)
        {
            outtextxy(20, 110, L"护盾：可抵挡一次碰撞");
        }
        else
        {
            outtextxy(20, 110, L"护盾：已使用");
        }
    }

    // 游戏结束界面
    if (gameState == GAME_OVER)
    {
        settextstyle(40, 0, L"Comic Sans MS");

        if (mode == 6)
        {
            settextcolor(WHITE);
        }
        else
        {
            settextcolor(BLACK);
        }

        outtextxy(300, 170, L"GAME OVER");

        settextstyle(24, 0, L"微软雅黑");
        outtextxy(255, 230, L"按 R 重新开始，ESC返回菜单");
    }
}


// =====================
// 处理按键
// =====================
void handleInput()
{
    // 菜单状态
    if (gameState == MENU)
    {
        if (GetAsyncKeyState('1') & 0x8000)
        {
            applyMode(1);
            resetGame();
            Sleep(150);
        }
        else if (GetAsyncKeyState('2') & 0x8000)
        {
            applyMode(2);
            resetGame();
            Sleep(150);
        }
        else if (GetAsyncKeyState('3') & 0x8000)
        {
            applyMode(3);
            resetGame();
            Sleep(150);
        }
        else if (GetAsyncKeyState('4') & 0x8000)
        {
            applyMode(4);
            resetGame();
            Sleep(150);
        }
        else if (GetAsyncKeyState('5') & 0x8000)
        {
            applyMode(5);
            resetGame();
            Sleep(150);
        }
        else if (GetAsyncKeyState('6') & 0x8000)
        {
            applyMode(6);
            resetGame();
            Sleep(150);
        }

        return;
    }

    // ESC 返回菜单
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        gameState = MENU;
        Sleep(150);
        return;
    }

    // 空格跳跃
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        if (!isJumping && gameState == PLAYING)
        {
            isJumping = true;
            velocityY = jumpPower;
        }
    }

    // R 重新开始
    if (GetAsyncKeyState('R') & 0x8000)
    {
        resetGame();
        Sleep(150);
    }
}


// =====================
// 主函数
// =====================
int main()
{
    initgraph(WIDTH, HEIGHT);

    loadRoleImages();

    BeginBatchDraw();

    while (true)
    {
        handleInput();

        if (gameState == MENU)
        {
            drawMenu();
        }
        else
        {
            updateGame();
            drawGame();
        }

        FlushBatchDraw();
        Sleep(16);
    }

    EndBatchDraw();
    closegraph();

    return 0;
}