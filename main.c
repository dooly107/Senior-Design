/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#include <stdio.h>
#include <string.h>
#include <Hardware/SPI_Driver.h>
#include <Hardware/GPIO_Driver.h>
#include <Hardware/CS_Driver.h>
#include <Hardware/TIMERA_Driver.h>
#include <fatfs/ff.h>
#include <fatfs/diskio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
/* Standard Includes */
#include <stdint.h>

/* GrLib Includes */
#include <ti/grlib/button.h>
#include <ti/grlib/imageButton.h>
#include <ti/grlib/radioButton.h>
#include <ti/grlib/checkbox.h>
#include "LcdDriver/kitronix320x240x16_ssd2119_spi.h"
#include "images/images.h"
#include "touch_P401R.h"

//Touch screen context
touch_context g_sTouchContext;
//Graphics_ImageButton primitiveButton;
Graphics_Button yesButton;
Graphics_Button noButton;

//Sample Button
Graphics_Button maybeButton;
Graphics_ImageButton gamesButton;
Graphics_ImageButton gamesImageButton;
Graphics_Button goBackButton;
Graphics_Button mario;
Graphics_Button zelda;
Graphics_Button start;
Graphics_Button Home;
Graphics_Button Next;
Graphics_Button Back;
Graphics_ImageButton groupImage;
Graphics_Button creditButton;
Graphics_Button Hotkey1;
Graphics_Button Hotkey2;
Graphics_Button Hotkey3;
Graphics_Button Hotkey4;
Graphics_Button Bluetooth;
Graphics_Button Edit;
Graphics_Button Done;
bool EditPressed = false;


// Graphic library context
Graphics_Context g_sContext;

//Flag to know if a demo was run
bool g_ranDemo = false;

void Delay(uint16_t msec);
void boardInit(void);
void clockInit(void);
void initializeDemoButtons(void);
void drawMainMenu(void);
void runCredits(void);
void runGameListBeg(char[10][24], int);
void runGameListMid(char[10][24], int);
void runGameListEnd(char[10][24], int);
void viewHotkeyList(char[10][24], int, int, int);
void GameHotkeyList(char[10][24], int, int);
void drawEditMode(void);
void getGameList(void);
char gameList[10][24] = {{'\0'}};
char gameListFile[10][24] = {{'\0'}};
int8_t hotKey[200] = {0};
void getHotkey(int, int);
char* getHotkeyfromList(int);
void EditHotkey(int, int, char*);

void getGameList(void) {
    eUSCI_SPI_MasterConfig SPI0MasterConfig =
    {
         EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
         3000000,
         500000,
         EUSCI_B_SPI_MSB_FIRST,
         EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
         EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
         EUSCI_B_SPI_3PIN
    };

    /* Timer_A UpMode Configuration Parameters */
    Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
            30000,                                  // 1 ms tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    FATFS fatfs;                                            /* File system object */
    FRESULT errCode;                                        /* Error code object  */


    DIR DI;
    FILINFO FI;
    WDT_A_holdTimer();

    CS_Init();

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B3_BASE, SPI0MasterConfig);
    //UART_Init(EUSCI_A0_BASE, UART0Config);
    GPIO_Init(GPIO_PORT_P10, GPIO_PIN0);
    TIMERA_Init(TIMER_A1_BASE, UP_MODE, &upConfig, disk_timerproc);

    Interrupt_enableMaster();

    errCode = f_mount(&fatfs, "0", 1);
    if (errCode) {
        printf("HERE\n");
    }

    errCode = f_opendir(&DI, "/");
    if (errCode) {
        printf("HERE2");
    }
    int gameIndex = 0;
    do
    {
        /*Read a directory/file*/
        errCode = f_readdir(&DI, &FI);
        /*Check for errors. Trap MSP432 if there is an error*/
        if (errCode) {
            printf("HERE3");
        }
        if (!strstr(FI.fname, "~1")) {
            if (!strcmp(FI.fname, "MARIO")) {
                strcpy(gameList[gameIndex], "MARIO");
            }
            else if (!strcmp(FI.fname, "ZELDA")) {
                            strcpy(gameList[gameIndex], "ZELDA");
            }
            else if (!strcmp(FI.fname, "STREET")) {
                strcpy(gameList[gameIndex], "STREETFIGHTERS");
            }
            gameIndex++;
        }
    }while(FI.fname[0]);
}

