#ifndef CURRENT_H
#define CURRENT_H
//两电流采样
void offset(float* I_in);
void current(float* I_out, float angle);
void getPhaseCurrents();

#endif
