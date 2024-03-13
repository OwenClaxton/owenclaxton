#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <mmsystem.h>
#include <windef.h>
#include <wingdi.h>
#include <errno.h>
#include <limits.h>
//#include <fontsub.h>
//#include <prnasnot.h>
//#include <prntvpt.h>
//#include <t2embapi.h>
//#include <tvout.h>
//#include <xpsprint.h>

/**
 * START
 * 
 * Author:
 *      Mr. Owen Thomas Claxton
 *      owenclaxton@gmail.com
 * 
 * Project:
 *      Win32 API and GDI/GDI+ experimentation
 * 
 * Compilation:
 *      gcc -o fun fun.c -lgdi32
 * 
 * END
 **/

/**
 * Useful Information:
 *      Window Messages: https://learn.microsoft.com/en-us/windows/win32/winmsg/window-notifications
 *      Window Creation: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
 * 
 * Referenced Examples:
 *      http://www.winprog.org/tutorial/simple_window.html
 **/

/**
 * Todo:
 * - Correct window movement pixel population (offscreen/onscreen)
 * 
 **/

#pragma comment(lib, "gdi32.lib")

#define CHAR_HEIGHT (5)
#define CHAR_WIDTH (5)
#define MIN_SCREEN_WIDTH (400)
#define MIN_SCREEN_HEIGHT (400)
#define MAX_SCREEN_WIDTH (1500)
#define MAX_SCREEN_HEIGHT (800)

#define OPF_ERROR (0b11100000)
#define OPF_CRTCL (0b11000000)
#define OPF_USRWT (0b10100000)
#define OPF_UPDTE (0b10000000)
#define OPF_STNDA (0b01100000)
#define OPF_STNDB (0b01000000)
#define OPF_DEBUG (0b00100000)
#define OPF_MRKER (0b00000000)

#define LOGLEVEL (-1)

#define OPF_FCODE (0b11100000)
#define OPF_SCODE (0b00011111)
#define BITS_PER_PIXEL (4)

#define LL_NOT_NULL     (0)
#define LL_IS_NULL      (1)
#define LL_SUBNOT_NULL  (2)
#define LL_SUBIS_NULL   (3)
#define LL_CALLOC_FAIL  (4)
#define LL_FREE_FAIL    (5)
#define LL_SUBMAKE_NULL (6)
#define LL_SUBMAKE_FAIL (7)
#define LL_SUBFREE_NULL (8)
#define LL_SUBFREE_FAIL (9)
#define LL_SUCCESS      (10)

#define CLR_WHT (0xFFFFFF)
#define CLR_BLK (0x000000)
#define CLR_PR_ (0xFF0000)
#define CLR_PG_ (0x00FF00)
#define CLR_PB_ (0x0000FF)

typedef struct linkedlist_ele   LL_ELE;
typedef struct linkedlist_slf   LL_SLF;

typedef int (*makeObj)(LL_SLF* lls_ptr, LL_ELE* lle_ptr);
typedef int (*freeObj)(LL_SLF* lls_ptr, LL_ELE* lle_ptr);
typedef int (*makeEle)(LL_SLF* lls_ptr, LL_ELE* lle_ptr, LL_ELE* lle_ptr_prev, void* objPremade);
typedef int (*freeEle)(LL_SLF* lls_ptr, LL_ELE* lle_ptr, bool force);

//Linked list for all the commands input in the terminal when run
struct linkedlist_ele { // LL_ELE
    LL_ELE* p_ptr;  // Pointer to previous
    LL_ELE* n_ptr;  // Pointer to next
    void* o;        // Object
};

//Struct for storing the linked list
struct linkedlist_slf { // LL_SLF
    LL_ELE* h_ptr;  // Pointer to head of list
    LL_ELE* t_ptr;  // Pointer to tail of list
    makeEle mEle;   // Function pointer to make linked list element
    makeObj mObj;   // Function pointer to make linked list object
    freeEle fEle;   
    freeObj fObj;
    int size;
    void* data;     // General void* for user data
};

typedef struct {
    int x;
    int y;
} pairI;

typedef struct {
    double x;
    double y;
} pair;

typedef struct {
    uint32_t* bMemory;
    pairI size;
    pair* pos;
} bitmapStruct;

typedef struct {
    pair lpos;
    pair lvel;
    pair lacc;
} odometry;

