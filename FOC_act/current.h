#ifndef CURRENT_H
#define CURRENT_H

void offset(float* I_in);
void current(float* I_in, float* I_out, float angle);
void getPhaseCurrents();

#endif
