#ifndef DTMF_H_
#define DTMF_H_

#include "goertzel.h"
#include "maplogic.h"

void     DTMFAddSample(unsigned x);
void     DTMFReset();
unsigned DTMFPower();
int DTMFfindSpot(tcellstate map[9]);
void convert2PowerMeter(unsigned thresh1, unsigned thresh2);

#endif /* DTMF_H_ */