typedef struct {
    char name[21];
    int level;
} biography;
    
typedef struct {
    biography bio;
    odometry odom;
    bitmapStruct bStruct;
} character;

typedef struct {
    bool pause;
    LL_SLF chars;
    LL_SLF sprites;
    character* player;
} gameSettings;

typedef struct {
    char g_szClassName[30];
    bitmapStruct bStruct;
    BITMAPINFO bInfo;
    bool processReady;
    bool running;
    bool resize;
    bool pause;
    bool keyFocus;
    bool mseFocus;
    bool mouseTracking;
    bool startup;
    RECT clientRect;
    TRACKMOUSEEVENT tme;
    HINSTANCE hInstance;
    HWND hwnd;
    gameSettings gs;
} windowVariables;

// Sprites /////////////////////////////////////////////////////////////////////////////

uint32_t blockA[] = {  
                        CLR_WHT, CLR_PR_, CLR_PG_, CLR_PR_, CLR_WHT,
                        CLR_PR_, CLR_WHT, CLR_PG_, CLR_WHT, CLR_PR_,
                        CLR_PG_, CLR_PG_, CLR_PG_, CLR_PG_, CLR_PG_,
                        CLR_PR_, CLR_WHT, CLR_PG_, CLR_WHT, CLR_PR_,
                        CLR_WHT, CLR_PR_, CLR_PG_, CLR_PR_, CLR_WHT 
};

uint32_t blockB[] = {  
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
};

uint32_t BM_CharacterGeneric[] = {  
                        CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK,
                        CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT,
                        CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK,
                        CLR_BLK, CLR_BLK, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_WHT, CLR_BLK, CLR_BLK,
};

// Constructors ////////////////////////////////////////////////////////////////////////
    
    // Graphics
    void setScreen(windowVariables* wVars);
    void drawPixel(windowVariables* wVars, int X, int Y, uint32_t Color);
    void updateScreen(windowVariables* wVars);
    void drawBitmap(windowVariables* wVars, bitmapStruct* bStruct);

    // System Functions
    void doGraphics(windowVariables* wVars);
    void doResize(windowVariables* wVars);
    void doKeyPress(windowVariables* wVars, WPARAM wParam);
    void doGreet(windowVariables* wVars);

    // Operational
    int main(void);
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    int WINAPI prepareWindow(windowVariables* wVars);

    // Variable Manipulation
    long int boundLong(long int bounded, long int lower, long int upper);
    int boundInt(int bounded, int lower, int upper);
    int parseInteger(char* message, char* errorMessage, int repeatMessage, int skippable);

    // System State Manipulation
    void unpause(windowVariables* wVars);
    void pause(windowVariables* wVars);

    // Helper
    void oprintf(uint8_t inCode, char * format, ...);

    // Linked Lists
    int freeObject(LL_SLF* lls_ptr, LL_ELE* lle_ptr);
    int makeObject(LL_SLF* lls_ptr, LL_ELE* lle_ptr);
    int freeElement(LL_SLF* lls_ptr, LL_ELE* lle_ptr, bool force);
    int makeElement(LL_SLF* lls_ptr, LL_ELE* lle_ptr, LL_ELE* lle_ptr_prev, void* objPremade);
    int freeCharacter(LL_SLF* lls_ptr, LL_ELE* lle_ptr);
    int makeCharacter(LL_SLF* lls_ptr, LL_ELE* lle_ptr);

////////////////////////////////////////////////////////////////////////////////////////

int freeObject(LL_SLF* lls_ptr, LL_ELE* lle_ptr) {
    return LL_SUCCESS;
}

int makeObject(LL_SLF* lls_ptr, LL_ELE* lle_ptr) {
    return LL_SUCCESS;
}

int freeCharacter(LL_SLF* lls_ptr, LL_ELE* lle_ptr) {
    if (lle_ptr == NULL) return LL_IS_NULL;
    free(lle_ptr->o);
    lle_ptr->o = NULL;
    return LL_SUCCESS;
}

