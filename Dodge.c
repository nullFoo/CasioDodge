// the skeleton of this code was stolen from https://github.com/itisrazza/casio-pong


// Import some libraries
#include "stdio.h"    // Standard IO
#include <stdlib.h>
#include "fxlib.h"    // CASIO fx-9860G SDK
#include "dispbios.h" // Display BIOS
#include "timer.h"    // Timer

#pragma region KeyFixStuffMikeySentMe

#define __KEYBIOS_H__
#define KEY_CHAR_0 71
#define KEY_CHAR_1 72
#define KEY_CHAR_2 62
#define KEY_CHAR_3 52
#define KEY_CHAR_4 73
#define KEY_CHAR_5 63
#define KEY_CHAR_6 53
#define KEY_CHAR_7 74
#define KEY_CHAR_8 64
#define KEY_CHAR_9 54
#define KEY_CHAR_DP 61
#define KEY_CHAR_EXP 51
#define KEY_CHAR_PMINUS 41
#define KEY_CHAR_PLUS 42
#define KEY_CHAR_MINUS 32
#define KEY_CHAR_MULT 43
#define KEY_CHAR_DIV 33
#define KEY_CHAR_FRAC 75
#define KEY_CHAR_LPAR 55
#define KEY_CHAR_RPAR 45
#define KEY_CHAR_COMMA 35
#define KEY_CHAR_STORE 25
#define KEY_CHAR_LOG 66
#define KEY_CHAR_LN 56
#define KEY_CHAR_SIN 46
#define KEY_CHAR_COS 36
#define KEY_CHAR_TAN 26
#define KEY_CHAR_SQUARE 67
#define KEY_CHAR_POW 57
#define KEY_CTRL_EXE 31
#define KEY_CTRL_DEL 44
#define KEY_CTRL_AC 32
#define KEY_CTRL_FD 65
#define KEY_CTRL_EXIT 47
#define KEY_CTRL_SHIFT 78
#define KEY_CTRL_ALPHA 77
#define KEY_CTRL_OPTN 68
#define KEY_CTRL_VARS 58
#define KEY_CTRL_UP 28
#define KEY_CTRL_DOWN 37
#define KEY_CTRL_LEFT 38
#define KEY_CTRL_RIGHT 27
#define KEY_CTRL_F1 79
#define KEY_CTRL_F2 69
#define KEY_CTRL_F3 59
#define KEY_CTRL_F4 49
#define KEY_CTRL_F5 39
#define KEY_CTRL_F6 29
#define KEY_CTRL_MENU 48
#ifndef OS2Change
#define OS2Change
#ifndef OS2Change_GetOS2
#define OS2Change_GetOS2
typedef int(*sc_i2cp2sip)(char*, char*, short int*, short int*);
const unsigned int sc0015[] = { 0xD201D002, 0x422B0009, 0x80010070, 0x0015 };
#define GlibGetOSVersionInfo (*(sc_i2cp2sip)sc0015)
int OSVersionAsInt(void)
{
unsigned char mainversion;
unsigned char minorversion;
unsigned short release;
unsigned short build;
GlibGetOSVersionInfo( &mainversion, &minorversion, &release, &build );
return ( ( mainversion << 24 ) & 0xFF000000 ) | ( ( minorversion << 16 ) & 0x00FF0000 ) | ( release & 0x0000FFFF );
}
#define isOS2 (OSVersionAsInt() >= 0x02020000)
#define OS2(x,y) ((OSVersionAsInt() >= 0x02020000)?y:x)
#endif
#ifndef OS2Change_Keyboard
#define OS2Change_Keyboard
void delay(void)
{
char i;
for (i=0; i<5; i++){};
}
unsigned char CheckKeyRow(unsigned char code)
{
unsigned char result=0;
short*PORTB_CTRL=(void*)0xA4000102;
short*PORTM_CTRL=(void*)0xA4000118;
char*PORTB=(void*)0xA4000122;
char*PORTM=(void*)0xA4000138;
char*PORTA=(void*)0xA4000120;
short smask;
char cmask;
unsigned char column, row;
column = code>>4;
row = code &0x0F;
smask = 0x0003 << (( row %8)*2);
cmask = ~( 1 << ( row %8) );
if(row <8)
{
  *PORTB_CTRL = 0xAAAA ^ smask;
  *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;
  delay();
  *PORTB = cmask;
  *PORTM = (*PORTM & 0xF0 ) | 0x0F;
}
else
{
  *PORTB_CTRL = 0xAAAA;
  *PORTM_CTRL = ((*PORTM_CTRL & 0xFF00 ) | 0x00AA)  ^ smask;
  delay();
  *PORTB = 0xFF;
  *PORTM = (*PORTM & 0xF0 ) | cmask;
}

delay();
result = (~(*PORTA))>>column & 1;
delay();
*PORTB_CTRL = 0xAAAA;
*PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;
delay();
*PORTB_CTRL = 0x5555;
*PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x0055;
delay();

return result;
}

