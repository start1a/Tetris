#include "raylib.h"
#define GRID_VERTICAL 20
#define GRID_HORIZONTAL 10
#define GRID_LENGTH 40
#define MAX_CIRCLES  64

// Function
static void Init_Player();
static void Draw_InitGame();
static void Block_Create();
static void Block_Save();
static void Block_Control();
static bool Check_Blocks_Collision();
static bool Check_Wall();
static bool Block_Adjust();
static void Block_Change();
static void BlockLine_Delete();
static void Play_Game();
void Attack_Player();
static void Init_Game();



// Global variables
int frames = 0;
int num_Player = 0;
static Color color[8] = { VIOLET, RED, PURPLE, BLUE, GREEN, YELLOW, LIME, GRAY };
typedef enum { MAIN, COUNT, PLAY, END } GameScreen;
GameScreen gameScreen = MAIN;
Sound sound_click;
Sound sound_rotation;
Sound sound_arrival;
Sound sound_destroy;
Sound sound_count3;
Sound sound_count2;
Sound sound_count1;



// Player variables struct
typedef struct{
    int player_number;
    bool block_Arrival;
    int Attack;
    int block_number;
    int block_number2;
    int block_number_adjust;
    int block_location;
    int blockX[4];          // vertical coordinate
    int blockY[4];          // horizontal coordinate
    int next_blockX[4];
    int next_blockY[4];
    int pre_blockX[4];
    int pre_blockY[4];
    int block_arriveX[4];
    int block_arriveY[4];
    int block_next[4][2];
    int Grid[GRID_VERTICAL][GRID_HORIZONTAL];
}Player;

typedef struct {
    Vector2 position;
    float radius;
    float alpha;
    float speed;
    Color color;
} CircleWave;
 