int makeCharacter(LL_SLF* lls_ptr, LL_ELE* lle_ptr) {
    if (lle_ptr->o != NULL) return LL_NOT_NULL;
    lle_ptr->o = calloc(1, sizeof(character));
    if (lle_ptr->o == NULL) return LL_CALLOC_FAIL;
    // on success, as bio/odom/bStruct are not pointers, calloc has allocated for these too.
    character init_chr = {
            .bio = {
                        .name = "John Smith",
                        .level = 0,
        },
            .odom = { 
                        .lpos = { .x = 0.0, .y = 0.0 },
                        .lvel = { .x = 0.0, .y = 0.0 },
                        .lacc = { .x = 0.0, .y = 0.0 },
        },
            .bStruct = {
                        .size = { .x = 9, .y = 9},
                        .bMemory = (BM_CharacterGeneric),
        }
    };
    memcpy(lle_ptr->o, &init_chr, sizeof(character));
    ((character*) lle_ptr->o)->bStruct.pos = &(((character*) lle_ptr->o)->odom.lpos); // relative to self
    return LL_SUCCESS;
}


int freeElement(LL_SLF* lls_ptr, LL_ELE* lle_ptr, bool force) {
    if (lle_ptr == NULL) return LL_IS_NULL;
    if (lle_ptr->o == NULL) {
        if (!force) {
            return LL_SUBIS_NULL;
        }
    } else { // clear if possible, otherwise...
        if (lls_ptr->fObj == NULL) return LL_SUBFREE_NULL;
        int tryFObj = lls_ptr->fObj(lls_ptr, lle_ptr);
        if (tryFObj != LL_SUCCESS) return LL_SUBFREE_FAIL;
        lle_ptr->o = NULL;
    }
    // ... push ahead to free element
    if (lle_ptr->p_ptr != NULL) { 
        lle_ptr->p_ptr->n_ptr = lle_ptr->n_ptr;
    } else {
        lls_ptr->h_ptr = lle_ptr->n_ptr;
    }
    if (lle_ptr->n_ptr != NULL) {
        lle_ptr->n_ptr->p_ptr = lle_ptr->p_ptr;
    } else {
        lls_ptr->t_ptr = lle_ptr->p_ptr;
    }
    
    lle_ptr->p_ptr = NULL;
    lle_ptr->n_ptr = NULL;

    free(lle_ptr);
    lle_ptr = NULL;

    lls_ptr->size = lls_ptr->size - 1;
    return LL_SUCCESS;
}

int makeElement(LL_SLF* lls_ptr, LL_ELE* lle_ptr, LL_ELE* lle_ptr_prev, void* objPremade) {
    if (lle_ptr != NULL) return LL_NOT_NULL;
    lle_ptr = (LL_ELE*) calloc(1, sizeof(LL_ELE));
    if (lle_ptr == NULL) return LL_CALLOC_FAIL;

    // Prepare linked list pointers (several conditions): ...
    if (lls_ptr->size == 0) { //... if first linkedlist element:
        lls_ptr->h_ptr = lle_ptr;
        lls_ptr->t_ptr = lle_ptr;
        lle_ptr->p_ptr = NULL;
        lle_ptr->n_ptr = NULL;
    } 
    else { // ... if not first linkedlist element:
        lle_ptr->p_ptr = lle_ptr_prev;
        if (lle_ptr_prev == NULL) { // ... then we are mounting before head
            lle_ptr->n_ptr = lls_ptr->h_ptr;
            lls_ptr->h_ptr = lle_ptr; // update head
        } else {
            lle_ptr->n_ptr = lle_ptr_prev->n_ptr; // insert between
            lle_ptr_prev->n_ptr = lle_ptr;
            if (lle_ptr->n_ptr == NULL) { // ... then we are mounting after tail
                lls_ptr->t_ptr = lle_ptr;
            } else { // ... then we are mounting between two elements
                lle_ptr->n_ptr->p_ptr = lle_ptr;
            }
        }
    }

    lls_ptr->size = lls_ptr->size + 1;
    if (lls_ptr->mObj == NULL) return LL_SUBMAKE_NULL;
    int tryMObj;
    lle_ptr->o = NULL;
    if (objPremade == NULL) { 
        oprintf(OPF_MRKER, "Making object...\n");
        tryMObj = lls_ptr->mObj(lls_ptr, lle_ptr);

    } else {
        oprintf(OPF_MRKER, "Using premade object...\n");
        lle_ptr->o = objPremade;
        tryMObj = LL_SUCCESS;
    }
    if (tryMObj != LL_SUCCESS) return LL_SUBMAKE_FAIL;
    return LL_SUCCESS;
}