void main(void)
{
    getGameList();
    // Initialization routines
    WDT_A_hold(WDT_A_BASE);
    boardInit();
    clockInit();
    initializeDemoButtons();
    int index;
    int bluetoothMode = 0;

    __enable_interrupt();

    // LCD setup using Graphics Library API calls
    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119, &g_sKitronix320x240x16_SSD2119_funcs);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss18b);
    Graphics_clearDisplay(&g_sContext);

    touch_initInterface();

    P1DIR &= ~BIT4;
    P1REN = BIT4;
    P1OUT = BIT4;
    P1DIR |= BIT4;
    P1OUT &= ~BIT4;

    drawMainMenu();

    // Loop to detect touch
    while(1)
    {
        index = 0;
        touch_updateCurrentTouch(&g_sTouchContext);

        if(g_sTouchContext.touch)
        {
            if(Graphics_isImageButtonSelected(&gamesButton,
                                              g_sTouchContext.x,
                                              g_sTouchContext.y))
            {
                Graphics_drawSelectedImageButton(&g_sContext,&gamesButton);
                runGameListBeg(gameList, index);
                g_ranDemo = true;
            }

            if(Graphics_isButtonSelected(&creditButton,
                                         g_sTouchContext.x,
                                         g_sTouchContext.y))
            {
                Graphics_drawSelectedButton(&g_sContext, &creditButton);
                runCredits();
                g_ranDemo = true;
            }
            if(Graphics_isButtonSelected(&Bluetooth,
                                         g_sTouchContext.x,
                                         g_sTouchContext.y))
            {
                Graphics_drawSelectedButton(&g_sContext, &Bluetooth);
                if (bluetoothMode == 0) {
                    P1OUT |= BIT4;
                    bluetoothMode = 1;
                }
                else if (bluetoothMode == 1) {
                    P1OUT &= ~BIT4;
                    bluetoothMode = 0;
                }
                g_ranDemo = true;
            }
            if(g_ranDemo == true)
            {
                g_ranDemo = false;
                drawMainMenu();
            }
        }
    }
}

