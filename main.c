
#include <msp430.h>
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void countdown();
void BuzzerOnPitch(int pitch);
void drawgame();
void createaliens();
int drawaliens(int x_pos, int y_pos);
int checkrow(int row);
void resetaliens();
void destroyaliens(int col);
void flashingleds();
void winbuzzer();
void beatgamebuzzer();

// Declare globals here
int fuckeverything = 1;
int alien_x_pos;
int alien_y_pos;
char aliens_test[5][6] = {
                          {1,2,0,4,0,0},
                          {1,0,0,0,5,0},            //aliens matrix for testing
                          {0,0,0,0,0,0},
                          {0,0,0,0,0,0},
                          {0,0,0,0,0,0}
                         };

char aliens_pos[5][6] = {
                         {0,0,0,0,0,0},
                         {0,0,0,0,0,0},
                         {0,0,0,0,0,0},            //actual aliens matrix
                         {0,0,0,0,0,0},
                         {0,0,0,0,0,0}
                        };
int level = 1 , m = 1;

typedef enum {MENU, CNTDOWN, INGAME, WIN, LOSE} state_t;        //declare all cases of state machine
state_t state = MENU;                                           //first state is menu

// Main
void main(void)
{
    // Define some local variables
    int forshitandgiggles = 1;
    char currKey = 0;
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer
    initLeds();
    configDisplay();
    configKeypad();

    while (1)    // Forever loop
    {
        switch(state)
        {
        case MENU:                                                                                                  //Main menu state
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "SPACE", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "INVADERS", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Press *", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
            Graphics_Rectangle box = {.xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
            Graphics_drawRectangle(&g_sContext, &box);
            Graphics_flushBuffer(&g_sContext);
            for (m = 0; m < 10000; m++)                                                                             //Polling loop
            {
                m=0;
                currKey = getKey();
                if(currKey == '*')                                                                                  //If user press * starts countdown timer
                {
                    state = CNTDOWN;
                    break;
                }
            }

            break;

        case CNTDOWN:                                                                                               //Countdown state
            countdown();
            state = INGAME;                                                                                         //After countdown jump into game
            createaliens();
            break;

        case INGAME:
            drawgame();                                                                                             //Draw the axis
            int fall = 0;                                                                                           //This int checks how many times the aliens fell
            int statcheck = 0;                                                                                      //This int checks the status: 0 = ongoing, 1 = win, 2 = lose
            while(statcheck==0)
            {
                int column;
                int polling = 0;
                statcheck = drawaliens(0,(5*fall));                                                                 //Draw aliens, y-position depends on fall
                if (statcheck==1)
                {
                    state = WIN;                                                                                    //If drawaliens() returns 1, win
                }
                else if (statcheck==2)
                {
                    state = LOSE;                                                                                   //If drawaliens() returns 2, lose
                }
                fall++;
                for(polling = 0; polling < 2000*(3/level); polling++ )                                              //Polling loop, wait for keypress. Faster as level goes up. When polling reaches condition, aliens fall and restart polling
                {
                    column = (int)getKey() - 48;                                                                    //Get key to destroy aliens
                    if (column > 0)
                    {
                    break;
                    }
                }
               destroyaliens(column);
               column = 0;
            }
            break;

        case WIN:                                                                                                   //Win state
            level++;
            winbuzzer();
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "YOU WIN", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);      //Level up, congratulations messages, buzzers, leds
            Graphics_flushBuffer(&g_sContext);
            flashingleds();
            swDelay(3);
            if (level == 4)                                                                                         //If level == 4 you beat the game, level reset to 1, return to menu
            {
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "CONGRATULATIONS!", AUTO_STRING_LENGTH, 49, 40, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "YOU BEAT", AUTO_STRING_LENGTH, 51, 60, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "THE GAME!", AUTO_STRING_LENGTH, 51, 70, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                flashingleds();
                flashingleds();
                beatgamebuzzer();
                swDelay(3);
                level = 1;
                resetaliens();
                state = MENU;
            }
            else                                                                                                    //If level<4 go to cntdown for next level
            {
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "LEVEL UP!", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                resetaliens();
                swDelay(3);
                state = CNTDOWN;
            }
            break;

        case LOSE:                                                                                                  //Lose state
            BuzzerOnPitch(350);
            swDelay(3);
            BuzzerOff();
            resetaliens();
            level = 1;
            Graphics_clearDisplay(&g_sContext);                                                                     //Lose message, buzzer, reset level to 1. return to menu
            Graphics_drawStringCentered(&g_sContext, "GAME OVER", AUTO_STRING_LENGTH, 51, 50, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(4);
            state = MENU;
            break;





        }  // end while (1)
    }
}

void countdown()                                                                                                    //Draw countdown timer
{
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "3...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOnPitch(200);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "2...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOnPitch(100);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "1...", AUTO_STRING_LENGTH, 51, 36, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOnPitch(300);
    swDelay(2);
    Graphics_clearDisplay(&g_sContext);
    BuzzerOff();
}

void BuzzerOnPitch(int pitch)                                                                                      //Buzz the buzzer with different pitch depends on the int pitch
{
    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    TB0CCR0   = pitch;                    // Set the PWM period in ACLK ticks                                      //This line changes the pitch
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

void drawgame()                                                                                                     //Draw the axis
{
    Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 10, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 25, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 40, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 55, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "5", AUTO_STRING_LENGTH, 70, 90, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "6", AUTO_STRING_LENGTH, 85, 90, OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

int drawaliens(int x_pos, int y_pos)                                                                                //Draw and move aliens
{
    int i = 0;
    int j = 0;

    Graphics_clearDisplay(&g_sContext);
    while (i<5)
    {
        j = 0;
        while(j<6)                                                                                                  //Check all rows and columns of matrix
        {
            alien_x_pos = x_pos + j*15 + 10;
            alien_y_pos = y_pos + i*10 + 5;
            if (aliens_pos[i][j] != 0)                                                                              //If there's alien on the cell of the aliens matrix, draw it
            {
                Graphics_fillCircle(&g_sContext,alien_x_pos, alien_y_pos, 4);                                       //This makes the aliens a circle
            }
            j++;
        }
        i++;
    }
    drawgame();
    int r = checkallrows();                                                                                         //Check for empty rows, return values accordingly
    if (r>0)
    {
        if (alien_y_pos > (76+8.5*r))                                                                               ////If there are non-empty rows and if the alien pos reach the end of screen, return 2. Player loses.
        {
            return 2;
        }
    }
    else                                                                                                            //If all rows are empty, return 1. Player wins.
    {
        return 1;
    }
    return 0;                                                                                                       //If neither of the things above happen, return 0, continue game.
}


void createaliens()                                                                                                 //Put the aliens in the aliens_pos matrix
{
    int cnt = 0;
    int i;
    int j;
    int noofaliens = level*2 + 5;                                                                                   //Number of aliens depend on the level
    while (cnt <= noofaliens)
    {
        i = rand()%5;
        j = rand()%6;
        if (aliens_pos[i][j] == 0)                                                                                  //Put aliens in random, zero cells
        {
            aliens_pos[i][j] = j+1;
        }
        else                                                                                                        //If the cell is occupied, try again
        {
            cnt = cnt-1;
        }
        cnt++;
    }
    return;
}



void destroyaliens(int col)                                                                                         //Destroy the last alien in the column
{
    if (col+48<1)
    {
        return;
    }
    int p = 4;
    while (p >= 0)
    {
        if (aliens_pos[p][col-1] != 0)
        {
            aliens_pos[p][col-1] = 0;
            return;
        }
        p--;
    }
}

int checkrow(int row)                                                                                               //Check if a row is empty
{
    int j = 0;
    while(j<=6)
    {
        if (j == 6)
        {
            return 0;
        }
        else if (aliens_pos[row][j] == 0)
        {
            j++;
        }
        else
        {
            return 1;
        }
    }
}

int checkallrows()                                                                                                  //Check if all rows are empty, return int accordingly
{
    int r0=checkrow(0);
    int r1=checkrow(1);
    int r2=checkrow(2);
    int r3=checkrow(3);
    int r4=checkrow(4);
    if (r4 == 1)
    {
        return 1;
    }
    else if (r3 == 1)
    {
        return 2;
    }
    else if (r2 == 1)
    {
        return 3;
    }
    else if (r1 == 1)
    {
        return 4;
    }
    else if (r0 == 1)
    {
        return 5;
    }
    return 0;
}

void resetaliens()                                                                                                  //Reset the aliens_pos matrix to 0
{
    int i , j;
    for (i = 0; i<5; i++)
    {
        for (j = 0; j<6; j++)
        {
            {
                aliens_pos[i][j] = 0;
            }
        }
    }
}

void flashingleds()                                                                                                 //Flash LEDs
{
        setLeds(0x0001);
        swDelay(1);
        setLeds(0x0002);
        swDelay(1);
        setLeds(0x0004);
        swDelay(1);
        setLeds(0x0008);
        swDelay(1);
        setLeds(0);
}

void winbuzzer()                                                                                                    //Winning soundtrack
{
    BuzzerOnPitch(30);
    swDelay(1);
    BuzzerOff();
    BuzzerOnPitch(20);
    swDelay(1);
    BuzzerOff();
    BuzzerOnPitch(10);
    swDelay(1);
    BuzzerOff();
}

void beatgamebuzzer()                                                                                               //Beat game soundtrack
{
    BuzzerOnPitch(45);
    swDelay(1);
    BuzzerOff();
    BuzzerOnPitch(70);
    swDelay(1);
    BuzzerOff();
    BuzzerOnPitch(20);
    swDelay(1);
    BuzzerOff();
}