void oprintf(uint8_t inCode, char * format, ...) {
    char buffer[256];

    va_list args;
    va_start (args, format);
    vsnprintf (buffer, sizeof(buffer), format, args);

    uint8_t inCode_F = (inCode & OPF_FCODE);
    uint8_t inCode_S = (inCode & OPF_SCODE);

    int k_prefix = 7;
    int k_body = 7;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    char prefix[16];

    switch (inCode_F) {
        case OPF_ERROR:
            strcpy(prefix, "ERROR"); //red with white
            k_prefix = 12;
            k_body = 15;
            break;
        case OPF_CRTCL:
            strcpy(prefix, "CRTCL"); //red
            k_prefix = 12;
            break;
        case OPF_UPDTE:
            strcpy(prefix, "UPDTE"); //green
            k_prefix = 10;
            break;
        case OPF_DEBUG:
            strcpy(prefix, "DEBUG"); //purple
            k_prefix = 5;
            break;
        case OPF_MRKER:
            strcpy(prefix, "!!!!!"); //all pink
            k_prefix = 13;
            k_body = 13;
            break;
        case OPF_USRWT:
            strcpy(prefix, "USRWT"); //cyan
            k_prefix = 11;
            break;
        default:
            strcpy(prefix, "     ");
            break;
    }

    if (inCode_F > LOGLEVEL) {
        SetConsoleTextAttribute(hConsole, k_body);
        printf("[");
        SetConsoleTextAttribute(hConsole, k_prefix);
        printf("%s", prefix);
        SetConsoleTextAttribute(hConsole, k_body);
        printf("] %s", buffer);
        SetConsoleTextAttribute(hConsole, 7); // ensure it resets afterwards.
    }

    va_end (args);
}

void doGreet(windowVariables* wVars) {
    char* errorMessage = "Please enter a non-zero integer, or enter to use default.\n";
    char* message1     = "Please enter a non-zero integer corresponding to screen width (in pixels): ";
    char* message2     = "Please enter a non-zero integer corresponding to screen height (in pixels): ";

    int sw_store = parseInteger(message1, errorMessage, 1, 1);
    if (sw_store != 0) {
        wVars->bStruct.size.x = boundLong(sw_store, MIN_SCREEN_WIDTH, MAX_SCREEN_WIDTH);
    }

    int sh_store = parseInteger(message2, errorMessage, 1, 1);    
    if (sh_store != 0) {
        wVars->bStruct.size.y = boundLong(sh_store, MIN_SCREEN_HEIGHT, MAX_SCREEN_HEIGHT);
    } 

    oprintf(OPF_UPDTE, "Screen size: %dx%d\n", wVars->bStruct.size.x, wVars->bStruct.size.y);
    oprintf(OPF_USRWT, "Press any key to begin.\n");
    getch();
    return;
}

void doKeyPress(windowVariables* wVars, WPARAM wParam) {
    switch(wParam) {
        case 'P':
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    drawPixel(wVars, i+1, j+1, CLR_PG_);
                }
            }
            updateScreen(wVars);
            break;
        case 'O':
            break;
        default:
            break;
    }
    return;
}

void drawBitmap(windowVariables* wVars, bitmapStruct* bStruct) {
    uint32_t* pixel = wVars->bStruct.bMemory + ((int) round(bStruct->pos->y)) * wVars->bStruct.size.x + ((int) round(bStruct->pos->x));
    for (int Index = 0; Index < bStruct->size.y; Index++) {
        memcpy(pixel, bStruct->bMemory + Index * bStruct->size.x, sizeof(uint32_t) * bStruct->size.x);
        pixel+= wVars->bStruct.size.x;
    }
}

void drawPixel(windowVariables* wVars, int X, int Y, uint32_t Color) {
    uint32_t* pixel = wVars->bStruct.bMemory;
    pixel += Y * wVars->bStruct.size.x + X;
    *pixel = Color;
}

void clearScreen(windowVariables* wVars) {
    memset(wVars->bStruct.bMemory, 0, sizeof(uint32_t) * wVars->bStruct.size.x * wVars->bStruct.size.y);
}

void setScreen(windowVariables* wVars) {
    uint32_t* pixel = wVars->bStruct.bMemory;
    for(int Index = 0; Index < wVars->bStruct.size.x * wVars->bStruct.size.y; ++Index) {
        *pixel++ = CLR_WHT;
    }
}

