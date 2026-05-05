#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


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
    HOME,       // 首页
    MENU,       // 角色选择页
    INTRO,      // 游戏介绍页
    PLAYING,
    GAME_OVER
};

GameState gameState = HOME;

// =====================
// 当前模式
// =====================
int mode = 1;
int score = 0;

// 当前这一局吃到的食物数量
int foodCount = 0;

// =====================
// BGM 状态
// =====================
int currentBGM = 0;
// 0：没有音乐
// 1：首页音乐
// 2：角色选择音乐
// 3：游戏音乐

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
// 六个模式对应的食物图片
IMAGE foodImgs[6];
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

/// =====================
// 空中食物：每组摆成弧线
// =====================
const int FOOD_COUNT = 5;

int foodX[FOOD_COUNT];
int foodY[FOOD_COUNT];
bool foodVisible[FOOD_COUNT];

int foodW = 38;
int foodH = 38;

// =====================
// 护盾模式
// =====================
bool hasShield = false;

// =====================
// 函数声明
// =====================
void loadRoleImages();
void drawTransparentImage(int x, int y, IMAGE* img);
void drawHome();
void drawIntro();
bool isMouseInRect(int mx, int my, int x1, int y1, int x2, int y2);
void resetGame();
void applyMode(int selectedMode);
void drawMenu();
void drawGame();
void updateGame();
void handleInput();
bool checkCollisionOne(int ox, int oy, int ow, int oh);
void handleCollision();
bool checkFoodCollision(int index);
void resetFoods(int baseX);
bool checkFoodCollision(int index);
void playBGMByState();
void stopAllBGM();






void stopAllBGM()
{
    mciSendString(L"stop homeBGM", NULL, 0, NULL);
    mciSendString(L"close homeBGM", NULL, 0, NULL);

    mciSendString(L"stop menuBGM", NULL, 0, NULL);
    mciSendString(L"close menuBGM", NULL, 0, NULL);

    mciSendString(L"stop gameBGM", NULL, 0, NULL);
    mciSendString(L"close gameBGM", NULL, 0, NULL);

    currentBGM = 0;
}

