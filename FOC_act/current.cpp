#include <math.h>

float I_abc[3],I_offset[3];
float I_a, I_b;
float Iq_Ref, Id_Ref;/*Id为励磁电流，Iq为转矩电流*/

void sampling(float *I_in)
{
	I_abc[0] = I_in[0];
	I_abc[1] = I_in[1];
	I_abc[2] = I_in[2];
}

void offset(float* I_in)
{
	for (int i = 0; i < 1000; i++)
	{
		sampling(I_in);
		I_offset[0] += I_abc[0];
		I_offset[1] += I_abc[1];
		I_offset[2] += I_abc[2];
	}
	I_offset[0] = I_offset[0]/1000;
	I_offset[1] = I_offset[1]/1000;
	I_offset[2] = I_offset[2]/1000;
}

void CLARKE(float* I_in)
{
	sampling(I_in);
	I_abc[0] += I_offset[0];
	I_abc[1] += I_offset[1];
	I_abc[2] += I_offset[2];

	//I_a = I_abc[0] * 2 / 3 - I_abc[1] / 3 - I_abc[2] / 3;
	//I_b = I_abc[1] * 1.732 / 3 - I_abc[2] * 1.732 / 3;
	//标准

	I_a = I_abc[0];
	I_b = (I_abc[0] + 2 * I_abc[1]) / 1.732;
	//根据I_abc[0] + I_abc[1]  + I_abc[2] = 0 推导得到
}

void PARK(float angle)
{
	Id_Ref = I_a * cos(angle) + I_b * sin(angle);
	Iq_Ref =-I_a * sin(angle) + I_b * cos(angle);
}

void current(float* I_in, float* I_out, float angle)
{
	CLARKE(I_in);
	PARK(angle);
	I_out[0] = Id_Ref;
	I_out[0] = Iq_Ref;
}