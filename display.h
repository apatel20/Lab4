#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "maplogic.h"
#include <ti/grlib/grlib.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

/* palette */
#define BACKGROUNDCOLOR GRAPHICS_COLOR_PURPLE
#define LINECOLOR       GRAPHICS_COLOR_WHITE
#define FOREGROUNDCOLOR GRAPHICS_COLOR_CYAN
#define EMPHASISCOLOR   GRAPHICS_COLOR_YELLOW

void InitDisplay();
void DrawTime   (unsigned minutes, unsigned seconds);
void DrawMessage(char *s, uint32_t color);
void DrawScore  (int computerscore, int humanscore, uint32_t color);
void DrawBoard  (tcellstate map[9]);
void DrawWinner (tcellstate map[9],int winner, uint32_t color);
void DrawBoardGame(tcellstate map[9], uint32_t color1, uint32_t color2);
void PrintString(char *str, int row, int col);
void LCDDrawChar(unsigned row, unsigned col, int8_t c);
void PrintStringEC(char *str, int row, int col);

#endif /* DISPLAY_H_ */