int main()
{
    // Initialization 
    //--------------------------------------------------------------------------------------
    int screenWidth = 1600;
    int screenHeight = 1000;
    int startCount = 3;
    bool on_Countsound[3] = {0};
    
    // Sound ------------------------------------
    InitAudioDevice();
    sound_click = LoadSound("ClickButton.wav");
    sound_rotation = LoadSound("Rotation.wav");
    sound_arrival = LoadSound("Arrival.wav");
    sound_destroy = LoadSound("Destroy.wav");
    sound_count3 = LoadSound("Count3.wav");
    sound_count2 = LoadSound("Count2.wav");
    sound_count1 = LoadSound("Count1.wav");
    
    SetSoundVolume(sound_destroy, 0.2);
    SetSoundVolume(sound_click, 0.2);
    SetSoundVolume(sound_rotation, 0.1);
    SetSoundVolume(sound_arrival, 0.2);
    
    Music music_main = LoadMusicStream("Main_Theme.ogg");
    SetMusicVolume(music_main, 0.5);
    PlayMusicStream(music_main);
    
    Music music_game = LoadMusicStream("Game_Theme.ogg");
    SetMusicVolume(music_game, 0.1);
    PlayMusicStream(music_game);
    
    // Main Animation --------------------------------
    bool pause = false;
    Color colors[14] = { ORANGE, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK,
                         YELLOW, GREEN, SKYBLUE, PURPLE, BEIGE };
                        
    // Creates come circles for visual effect
    CircleWave circles[MAX_CIRCLES];
    
    for (int i = MAX_CIRCLES - 1; i >= 0; i--)
    {
        circles[i].alpha = 0.0f;
        circles[i].radius = GetRandomValue(10, 40);
        circles[i].position.x = GetRandomValue(circles[i].radius, screenWidth - circles[i].radius);
        circles[i].position.y = GetRandomValue(circles[i].radius, screenHeight - circles[i].radius);
        circles[i].speed = (float)GetRandomValue(1, 100)/20000.0f;
        circles[i].color = colors[GetRandomValue(0, 13)];
    }
    
    // Create Player ----------------------------
    Player p1 = { 1,1,0,0,0,0,50  };
    Player p2 = { 2,1,0,0,0,0,700 };
    
    InitWindow(screenWidth, screenHeight, "MY TETRIS");
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        BeginDrawing();
        
        switch (gameScreen)
        {
            case MAIN:
            {
                UpdateMusicStream(music_main);
                DrawRectangle(0,0,screenWidth, screenHeight, WHITE);
                
                // Animation
                for (int i = MAX_CIRCLES - 1; (i >= 0) && !pause; i--)
                {
                    circles[i].alpha += circles[i].speed;
                    circles[i].radius += circles[i].speed*10.0f;
                    
                    if (circles[i].alpha > 1.0f) circles[i].speed *= -1;
            
                    if (circles[i].alpha <= 0.0f)
                    {
                        circles[i].alpha = 0.0f;
                        circles[i].radius = GetRandomValue(10, 40);
                        circles[i].position.x = GetRandomValue(circles[i].radius, screenWidth - circles[i].radius);
                        circles[i].position.y = GetRandomValue(circles[i].radius, screenHeight - circles[i].radius);
                        circles[i].color = colors[GetRandomValue(0, 13)];
                        circles[i].speed = (float)GetRandomValue(1, 100)/20000.0f;
                    }
                }
                
                for (int i = MAX_CIRCLES - 1; i >= 0; i--)
                {
                    DrawCircleV(circles[i].position, circles[i].radius, Fade(circles[i].color, circles[i].alpha));
                }
                
                
                // Button
                for (int i = 0; i < 4; i++)
                    DrawRectangle(screenWidth/4, screenHeight/2+(i*120)-120, screenWidth/2, screenHeight/10, SKYBLUE);
                
                DrawText("TETRIS", 390, 150, 200, BLACK);
                DrawText("Solo", 720, 395, 80, WHITE);
                DrawText("Duo", 740, 515, 80, WHITE);
                DrawText("Trio", 720, 635, 80, WHITE);
                DrawText("Quarter", 650, 755, 80, WHITE);                
                
                // Function
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    StopMusicStream(music_game);
                    PlayMusicStream(music_game);
                    
                    // solo
                    if (GetMouseX() > (screenWidth/4) && GetMouseX() < (screenWidth*3)/4 && GetMouseY() > 380 && GetMouseY() < 480)
                    {
                        Init_Player(&p1);
                        num_Player = 1;
                        Init_Game(&startCount, on_Countsound);
                    }
                    
                    // duo
                    else if (GetMouseX() > (screenWidth/4) && GetMouseX() < (screenWidth*3)/4 && GetMouseY() > 500 && GetMouseY() < 600)
                    {
                        Init_Player(&p1);
                        Init_Player(&p2);
                        num_Player = 2;
                        Init_Game(&startCount, on_Countsound);
                    }
                    
                    // trio
                    else if (GetMouseX() > (screenWidth/4) && GetMouseX() < (screenWidth*3)/4 && GetMouseY() > 620 && GetMouseY() < 720)
                    {
                        DrawText("NOT IMPLEMENTED", 400, 875, 80, BLACK);
                    }
                    
                    // quarter
                    else if (GetMouseX() > (screenWidth/4) && GetMouseX() < (screenWidth*3)/4 && GetMouseY() > 740 && GetMouseY() < 840)
                    {
                        DrawText("NOT IMPLEMENTED", 400, 875, 80, BLACK);
                    }
                    
                }
                
                break;
            }
            
            
            
            case COUNT:
            {
                frames++;
                // On count-sound per sec
                if (startCount == 3 && !on_Countsound[2]) // 1초에 1번 울리도록 설정
                {
                    PlaySound(sound_count3);
                    on_Countsound[2] = true;
                }
                if (startCount == 2 && !on_Countsound[1])
                {
                    PlaySound(sound_count2);
                    on_Countsound[1] = true;
                }
                if (startCount == 1 && !on_Countsound[0])
                {
                    PlaySound(sound_count1);
                    on_Countsound[0] = true;
                }
                
                if (startCount != 0)
                {
                    DrawRectangle(0,0,screenWidth, screenHeight, WHITE);
                    DrawText(FormatText("%i", startCount), screenWidth/20 * 9, screenHeight/4, screenWidth/3, BLACK);
                    
                    if (((frames / 60) % 2) == 1)
                    {
                        startCount--;
                        frames = 0;
                    }
                }
                else gameScreen = PLAY;
                break;
            }
            
            
            
            case PLAY:
            {
                UpdateMusicStream(music_game);
                
                frames++;
                Draw_InitGame();
                
                switch (num_Player)
                {
                    case 1:
                        Play_Game(&p1);
                        break;
                    case 2:
                        Play_Game(&p1);
                        Play_Game(&p2);
                        
                            if (p1.Attack > 0)
                            {
                                Attack_Player(&p2, p1.Attack);
                                p1.Attack = 0;
                            }
                            if (p2.Attack > 0)
                            {
                                Attack_Player(&p1, p2.Attack);
                                p2.Attack = 0;
                            }
                        break;
                }

                // Realtime 0.8 second
                if (((frames / 50) % 2) == 1)
                    frames = 0;
                
                break;
            }
            
            case END:
            
                // Draw
                DrawRectangle(0,0,screenWidth, screenHeight, WHITE);
                DrawRectangle(screenWidth/4, screenHeight/2, screenWidth/2, screenHeight/10, SKYBLUE);
                DrawText("Return to Main", 600, 525, 60, WHITE);
                
                
                // Victory Player number
                if ( p1.player_number == -1 && num_Player != 1 )
                {
                    DrawText("PLAYER 2 IS WIN!", screenWidth/10, screenHeight/10, 150, BLUE);
                }
                else if ( p2.player_number == -1 )
                {
                    DrawText("PLAYER 1 IS WIN!", screenWidth/10, screenHeight/10, 150, RED);
                }
                else if ( num_Player == 1 )
                {
                    DrawText("GAME END", screenWidth/4, screenHeight/10, 150, BLUE);
                }
                
                
                // Is Button Clicked?
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {   
                    if (GetMouseX() > (screenWidth/4) && GetMouseX() < (screenWidth*3)/4 && GetMouseY() > 500 && GetMouseY() < 600)
                    {
                        gameScreen = MAIN;
                        p1.player_number = 1;
                        p2.player_number = 2;
                        frames = 0;
                        
                        PlaySound(sound_click);
                        StopMusicStream(music_main);
                        PlayMusicStream(music_main);
                    }
                }
                break;
                
        }
        
        EndDrawing();
        
    }    

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadSound(sound_destroy);
    UnloadSound(sound_click);
    UnloadSound(sound_rotation);
    UnloadSound(sound_arrival);
    UnloadSound(sound_count1);
    UnloadSound(sound_count2);
    UnloadSound(sound_count3);
    UnloadMusicStream(music_main); 
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context

    //--------------------------------------------------------------------------------------

    return 0;
}



