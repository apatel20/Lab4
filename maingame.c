#include <stdlib.h>
#include "swtimer.h"
#include "maplogic.h"
#include "button.h"
#include "display.h"
#include "hwtimer.h"
#include "microphone.h"
#include "goertzel.h"
#include "dtmf.h"
#include "sound.h"
#include "LED_HAL.h"

#define COLOR1 GRAPHICS_COLOR_GOLD
#define COLOR2 GRAPHICS_COLOR_LIME
#define COLOR4 GRAPHICS_COLOR_HOT_PINK

// This is the state definition for the top-level state machine,
// implemented in ProcessStep. As you build the game, you will
// have to define additional states to implement the actual game
// logic.
typedef enum {idle, gameCompStart, gamePlayerStart} state_t;
typedef enum {message, scores, messageS1, messageS2} messageState;
typedef enum {playerTurn, computerTurn, neutral, illegalMove, abortGame, winning} gaming;

gaming currentState = neutral;//initialize global variables
state_t S = idle;
unsigned glbListening = 0;
tSWTimer every50ms2;
tSWTimer every50ms3;
bool computerWait, playerWait = false;
unsigned determineSound = 0;

tSWTimer everySec2;//initialize timers
tSWTimer everySec3;
tSWTimer wait2secAbort;

// This structure is a game state. It keeps track of the current
// playing field, the score achieved by the human, and the score
// achieved by the computer. The game state gets passed around
// during the game to allow different software components access
// to the game state.

typedef struct {
    tcellstate map[9];
    int computerscore;
    int humanscore;
} gamestate_t;

gamestate_t G;

void messageFSM(messageState *currentMessage)
{
    switch (*currentMessage)
    {
    case message:
        DrawMessage("  *TicTacTone*  ", EMPHASISCOLOR);//FSM
        break;

    case scores:
        DrawScore(G.computerscore, G.humanscore, EMPHASISCOLOR);//alternate between things in the message area
        break;

    case messageS1:
        DrawMessage("  S1: I Start  ", EMPHASISCOLOR);
        break;

    case messageS2:
        DrawMessage("  S2: You Start  ", EMPHASISCOLOR);
        break;
    }
}

// This function implements the functionality of Tic Tac Tone during
// the idle state, i.e. when it is waiting for the player to provide
// a 'start game' command.
//
// This function is called every time the top-level FSM visits the
// idle state. The function has five parameters. You are welcome to
// add more, but know that the reference solution was implemented using
// only these 5 parameters.
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses
//    G    = gamestate, as defined above. Pass by reference to make
//           sure that changes to G within this function will be
//           propagated out of the function.
//
// Note that this function RETURNS a state_t. This can be used to influence
// the state transition in the top-level FSM that will call processIdle.
// Currently, the ProcessIdle always returns idle (which means the top-level
// FSM will iterate in the idle state), but when the game is extended,
// the return state could change into something different (such as playing_circle
// or playing_cross, depending on whether S1 or S2 is pressed).

