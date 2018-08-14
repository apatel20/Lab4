#include "dtmf.h"
#include "display.h"

#define threshL 0x2000
#define thresh1L 0x500
#define thresh2L 0x1000
#define thresh3L 0x1500

// TO BE DEFINED BY YOU
//
// Compute Goertzel Filter Coefficients for
// Frequency Detectors for the following frequencies:
//
//      Gtap structure     Fgoertzel
//      -----------------------------
//      t697                 697 Hz
//      t770                 770 Hz
//      t852                 852 Hz
//      t1209               1209 Hz
//      t1336               1336 Hz
//      t1477               1477 Hz
//
// A Goertzel filter coefficient is defined by
//
//           c = 2*cos(2*pi*v)
//
// with v equal to Fgoertzel / Fsample.
// For example, if Fgoertzel = 1000Hz and Fsample = 8000Hz
// then c = 2 * cos (2*pi*1/8) = 1.4142
// Be sure to compute cos() in radians
//
// This results in the following filter coefficients
//
//      Gtap structure     Fgoertzel       Coefficient
//      ----------------------------------------------
//      t697                 697 Hz      0.8538689 * 2
//      t770                 770 Hz      0.8226405 * 2
//      t852                 852 Hz      0.7843435 * 2
//      t1209               1209 Hz      0.5820520 * 2
//      t1336               1336 Hz      0.4981851 * 2
//      t1477               1477 Hz      0.3993092 * 2
//
// To encode the Goertzel filter coefficient into Gtap,
// you have to convert the floating point value of c
// into a fixed point value.
//
// For a fixed point representation <WLEN,FRACBITS>,
// The resulting Gtap coef value is
//
//         coef =  (int) (c * (1 << FRACBITS))
//
// Make sure that you do NOT use double or float in your program.
// Use ONLY integer types (signed or unsigned).

// FILL IN THE FOLLOWING VALUES TO DEFINE PROPER COEF VALUES

static Gtap  t697 = {(int)((0.8538689*2)*(1 << FRACBITS)), 0, 0};
static Gtap  t770 = {(int)((0.8226405*2)*(1 << FRACBITS)), 0, 0};
static Gtap  t852 = {(int)((0.7843435*2)*(1 << FRACBITS)), 0, 0};
static Gtap t1209 = {(int)((0.5820520*2)*(1 << FRACBITS)), 0, 0};
static Gtap t1336 = {(int)((0.4981851*2)*(1 << FRACBITS)), 0, 0};
static Gtap t1477 = {(int)((0.3993092*2)*(1 << FRACBITS)), 0, 0};

void DTMFAddSample(unsigned x) {
    unsigned vx = ScaleSample(x);

    SampleGoertzel( &t697, vx);
    SampleGoertzel( &t770, vx);
    SampleGoertzel( &t852, vx);
    SampleGoertzel(&t1209, vx);
    SampleGoertzel(&t1336, vx);
    SampleGoertzel(&t1477, vx);
}

void DTMFReset() {
    ResetGoertzel( &t697);
    ResetGoertzel( &t770);
    ResetGoertzel( &t852);
    ResetGoertzel(&t1209);
    ResetGoertzel(&t1336);
    ResetGoertzel(&t1477);
}

void convert2PowerMeter(unsigned thresh1, unsigned thresh2)
{
    char buf[7] = "Pow:";

    int average = ((int)thresh1+(int)thresh2) % 8;
    average -= 1;

    if (average == 3 || average == 8)//100%
    {
        PrintString(buf, 7, 0);
        PrintStringEC("            ", 7, 4);
        //PrintString(average + '0', 7, 4);
    }
    if (average == 6 || average == 1)//75%
    {
        PrintString(buf, 7, 0);
        PrintStringEC("        ", 7, 4);
        //PrintString(average + '0', 7, 4);
    }
    if (average == 0 || average == 9 || average == 4)//50%
    {
        PrintString(buf, 7, 0);
        PrintStringEC("      ", 7, 4);
    }
    if (average == 2 || average == 5 || average == 7)//25%
    {
        PrintString(buf, 7, 0);
        PrintStringEC("    ", 7, 4);
    }
}

int DTMFfindSpot(tcellstate map[9])
{
    unsigned p697  = PowerGoertzel(  &t697);
    unsigned p770  = PowerGoertzel(  &t770);
    unsigned p852  = PowerGoertzel(  &t852);
    unsigned p1209 = PowerGoertzel( &t1209);
    unsigned p1336 = PowerGoertzel( &t1336);
    unsigned p1477 = PowerGoertzel( &t1477);

    if ((p697 > threshL) && (p1209 > threshL))//map[0]
    {
        convert2PowerMeter(p697, p1209);
        if (map[0] == empty)
        {
            map[0] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p697 > threshL) && (p1336 > threshL))//map[1]
    {
        convert2PowerMeter(p697, p1336);
        if (map[1] == empty)
        {
            map[1] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p697 > threshL) && (p1477 > threshL))//map[2]
    {
        convert2PowerMeter(p697, p1477);
        if (map[2] == empty)
        {
            map[2] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p770 > threshL) && (p1209 > threshL))//map[3]
    {
        convert2PowerMeter(p770, p1209);
        if (map[3] == empty)
        {
            map[3] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p770 > threshL) && (p1336 > threshL))//map[4]
    {
        convert2PowerMeter(p770, p1336);
        if (map[4] == empty)
        {
            map[4] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p770 > threshL) && (p1477 > threshL))//map[5]
    {
        convert2PowerMeter(p770, p1477);
        if (map[5] == empty)
        {
            map[5] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p852 > threshL) && (p1209 > threshL))//map[6]
    {
        convert2PowerMeter(p852, p1209);
        if (map[6] == empty)
        {
            map[6] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p852 > threshL) && (p1336 > threshL))//map[7]
    {
        convert2PowerMeter(p852, p1336);
        if (map[7] == empty)
        {
            map[7] = circle;
            return 1;
        }
        return 3;
    }
    else if ((p852 > threshL) && (p1477 > threshL))//map[8]
    {
        convert2PowerMeter(p852, p1477);
        if (map[8] == empty)
        {
            map[8] = circle;
            return 1;
        }
        return 3;
    }
    else
    {
        return 0;
    }
}

