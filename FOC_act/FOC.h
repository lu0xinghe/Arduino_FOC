#ifndef FOC_H
#define FOC_H

#include <math.h>

// 只声明不定义（使用extern）
extern float angle;
extern float angle_act;
extern float X;
extern float U_dq[2];
extern float U_ab[2];
extern float T_vector[3];
extern int judge;
extern int* j;

// 函数声明
void FOC_act(float Uq,float T_update);

#endif