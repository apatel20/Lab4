#include "sound.h"
#include "hwtimer.h"
#include "swtimer.h"
#include "LED_HAL.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// This configuration sets a 50% duty cycle configuration
// for CCR4. You will have to figure out what Timer_A module
// drives the buzzer, i.e. what pin TAx.4 is driving the
// buzzer. The Timer_A module then is x.

#define DURATION 100

Timer_A_PWMConfig pwmConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        (int) (SYSTEMCLOCK / 523.25),         // C5
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        (int) ((SYSTEMCLOCK/2) / 523.25)
};


void InitSound() {

    // This function switches the IC pin connected to
    // the buzzer from GPIO functionality to Timer_A functionality
    // so that we can drive it with PWM.

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P2,
            GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);
}

void playWinningSounds()
{
    PlaySound(note_a4,  3*DURATION);   // G4 // 3
    ColorLEDSet(red);
    PlaySound(note_a4,    DURATION);   // A4 // 1
    PlaySound(note_a4,  2*DURATION);   // B4 // 2
    ColorLEDSet(cyan);
    PlaySound(note_f5,  2*DURATION);   // C5 // 2
    PlaySound(note_c5,  2*DURATION);   // D5 // 2
    PlaySound(note_a5,  2*DURATION);   // E5 // 2
    ColorLEDSet(white);
    PlaySound(note_c5,  2*DURATION);   // F5 // 2
    PlaySound(note_f5, 2*DURATION);
    ColorLEDSet(yellow);// F#5 // 2
    PlaySound(note_a4, 16*DURATION);   // G5 // 16
    ColorLEDSet(black);
}

void playSound_tie()
{
    PlaySound(note_f4,  2*DURATION);   // G4 // 2
    ColorLEDSet(blue);
    PlaySound(note_e5,  2*DURATION);   // E5 // 2
    ColorLEDSet(red);
    PlaySound(note_a5,  2*DURATION);   // C5 // 2
    PlaySound(note_e5,    DURATION);   // E5 // 1
    ColorLEDSet(green);
    PlaySound(note_c5,    DURATION);   // C5 // 1
    PlaySound(note_a4,    DURATION);   // A4 // 1
    PlaySound(note_b4,  2*DURATION);   // B4 // 2
    ColorLEDSet(magenta);
    PlaySound(note_c5,  2*DURATION);   // C5 // 2
    PlaySound(note_d5,  2*DURATION);   // D5 // 2
    ColorLEDSet(red);
    PlaySound(note_e5,  2*DURATION);   // E5 // 2
    PlaySound(note_b5,  2*DURATION);   // F5 // 2
    PlaySound(note_f5, 2*DURATION);   // F#5 // 2
    ColorLEDSet(black);
}

void playSound_Cells(int arrayLocation)
{
    switch (arrayLocation)
    {
    case 0:
        PlaySound(note_c5,  2*DURATION);
        PlaySound(note_g5,  DURATION);
        break;
    case 1:
        PlaySound(note_c5,  2*DURATION);
        PlaySound(note_a5,  DURATION);
        break;
    case 2:
        PlaySound(note_c5,  2*DURATION);
        PlaySound(note_b5,  DURATION);
        break;
    case 3:
        PlaySound(note_d5,  2*DURATION);
        PlaySound(note_g5,  DURATION);
        break;
    case 4:
        PlaySound(note_d5,  2*DURATION);
        PlaySound(note_a5,  DURATION);
        break;
    case 5:
        PlaySound(note_d5,  2*DURATION);
        PlaySound(note_b5,  DURATION);
        break;
    case 6:
        PlaySound(note_e5,  2*DURATION);
        PlaySound(note_g5,  DURATION);
        break;
    case 7:
        PlaySound(note_e5,  2*DURATION);
        PlaySound(note_a5,  DURATION);
        break;
    case 8:
        PlaySound(note_e5,  2*DURATION);
        PlaySound(note_b5,  DURATION);
        break;
    case 9:
        break;
    }
}

void PlaySound(tnote n, unsigned ms) {

    //=============================================================
    // TO BE COMPLETED BY YOU
    switch (n)
    {
        case note_silent:
            pwmConfig.timerPeriod = 0;
            pwmConfig.dutyCycle = 0;
            break;

        case note_c4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 261.63);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 261.63);
            break;

        case note_d4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 293.66);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 293.66);
            break;

        case note_e4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 329.63);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 329.63);
            break;

        case note_f4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 349.23);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 349.23);
            break;

        case note_g4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 392.00);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 392.00);
            break;

        case note_a4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 440.00);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 440.00);
            break;

        case note_b4:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 493.88);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 493.88);
            break;

        case note_c5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 523.25);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 523.25);
            break;

        case note_d5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 587.33);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 587.33);
            break;

        case note_e5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 659.25);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 659.25);
            break;

        case note_f5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 698.46);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 698.46);
            break;

        case note_fs5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 739.99);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 739.99);
            break;

        case note_g5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 783.99);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 783.99);
            break;

        case note_a5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 880.00);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 880.00);
            break;

        case note_b5:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 987.77);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 987.77);
            break;

        case note_c6:
            pwmConfig.timerPeriod = (int) (SYSTEMCLOCK / 1046.50);
            pwmConfig.dutyCycle = (int) ((SYSTEMCLOCK/2) / 1046.50);
            break;
    }

        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

        tSWTimer oneshotms;
        InitSWTimer (&oneshotms, TIMER32_1_BASE, ms * SYSTEMCLOCK/1000);
        StartSWTimer(&oneshotms);

        while (!SWTimerOneShotExpired(&oneshotms))
        {

        }

        Timer_A_stopTimer(TIMER_A0_BASE);

        Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    // Play note n for ms milliseconds.

    // You have to use the PWM setting of the Timer_A
    // peripheral that drives the buzzer to sound it

    // The delay ms is generated using a software timer
    // (different from Timer_A!)

    // PlaySound can be implemented as a blocking function.
    // That means that the function turns on the PWM
    // generation, then waits for ms milliseconds, then
    // turns the PWM generation off again.











    //=============================================================

}