state_t ProcessIdle(int b1, int b2,
                    int sec, int ms50) {

    // These are the states of a _local_ FSM.
    // The state labels are prefixed with 'processidle' to
    // make sure they are distinct from the labels used for the
    // top-level FSM.
    //
    // The local FSM functionality plays a game of tic-tac-toe
    // against itself, using randomized moves. However, the
    // rules of tic-tac-toe are followed, including the game
    // map drawing and coloring over the reference solution.

    typedef enum {processidle_idle,
                  processidle_playingcircle,
                  processidle_playingcross,
                  processidle_winning} processidle_state_t;
    static processidle_state_t localstate = processidle_idle;

    unsigned w;
    static int counter = -1;
    static messageState currentMessage = messageS2;

    // We will run this local state machine only once per second,
    // therefore, we only run it when sec is 1. sec is
    // a software-timer generated in the main function.
    //
    // To add more functionality, you can extend this function. For example,
    // to display a label every three seconds, you can add a counter that is
    // incremented for every sec, modulo-3. When the counter is two, it means
    // that the three-second timer mark is reached.
    //
    // A longer counter period (eg modulo-12 iso modulo-3) can be used to
    // display rotating messages.

    if (b1)
    {
        StartSWTimer(&wait2secAbort);//if button 1 is pressed
        return gameCompStart;
    }
    if (b2)
    {
        return gamePlayerStart;//if button 2 is pressed, player starts
    }

    if (sec) {

        counter++;

        if (counter % 3 == 0)
        {
            currentMessage++;
            if (counter % 4 == 0)//algorithm to alternate messages
            {
                currentMessage = message;
            }
            messageFSM(&currentMessage);
        }

        switch (localstate) {
            case processidle_idle:
              // Initially, just draw the playing field
              ClearMap(G.map);
              DrawBoard(G.map);
              localstate = processidle_playingcircle;
              break;
            case processidle_playingcircle:
              // This is circle who is playing. A circle is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.
              // Decide what position to play
              RandomAdd(G.map, circle);
              // Next, it's cross' turn
              localstate = processidle_playingcross;
              // If we win or tie, go to winning state instead
              if (CircleWins(G.map) || Tie(G.map))
                  localstate = processidle_winning;
              // update game board status on display
              DrawBoard(G.map);
              break;
            case processidle_playingcross:
              // This is cross who is playing. A cross is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.
              // Decide what position to play
              RandomAdd(G.map, cross);
              // Next, it's circles' turn
              localstate = processidle_playingcircle;
              // If we win or tie, go to winning state instead
              if (CrossWins(G.map) || Tie(G.map))
                  localstate = processidle_winning;
              // update game board status on display
              DrawBoard(G.map);
              break;
            case processidle_winning:
              // This state is entered when there is a winner,
              // or it's a tie. In this state, we redraw the
              // winning combination in the emphasis color.
              // After that, we go for the next round.
              if (w = CircleWins(G.map))
                DrawWinner(G.map, w, EMPHASISCOLOR);
              if (w = CrossWins(G.map))
                DrawWinner(G.map, w, EMPHASISCOLOR);
              localstate = processidle_idle;
              break;
        }
    }
    return idle;
}

// This is the top-level FSM, which is called from within
// the cyclic executive. You will have to extend this FSM
// with the game logic. The FSM takes four inputs:
//
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses

void ProcessStep(int b1, int b2, int sec, int ms50) {
    switch (S)
    {
    case idle:
        S = ProcessIdle(b1, b2, sec, ms50);
        break;
    case gameCompStart:
        DrawBoardGame(G.map, COLOR1, COLOR4);
        process(b1);
        break;
    case gamePlayerStart:
        DrawBoardGame(G.map, COLOR1, COLOR4);
        process(b1);
        break;
    }
}

//typedef enum {playerTurn, computerTurn, neutral, illegalMove, thinking, abortGame, winning} gaming;
void process(int b1)
{
    int winner = 0;

    if (S == gameCompStart && computerWait == false){
        ClearMap(G.map);
        DrawBoardGame(G.map, COLOR1, COLOR4);//draw the board
        currentState = computerTurn;
        StartSWTimer(&everySec3);
        DrawMessage("  Thinking...  ", EMPHASISCOLOR);//go to thinking in computer turn
        computerWait = true;
    }
    else if (S == gamePlayerStart && playerWait == false){
        ClearMap(G.map);
        DrawBoardGame(G.map, COLOR1, COLOR4);
        currentState = playerTurn;//go to playerTurn
        playerWait = true;
    }
    switch(currentState){
    case playerTurn:
        if (b1 && SWTimerExpired(&wait2secAbort))
        {
            currentState = abortGame;
            break;
        }
        DrawMessage("  Listening?  ", EMPHASISCOLOR);
        if (!glbListening){
            currentState = DTMFfindSpot(G.map);//algorithm to listen
            ColorLEDSet(currentState);
            StartSWTimer(&everySec2);
            DTMFReset();
            glbListening = 1;
        }
        DrawBoardGame(G.map, COLOR1, COLOR4);
        if (CircleWins(G.map) || Tie(G.map)){//test winning
            currentState = winning;
        }
        break;
    case computerTurn:
       if (b1 && SWTimerExpired(&wait2secAbort))
       {
           currentState = abortGame;//if button is pressed, go to abort
           break;
       }
        if (SWTimerExpired(&everySec3)){
            if (SWTimerExpired(&every50ms2)){
                DrawMessage("  Thinking...  ", EMPHASISCOLOR);
                determineSound = RandomAdd(G.map, cross);//add empty and play the sound of a cell
                ColorLEDSet(magenta);
                playSound_Cells(determineSound);
                DrawBoardGame(G.map, COLOR1, COLOR4);
                if (CrossWins(G.map) || Tie(G.map)){
                    currentState = winning;
                }
                else{
                    currentState = playerTurn;
                }
           }
        }
        break;
    case neutral:

        break;
    case illegalMove:
        DrawMessage(" Illegal Move! ", EMPHASISCOLOR);
        if (SWTimerExpired(&everySec2)){//go back when timer expired
            currentState = playerTurn;
        }
        break;
    case abortGame:
        AbortMap(G.map);
        DrawBoard(G.map);
        if (winner = CrossWins(G.map)){
            currentState = winning;//test winner
        }
        break;
    case winning:
        if (winner = CircleWins(G.map)){
            playerWins(winner);
        }
        else if (winner = CrossWins(G.map)){
            computerWins(winner);
        }
        else{//tie
            DrawMessage("  It's a Tie!  ", EMPHASISCOLOR);
            playSound_tie();
            ClearMap(G.map);//show tie and go back to state
            DrawBoard(G.map);
            InitColorLED();
            InitLEDs();
        }
        computerWait = false;//reset all variables
        playerWait = false;
        PrintString("                ", 7, 0);
        S = idle;
        break;
    }
}