static void Init_Player(Player *p)
{
    for(int i = 0; i < GRID_VERTICAL; i++)
    {
        for (int j = 0; j < GRID_HORIZONTAL; j++)
        {
            p->Grid[i][j] = -1;
        }
    }
    
    for (int i = 0; i < 4; i++)
    {
        p->blockX[i] = 0;
        p->blockY[i] = 0;
        p->next_blockX[i] = 0;
        p->next_blockY[i] = 0;
        p->pre_blockX[i] = 0;
        p->pre_blockY[i] = 0;
    }
    
    p->block_Arrival = true;
    p->block_number2 = GetRandomValue(0,6);
    p->block_number_adjust = 0;
    p->Attack = false;
}


static void Init_Game(int startCount, int on_Countsound[])
{
    startCount = 3;
    PlaySound(sound_click);
    for (int i = 0; i < 3; i++) on_Countsound[i] = false;
    gameScreen = COUNT;
}

// Draw Tetris GridLine
static void Draw_InitGame()
{

    ClearBackground(RAYWHITE);
    // game explanation
    DrawText("R : Transform, F : Down, D,G : <-->", 50, 900, 30, RED);
    DrawText("LShift : Immeditely Arrive", 50, 940, 30, RED);
    DrawText("Uarrow : Transform, Darrow : Down, L,Rarrow : <-->", 700, 900, 30, BLUE);
    DrawText("RShift : Immeditely Arrive", 700, 940, 30, BLUE);

    // Draw horizontal Line
    for (int i = 0; i < GRID_VERTICAL+1; i++)
    {

        DrawLine(50, 50+(i*GRID_LENGTH), 50+(GRID_LENGTH*GRID_HORIZONTAL), 50+(i*GRID_LENGTH), BLACK);
        DrawLine(700, 50+(i*GRID_LENGTH), 700+(GRID_LENGTH*GRID_HORIZONTAL), 50+(i*GRID_LENGTH), BLACK);

    }

    // Draw Vertical Line
    for (int i = 0; i < GRID_HORIZONTAL+1; i++)
    {

        DrawLine(50+(i*GRID_LENGTH), 50, 50+(i*GRID_LENGTH), 50+(GRID_LENGTH*GRID_VERTICAL), BLACK);
        DrawLine(700+(i*GRID_LENGTH), 50, 700+(i*GRID_LENGTH), 50+(GRID_LENGTH*GRID_VERTICAL), BLACK);
        
    }
    
    // Draw Next Block Grid Line
    for (int i = 0; i < 5; i++)
    {
        DrawLine(80+(GRID_LENGTH*GRID_HORIZONTAL), 50+(i*GRID_LENGTH), 80+(GRID_LENGTH*4)+(GRID_LENGTH*GRID_HORIZONTAL), 50+(i*GRID_LENGTH), BLACK);
        DrawLine(730+(GRID_LENGTH*GRID_HORIZONTAL),  50+(i*GRID_LENGTH), 730+(GRID_LENGTH*4)+(GRID_LENGTH*GRID_HORIZONTAL), 50+(i*GRID_LENGTH), BLACK);
        
        DrawLine(80+(GRID_LENGTH*GRID_HORIZONTAL)+(i*GRID_LENGTH), 50, 80+(GRID_LENGTH*GRID_HORIZONTAL)+(i*GRID_LENGTH), 50+(GRID_LENGTH*4), BLACK);
        DrawLine(730+(GRID_LENGTH*GRID_HORIZONTAL)+(i*GRID_LENGTH), 50, 730+(GRID_LENGTH*GRID_HORIZONTAL)+(i*GRID_LENGTH), 50+(GRID_LENGTH*4), BLACK);
    }

}


