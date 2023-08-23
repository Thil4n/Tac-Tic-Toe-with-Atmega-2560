#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define RED 1
#define GREEN 2

#define ON 1
#define OFF 0

#define TRUE 1
#define FALSE 0

#define DEBOUNCE_DELAY 50

void controlLed(int row, int column, int color, int mode);
void play(int index);
void initGrid();
void initKeypad();
char keypad();
int checkWinner();
void resetGrid();
void turnOffLeds();
void animation(int winner);

int turn = RED;

volatile int grid[3][3] = {0};

volatile char prevKey = '\0';

int main(void)
{
    initGrid();
    initKeypad();

    while (TRUE)
    {

        char key = keypad();

        if ('1' <= key && key <= '9')
        {

            int index = key - '0';
            play(index);

            int winner = 0;

            if (winner = checkWinner())
            {
                _delay_ms(100);
                animation(winner);
                break;
            }
        }
    }

    return 0;
}

void initKeypad()
{
    DDRK = 0x0F;
    PORTK |= 0x70;
}

/**********************************************************/
/*                   INIT PORTS FOR GRID                  */
/**********************************************************/

void initGrid()
{
    DDRA = 0xFF;
    DDRC = 0xFF;
    DDRD = 0B10000000;
    DDRG = 0B00000010;

    PORTA = 0xFF;
    PORTC = 0xFF;
    PORTD = 0B10000000;
    PORTG = 0B00000100;

    DDRF = 0B11111111;
}

/**********************************************************/
/*                   GET INPUT FROM KEYPAD                */
/**********************************************************/
volatile char lastKey = '\0';
char keypad()
{
    char key = '\0';

    // Keypad polling
    PORTK = 0b11111110;
    if ((PINK & (1 << PINK4)) == 0)
    {
        key = '1';
    }
    else if ((PINK & (1 << PINK5)) == 0)
    {
        key = '2';
    }
    else if ((PINK & (1 << PINK6)) == 0)
    {
        key = '3';
    }

    PORTK = 0b11111101;
    if ((PINK & (1 << PINK4)) == 0)
    {
        key = '4';
    }
    else if ((PINK & (1 << PINK5)) == 0)
    {
        key = '5';
    }
    else if ((PINK & (1 << PINK6)) == 0)
    {
        key = '6';
    }

    PORTK = 0b11111011;
    if ((PINK & (1 << PINK4)) == 0)
    {
        key = '7';
    }
    else if ((PINK & (1 << PINK5)) == 0)
    {
        key = '8';
    }
    else if ((PINK & (1 << PINK6)) == 0)
    {
        key = '9';
    }

    PORTK = 0b11110111;
    if ((PINK & (1 << PINK4)) == 0)
    {
        key = '*';
    }
    else if ((PINK & (1 << PINK5)) == 0)
    {
        key = '0';
    }
    else if ((PINK & (1 << PINK6)) == 0)
    {
        key = '#';
    }

    // Check for debouncing
    if (prevKey != key)
    {
        _delay_ms(DEBOUNCE_DELAY); // Wait for the debounce delay
        if (prevKey != key)
        {
            prevKey = key;
            return key;
        }
    }

    return '\0';
}

/**********************************************************/
/*                  CONTROL THE LED GRID                  */
/**********************************************************/

void controlLed(int row, int column, int color, int mode)
{
    if (row < 0 || row > 2 || column < 0 || column > 2)
    {
        return;
    }

    static const int ledPorts[18] = {
        0B00000001, // PORTA
        0B00000010,
        0B00000100,
        0B00001000,
        0B00010000,
        0B00100000,
        0B01000000,
        0B10000000,

        0B10000000, // PORTC
        0B01000000,
        0B00100000,
        0B00010000,
        0B00001000,
        0B00000100,
        0B00000010,
        0B00000001,

        0B10000000, // PORTD

        0B00000100, // PORTG

    };

    int index;

    if (color == RED)
    {
        index = row * 6 + (column * 2);
    }
    else
    {
        index = row * 6 + (column * 2) + 1;
    }

    int port = ledPorts[index];

    if (index < 8)
    {
        if (mode == ON)
        {
            PORTA &= ~port;
        }
        else
        {
            PORTA |= port;
        }
    }
    else if (index < 16)
    {
        if (mode == ON)
        {

            PORTC &= ~port;
        }
        else
        {
            PORTC |= port;
        }
    }
    else if (index == 16)
    {
        if (mode == ON)
        {

            PORTD &= ~port;
        }
        else
        {
            PORTD |= port;
        }
    }
    if (index == 17)
    {
        if (mode == ON)
        {

            PORTG &= ~port;
        }
        else
        {
            PORTG |= port;
        }
    }
}

/**********************************************************/
/*                      BEEP WARNING                      */
/**********************************************************/
void warn()
{
    // PORTF = 0xFF;
    // _delay_ms(1000);
    // PORTF = 0x00;
}

/**********************************************************/
/*                     PLAY THE GAME                      */
/**********************************************************/
void play(int index)
{
    int row = (index - 1) / 3;
    int col = (index - 1) % 3;

    if (grid[row][col] == 0)
    {
        grid[row][col] = turn;

        if (turn == RED)
        {
            controlLed(row, col, RED, ON);
        }
        else
        {
            controlLed(row, col, GREEN, ON);
        }

        if (turn == RED)
        {
            turn = GREEN;
        }
        else
        {
            turn = RED;
        }
    }
}

/**********************************************************/
/*                  CHECK IF THE GAME IS WIN              */
/**********************************************************/
int checkWinner()
{
    int winner = 0;

    // Check rows and columns
    for (int i = 0; i < 3; i++)
    {
        if ((grid[i][0] == grid[i][1]) && (grid[i][1] == grid[i][2]))
        {
            winner = grid[i][0];
            break;
        }

        if ((grid[0][i] == grid[1][i]) && (grid[1][i] == grid[2][i]))
        {
            winner = grid[0][i];
            break;
        }
    }

    // Check diagonals
    if ((grid[0][0] == grid[1][1]) && (grid[1][1] == grid[2][2]))
    {
        winner = grid[0][0];
    }
    else if ((grid[0][2] == grid[1][1]) && (grid[1][1] == grid[2][0]))
    {
        winner = grid[0][2];
    }

    return winner;
}

/**********************************************************/
/*                      RESET THE GRID                    */
/**********************************************************/

void resetGrid()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            grid[i][j] = 0;
        }
    }
}
/**********************************************************/
/*                      TURN OFF LEDS                     */
/**********************************************************/

void turnOffLeds()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            controlLed(i, j, RED, OFF);
            controlLed(i, j, GREEN, OFF);
        }
    }
}

/**********************************************************/
/*                      WIN ANIMATION                     */
/**********************************************************/
void animation(int winner)
{
    turnOffLeds();

    _delay_ms(300);

    for (int i = 0; i < 3; i++)
    {
        controlLed(0, i, winner, ON);
        controlLed(1, i, winner, ON);
        controlLed(2, i, winner, ON);
        _delay_ms(10);
    }
    _delay_ms(300);
    turnOffLeds();
    for (int i = 0; i < 3; i++)
    {
        controlLed(0, i, winner, ON);
        controlLed(1, i, winner, ON);
        controlLed(2, i, winner, ON);
        _delay_ms(10);
    }
    turnOffLeds();
    _delay_ms(300);
    for (int i = 0; i < 3; i++)
    {
        controlLed(0, i, winner, ON);
        controlLed(1, i, winner, ON);
        controlLed(2, i, winner, ON);
        _delay_ms(10);
    }
}