void playBGMByState()
{
    if (gameState == HOME)
    {
        if (currentBGM != 1)
        {
            stopAllBGM();

            mciSendString(
                L"open \"C:\\Users\\lenovo\\Desktop\\Game\\Game\\sounds\\home_bgm.wav\" type waveaudio alias homeBGM",
                NULL,
                0,
                NULL
            );

            mciSendString(L"play homeBGM", NULL, 0, NULL);
            currentBGM = 1;
        }
    }
    else if (gameState == MENU || gameState == INTRO)
    {
        if (currentBGM != 2)
        {
            stopAllBGM();

            mciSendString(
                L"open \"C:\\Users\\lenovo\\Desktop\\Game\\Game\\sounds\\menu_bgm.wav\" type waveaudio alias menuBGM",
                NULL,
                0,
                NULL
            );

            mciSendString(L"play menuBGM", NULL, 0, NULL);
            currentBGM = 2;
        }
    }
    else if (gameState == PLAYING )
    {
        if (currentBGM != 3)
        {
            stopAllBGM();

            mciSendString(
                L"open \"C:\\Users\\lenovo\\Desktop\\Game\\Game\\sounds\\game_bgm.wav\" type waveaudio alias gameBGM",
                NULL,
                0,
                NULL
            );

            mciSendString(L"play gameBGM", NULL, 0, NULL);
            currentBGM = 3;
        }
    }
    else if (gameState == GAME_OVER)
    {
        if (currentBGM != 0)
        {
            stopAllBGM();
        }
    }
}

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

    // 六个模式食物图片
    loadimage(&foodImgs[0], L"images/food1.png", 45, 45);
    loadimage(&foodImgs[1], L"images/food2.png", 45, 45);
    loadimage(&foodImgs[2], L"images/food3.png", 45, 45);
    loadimage(&foodImgs[3], L"images/food4.png", 45, 45);
    loadimage(&foodImgs[4], L"images/food5.png", 45, 45);
    loadimage(&foodImgs[5], L"images/food6.png", 45, 45);
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
// 生成一组弧线食物
// baseX 表示这一组食物的起始 x 坐标
// =====================
void resetFoods(int baseX)
{
    int ground = groundY[mode - 1];

    for (int i = 0; i < FOOD_COUNT; i++)
    {
        foodX[i] = baseX + i * 45;

        // 弧线：中间高，两边低
        if (i == 0 || i == 4)
        {
            foodY[i] = ground - 150;
        }
        else if (i == 1 || i == 3)
        {
            foodY[i] = ground - 185;
        }
        else
        {
            foodY[i] = ground - 210;
        }

        foodVisible[i] = true;
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

    resetFoods(WIDTH + 120);

    score = 0;

    foodCount = 0;

    gameState = PLAYING;
}

bool isMouseInRect(int mx, int my, int x1, int y1, int x2, int y2)
{
    return mx >= x1 && mx <= x2 && my >= y1 && my <= y2;
}

void drawHome()
{
    cleardevice();

    // 首页背景，可以直接用封面背景
    putimage(0, 0, &menuBg);

    setbkmode(TRANSPARENT);

    // 游戏标题
    settextcolor(RGB(40, 40, 40));
    settextstyle(48, 0, L"Comic Sans MS");
    outtextxy(230, 80, L"WISH DOLL GAME");

   

    // 按钮 1：角色选择
   

    settextcolor(RGB(70, 45, 65));
    settextstyle(30, 0, L"Comic Sans MS");
    outtextxy(325, 210, L"MENU");

    // 按钮 2：游戏介绍
   
    settextcolor(RGB(40, 60, 90));
    settextstyle(30, 0, L"Comic Sans MS");
    outtextxy(325, 290, L"How to Play");

    settextstyle(16, 0, L"Comic Sans MS");
    settextcolor(RGB(60, 60, 60));
    outtextxy(285, 390, L"Click a button with left mouse button");
}

void drawIntro()
{
    cleardevice();

    putimage(0, 0, &menuBg);
    setbkmode(TRANSPARENT);

    settextcolor(RGB(40, 40, 40));
    settextstyle(42, 0, L"Comic Sans MS");
    outtextxy(270, 45, L"How to Play");

    settextstyle(22, 0, L"Comic Sans MS");
    outtextxy(250, 130, L"1. Choose one of six characters.");
    outtextxy(250, 170, L"2. Press SPACE to jump.");
    outtextxy(250, 210, L"3. Avoid obstacles on the ground.");
    outtextxy(250, 250, L"4. Collect floating food to get points.");
    outtextxy(250, 290, L"5. Press R to restart, ESC to go back.");

    // 返回按钮
    
    settextcolor(RGB(80, 50, 30));
    settextstyle(26, 0, L"Comic Sans MS");
    outtextxy(70, 390, L"Back");
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
    settextstyle(48, 0, L"Comic Sans MS");
    outtextxy(230, 80, L"WISH DOLL GAME");

    settextstyle(16, 0, L"Comic Sans MS");
    outtextxy(320, 390, L"Click a character to start");

    // 六张角色图片，使用粉色透明绘制
    drawTransparentImage(60, 200, &roleImgs[0]);
    drawTransparentImage(180, 200, &roleImgs[1]);
    drawTransparentImage(310, 200, &roleImgs[2]);
    drawTransparentImage(430, 200, &roleImgs[3]);
    drawTransparentImage(550, 200, &roleImgs[4]);
    drawTransparentImage(670, 200, &roleImgs[5]);
    settextstyle(18, 0, L"Comic Sans MS");

    

    outtextxy(70, 285, L"SIONING");
    outtextxy(205, 285, L"KURI");
    outtextxy(305, 285, L"BUBBLE NYANG");
    outtextxy(440, 285, L"DANGTERI");
    outtextxy(555, 285, L"RYONRYON");
    outtextxy(675, 285, L"SAKUPANG");

   

    // Back 返回首页
    settextcolor(RGB(60, 60, 60));
    settextstyle(24, 0, L"Comic Sans MS");
    outtextxy(70, 390, L"Back");
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
// 食物碰撞检测
// =====================
bool checkFoodCollision(int index)
{
    if (!foodVisible[index])
    {
        return false;
    }

    if (playerX < foodX[index] + foodW &&
        playerX + playerW > foodX[index] &&
        playerY < foodY[index] + foodH &&
        playerY + playerH > foodY[index])
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

    
    // =====================
// 食物组移动与收集
// =====================
    for (int i = 0; i < FOOD_COUNT; i++)
    {
        foodX[i] -= obstacleSpeed;

        if (checkFoodCollision(i))
        {
            score += 1;
            foodCount += 1;
            foodVisible[i] = false;
        }
    }

    // 如果最后一个食物飞出屏幕左边，重新生成一组
    if (foodX[FOOD_COUNT - 1] + foodW < 0)
    {
        resetFoods(WIDTH + 150);
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

    if (gameState == PLAYING)
    {
        for (int i = 0; i < FOOD_COUNT; i++)
        {
            if (foodVisible[i])
            {
                drawTransparentImage(foodX[i], foodY[i], &foodImgs[mode - 1]);
            }
        }
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

    drawTransparentImage(20, 18, &foodImgs[mode - 1]);

    wchar_t foodText[80];
    swprintf_s(foodText, L"× %d", foodCount);

    settextstyle(24, 0, L"Comic Sans MS");
    outtextxy(65, 25, foodText);

    wchar_t scoreText[80];
    swprintf_s(scoreText, L"Score: %d", score);

    settextstyle(22, 0, L"Comic Sans MS");
    outtextxy(20, 60, scoreText);

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

    outtextxy(20, 90, modeText);

    outtextxy(20, 120, L"空格跳跃");

    if (mode == 5)
    {
        if (hasShield)
        {
            outtextxy(20, 150, L"护盾：可抵挡一次碰撞");
        }
        else
        {
            outtextxy(20, 150, L"护盾：已使用");
        }
    }

    // 游戏结束界面
    if (gameState == GAME_OVER)
    {
        setbkmode(TRANSPARENT);

        if (mode == 6)
        {
            settextcolor(WHITE);
        }
        else
        {
            settextcolor(BLACK);
        }

        settextstyle(44, 0, L"Comic Sans MS");
        outtextxy(290, 165, L"GAME OVER");

        // 三个鼠标按钮：Restart / Home / Menu
        settextstyle(24, 0, L"Comic Sans MS");

        outtextxy(260, 240, L"Restart");
        outtextxy(370, 240, L"Home");
        outtextxy(460, 240, L"Menu");
    }
}


// =====================
// 处理按键
// =====================
void handleInput()
{
    // 首页：鼠标点击两个按钮
    if (gameState == HOME)
    {
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                int mx = msg.x;
                int my = msg.y;

                // 点击 Character 按钮
                if (isMouseInRect(mx, my, 280, 220, 520, 275))
                {
                    gameState = MENU;
                    Sleep(150);
                    return;
                }

                // 点击 How to Play 按钮
                if (isMouseInRect(mx, my, 280, 305, 520, 360))
                {
                    gameState = INTRO;
                    Sleep(150);
                    return;
                }
            }
        }

        return;
    }

    // 游戏介绍页：鼠标点击 Back 按钮
    if (gameState == INTRO)
    {
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                int mx = msg.x;
                int my = msg.y;

                // 点击左下角 Back 返回首页
                if (isMouseInRect(mx, my, 60, 375, 150, 430))
                {
                    gameState = HOME;
                    Sleep(150);
                    return;
                }
            }
        }

        return;
    }

    // 游戏结束页：鼠标点击 Restart / Home / Menu
    if (gameState == GAME_OVER)
    {
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                int mx = msg.x;
                int my = msg.y;

                // Restart：重新开始当前模式
                if (isMouseInRect(mx, my, 250, 210, 350, 260))
                {
                    resetGame();
                    Sleep(150);
                    return;
                }

                // Home：返回首页
                if (isMouseInRect(mx, my, 360, 210, 440, 260))
                {
                    gameState = HOME;
                    Sleep(150);
                    return;
                }

                // Menu：返回角色选择页
                if (isMouseInRect(mx, my, 450, 210, 540, 260))
                {
                    gameState = MENU;
                    Sleep(150);
                    return;
                }
            }
        }

        return;
    }

    // 角色选择页：鼠标左键点击角色图片或文字选择
    if (gameState == MENU)
    {
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                int mx = msg.x;
                int my = msg.y;

                // 点击角色 1：图片 + SIONING 文字
                if (isMouseInRect(mx, my, 45, 140, 150, 315))
                {
                    applyMode(1);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击角色 2：图片 + KURI 文字
                if (isMouseInRect(mx, my, 165, 140, 270, 315))
                {
                    applyMode(2);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击角色 3：图片 + BUBBLE NYANG 文字
                if (isMouseInRect(mx, my, 285, 140, 420, 315))
                {
                    applyMode(3);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击角色 4：图片 + DANGTERI 文字
                if (isMouseInRect(mx, my, 415, 140, 530, 315))
                {
                    applyMode(4);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击角色 5：图片 + RYONRYON 文字
                if (isMouseInRect(mx, my, 535, 140, 650, 315))
                {
                    applyMode(5);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击角色 6：图片 + SAKUPANG 文字
                if (isMouseInRect(mx, my, 655, 140, 780, 315))
                {
                    applyMode(6);
                    resetGame();
                    Sleep(150);
                    return;
                }

                // 点击 Back 返回首页
                if (isMouseInRect(mx, my, 60, 375, 150, 430))
                {
                    gameState = HOME;
                    Sleep(150);
                    return;
                }
            }
        }

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
            playBGMByState();

            if (gameState == HOME)
            {
                drawHome();
            }
            else if (gameState == INTRO)
            {
                drawIntro();
            }
            else if (gameState == MENU)
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
        stopAllBGM();
        closegraph();

        return 0;
    }