void updateScreen(windowVariables* wVars) {
    HDC hdc = GetDC(wVars->hwnd);
    StretchDIBits(  hdc, 
                    0, 0,
                    wVars->bStruct.size.x, wVars->bStruct.size.y,
                    0, 0,
                    wVars->bStruct.size.x, wVars->bStruct.size.y,
                    wVars->bStruct.bMemory, &wVars->bInfo,
                    DIB_RGB_COLORS, SRCCOPY
                );
    ReleaseDC(wVars->hwnd, hdc);
}

void doGraphics(windowVariables* wVars) {
    if (wVars->processReady) {
        for (LL_ELE* loop_ele_ptr = wVars->gs.sprites.h_ptr; loop_ele_ptr != NULL; loop_ele_ptr = loop_ele_ptr->n_ptr) {
            if (loop_ele_ptr->o != NULL) { // safety :)
                bitmapStruct* objToDraw = (bitmapStruct*) loop_ele_ptr->o;
                if (objToDraw->bMemory != NULL) {
                    drawBitmap(wVars, objToDraw);
                } else {
                    oprintf(OPF_MRKER, "Drawing bitmap: bMemory NULL!\n");
                }
            } else {
                oprintf(OPF_MRKER, "doGraphics\n");
            }
        }
        updateScreen(wVars);
    }
    return;
}

void unpause(windowVariables* wVars) {
    wVars->pause = false;
    oprintf(OPF_UPDTE, "Application unpaused.\n");
    return;
}

void pause(windowVariables* wVars) {
    wVars->pause = true;
    wVars->resize = true; // in case necessary.
    oprintf(OPF_CRTCL, "Resize recorded.\n");
    oprintf(OPF_UPDTE, "Application paused.\n");
    return;
}

long int boundLong(long int bounded, long int lower, long int upper) {
    if (bounded > upper) {
        bounded = upper;
    } else if (bounded < lower) {
        bounded = lower;
    }
    return bounded;
}

int boundInt(int bounded, int lower, int upper) {
    if (bounded > upper) {
        bounded = upper;
    } else if (bounded < lower) {
        bounded = lower;
    }
    return bounded;
}

int parseInteger(char* message, char* errorMessage, int repeatMessage, int skippable) {
    long int ret = 0;
    char stringReceived[30];
    char* end; // end of number pointer
    oprintf(OPF_USRWT, message);
    while (1) {
        //scanf("%s", stringReceived);
        fgets(stringReceived, sizeof stringReceived, stdin);
        if (skippable && stringReceived[0] == '\n') {
            break;
        }
        errno = 0;
        ret = strtol(stringReceived, &end, 10); // base 10
        //oprintf(OPF_DEBUG, "RET: %d, ERRNO: %d, [%d, %d], [%d, %d]\n", ret, errno, EINVAL, ERANGE, INT_MAX, INT_MIN);
        //oprintf(OPF_DEBUG, "ptr: [%d|%d]\n", *end, stringReceived[strlen(stringReceived)-1]);
        if (*end != stringReceived[strlen(stringReceived)-1] || errno == EINVAL || errno == ERANGE || (ret > INT_MAX) || (ret < INT_MIN)) {
            oprintf(OPF_USRWT, errorMessage);
            if (repeatMessage) {
                oprintf(OPF_USRWT, message);
            };
        } else {
            break;
        }
    }
    return (int) ret;
}