unsigned char KeyDown(unsigned char keycode)
{
unsigned short key[8];
const unsigned short* keyboardregister = (unsigned short*)0xA44B0000;
if(isOS2)
{
  unsigned char row = keycode%10;
  memcpy(key, keyboardregister, sizeof(unsigned short) << 3);
 
  return (0 != (key[row >> 1] & 1 << keycode / 10 - 1 + ((row & 1) << 3)));
}
else
{
  return CheckKeyRow((keycode % 10) + ((keycode / 10 - 1) << 4));
}
}
unsigned char GetKeyMod(unsigned int *key)
{
unsigned char x, ret;

ret = GetKey(key);

for(x = 0; x < 80; x++)
{
  if(KeyDown(x))
  {
   *key = x;
   break;
  }
}
return ret;
}
#define IsKeyDown(x) KeyDown(x)
#define IsKeyUp(x) !KeyDown(x)
#define GetKey(x) GetKeyMod(x)
#endif
#endif

#pragma endregion

#pragma region Constants

// true/false since it's 2018
const char true  = 1;           // True and false aren't assigned, weird.
const char false = 0;

// Display properties
const char DISPLAY_WIDTH    = 128;
const char DISPLAY_HEIGHT   =  64;
const char DISPLAY_CENTER_X =  64; // 128 / 2
const char DISPLAY_CENTER_Y =  32; //  64 / 2

#pragma endregion

struct Projectile {
    char enabled;
    float x;
    float y;
    float xVelocity;
    float yVelocity;
    float xAcceleration;
    float yAcceleration;
};
struct Projectile projectiles[128];

#pragma region Variables

int playerHealth = 128;
int playerX = 64;
int playerY = 32;

int projectileIndex;
int maxProjectiles;
int currentAttack = 0;
const int attacksTotal = 3;

char quit = 0;

#pragma endregion