void computerWins(int winner)
{
    DrawMessage("    I Win!    ", EMPHASISCOLOR);
    DrawWinner(G.map, winner, EMPHASISCOLOR);
    playWinningSounds();
    G.computerscore++;
    ClearMap(G.map);
    DrawBoard(G.map);
    InitColorLED();
    InitLEDs();
}

void playerWins(int winner)
{
    DrawMessage("   You Win!   ", EMPHASISCOLOR);
    DrawWinner(G.map, winner, EMPHASISCOLOR);
    playWinningSounds();
    G.humanscore++;
    ClearMap(G.map);
    DrawBoard(G.map);
    InitColorLED();
    InitLEDs();
}

void T32_INT1_IRQHandler() {
    unsigned vx;
    static unsigned SamplesListened = 0;
    if (glbListening) {
        vx = GetSampleMicrophone();
        DTMFAddSample(vx);
        SamplesListened++;
        if (SamplesListened == 400) {
            glbListening = 0;
            SamplesListened = 0;
        }
    }
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}

int main(void) {

    // Device Initialization
    InitTimer();
    InitDisplay();
    InitButtonS1();
    InitButtonS2();
    InitMicrophone();
    InitSound();
    InitColorLED();
    InitLEDs();

    Interrupt_enableInterrupt(INT_T32_INT1);
    Interrupt_enableMaster();

    // Software Timer - per second
    // Note that software timers MUST be tied to TIMER32_1_BASE;
    // TIMER32_1_BASE is configured in continuous mode
    // (TIMER32_0_BASE can then be used for periodic interrupts
    //  which will be needed to take samples from microphone)
    tSWTimer everySec;
    InitSWTimer(&everySec, TIMER32_1_BASE, SYSTEMCLOCK);
    StartSWTimer(&everySec);

    // Software Timer - per 50ms = 20Hz
    tSWTimer every50ms;
    InitSWTimer(&every50ms, TIMER32_1_BASE, SYSTEMCLOCK/20);
    StartSWTimer(&every50ms);

    InitSWTimer(&everySec2, TIMER32_1_BASE, SYSTEMCLOCK);
    InitSWTimer(&everySec3, TIMER32_1_BASE, SYSTEMCLOCK);
    InitSWTimer(&every50ms2, TIMER32_1_BASE, SYSTEMCLOCK/2);
    InitSWTimer(&every50ms3, TIMER32_1_BASE, SYSTEMCLOCK/2);
    InitSWTimer(&wait2secAbort, TIMER32_1_BASE, SYSTEMCLOCK*2);
    // The cyclic executive is simple: read the buttons and software
    // timers, and call the top-level FSM in ProcessStep.
    glbListening = 1;

    while (1) {
        int b1    = ButtonS1Pressed();
        int b2    = ButtonS2Pressed();
        int sec   = SWTimerExpired(&everySec);
        int ms50  = SWTimerExpired(&every50ms);

        ProcessStep(b1, b2, sec, ms50);
    }

}