void doResize(windowVariables* wVars) {
    if (wVars->resize) {
        GetClientRect(wVars->hwnd, &wVars->clientRect);
        wVars->bStruct.size.x = wVars->clientRect.right - wVars->clientRect.left;
        wVars->bStruct.size.y = wVars->clientRect.bottom - wVars->clientRect.top;
        wVars->resize = false;

        // Allocate memory for the bitmap
        if (!wVars->startup) VirtualFree(wVars->bStruct.bMemory, 0, MEM_RELEASE);
        wVars->bStruct.bMemory = VirtualAlloc(0,
                                    wVars->bStruct.size.x * wVars->bStruct.size.y * BITS_PER_PIXEL,
                                    MEM_RESERVE|MEM_COMMIT,
                                    PAGE_READWRITE
                                    );

        // BitmapInfo struct for StretchDIBits
        wVars->bInfo.bmiHeader.biSize = sizeof(wVars->bInfo.bmiHeader);
        wVars->bInfo.bmiHeader.biWidth = wVars->bStruct.size.x;
        wVars->bInfo.bmiHeader.biHeight = -wVars->bStruct.size.y; // Negative height for top left as origin
        if (wVars->startup) {
            wVars->bInfo.bmiHeader.biPlanes = 1;
            wVars->bInfo.bmiHeader.biBitCount = 32;
            wVars->bInfo.bmiHeader.biCompression = BI_RGB;
            wVars->bStruct.pos = NULL; // would need to calloc this, but pos.x and pos.y will always be zero.
        }
        clearScreen(wVars);
    }
    return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    windowVariables* wVars = (windowVariables*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch(msg) {
        //case WM_PAINT:
        //    break;
        //case WM_CLOSE:
        //    DestroyWindow(hwnd);
        //    break;
        case WM_CHAR: //WM_KEYDOWN cannot distinguish capitalisation without getting real funky
            doKeyPress(wVars, wParam);
            break;
        case WM_GETMINMAXINFO:
            if (wVars && wVars->processReady) {
                MINMAXINFO* mmi = (MINMAXINFO*) lParam;
                mmi->ptMinTrackSize.x = MIN_SCREEN_WIDTH;
                mmi->ptMinTrackSize.y = MIN_SCREEN_HEIGHT;
                mmi->ptMaxTrackSize.x = MAX_SCREEN_WIDTH;
                mmi->ptMaxTrackSize.y = MAX_SCREEN_HEIGHT;
                wVars->resize = true;
            }
            break;
        case WM_SETFOCUS:
            wVars->keyFocus = true;
            unpause(wVars);
            break;
        case WM_KILLFOCUS:
            wVars->keyFocus = false;
            pause(wVars);
            break;
        case WM_MOUSEMOVE:
            if (!wVars->mouseTracking) { // start tracking if we aren't already
                oprintf(OPF_DEBUG, "[Mouse] Move Tracking.\n");
                wVars->tme.cbSize = sizeof(TRACKMOUSEEVENT);
                wVars->tme.dwFlags = TME_HOVER | TME_LEAVE;
                wVars->tme.hwndTrack = wVars->hwnd;
                wVars->tme.dwHoverTime = HOVER_DEFAULT;
                wVars->mouseTracking = TrackMouseEvent(&wVars->tme);
            }
            break;
        case WM_MOUSELEAVE:
            oprintf(OPF_DEBUG, "[Mouse] Leave detected.\n");
            wVars->mseFocus = false;
            wVars->mouseTracking = false;
            pause(wVars);
            break;
        case WM_MOUSEHOVER:
            oprintf(OPF_DEBUG, "[Mouse] Hover detected.\n");
            wVars->mseFocus = true;
            wVars->mouseTracking = FALSE;    // Mouse tracking
            wVars->tme.cbSize = sizeof(TRACKMOUSEEVENT);
            wVars->tme.dwFlags = TME_LEAVE;
            wVars->tme.hwndTrack = wVars->hwnd;
            wVars->mouseTracking = TrackMouseEvent(&wVars->tme);
            if (wVars->keyFocus) {
                unpause(wVars);
            }
            break;
        case WM_SIZE:
            switch(wParam) {
                case SIZE_MINIMIZED:
                    pause(wVars);
                    break;
                case SIZE_RESTORED:
                    unpause(wVars);
                    break;
                case SIZE_MAXIMIZED:
                    unpause(wVars);
                    break;
                default:
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI prepareWindow(windowVariables* wVars) {
    WNDCLASSEX windowClass;

    //Step 1: Registering the Window Class 
    windowClass.cbSize        = sizeof(WNDCLASSEX);                 //UINT Size in bytes of structure
    windowClass.style         = 0;                                  //UINT Class style
    windowClass.lpfnWndProc   = WndProc;                            //WNDPROC Pointer to window procedure
    windowClass.cbClsExtra    = 0;                                  //int Number of extra bytes to allocate for window-class structure
    windowClass.cbWndExtra    = 0;                                  //int Number of extra bytes to allocate for window instance
    windowClass.hInstance     = wVars->hInstance;                   //HINSTANCE Handle to the instance that contains the window procedure for the class
    windowClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);    //HICON Handle to class icon
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);        //HCURSOR Handle to class cursor
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);           //HBRUSH Handle to class background brush
    windowClass.lpszMenuName  = NULL;                               //LPCSTR Pointer to a null-terminted character string specifying resource name of class menu
    windowClass.lpszClassName = wVars->g_szClassName;               //LPCSTR Pointer to a null-terminted string or is an atom
    windowClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);    //HICON Handle to a small icon associated with window class

    if(!RegisterClassEx(&windowClass)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    wVars->hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, // Style
        wVars->g_szClassName, // Class name
        "The title of my window", // Title bar text
        WS_OVERLAPPEDWINDOW, // Window Style
        CW_USEDEFAULT, CW_USEDEFAULT, wVars->bStruct.size.x, wVars->bStruct.size.y, // X, Y, Width, Height (top left is zero)
        NULL, NULL, wVars->hInstance, NULL); // Parent window handle, menuhandle, application instance hanle, pointer to window creation data

    if(wVars->hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return GetLastError(); //0
    }

    SetWindowLongPtr(wVars->hwnd, GWLP_USERDATA, (LONG_PTR) wVars);
    ShowWindow(wVars->hwnd, 1); //int nCmdShow = 1
    UpdateWindow(wVars->hwnd);
    oprintf(OPF_CRTCL, "Window ready.\n");
    return 1;
}