// Random Block Selected
static void Block_Create(Player *p)
{
    if (p->block_Arrival)
    {
        p->block_Arrival = false;
        p->block_number = p->block_number2;
        p->block_number2 = GetRandomValue(0,6);
        
        // Select Block Number
        switch (p->block_number)
        {
            case 0:
                p->blockX[0] = -1, p->blockX[1] = -1, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 4, p->blockY[1] = 5, p->blockY[2] = 4, p->blockY[3] = 5;
                break;
            
            case 1:
                p->blockX[0] = 0, p->blockX[1] = 0, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 3, p->blockY[1] = 4, p->blockY[2] = 5, p->blockY[3] = 6;
                break;

            case 2:
                p->blockX[0] = -1, p->blockX[1] = 0, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 5, p->blockY[1] = 4, p->blockY[2] = 5, p->blockY[3] = 6;
                break;

            case 3:
                p->blockX[0] = -1, p->blockX[1] = -1, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 4, p->blockY[1] = 5, p->blockY[2] = 5, p->blockY[3] = 6;
                break;

            case 4:
                p->blockX[0] = -1, p->blockX[1] = -1, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 6, p->blockY[1] = 5, p->blockY[2] = 5, p->blockY[3] = 4;
                break;

            case 5:
                p->blockX[0] = -1, p->blockX[1] = 0, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 4, p->blockY[1] = 4, p->blockY[2] = 5, p->blockY[3] = 6;
                break;

            case 6:
                p->blockX[0] = -1, p->blockX[1] = 0, p->blockX[2] = 0, p->blockX[3] = 0;
                p->blockY[0] = 6, p->blockY[1] = 6, p->blockY[2] = 5, p->blockY[3] = 4;
                break;
        }
        
        switch (p->block_number2)
    {
        case 0: { p->block_next[0][0] = 1; p->block_next[1][0] = 1; p->block_next[2][0] = 2; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 1; p->block_next[1][1] = 2; p->block_next[2][1] = 1; p->block_next[3][1] = 2;
                  break; }      // Cube
                  
        case 1: { p->block_next[0][0] = 1; p->block_next[1][0] = 1; p->block_next[2][0] = 1; p->block_next[3][0] = 1;
                  p->block_next[0][1] = 0; p->block_next[1][1] = 1; p->block_next[2][1] = 2; p->block_next[3][1] = 3;
                  break; }      // Linear
                  
        case 2: { p->block_next[0][0] = 1; p->block_next[1][0] = 1; p->block_next[2][0] = 1; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 1; p->block_next[1][1] = 2; p->block_next[2][1] = 3; p->block_next[3][1] = 2;
                  break; }      // ㅜ
                  
        case 3: { p->block_next[0][0] = 1; p->block_next[1][0] = 1; p->block_next[2][0] = 2; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 0; p->block_next[1][1] = 1; p->block_next[2][1] = 1; p->block_next[3][1] = 2;
                  break; }      // Z
                  
        case 4: { p->block_next[0][0] = 1; p->block_next[1][0] = 1; p->block_next[2][0] = 2; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 3; p->block_next[1][1] = 2; p->block_next[2][1] = 2; p->block_next[3][1] = 1;
                  break; }      // Z inversa
                  
        case 5: { p->block_next[0][0] = 1; p->block_next[1][0] = 2; p->block_next[2][0] = 2; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 1; p->block_next[1][1] = 1; p->block_next[2][1] = 2; p->block_next[3][1] = 3;
                  break; }      // L
                  
        case 6: { p->block_next[0][0] = 1; p->block_next[1][0] = 2; p->block_next[2][0] = 2; p->block_next[3][0] = 2;
                  p->block_next[0][1] = 2; p->block_next[1][1] = 2; p->block_next[2][1] = 1; p->block_next[3][1] = 0;
                  break; }      // L inversa
    }

        
        // Is block full?
            if ( Check_Blocks_Collision(p, p->blockX, p->blockY, 0,0) )
            {
                p->player_number = -1;
                gameScreen = END;
            }
    }
    
    
    // Go down Block per 0.8 second
    if (((frames / 50) % 2) == 1)
    {
        
        // Is block arrive floor?
        if (Check_Wall(p->blockX, '=', 19) || Check_Blocks_Collision(p ,p->blockX, p->blockY, 1, 0))
        {
            p->block_Arrival = true;                                                           // Arrive & Save
            PlaySound(sound_arrival);
            
            for (int i = 0; i < 4; i++)
                p->Grid[p->blockX[i]][p->blockY[i]] = p->block_number;
        }
        
        else                                                                                // No
        {
            for (int i = 0; i < 4; i++)                                                     // Keep going Down
                p->blockX[i]++;
        }
        
    }
         
    
    // Copy Block to block_arrive
    for (int i = 0; i < 4; i++)
    {
        p->block_arriveX[i] = p->blockX[i];
        p->block_arriveY[i] = p->blockY[i];
    }
    
    // Until No collide wall & other blocks 
    while ( Check_Wall(p->block_arriveX, '<', 19) && !Check_Blocks_Collision(p, p->block_arriveX, p->block_arriveY, 1, 0) )
    {
        for (int i = 0; i < 4; i++)
            p->block_arriveX[i]++;
    }
    
}