void initializeDemoButtons(void)
{
    Hotkey1.xMin = 40;
    Hotkey1.xMax = 140;
    Hotkey1.yMin = 60;
    Hotkey1.yMax = 110;
    Hotkey1.borderWidth = 1;
    Hotkey1.selected = false;
    Hotkey1.fillColor = GRAPHICS_COLOR_GRAY;
    Hotkey1.borderColor = GRAPHICS_COLOR_WHITE;
    Hotkey1.selectedColor = GRAPHICS_COLOR_RED;
    Hotkey1.textColor = GRAPHICS_COLOR_WHITE;
    Hotkey1.selectedTextColor = GRAPHICS_COLOR_RED;
    Hotkey1.textXPos = 55;
    Hotkey1.textYPos = 75;
    Hotkey1.text = "Hotkey 1";
    Hotkey1.font = &g_sFontCm18b;

    Hotkey2.xMin = 170;
    Hotkey2.xMax = 270;
    Hotkey2.yMin = 60;
    Hotkey2.yMax = 110;
    Hotkey2.borderWidth = 1;
    Hotkey2.selected = false;
    Hotkey2.fillColor = GRAPHICS_COLOR_ORANGE;
    Hotkey2.borderColor = GRAPHICS_COLOR_WHITE;
    Hotkey2.selectedColor = GRAPHICS_COLOR_RED;
    Hotkey2.textColor = GRAPHICS_COLOR_BLACK;
    Hotkey2.selectedTextColor = GRAPHICS_COLOR_RED;
    Hotkey2.textXPos = 185;
    Hotkey2.textYPos = 75;
    Hotkey2.text = "Hotkey 2";
    Hotkey2.font = &g_sFontCm18b;

    Hotkey3.xMin = 40;
    Hotkey3.xMax = 140;
    Hotkey3.yMin = 130;
    Hotkey3.yMax = 180;
    Hotkey3.borderWidth = 1;
    Hotkey3.selected = false;
    Hotkey3.fillColor = GRAPHICS_COLOR_BLUE;
    Hotkey3.borderColor = GRAPHICS_COLOR_WHITE;
    Hotkey3.selectedColor = GRAPHICS_COLOR_RED;
    Hotkey3.textColor = GRAPHICS_COLOR_RED;
    Hotkey3.selectedTextColor = GRAPHICS_COLOR_RED;
    Hotkey3.textXPos = 55;
    Hotkey3.textYPos = 145;
    Hotkey3.text = "Hotkey 3";
    Hotkey3.font = &g_sFontCm18b;

    Hotkey4.xMin = 170;
    Hotkey4.xMax = 270;
    Hotkey4.yMin = 130;
    Hotkey4.yMax = 180;
    Hotkey4.borderWidth = 1;
    Hotkey4.selected = false;
    Hotkey4.fillColor = GRAPHICS_COLOR_YELLOW;
    Hotkey4.borderColor = GRAPHICS_COLOR_WHITE;
    Hotkey4.selectedColor = GRAPHICS_COLOR_RED;
    Hotkey4.textColor = GRAPHICS_COLOR_BLACK;
    Hotkey4.selectedTextColor = GRAPHICS_COLOR_RED;
    Hotkey4.textXPos = 185;
    Hotkey4.textYPos = 145;
    Hotkey4.text = "Hotkey 4";
    Hotkey4.font = &g_sFontCm18b;

    Bluetooth.xMin = 130;
    Bluetooth.xMax = 200;
    Bluetooth.yMin = 200;
    Bluetooth.yMax = 240;
    Bluetooth.borderWidth = 1;
    Bluetooth.selected = false;
    Bluetooth.fillColor = GRAPHICS_COLOR_RED;
    Bluetooth.borderColor = GRAPHICS_COLOR_RED;
    Bluetooth.selectedColor = GRAPHICS_COLOR_BLACK;
    Bluetooth.textColor = GRAPHICS_COLOR_BLACK;
    Bluetooth.selectedTextColor = GRAPHICS_COLOR_RED;
    Bluetooth.textXPos = 140;
    Bluetooth.textYPos = 210;
    Bluetooth.text = "Bluetooth";
    Bluetooth.font = &g_sFontCm12;

    creditButton.xMin = 180;
    creditButton.xMax = 290;
    creditButton.yMin = 50;
    creditButton.yMax = 160;
    creditButton.borderWidth = 1;
    creditButton.selected = false;
    creditButton.fillColor = GRAPHICS_COLOR_GRAY;
    creditButton.borderColor = GRAPHICS_COLOR_WHITE;
    creditButton.selectedColor = GRAPHICS_COLOR_RED;
    creditButton.textColor = GRAPHICS_COLOR_WHITE;
    creditButton.selectedTextColor = GRAPHICS_COLOR_RED;
    creditButton.textXPos = 210;
    creditButton.textYPos = 95;
    creditButton.text = "Credits";
    creditButton.font = &g_sFontCm18b;

    Back.xMin = 40;
    Back.xMax = 110;
    Back.yMin = 200;
    Back.yMax = 240;
    Back.borderWidth = 1;
    Back.selected = false;
    Back.fillColor = GRAPHICS_COLOR_RED;
    Back.borderColor = GRAPHICS_COLOR_RED;
    Back.selectedColor = GRAPHICS_COLOR_BLACK;
    Back.textColor = GRAPHICS_COLOR_BLACK;
    Back.selectedTextColor = GRAPHICS_COLOR_RED;
    Back.textXPos = 50;
    Back.textYPos = 210;
    Back.text = "BACK";
    Back.font = &g_sFontCm18;

    Home.xMin = 130;
    Home.xMax = 200;
    Home.yMin = 200;
    Home.yMax = 240;
    Home.borderWidth = 1;
    Home.selected = false;
    Home.fillColor = GRAPHICS_COLOR_RED;
    Home.borderColor = GRAPHICS_COLOR_RED;
    Home.selectedColor = GRAPHICS_COLOR_BLACK;
    Home.textColor = GRAPHICS_COLOR_BLACK;
    Home.selectedTextColor = GRAPHICS_COLOR_RED;
    Home.textXPos = 140;
    Home.textYPos = 210;
    Home.text = "HOME";
    Home.font = &g_sFontCm18;

    Next.xMin = 220;
    Next.xMax = 290;
    Next.yMin = 200;
    Next.yMax = 240;
    Next.borderWidth = 1;
    Next.selected = false;
    Next.fillColor = GRAPHICS_COLOR_RED;
    Next.borderColor = GRAPHICS_COLOR_RED;
    Next.selectedColor = GRAPHICS_COLOR_BLACK;
    Next.textColor = GRAPHICS_COLOR_BLACK;
    Next.selectedTextColor = GRAPHICS_COLOR_RED;
    Next.textXPos = 230;
    Next.textYPos = 210;
    Next.text = "NEXT";
    Next.font = &g_sFontCm18;

    Edit.xMin = 220;
    Edit.xMax = 290;
    Edit.yMin = 200;
    Edit.yMax = 240;
    Edit.borderWidth = 1;
    Edit.selected = false;
    Edit.fillColor = GRAPHICS_COLOR_RED;
    Edit.borderColor = GRAPHICS_COLOR_RED;
    Edit.selectedColor = GRAPHICS_COLOR_BLACK;
    Edit.textColor = GRAPHICS_COLOR_BLACK;
    Edit.selectedTextColor = GRAPHICS_COLOR_RED;
    Edit.textXPos = 230;
    Edit.textYPos = 210;
    Edit.text = "EDIT";
    Edit.font = &g_sFontCm18;

    Done.xMin = 220;
    Done.xMax = 290;
    Done.yMin = 200;
    Done.yMax = 240;
    Done.borderWidth = 1;
    Done.selected = false;
    Done.fillColor = GRAPHICS_COLOR_RED;
    Done.borderColor = GRAPHICS_COLOR_RED;
    Done.selectedColor = GRAPHICS_COLOR_BLACK;
    Done.textColor = GRAPHICS_COLOR_BLACK;
    Done.selectedTextColor = GRAPHICS_COLOR_RED;
    Done.textXPos = 230;
    Done.textYPos = 210;
    Done.text = "DONE";
    Done.font = &g_sFontCm18;

    start.xMin = 130;
    start.xMax = 200;
    start.yMin = 200;
    start.yMax = 240;
    start.borderWidth = 1;
    start.selected = false;
    start.fillColor = GRAPHICS_COLOR_RED;
    start.borderColor = GRAPHICS_COLOR_RED;
    start.selectedColor = GRAPHICS_COLOR_BLACK;
    start.textColor = GRAPHICS_COLOR_BLACK;
    start.selectedTextColor = GRAPHICS_COLOR_RED;
    start.textXPos = 130;
    start.textYPos = 205;
    start.text = "START";
    start.font = &g_sFontCm18;

    // Initiliaze primitives Demo Button
    gamesButton.xPosition = 20;
    gamesButton.yPosition = 50;
    gamesButton.borderWidth = 1;
    gamesButton.selected = false;
    gamesButton.imageWidth = games4BPP_UNCOMP.xSize;
    gamesButton.imageHeight = games4BPP_UNCOMP.ySize;
    gamesButton.borderColor = GRAPHICS_COLOR_WHITE;
    gamesButton.selectedColor = GRAPHICS_COLOR_RED;
    gamesButton.image = &games4BPP_UNCOMP;

    groupImage.xPosition = 85;
    groupImage.yPosition = 30;
    groupImage.borderWidth = 1;
    groupImage.selected = false;
    groupImage.imageWidth = group4BPP_UNCOMP.xSize;
    groupImage.imageHeight = group4BPP_UNCOMP.ySize;
    groupImage.borderColor = GRAPHICS_COLOR_WHITE;
    groupImage.selectedColor = GRAPHICS_COLOR_RED;
    groupImage.image = &group4BPP_UNCOMP;
}