int Sign(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
float Signf(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
int Max(int a, int b) {
    if(a > b)
        return a;
    else
        return b;
}
int Min(int a, int b) {
    if(a < b)
        return a;
    else
        return b;
}

void AppQuit()
{
    KillTimer(ID_USER_TIMER1);
    KillTimer(ID_USER_TIMER2);
    KillTimer(ID_USER_TIMER3);
    KillTimer(ID_USER_TIMER4);
    KillTimer(ID_USER_TIMER5);

    quit = true;

    return;
}

#pragma region Drawing functions

/** Draws the projectils */
void DrawProjectiles()
{   
    float x1;
    float y1;
    float x2;
    float y2;
    float dx;
    float dy;
    int x;
    int y;
    int decide;
    int pk;
    int j;

    size_t i;
    for (i = 0; i < 128; i++)
    {
        if(projectiles[i].enabled) {
            x1 = projectiles[i].x;
            y1 = projectiles[i].y;
            x2 = projectiles[i].x + projectiles[i].xVelocity;
            y2 = projectiles[i].y + projectiles[i].yVelocity;
            Bdisp_SetPoint_VRAM(x1, y1, 1);
            Bdisp_SetPoint_VRAM(x2, y2, 1);

    // Bresenham's Line Generation Algorithm implementation stolen from https://www.geeksforgeeks.org/bresenhams-line-generation-algorithm/
            dx = abs(x2 - x1);
            dy = abs(y2 - y1);

            decide = (dx > dy);
            
            pk = 2 * dy - dx;
            for (j = 0; j <= dx; j++) {
                Bdisp_SetPoint_VRAM(x1, y1, 1);
                
                // checking either to decrement or increment the
                // value if we have to plot from (0,100) to (100,0)
                x1 < x2 ? x1++ : x1--;
                if (pk < 0) {
                    // decision value will decide to plot
                    // either  x1 or y1 in x's position
                    if (decide == 0) {
                        // putpixel(x1, y1, RED);
                        pk = pk + 2 * dy;
                    }
                    else {
                        //(y1,x1) is passed in xt
                        // putpixel(y1, x1, YELLOW);
                        pk = pk + 2 * dy;
                    }
                }
                else {
                    y1 < y2 ? y1++ : y1--;
                    if (decide == 0) {
        
                        // putpixel(x1, y1, RED);
                    }
                    else {
                        //  putpixel(y1, x1, YELLOW);
                    }
                    pk = pk + 2 * dy - 2 * dx;
                }
            }
        }
    }
}
/** Draws the player */
void DrawPlayer()
{   
    Bdisp_SetPoint_VRAM(playerX, playerY, 1);
    Bdisp_SetPoint_VRAM(playerX+1, playerY, 1);
    Bdisp_SetPoint_VRAM(playerX-1, playerY, 1);
    Bdisp_SetPoint_VRAM(playerX, playerY+1, 1);
    Bdisp_SetPoint_VRAM(playerX, playerY-1, 1);
}

void DrawHealthBar() {
    size_t i;
    for (i = 0; i < playerHealth; i++)
    {
        Bdisp_SetPoint_VRAM(i, 63, 1);
    }
    // debug bars
    // for (i = 0; i < projectileIndex; i++)
    //     Bdisp_SetPoint_VRAM(i, 1, 1);
    // for (i = 0; i < maxProjectiles; i++)
    //     Bdisp_SetPoint_VRAM(i, 2, 1);
}

#pragma endregion

// spawn a new projectile at x and y with velocity and acceleration
void SpawnProjectile(float x, float y, float xVelocity, float yVelocity, float xAcceleration, float yAcceleration) {
    size_t i;
    for (i = 0; i < 128; i++)
    {
        if(!projectiles[i].enabled) { // found a free spot in the array
            // set up the projectile
            projectiles[i].x = x;
            projectiles[i].y = y;
            projectiles[i].xVelocity = xVelocity;
            projectiles[i].yVelocity = yVelocity;
            projectiles[i].xAcceleration = xAcceleration;
            projectiles[i].yAcceleration = yAcceleration;
            // enable it
            projectiles[i].enabled = true;
            // stop the loop, we're done
            return;
        }
    }
    
}

int IsCollidingWithPlayer(int x, int y) {
    if(x == playerX && y == playerY) return true;
    if(x == playerX) {
        if(y == playerY) return true;
        if(y == playerY + 1) return true;
        if(y == playerY - 1) return true;
    }
    if(y == playerY) {
        if(x == playerX) return true;
        if(x == playerX + 1) return true;
        if(x == playerX - 1) return true;
    }

    return false;
}

#pragma region Timer functions

/** Timer #2: Renders screen */
void RenderScreen()
{
    if(!quit) {
        // Clear current screen
        Bdisp_AllClr_VRAM();
            
        // Draw the different parts of the screen
        DrawProjectiles();
        DrawPlayer();
        DrawHealthBar();

        // Move VRAM to DD
        Bdisp_PutDisp_DD();
    }
}

void MoveProjectile(int arrayIndex, float x1, float y1, float x2, float y2) {
    // Bresenham's Line Generation Algorithm implementation stolen from https://www.geeksforgeeks.org/bresenhams-line-generation-algorithm/
    float dx;
    float dy;
    int decide;
    int pk;
    int j;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    decide = (dx > dy);
    
    pk = 2 * dy - dx;
    for (j = 0; j <= dx; j++) {
        // check for collision
        if(IsCollidingWithPlayer((int)x1, (int)y1)) {
            // damage player
            playerHealth -= 1;
            // destroy projectile
            projectiles[arrayIndex].enabled = false;
            return;
        }
        
        // checking either to decrement or increment the
        // value if we have to plot from (0,100) to (100,0)
        x1 < x2 ? x1++ : x1--;
        if (pk < 0) {
            // decision value will decide to plot
            // either  x1 or y1 in x's position
            if (decide == 0) {
                // putpixel(x1, y1, RED);
                pk = pk + 2 * dy;
            }
            else {
                //(y1,x1) is passed in xt
                // putpixel(y1, x1, YELLOW);
                pk = pk + 2 * dy;
            }
        }
        else {
            y1 < y2 ? y1++ : y1--;
            if (decide == 0) {

                // putpixel(x1, y1, RED);
            }
            else {
                //  putpixel(y1, x1, YELLOW);
            }
            pk = pk + 2 * dy - 2 * dx;
        }
    }

    projectiles[arrayIndex].x = x2;
    projectiles[arrayIndex].y = y2;
}

void Physics()
{
    size_t i;
    for (i = 0; i < 128; i++)
    {
        if(projectiles[i].enabled) {
            // collision with edge of map
            if(projectiles[i].x < 0 || projectiles[i].x > 128 || projectiles[i].y < 0 || projectiles[i].y > 64) {
                // destroy projectile
                projectiles[i].enabled = false;
                continue;
            }
            // move + detect player collisions
            MoveProjectile(i,
                projectiles[i].x,
                projectiles[i].y,
                projectiles[i].x + projectiles[i].xVelocity,
                projectiles[i].y + projectiles[i].yVelocity
            );
            // projectiles[i].x += projectiles[i].xVelocity;
            // projectiles[i].y += projectiles[i].yVelocity;
            // update velocity
            projectiles[i].xVelocity += projectiles[i].xAcceleration;
            projectiles[i].yVelocity += projectiles[i].yAcceleration;
        }
    }

    // stop player from leaving map
        if(playerX < 1)
            playerX = 1;
        if(playerX > 127)
            playerX = 127;
        if(playerY < 1)
            playerY = 1;
        if(playerY > 63)
            playerY = 63;
}

void Controls()
{
    if(!quit) {
        if(IsKeyDown(KEY_CTRL_LEFT)) {
            playerX--;
        }
        if(IsKeyDown(KEY_CTRL_RIGHT)) {
            playerX++;
        }
        if(IsKeyDown(KEY_CTRL_UP)) {
            playerY--;
        }
        if(IsKeyDown(KEY_CTRL_DOWN)) {
            playerY++;
        }
    }
    
}

#pragma region attacks

// next projectile in this attack
void SpawnNext() {
    // I HATE THE FACT THAT YOU HAVE TO DECLARE VARIABLES AT THE START OF THE FUNCTION IN C
    #pragma region a whole bunch of variable declarations
    int mod;
    int mod2;
    int xPos;
    int direction;
    int directionX;
    int directionY;
    int i;
    int j;
    #pragma endregion
    switch (currentAttack)
    {
        case 0:
            mod = projectileIndex % 2;
            xPos = mod == 0 ? 1 : 128 - 1;
            direction = mod == 0 ? 1 : -1;
            SpawnProjectile(xPos, (projectileIndex + 1) * 4, 0, 0, (float)direction * 0.1f, 0);
            break;
        case 1:
            mod = projectileIndex % 2;
            mod2 = (projectileIndex + 1) % 4;
            directionX = mod == 0 ? 1 : -1;
            // directionY = mod2 < 2 ? 1 : -1;
            directionY = ((projectileIndex % 8) - 4);
            SpawnProjectile(64, 32, (float)directionX * 2, (float)directionY, 0, 0);//Sign(-directionY) * 0.1f);
            break;
        case 2:
            if(projectileIndex % 5 == 0) {
                if(projectileIndex % 10 == 0) {
                    for (i = 0; i < 16; i++)
                    {
                        SpawnProjectile(i * 8, 0, 0, 2, 0, 0);
                    }
                }
                else {
                    for (i = 0; i < 16; i++)
                    {
                        SpawnProjectile(i * 8 + 4, 64, 0, -2, 0, 0);
                    }
                }
                
            }
            break;
        default:
            mod2 = projectileIndex % 2;
            xPos = mod2 == 0 ? 1 : 128 - 1;
            direction = mod2 == 0 ? 1 : -1;
            SpawnProjectile(xPos, (projectileIndex + 1) * 4, 0, 0, (float)direction * 0.1f, 0);
            break;
    }

    projectileIndex++;
    if(projectileIndex > maxProjectiles) {
        KillTimer(ID_USER_TIMER5); // stop this attack
        currentAttack++;
        if(currentAttack >= attacksTotal)
            currentAttack = 0;
    }
}
// the next "attack"
void NextProjectiles() {
    projectileIndex = 0;

    // note maxProjectiles can't be more than 50 or it'll run into the next attack and cause issues
    switch (currentAttack)
    {
        case 0:
            maxProjectiles = 16;
            break;
        case 1:
            maxProjectiles = 32;
            break;
        case 2:
            maxProjectiles = 15;
            break;
        default:
            maxProjectiles = 16;
            break;
    }

    SetTimer(ID_USER_TIMER5, 100, SpawnNext);
}

#pragma endregion

#pragma endregion

/** Add-in entry point
 * 
 * Parameters: app_mode - Is launched from the main menu
 *             strip_no - Strip number from eActivity
 * 
 * Returns:    NOT error
 */
// Start function
int AddIn_main(int app_mode, unsigned short strip_no)
{
    unsigned int i, j; // Iteration variables
    unsigned char str[3];
    unsigned int key;  // Keyboard input

    // Clear out the display
    Bdisp_AllClr_DDVRAM();

    // Set up timers for rendering and ball
    SetTimer(ID_USER_TIMER1, 50, RenderScreen);
    SetTimer(ID_USER_TIMER2, 50, Controls);
    SetTimer(ID_USER_TIMER3, 50, Physics);
    SetTimer(ID_USER_TIMER4, 5000, NextProjectiles);

    // Set quit handler
    SetQuitHandler(AppQuit);

    // The main thread manages the keys
    while (IsKeyUp(KEY_CTRL_MENU))
    {
        int keyResp = GetKeyWait(KEYWAIT_HALTON_TIMEROFF, 0, 0, &key);
    
        if (keyResp == KEYREP_KEYEVENT)
        {
            if (key == KEY_CTRL_MENU || key == KEY_CTRL_EXIT)
            {
                AppQuit();
                break;
            }
        }

        if (IsKeyDown(KEY_CTRL_MENU) || IsKeyDown(KEY_CTRL_EXIT))
        {
            AppQuit();
            break;
        }
    }

    // Good job on somehow breaking the loop. Cleanup time.
    AppQuit();

    // At the end, return application status
    return true;
}

#pragma region Do not touch

// Source code from here down should not be changed according to the
// CASIO SDK sample project.

#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

#pragma endregion