// Check & Save the Block State 
static void Block_Save(Player *p)
{
    
    // Draw Block
    for (int i = 0; i < 4; i++)
    {
        DrawRectangle(p->block_location+(GRID_LENGTH*p->blockY[i]), 50+(GRID_LENGTH*p->blockX[i]), GRID_LENGTH-2, GRID_LENGTH-2, WHITE);
        DrawRectangle(p->block_location+1+(GRID_LENGTH*p->blockY[i]), 51+(GRID_LENGTH*p->blockX[i]), GRID_LENGTH-3, GRID_LENGTH-3, color[p->block_number]);
        DrawRectangle(p->block_location+(GRID_LENGTH*p->block_arriveY[i]), 50+(GRID_LENGTH*p->block_arriveX[i]), GRID_LENGTH, GRID_LENGTH, Fade(PURPLE, 0.3));
    }
    
    // Save Block
    for (int i = 0; i < GRID_VERTICAL; i++)
    {
        for (int j = 0; j < GRID_HORIZONTAL; j++)
        {
            if (p->Grid[i][j] != -1)
            {
                DrawRectangle(p->block_location+(GRID_LENGTH*j), 50+(GRID_LENGTH*i), GRID_LENGTH-2, GRID_LENGTH-2, WHITE);
                DrawRectangle(p->block_location+1+(GRID_LENGTH*j), 51+(GRID_LENGTH*i), GRID_LENGTH-3, GRID_LENGTH-3, color[p->Grid[i][j]]);
            }
        }
    }
    
    for (int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++)
        {
            DrawRectangle(p->block_location+30+(GRID_LENGTH*GRID_HORIZONTAL)+(GRID_LENGTH*p->block_next[i][1]), 50+(p->block_next[i][0]*GRID_LENGTH), GRID_LENGTH-2, GRID_LENGTH-2, WHITE);
            DrawRectangle(p->block_location+31+(GRID_LENGTH*GRID_HORIZONTAL)+(GRID_LENGTH*p->block_next[i][1]), 51+(p->block_next[i][0]*GRID_LENGTH), GRID_LENGTH-3, GRID_LENGTH-3, color[p->block_number2]);
        }
}