void drawMainMenu(void)
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss18b);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Welcome to the Challenger's World",
                                AUTO_STRING_LENGTH,
                                159,
                                30,
                                TRANSPARENT_TEXT);
    drawEditMode();

    // Draw Primitives image button
    Graphics_drawImageButton(&g_sContext, &gamesButton);

    // Draw Images image button
    Graphics_drawButton(&g_sContext, &creditButton);
    Graphics_drawButton(&g_sContext, &Bluetooth);

    Graphics_setFont(&g_sContext, &g_sFontCmss14b);
    Graphics_drawStringCentered(&g_sContext, "Click on Images to check out awesome features",
                                AUTO_STRING_LENGTH,
                                159,
                                170,
                                TRANSPARENT_TEXT);
}

void chooseGameImage(char gameList[10][24], int index) {
    if (strcmp(gameList[index],"MARIO") == 0) {
        gamesImageButton.xPosition = 20;
        gamesImageButton.yPosition = 40;
        gamesImageButton.borderWidth = 1;
        gamesImageButton.selected = false;
        gamesImageButton.imageWidth = mario4BPP_UNCOMP.xSize;
        gamesImageButton.imageHeight = mario4BPP_UNCOMP.ySize;
        gamesImageButton.borderColor = GRAPHICS_COLOR_WHITE;
        gamesImageButton.selectedColor = GRAPHICS_COLOR_RED;
        gamesImageButton.image = &mario4BPP_UNCOMP;
    }
    else if (strcmp(gameList[index],"ZELDA") == 0) {
        gamesImageButton.xPosition = 20;
        gamesImageButton.yPosition = 40;
        gamesImageButton.borderWidth = 1;
        gamesImageButton.selected = false;
        gamesImageButton.imageWidth = zelda4BPP_UNCOMP.xSize;
        gamesImageButton.imageHeight = zelda4BPP_UNCOMP.ySize;
        gamesImageButton.borderColor = GRAPHICS_COLOR_WHITE;
        gamesImageButton.selectedColor = GRAPHICS_COLOR_RED;
        gamesImageButton.image = &zelda4BPP_UNCOMP;
    }
    else if (strcmp(gameList[index],"STREETFIGHTERS") == 0) {
        gamesImageButton.xPosition = 20;
        gamesImageButton.yPosition = 40;
        gamesImageButton.borderWidth = 1;
        gamesImageButton.selected = false;
        gamesImageButton.imageWidth = street_fighter4BPP_UNCOMP.xSize;
        gamesImageButton.imageHeight = street_fighter4BPP_UNCOMP.ySize;
        gamesImageButton.borderColor = GRAPHICS_COLOR_WHITE;
        gamesImageButton.selectedColor = GRAPHICS_COLOR_RED;
        gamesImageButton.image = &street_fighter4BPP_UNCOMP;
    }
}

void drawEditMode() {
    Graphics_setFont(&g_sContext, &g_sFontCm12);
    Graphics_drawString(&g_sContext, "Edit:",
                        AUTO_STRING_LENGTH,
                        260,
                        7,
                        TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss12b);
    if (EditPressed == false) {
        Graphics_drawString(&g_sContext, "OFF",
                            AUTO_STRING_LENGTH,
                            290,
                            7,
                            TRANSPARENT_TEXT);
    }
    else {
        Graphics_drawString(&g_sContext, "ON",
                            AUTO_STRING_LENGTH,
                            290,
                            7,
                            TRANSPARENT_TEXT);
    }
}