int main(void) {
    windowVariables wVars = {
        .g_szClassName = "myWindowClass",
        .processReady = false,
        .pause = false,
        .resize = true,
        .running = true,
        .keyFocus = false,
        .mseFocus = false,
        .startup = true,
        .mouseTracking = false,
        .bStruct.size.x = MIN_SCREEN_WIDTH,
        .bStruct.size.y = MIN_SCREEN_HEIGHT,
        .bStruct.bMemory = NULL,
        .gs = (gameSettings) {
            .pause = false,
            .chars = (LL_SLF) {
                .h_ptr = NULL,
                .t_ptr = NULL,
                .mEle = makeElement,
                .fEle = freeElement,
                .mObj = makeCharacter,
                .fObj = freeCharacter,
                .data = NULL,
            },
            .sprites = (LL_SLF) {
                .h_ptr = NULL,
                .t_ptr = NULL,
                .mEle = makeElement,
                .fEle = freeElement,
                .mObj = makeObject, // sprites reuse existing objects, pass default handle.
                .fObj = freeObject, // sprites reuse existing objects, pass default handle.
                .data = NULL,
            },
            .player = NULL,
        },
    };

    int CharMakeResult = wVars.gs.chars.mEle(&wVars.gs.chars, wVars.gs.chars.t_ptr, NULL, NULL);
    if(CharMakeResult != LL_SUCCESS) {
        oprintf(OPF_ERROR, "CharMakeResult: %d. This should not happen.\n", CharMakeResult);   
        return 1;
    }
    wVars.gs.player = (character*) wVars.gs.chars.t_ptr->o;
    oprintf(OPF_DEBUG, "MAIN Info: %s.\n", ((character*) wVars.gs.chars.t_ptr->o)->bio.name);

    int SprMakeResult = wVars.gs.sprites.mEle(&wVars.gs.sprites, wVars.gs.sprites.t_ptr, NULL, (void*) &wVars.gs.player->bStruct);
    if(SprMakeResult != LL_SUCCESS) {
        oprintf(OPF_ERROR, "SprMakeResult: %d. This should not happen.\n", SprMakeResult);   
        return 1;
    }

    doGreet(&wVars);
    oprintf(OPF_UPDTE, "Preparing window...\n");
    prepareWindow(&wVars);
    doResize(&wVars);
    oprintf(OPF_UPDTE, "Running main system loop...\n");
    wVars.processReady = true;
    wVars.startup = false;

    // System & Message Loop
    while(wVars.running) {
        MSG Message;
        while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            if(Message.message == WM_QUIT) {
                oprintf(OPF_CRTCL, "Received quit.\n");
                wVars.running = false;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        } 
        if (!wVars.pause) {
            doResize(&wVars);
            doGraphics(&wVars);
        }
        wVars.gs.player->odom.lpos.x = wVars.gs.player->odom.lpos.x + 0.01;
    }
    oprintf(OPF_UPDTE, "Done.");
    return 0;
}