// Control the Block by using Direction Key
void Block_Control(Player *p)
{
    
    // Block is Arrive immediately
    if (IsKeyPressed(KEY_RIGHT_SHIFT) && p->player_number == 2 || IsKeyPressed(KEY_LEFT_SHIFT) && p->player_number == 1)
    {
        for (int i = 0; i < 4; i++)
            p->Grid[p->block_arriveX[i]][p->block_arriveY[i]] = p->block_number;
        p->block_Arrival = true;
        
        PlaySound(sound_arrival);
    }
    
    
    // Move Right
    if (IsKeyPressed(KEY_RIGHT) && p->player_number == 2 || IsKeyPressed(KEY_G) && p->player_number == 1)
    {
        
        // <= 9이면 Y=9일 때, 오른쪽으로 이동 가능하여 이동 후 좌표 값이 10이 됨 (이하 동문)
        if (Check_Wall(p->blockY, '<', 9) && !Check_Blocks_Collision(p, p->blockX, p->blockY, 0, 1) )
            {
                for (int i = 0; i < 4; i++)
                    p->blockY[i]++;
            }
        
        if (Check_Wall(p->pre_blockY, '<', 9) && !Check_Blocks_Collision(p, p->pre_blockX, p->pre_blockY, 0, 1) )
            {
                for (int i = 0; i < 4; i++)
                    p->pre_blockY[i]++;
            }
    }
    
    // Move Left
    if (IsKeyPressed(KEY_LEFT) && p->player_number == 2 || IsKeyPressed(KEY_D) && p->player_number == 1)
    {
        if ( Check_Wall(p->blockY, '>', 0) && !Check_Blocks_Collision(p, p->blockX, p->blockY, 0, -1) )
            {
                for (int i = 0; i < 4; i++)
                    p->blockY[i]--;
            }
            
        if (Check_Wall(p->pre_blockY, '>', 0) && !Check_Blocks_Collision(p, p->pre_blockX, p->pre_blockY, 0, -1) )
            {
                for (int i = 0; i < 4; i++)
                    p->pre_blockY[i]--;
            }
    }
        
    // Move Down
    if (IsKeyDown(KEY_DOWN) && p->player_number == 2 || IsKeyDown(KEY_F) && p->player_number == 1)
    {
        if (Check_Wall(p->blockX, '<', 19) && !Check_Blocks_Collision(p, p->blockX, p->blockY, 1, 0))      // <= 19이면 X=19일 때, 아래쪽으로 이동 가능하여 이동 후 좌표 값이 20이 됨
            {
                for (int i = 0; i < 4; i++)
                    p->blockX[i]++;
            }
            
        if (Check_Wall(p->pre_blockX, '<', 19) && !Check_Blocks_Collision(p, p->pre_blockX, p->pre_blockY, 1, 0))      // <= 19이면 X=19일 때, 아래쪽으로 이동 가능하여 이동 후 좌표 값이 20이 됨
            {
                for (int i = 0; i < 4; i++)
                    p->pre_blockX[i]++;
            }
    }
    
    
    // Transform Block
    if (IsKeyPressed(KEY_UP) && p->player_number == 2 || IsKeyPressed(KEY_R) && p->player_number == 1)
    {
        PlaySound(sound_rotation);
        
        switch(p->block_number)
        {
            
            // Linear
            case 1:
            
                // state ㅡ
                if (p->blockX[0] == p->blockX[1])
                {
                    for (int i = 0; i < 4; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->next_blockX, p->next_blockY, -2,-1,0,1,1,0,-1,-2) )
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->next_blockX, p->next_blockY, -2,-1,0,1,2,1,0,-1) )
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 3 && !Block_Adjust(p, p->next_blockX, p->next_blockY, -1,0,1,2,2,1,0,-1) )
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 4 && !Block_Adjust(p, p->next_blockX, p->next_blockY,  -1,0,1,2,1,0,-1,-2) )
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }
                    
                }
                
                // state ㅣ
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 2,1,0,-1,-1,0,1,2) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 2,1,0,-1,-2,-1,0,1) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 3 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 1,0,-1,-2,-2,-1,0,1) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 4 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 1,0,-1,-2,-1,0,1,2) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }
                }
                break;
            
            
            // middle up
            case 2:
                // ㅗ
                if ( p->blockX[0] + 1 == p->blockX[2] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 1,-1,0,1,-1,1,0,-1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // ㅓ
                else if ( p->blockY[0] + 1 == p->blockY[2] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 1,1,0,-1,1,-1,0,1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // ㅜ
                else if ( p->blockX[0] - 1 == p->blockX[2] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, -1, 1, 0, -1, 1, 1, 0, -1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // ㅏ
                else if ( p->blockY[0] - 1 == p->blockY[2] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, -1, -1, 0, 1, -1, -1, 0, 1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                
                break;
            

            // Z
            case 3:
                
                // Z
                if ( p->blockX[0] == p->blockX[1] )
                {
                    for (int i = 0; i < 2; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,1,1,0,-1,-2) )   // 1st
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->next_blockX, p->next_blockY, -1,0,-1,0,2,1,0,-1) )   // 1st
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }

                }
                
                // N
                else {
                    for (int i = 0; i < 2; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 1,0,1,0,-2,-1,0,1) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 0,-1,0,-1,-1,0,1,2) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }
                }
                break;
                
                
            // opposite Z
            case 4:
                
                // Z
                if ( p->blockX[0] == p->blockX[1] )
                {
                    for (int i = 0; i < 2; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,1,-1,0,1,2) )   // 1st
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,1,-2,-1,0,1) )   // 1st
                        {
                            Block_Change(p->pre_blockX, p->pre_blockY, p->blockX, p->blockY);
                            Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }

                }
                
                // N
                else {
                    for (int i = 0; i < 2; i++)
                    {
                        if ( p->block_number_adjust == 1 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 0,-1,0,-1,2,1,0,-1) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust++;
                    
                        if ( p->block_number_adjust == 2 && !Block_Adjust(p, p->pre_blockX,  p->pre_blockY, 0,-1,0,-1,1,0,-1,-2) )
                        {
                            Block_Change(p->blockX, p->blockY, p->pre_blockX, p->pre_blockY);
                            break;
                        }
                        else p->block_number_adjust = 1;
                    }
                }
                break;
                
                
            // L
            case 5:
                
                // ㄴ
                if ( p->blockX[0] + 1 == p->blockX[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 1,0,-1,-2,0,1,0,-1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // opposite ㄴ
                else if ( p->blockY[0] + 1 == p->blockY[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,-1,0,1,1,0,-1,-2) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                    else if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,-1,0,1,2,1,0,-1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // ㄱ
                else if ( p->blockX[0] - 1 == p->blockX[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, -1,0,1,2,0,-1,0,1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // opposite ㄱ
                else if ( p->blockY[0] - 1 == p->blockY[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,-1,-1,0,1,2) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                    else if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,-1,-2,-1,0,1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                
                break;
                
            // L inverse
            case 6:
                
                // ㄴ
                if ( p->blockX[0] + 1 == p->blockX[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 1,0,-1,-2,0,-1,0,1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // opposite ㄴ
                else if ( p->blockY[0] - 1 == p->blockY[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,-1,0,1,-1,0,1,2) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                    else if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,-1,0,1,-2,-1,0,1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // ㄱ
                else if ( p->blockX[0] - 1 == p->blockX[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, -1,0,1,2,0,1,0,-1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                // opposite ㄱ
                else if ( p->blockY[0] + 1 == p->blockY[1] )
                {
                    if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,-1,1,0,-1,-2) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                    else if ( !Block_Adjust(p, p->next_blockX, p->next_blockY, 0,1,0,-1,2,1,0,-1) )
                    {
                        Block_Change(p->blockX, p->blockY, p->next_blockX, p->next_blockY);
                    }
                }
                
        }
    }
    
}



// Check Block Collision
static bool Check_Blocks_Collision(Player *p, int ArrayX[], int ArrayY[], int x, int y)
{
    
    // Index >= 0
    if ((ArrayX[0]+x) >= 0 && (ArrayX[1]+x) >= 0 && (ArrayX[2]+x) >= 0 && (ArrayX[3]+x) >= 0 )
    {
        if (p->Grid[ArrayX[0]+x][ArrayY[0]+y] != -1 || p->Grid[ArrayX[1]+x][ArrayY[1]+y] != -1 ||
            p->Grid[ArrayX[2]+x][ArrayY[2]+y] != -1 || p->Grid[ArrayX[3]+x][ArrayY[3]+y] != -1)
            {
                return true;
            }
        
        return false;
    }
    
    // Blocks start Outside Grid
    else {
        
        if (p->Grid[ArrayX[0]+x+2][ArrayY[0]+y] != -1 || p->Grid[ArrayX[1]+x+2][ArrayY[1]+y] != -1 ||
            p->Grid[ArrayX[2]+x+2][ArrayY[2]+y] != -1 || p->Grid[ArrayX[3]+x+2][ArrayY[3]+y] != -1)
            {
                if (p->Grid[ArrayX[0]+x+1][ArrayY[0]+y] != -1 || p->Grid[ArrayX[1]+x+1][ArrayY[1]+y] != -1 ||
                    p->Grid[ArrayX[2]+x+1][ArrayY[2]+y] != -1 || p->Grid[ArrayX[3]+x+1][ArrayY[3]+y] != -1 )
                        return true;
            }

        else return false;
    }
}



// Check Wall Collision
static bool Check_Wall(int Array[], int ch, int i)
{
    
    // ASCII
    switch (ch)
    {
        case 62:
        {
            if( Array[0] > i && Array[1] > i && Array[2] > i && Array[3] > i )
                return true;
            else
                return false;
        }
        break;
        
        case 61:
        {
            if( Array[0] == i || Array[1] == i || Array[2] == i || Array[3] == i )
                return true;
            else
                return false;
        }
        break;
        
        case 60:
        {
            if( Array[0] < i && Array[1] < i && Array[2] < i && Array[3] < i )
                return true;
            else
                return false;
        }
        break;
    }
    
    return false;
}


static bool Block_Adjust(Player *p, int ArrayX[], int ArrayY[], int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4)
{
    ArrayX[0] = p->blockX[0] + x1, ArrayX[1] = p->blockX[1] + x2, ArrayX[2] = p->blockX[2] + x3, ArrayX[3] = p->blockX[3] + x4,
    
    ArrayY[0] = p->blockY[0] + y1, ArrayY[1] = p->blockY[1] + y2, ArrayY[2] = p->blockY[2] + y3, ArrayY[3] = p->blockY[3] + y4;
    
    if ( Check_Blocks_Collision(p, ArrayX, ArrayY, 0, 0) || !Check_Wall(ArrayX, '<', 20) || !Check_Wall(ArrayY, '>', -1) || !Check_Wall(ArrayY, '<', 10) )
    {
        return true;
    }
    else
        return false;
}


static void Block_Change(int Array_X1[], int Array_Y1[], int Array_X2[], int Array_Y2[])
{
    for (int i = 0; i < 4; i++)
    {
        Array_X1[i] = Array_X2[i];
        Array_Y1[i] = Array_Y2[i];
    }
}



// Delete full block line
static void BlockLine_Delete(Player *p)
{
    
    int Deleted_Line = -1;
    int saved_block = 0;
    
    
    // Is a line full?
    for (int i = GRID_VERTICAL-1; i >= 0; i--)
    {
        for (int j = 0; j < GRID_HORIZONTAL; j++)
        {
            if ( p->Grid[i][j] != -1 )
                saved_block++;
        }
        
        if ( saved_block == GRID_HORIZONTAL )
        {
            PlaySound(sound_destroy);
            Deleted_Line = i;
            saved_block = 0;

            for (int k = 0; k < GRID_HORIZONTAL; k++)
                p->Grid[Deleted_Line][k] = -1;
        }
        else
            saved_block = 0;
        
        
        // Refresh Blocks
        if ( Deleted_Line != -1 )
        {
            int Temp_Grid[Deleted_Line][GRID_HORIZONTAL];
            
            for (int a = 0; a < Deleted_Line; a++)
            {
                for (int b = 0; b < GRID_HORIZONTAL; b++)
                {
                    Temp_Grid[a][b] = p->Grid[a][b];
                    p->Grid[a][b] = -1;
                }
            }
        
            // Go down 1 space & Fill blocks
            for (int c = 0; c < Deleted_Line; c++)
            {
                for (int d = 0; d < GRID_HORIZONTAL; d++)
                    p->Grid[c+1][d] = Temp_Grid[c][d];
            }
            
            Deleted_Line = -1;
            i++;
            p->Attack++;        // Attack On
        }
    }
}


// Destroy 1 line -> Attack 1 line to other players
void Attack_Player(Player *p, int num_Attack)
{
    
    int Temp_Grid[GRID_VERTICAL][GRID_HORIZONTAL];
    int Random_Hole = GetRandomValue(0,9);
    
    for (int k = 0; k < num_Attack; k++)
    {        
        // Refresh Blocks
        for (int i = 0; i < GRID_VERTICAL; i++)
        {
            for (int j = 0; j < GRID_HORIZONTAL; j++)
            {
                Temp_Grid[i][j] = p->Grid[i][j];
                p->Grid[i][j] = -1;
            }
        }
        
        // Lift up 1 blank
        for (int i = 1; i < GRID_VERTICAL; i++)
        {
            for (int j = 0; j < GRID_HORIZONTAL; j++)
                p->Grid[i-1][j] = Temp_Grid[i][j];
        }
        
        // Add 1 Line Blocks With 1 hole
        for (int i = 0; i < GRID_HORIZONTAL; i++)
        {
            if ( i == Random_Hole )
                continue;
            
            else
                p->Grid[GRID_VERTICAL-1][i] = 7;
        }
        
        if ( Check_Blocks_Collision(p, p->blockX, p->blockY, 1, 0) )
        {
            for (int i = 0; i < 4; i++)
            {
                p->blockX[i]--;
            }
        }
        
        if ( Check_Blocks_Collision(p, p->block_arriveX, p->block_arriveY, 1, 0) )
        {
            for (int i = 0; i < 4; i++)
            {
                p->block_arriveX[i]--;
            }
        }
        
    }
    
}


// Enable to play function for each player
void Play_Game(Player *p)
{
    Block_Create(p);
    Block_Control(p);
    BlockLine_Delete(p);
    Block_Save(p);
}