void runGameListBeg(char gameList[10][24], int index) {
    g_ranDemo = false;
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Click the Image to view Hotkeys",
                                AUTO_STRING_LENGTH,
                                159,
                                25,
                                TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss14b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawString(&g_sContext, title,
                                AUTO_STRING_LENGTH,
                                185,
                                100,
                                TRANSPARENT_TEXT);
    chooseGameImage(gameList, index);
    drawEditMode();
    Graphics_drawImageButton(&g_sContext, &gamesImageButton);
    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Next);

    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        if(Graphics_isImageButtonSelected(&gamesImageButton, g_sTouchContext.x,
                                     g_sTouchContext.y))
        {
            Graphics_drawSelectedImageButton(&g_sContext, &gamesImageButton);
            GameHotkeyList(gameList, index, 0);
            g_ranDemo = true;
        }
        else if(Graphics_isButtonSelected(&Next, g_sTouchContext.x,
                                          g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Next);
            Graphics_drawReleasedButton(&g_sContext, &Next);
            if (gameList[index + 2][0] != '\0') {
                index++;
                runGameListMid(gameList, index);
            }
            else {
                index++;
                runGameListEnd(gameList, index);
            }
            g_ranDemo = true;
        }
        else
        {
            if(g_ranDemo)
            {
                Graphics_drawReleasedImageButton(&g_sContext, &gamesImageButton);
                g_ranDemo = false;
                return;
            }
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                         g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void runGameListMid(char gameList[10][24], int index) {
    g_ranDemo = false;
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Click the Image to view Hotkeys",
                                AUTO_STRING_LENGTH,
                                159,
                                25,
                                TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss14b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawString(&g_sContext, title,
                        AUTO_STRING_LENGTH,
                        185,
                        100,
                        TRANSPARENT_TEXT);
    chooseGameImage(gameList, index);
    drawEditMode();
    Graphics_drawImageButton(&g_sContext, &gamesImageButton);
    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Next);
    Graphics_drawButton(&g_sContext, &Back);

    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        if(Graphics_isImageButtonSelected(&gamesImageButton, g_sTouchContext.x,
                                     g_sTouchContext.y))
        {
            Graphics_drawSelectedImageButton(&g_sContext, &gamesImageButton);
            GameHotkeyList(gameList, index, 1);
            g_ranDemo = true;
        }
        else if(Graphics_isButtonSelected(&Next, g_sTouchContext.x,
                                          g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Next);
            Graphics_drawReleasedButton(&g_sContext, &Next);
            if (gameList[index + 2][0] != '\0') {
                index++;
                runGameListMid(gameList, index);
            }
            else {
                index++;
                runGameListEnd(gameList, index);
            }
            g_ranDemo = true;
        }
        else if (Graphics_isButtonSelected(&Back, g_sTouchContext.x,
                                                  g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Back);
            Graphics_drawReleasedButton(&g_sContext, &Back);
            if (index - 1 == 0) {
                index--;
                runGameListBeg(gameList, index);
            }
            else {
                index--;
                runGameListMid(gameList, index);
            }
            g_ranDemo = true;
        }
        else
        {
            if(g_ranDemo)
            {
                Graphics_drawReleasedImageButton(&g_sContext, &gamesImageButton);
                g_ranDemo = false;
                return;
            }
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                         g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void runGameListEnd(char gameList[10][24], int index) {
    g_ranDemo = false;
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Click the Image to view Hotkeys",
                                AUTO_STRING_LENGTH,
                                159,
                                25,
                                TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss14b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawString(&g_sContext, title,
                        AUTO_STRING_LENGTH,
                        185,
                        100,
                        TRANSPARENT_TEXT);
    chooseGameImage(gameList, index);
    drawEditMode();
    Graphics_drawImageButton(&g_sContext, &gamesImageButton);
    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Back);

    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        if(Graphics_isImageButtonSelected(&gamesImageButton, g_sTouchContext.x,
                                     g_sTouchContext.y))
        {
            Graphics_drawSelectedImageButton(&g_sContext, &gamesImageButton);
            GameHotkeyList(gameList, index, 2);
            g_ranDemo = true;
        }
        else if (Graphics_isButtonSelected(&Back, g_sTouchContext.x,
                                                  g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Back);
            Graphics_drawReleasedButton(&g_sContext, &Back);
            if (index - 1 == 0) {
                index--;
                runGameListBeg(gameList, index);
            }
            else {
                index--;
                runGameListMid(gameList, index);
            }
            g_ranDemo = true;
        }
        else
        {
            if(g_ranDemo)
            {
                Graphics_drawReleasedImageButton(&g_sContext, &gamesImageButton);
                g_ranDemo = false;
                return;
            }
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                         g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void GameHotkeyList(char gameList[10][24], int index, int goback) {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    drawEditMode();
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawStringCentered(&g_sContext, title,
                        AUTO_STRING_LENGTH,
                        150,
                        20,
                        TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss16b);
    Graphics_drawStringCentered(&g_sContext, "Hotkey Lists",
                        AUTO_STRING_LENGTH,
                        150,
                        40,
                        TRANSPARENT_TEXT);
    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Back);
    Graphics_drawButton(&g_sContext, &Hotkey1);
    Graphics_drawButton(&g_sContext, &Hotkey2);
    Graphics_drawButton(&g_sContext, &Hotkey3);
    Graphics_drawButton(&g_sContext, &Hotkey4);
    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        if (Graphics_isButtonSelected(&Back, g_sTouchContext.x,
                                           g_sTouchContext.y))
        {
                Graphics_drawSelectedButton(&g_sContext, &Back);
                Graphics_drawReleasedButton(&g_sContext, &Back);
                if (goback == 0) {
                    runGameListBeg(gameList, index);
                }
                else if (goback == 1) {
                    runGameListMid(gameList, index);
                }
                else {
                    runGameListEnd(gameList, index);
                }
                g_ranDemo = true;
        }
        else if (Graphics_isButtonSelected(&Hotkey1, g_sTouchContext.x,
                                           g_sTouchContext.y)) {
            Graphics_drawSelectedButton(&g_sContext, &Hotkey1);
            Graphics_drawReleasedButton(&g_sContext, &Hotkey1);
            viewHotkeyList(gameList, index, goback, 0);
        }
        else if (Graphics_isButtonSelected(&Hotkey2, g_sTouchContext.x,
                                           g_sTouchContext.y)) {
            Graphics_drawSelectedButton(&g_sContext, &Hotkey2);
            Graphics_drawReleasedButton(&g_sContext, &Hotkey2);
            viewHotkeyList(gameList, index, goback, 1);
        }
        else if (Graphics_isButtonSelected(&Hotkey3, g_sTouchContext.x,
                                           g_sTouchContext.y)) {
            Graphics_drawSelectedButton(&g_sContext, &Hotkey3);
            Graphics_drawReleasedButton(&g_sContext, &Hotkey3);
            viewHotkeyList(gameList, index, goback, 2);
        }
        else if (Graphics_isButtonSelected(&Hotkey4, g_sTouchContext.x,
                                           g_sTouchContext.y)) {
            Graphics_drawSelectedButton(&g_sContext, &Hotkey4);
            Graphics_drawReleasedButton(&g_sContext, &Hotkey4);
            viewHotkeyList(gameList, index, goback, 3);
        }
        else
        {
            if(g_ranDemo)
            {
                g_ranDemo = false;
                return;
            }
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                             g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void getHotkey(int index, int listIndex) {
    eUSCI_SPI_MasterConfig SPI0MasterConfig =
    {
         EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
         3000000,
         500000,
         EUSCI_B_SPI_MSB_FIRST,
         EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
         EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
         EUSCI_B_SPI_3PIN
    };

    /* Timer_A UpMode Configuration Parameters */
    Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
            30000,                                  // 1 ms tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    FIL file_read;                                          /* Opened file object */
    FATFS fatfs;                                            /* File system object */
    FRESULT errCode;                                        /* Error code object  */
    unsigned int size_read = 0;

    WDT_A_holdTimer();

    CS_Init();
    DIR DI;
    FILINFO FI;

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B3_BASE, SPI0MasterConfig);
    GPIO_Init(GPIO_PORT_P10, GPIO_PIN0);
    TIMERA_Init(TIMER_A1_BASE, UP_MODE, &upConfig, disk_timerproc);
    int i;

    Interrupt_enableMaster();

    errCode = f_mount(&fatfs, "0", 1);
    if (errCode) {
        printf("HERE");
    }
    errCode = f_opendir(&DI, "/");
    if (errCode) {
        printf("HERE2");
    }
    char* compGame = gameList[index];
    if (!strcmp(compGame, "STREETFIGHTERS")) {
        compGame = "STREET";
    }
    char directory[10] = {'\0'};
    directory[0] = '/';
    int j;
    do
    {
        /*Read a directory/file*/
        errCode = f_readdir(&DI, &FI);
        /*Check for errors. Trap MSP432 if there is an error*/
        if (errCode) {
            printf("HERE3");
        }
        if (!strcmp(FI.fname, compGame)) {
            for (j = 0; j < sizeof(compGame) + 2; j++) {
                directory[j + 1] = compGame[j];
            }
            break;
        }
    }while(FI.fname[0]);
    errCode = f_opendir(&DI, directory);
    printf("%s\n", directory);
    if (errCode) {
        printf("HERE4");
    }
    if (listIndex == 0 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\1.TXT", FA_READ);
    }
    else if (listIndex == 1 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\2.TXT", FA_READ);
    }
    else if (listIndex == 2 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\3.TXT", FA_READ);
    }
    else if (listIndex == 3 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\4.TXT", FA_READ);
    }
    else if (listIndex == 0 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\1.TXT", FA_READ);
    }
    else if (listIndex == 1 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\2.TXT", FA_READ);
    }
    else if (listIndex == 2 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\3.TXT", FA_READ);
    }
    else if (listIndex == 3 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\4.TXT", FA_READ);
    }
    else if (listIndex == 0 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\1.TXT", FA_READ);
    }
    else if (listIndex == 1 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\2.TXT", FA_READ);
    }
    else if (listIndex == 2 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\3.TXT", FA_READ);
    }
    else if (listIndex == 3 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\4.TXT", FA_READ);
    }
    if (errCode) {
        printf("RETURN HERE\n");
        return 0;
        //return (int)errCode;
    }
    char temphotKey[200] = {'\0'};
    char temp[1023] = {'\0'};
    size_read = 0;
    for (;;) {
        errCode = f_read(&file_read, temp, sizeof(temp), &size_read);
        if (errCode || size_read == 0) {
            break;
        }
        for (i = 0; i < 1024; i++) {
            if (temp[i] == '\0') {
                break;
            }
            else {
                temphotKey[i] = (int8_t)(temp[i]);
            }
        }
        break;
    }
    Graphics_setFont(&g_sContext, &g_sFontCmss18b);
    Graphics_drawStringCentered(&g_sContext, temphotKey, AUTO_STRING_LENGTH, 150, 80, TRANSPARENT_TEXT);
    f_close(&file_read);
}

void viewHotkeyList(char gameList[10][24], int index, int goback, int listIndex) {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    drawEditMode();
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawStringCentered(&g_sContext, title,
                        AUTO_STRING_LENGTH,
                        150,
                        20,
                        TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss16b);
    if (listIndex == 0) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey1", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 1) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey2", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 2) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey3", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 3) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey4", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    Graphics_setFont(&g_sContext, &g_sFontCmss12);
    Graphics_drawStringCentered(&g_sContext, "If Edit:'ON', press buttons or move joystick to modify", AUTO_STRING_LENGTH, 150, 60, TRANSPARENT_TEXT);

    getHotkey(index, listIndex);

    WDT_A_hold(WDT_A_BASE);
    boardInit();
    clockInit();
    initializeDemoButtons();

    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Back);
    Graphics_drawButton(&g_sContext, &Edit);

    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        if (Graphics_isButtonSelected(&Back, g_sTouchContext.x,
                                      g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Back);
            Graphics_drawReleasedButton(&g_sContext, &Back);
            GameHotkeyList(gameList, index, goback);
        }
        if (Graphics_isButtonSelected(&Edit, g_sTouchContext.x,
                                      g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Edit);
            Graphics_drawReleasedButton(&g_sContext, &Edit);
            EditPressed = true;
            viewHotkeyListEdit(gameList, index, goback, listIndex);
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                     g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void EditHotkey(int index, int listIndex, char* storeHotkey) {
    eUSCI_SPI_MasterConfig SPI0MasterConfig =
    {
         EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
         3000000,
         500000,
         EUSCI_B_SPI_MSB_FIRST,
         EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
         EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
         EUSCI_B_SPI_3PIN
    };

    /* Timer_A UpMode Configuration Parameters */
    Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
            30000,                                  // 1 ms tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    FIL file_read;                                          /* Opened file object */
    FATFS fatfs;                                            /* File system object */
    FRESULT errCode;                                        /* Error code object  */
    unsigned int size_read = 0;

    WDT_A_holdTimer();

    CS_Init();
    DIR DI;
    FILINFO FI;

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B3_BASE, SPI0MasterConfig);
    GPIO_Init(GPIO_PORT_P10, GPIO_PIN0);
    TIMERA_Init(TIMER_A1_BASE, UP_MODE, &upConfig, disk_timerproc);
    int i;

    Interrupt_enableMaster();

    errCode = f_mount(&fatfs, "0", 1);
    if (errCode) {
        printf("HERE");
    }
    errCode = f_opendir(&DI, "/");
    if (errCode) {
        printf("HERE2");
    }
    char* compGame = gameList[index];
    if (!strcmp(compGame, "STREETFIGHTERS")) {
        compGame = "STREET";
    }
    char directory[10] = {'\0'};
    directory[0] = '/';
    int j;
    do
    {
        /*Read a directory/file*/
        errCode = f_readdir(&DI, &FI);
        /*Check for errors. Trap MSP432 if there is an error*/
        if (errCode) {
            printf("HERE3");
        }
        if (!strcmp(FI.fname, compGame)) {
            for (j = 0; j < sizeof(compGame) + 2; j++) {
                directory[j + 1] = compGame[j];
            }
            break;
        }
    }while(FI.fname[0]);
    errCode = f_opendir(&DI, directory);
    printf("%s\n", directory);
    if (errCode) {
        printf("HERE4");
    }
    if (listIndex == 0 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\1.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 1 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\2.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 2 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\3.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 3 && !strcmp(compGame, "MARIO")) {
        errCode = f_open(&file_read, "0:\\MARIO\\4.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 0 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\1.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 1 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\2.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 2 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\3.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 3 && !strcmp(compGame, "ZELDA")) {
        errCode = f_open(&file_read, "0:\\ZELDA\\4.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 0 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\1.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 1 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\2.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 2 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\3.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    else if (listIndex == 3 && !strcmp(compGame, "STREET")) {
        errCode = f_open(&file_read, "0:\\STREET\\4.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    }
    if (errCode) {
        printf("RETURN HERE\n");
        return 0;
        //return (int)errCode;
    }
    size_read = 0;
    storeHotkey[0] = 'U';
    storeHotkey[1] = 'P';
    for (;;) {
        errCode = f_write(&file_read, storeHotkey, sizeof(storeHotkey), &size_read);
        if (errCode) {
            printf("RETURN HERE Error Code\n");
            break;
        }
        if (size_read == 0) {
            printf("RETURN HERE size_read\n");
            break;
        }
        break;
    }
    f_mount(0, "", 0);                     /* Unmount the default drive */
    f_close(&file_read);
}

void viewHotkeyListEdit(char gameList[10][24], int index, int goback, int listIndex) {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    drawEditMode();
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    int8_t* title = (int8_t*)gameList[index];
    Graphics_drawStringCentered(&g_sContext, title,
                                AUTO_STRING_LENGTH,
                                150,
                                20,
                                TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss16b);
    if (listIndex == 0) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey1", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 1) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey2", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 2) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey3", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    else if (listIndex == 3) {
        Graphics_drawStringCentered(&g_sContext, "Hotkey4", AUTO_STRING_LENGTH, 150, 40, TRANSPARENT_TEXT);
    }
    Graphics_setFont(&g_sContext, &g_sFontCmss12);
    Graphics_drawStringCentered(&g_sContext, "If Edit:'ON', press buttons or move joystick to modify", AUTO_STRING_LENGTH, 150, 60, TRANSPARENT_TEXT);

    Graphics_drawButton(&g_sContext, &Home);
    Graphics_drawButton(&g_sContext, &Done);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_drawStringCentered(&g_sContext, "PLEASE WAIT", AUTO_STRING_LENGTH, 150, 80, TRANSPARENT_TEXT);

    char storeHotkey[200] = {'\0'};
    int count = 0;
    do
    {
        touch_updateCurrentTouch(&g_sTouchContext);
        /*
        //PB1
        if (!(P5IN & BIT1)) {

        }
        //PB2
        else if (!(P5IN & BIT0)) {
            P1OUT |= BIT4;
            printf("HERE PUSHING X");
            storeHotkey[count] = 'X';
            count++;
            storeHotkey[count] = ' ';
            count++;
            Delay(1000);
            P1OUT &= ~BIT4;
        }
        //PB3
        else if (!(P4IN & BIT7)) {
            storeHotkey[count] = 'A';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB4
        else if (!(P8IN & BIT4)) {

        }
        //PB5
        else if (!(P4IN & BIT6)) {
            storeHotkey[count] = 'Y';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB6
        else if (!(P4IN & BIT5)) {
            storeHotkey[count] = 'B';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB7
        else if (!(P4IN & BIT4)) {

        }
        //PB8
        else if (!(P4IN & BIT2)) {
            storeHotkey[count] = 'R';
            count++;
            storeHotkey[count] = 'B';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB9
        else if (!(P4IN & BIT1)) {
            storeHotkey[count] = 'R';
            count++;
            storeHotkey[count] = 'T';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB10
        else if (!(P4IN & BIT0)) {

        }
        //PB11
        else if (!!(P8IN & BIT6)) {
            storeHotkey[count] = 'L';
            count++;
            storeHotkey[count] = 'B';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        //PB12
        else if (!!(P5IN & BIT0)) {
            storeHotkey[count] = 'L';
            count++;
            storeHotkey[count] = 'T';
            count++;
            storeHotkey[count] = ' ';
            count++;
        }
        */
        if (Graphics_isButtonSelected(&Done, g_sTouchContext.x,
                                      g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &Done);
            Graphics_drawReleasedButton(&g_sContext, &Done);
            EditPressed = false;
            EditHotkey(index, listIndex, storeHotkey);
            printf("%s\n", storeHotkey);
            WDT_A_hold(WDT_A_BASE);
            boardInit();
            clockInit();
            viewHotkeyList(gameList, index, goback, listIndex);
        }
    }
    while(!Graphics_isButtonSelected(&Home, g_sTouchContext.x,
                                     g_sTouchContext.y));
    Graphics_drawSelectedButton(&g_sContext, &Home);
}

void runCredits(void)
{
    // Credit -> Sang Hun Kim
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setFont(&g_sContext, &g_sFontCmss12b);
    Graphics_drawStringCentered(&g_sContext, "LCD & External Memory Subsystem Manager",
                                AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_drawStringCentered(&g_sContext, "Sang Hun Kim",
                                AUTO_STRING_LENGTH, 159, 220, TRANSPARENT_TEXT);
    Graphics_drawImageButton(&g_sContext, &groupImage);
    Delay(1000);

    // Credit -> Jeong Min (Jimmy) Kim
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setFont(&g_sContext, &g_sFontCmss12b);
    Graphics_drawStringCentered(&g_sContext, "Power Supply, LED, PC, Vibration Subsystem Manager",
                                AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_drawStringCentered(&g_sContext, "Jeong Min (Jimmy) Kim",
                                AUTO_STRING_LENGTH, 159, 220, TRANSPARENT_TEXT);
    Graphics_drawImageButton(&g_sContext, &groupImage);
    Delay(1000);

    // Credit -> Han Xu
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setFont(&g_sContext, &g_sFontCmss12b);
    Graphics_drawStringCentered(&g_sContext, "Push Button, Joystick Subsystem & PCB Manager",
                                AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_drawStringCentered(&g_sContext, "Han Xu",
                                AUTO_STRING_LENGTH, 159, 220, TRANSPARENT_TEXT);
    Graphics_drawImageButton(&g_sContext, &groupImage);
    Delay(1000);

    // Credit -> Albert Hwang
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setFont(&g_sContext, &g_sFontCmss12b);
    Graphics_drawStringCentered(&g_sContext, "USB, Bluetooth Module Subsystem Manager",
                                AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCmss20b);
    Graphics_drawStringCentered(&g_sContext, "Albert Hwang",
                                AUTO_STRING_LENGTH, 159, 220, TRANSPARENT_TEXT);
    Graphics_drawImageButton(&g_sContext, &groupImage);
    Delay(1000);

    g_ranDemo = true;

    //drawRestarDemo();
}

void boardInit()
{
    FPU_enableModule();
}

void clockInit(void)
{
    /* 2 flash wait states, VCORE = 1, running off DC-DC, 48 MHz */
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    PCM_setPowerState(PCM_AM_DCDC_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_setDCOFrequency(48000000);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, 1);

    return;
}


void Delay(uint16_t msec){
    uint32_t i = 0;
    uint32_t time = (msec / 1000) * (SYSTEM_CLOCK_SPEED / 15);

    for(i = 0; i < time; i++)
    {
        ;
    }
}

