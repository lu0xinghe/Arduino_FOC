#include <math.h>
#include <stdio.h>

// 常量定义
#define T_pwm 1
#define U_dc 12.6

// 全局变量定义（只在这里定义一次）
float X = 1.732f * T_pwm / U_dc;
float angle = 0, angle_act = 0;
float U_dq[2] = {0,0}; // Ud, Uq
float U_ab[2], T_vector[3];
int judge = 0;
int* j = &judge;

static void angle_update(float T_update)
{
//	angle += angle_act;
	angle_act = T_update;
}

static void RevPAKE(float* U_in, float* U_out)
{
	U_out[0] = U_in[0] * cos(angle_act) - U_in[1] * sin(angle_act);
	U_out[1] = U_in[0] * sin(angle_act) + U_in[1] * cos(angle_act);
}

static void sectors(float* U_in, int* judge)//Uab������
{
	int A = 0, B = 0, C = 0;

	if (U_in[1] > 0)A = 1;
	if (1.732 * U_in[0] - U_in[1] > 0)B = 1;
	if (-1.732 * U_in[0] - U_in[1] > 0)C = 1;

	*judge = 4 * C + 2 * B + A;
}
int N=0;
static void Svpwm(int* judge, float* U_in,float *T_out)//Uad
{
	float T_xyz[3];
  N++;
	switch (*judge)
	{
		case 3:T_xyz[0] = X*(1.732*U_in[0]/2-U_in[1]/2); T_xyz[1] = X*U_in[1];break;
		case 1:T_xyz[0] = X*(1.732*U_in[0]/2+U_in[1]/2); T_xyz[1] = -X*(1.732*U_in[0]/2-U_in[1]/2);break;
		case 5:T_xyz[0] = X*U_in[1];                     T_xyz[1] = -X*(1.732*U_in[0]/2+U_in[1]/2);break;
		case 4:T_xyz[0] = -X*(1.732*U_in[0]/2-U_in[1]/2);T_xyz[1] = -X*U_in[1];break;
		case 6:T_xyz[0] = -X*(1.732*U_in[0]/2+U_in[1]/2);T_xyz[1] = X*(1.732*U_in[0]/2-U_in[1]/2);break;
		case 2:T_xyz[0] = -X*U_in[1];                    T_xyz[1] = X*(1.732*U_in[0]/2+U_in[1]/2);break;
	}
  T_xyz[2] = (1-T_xyz[0]-T_xyz[1])/2;
  switch (*judge)
	{
		case 3:T_out[0] = T_xyz[0]+T_xyz[1]+T_xyz[2];T_out[1] = T_xyz[1]+T_xyz[2];         T_out[2] = T_xyz[2];break;
		case 1:T_out[0] = T_xyz[0]+T_xyz[2];         T_out[1] = T_xyz[0]+T_xyz[1]+T_xyz[2];T_out[2] = T_xyz[2];break;
		case 5:T_out[0] = T_xyz[2];                  T_out[1] = T_xyz[0]+T_xyz[1]+T_xyz[2];T_out[2] = T_xyz[1]+T_xyz[2];break;
		case 4:T_out[0] = T_xyz[2];                  T_out[1] = T_xyz[0]+T_xyz[2];         T_out[2] = T_xyz[0]+T_xyz[1]+T_xyz[2];break;
		case 6:T_out[0] = T_xyz[1]+T_xyz[2];         T_out[1] = T_xyz[2];                  T_out[2] = T_xyz[0]+T_xyz[1]+T_xyz[2];break;
		case 2:T_out[0] = T_xyz[0]+T_xyz[1]+T_xyz[2];T_out[1] = T_xyz[2];                  T_out[2] = T_xyz[0]+T_xyz[2];break;
	}
}

void FOC_act(float Ud,float Uq,float T_update)//T_update,�ⲿ����,�Ƕȵĸ���������STM32��ʱ��������һʱ���뵱ǰʱ�����Ŀ���ٶ���˵õ�
{
  U_dq[0] = Ud;
  U_dq[1] = Uq;
  angle_update(T_update);
	RevPAKE(U_dq, U_ab);
	sectors(U_ab ,j);
	Svpwm(j, U_ab, T_vector);
}
