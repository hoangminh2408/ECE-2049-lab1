/************** ECE2049 DEMO CODE ******************/
/**************  20 August 2016   ******************/
/***************************************************/

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void countdown();
void BuzzerOnPitch(int pitch);
void drawgame();
void drawaliens();
void movealiens(int x_pos, int y_pos);
void destroyaliens();
// Declare globals here
char aliens_test[5][6] = {
                          {1,2,0,4,0,0},
                          {1,2,0,4,0,0},
                          {0,0,3,0,5,6},
                          {0,2,3,4,5,0},
                          {1,0,3,4,5,6}
                         };
char aliens_save[5][6] = {
                          {1,2,0,4,0,0},
                          {1,2,0,4,0,0},
                          {0,0,3,0,5,6},
                          {0,2,3,4,5,0},
                          {1,0,3,4,5,6}
                         };
char aliens_pos[5][6];
int level = 1 , m = 1;
int score = 0;


typedef enum {MENU, CNTDOWN, INGAME, WIN, LOSE} state_t;
state_t state = MENU;
// Main
void main(void)

    {
    // Define some local variables
    char currKey = 0;
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer
    initLeds();
    configDisplay();
    configKeypad();

    // *** Intro Screen ***



    while (1)    // Forever loop
    {
        switch(state)
        {
        case MENU:
            Graphics_clearDisplay(&g_sContext); // Clear the display
            // Write some text to the display
            Graphics_drawStringCentered(&g_sContext, "SPACE", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "INVADERS", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Press *", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
            Graphics_Rectangle box = {.xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
            Graphics_drawRectangle(&g_sContext, &box);
            Graphics_flushBuffer(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, " ", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
            for (m; m++; m<10000)
            {
                currKey = getKey();
                if(currKey == '*')
                {
               //     BuzzerOnPitch(200);
                    swDelay(2);
               //     BuzzerOnPitch(100);
                    swDelay(2);
               //     BuzzerOnPitch(300);
                    swDelay(2);
                    BuzzerOff();
                    state = CNTDOWN;
                    break;
                }
            }

            break;

        case CNTDOWN:
            countdown();
            state = INGAME;
            break;

        case INGAME:
            drawgame();
            drawaliens();
            swDelay(2);
            movealiens(10, 45);
            break;

        case WIN:
            level++;
            score += 100;
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "YOU WIN", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(3);

            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "LEVEL UP!", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(3);

            state = CNTDOWN;
            break;

        case LOSE:
            memcpy(aliens_test, aliens_save, 5 * 6 * sizeof(int));
            level = 0;
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "GAME OVER", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(4);
            state = MENU;
            break;





        }  // end while (1)
    }
}

void countdown()
{
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "3...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "2...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "1...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);
}

void BuzzerOnPitch(int pitch)
{
    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    TB0CCR0   = pitch;                    // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE;                  // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5  = OUTMOD_7;                   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;                      // Disable capture/compare interrupts
    TB0CCR5   = TB0CCR0/2;                  // Configure a 50% duty cycle
}

void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent optimization
			                            // by compiler

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}

void drawgame()
{
    Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 10, 90, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 25, 90, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 40, 90, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 55, 90, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "5", AUTO_STRING_LENGTH, 70, 90, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "6", AUTO_STRING_LENGTH, 85, 90, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void drawaliens()
{
    int i = 0;
    int j = 0;
    while (i<5)
    {
        j = 0;
        while(j<6)
        {
            int alien_x_pos = j*15 + 10;
            int alien_y_pos = i*10 + 5;
            if (aliens_test[i][j] != 0)
            {
                Graphics_drawStringCentered(&g_sContext, "X", AUTO_STRING_LENGTH, alien_x_pos, alien_y_pos, TRANSPARENT_TEXT);
            }
            else
            {
                Graphics_drawStringCentered(&g_sContext, " ", AUTO_STRING_LENGTH, alien_x_pos, alien_y_pos, TRANSPARENT_TEXT);
            }

            j++;
        }
        i++;

    }
    Graphics_flushBuffer(&g_sContext);
}

//void addaliens()
void movealiens(int x_pos, int y_pos)
{
    int i = 0;
    int j = 0;
    int alien_x_pos = x_pos, alien_y_pos;
    int last_y_pos = y_pos-35;
    Graphics_clearDisplay(&g_sContext);
    int destroyed_aliens = 0;
    while (i<5)
    {
        j = 0;
        alien_y_pos = last_y_pos + i*10;
        while(j<6)
        {
            alien_x_pos = j*15 + 10;
            if (aliens_test[i][j] != 0)
            {
                Graphics_drawStringCentered(&g_sContext, "X", AUTO_STRING_LENGTH, alien_x_pos, alien_y_pos, TRANSPARENT_TEXT);
            }
            else
            {
                destroyed_aliens++;
            }
            j++;
        }

        i++;
    }
    drawgame();
    Graphics_flushBuffer(&g_sContext);
    if (alien_y_pos < 85)
    {
        swDelay(2);
        movealiens(alien_x_pos, alien_y_pos);
    }
    else
    {
        BuzzerOnPitch(350);
        swDelay(3);
        BuzzerOff();
        state = LOSE;

    }
}

void destroyaliens()
{
    int i = 4;
    int j = 0;
    while (i >= 0)
    {
        j = 0;
        while (j<6)
        {
            char currKey = getKey();
            if (currKey == (j+1))
            {
                aliens_test[i][j] = 0;
            }
            j++;
        }
        i--;
    }
}

