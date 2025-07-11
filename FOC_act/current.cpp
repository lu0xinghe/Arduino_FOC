#include <math.h>
#include <Arduino.h> 
#define _ADC_VOLTAGE 3.3f            //ADC 电压
#define _ADC_RESOLUTION 4095.0f      //ADC 分辨率

// ADC 计数到电压转换比率求解
#define _ADC_CONV ( (_ADC_VOLTAGE) / (_ADC_RESOLUTION) )

extern int pinA;
extern int pinB;

float I_abc[3],I_offset[3]={0};
float I_a, I_b;
float Iq_Ref, Id_Ref;
float _shunt_resistor = 0.01;
float amp_gain = 50;
extern float current_ab[2];

float volts_to_amps_ratio = 1.0f /_shunt_resistor / amp_gain;

float gain_a = 1.0f /_shunt_resistor / amp_gain;
float gain_b = 1.0f /_shunt_resistor / amp_gain;
float gain_c = 1.0f /_shunt_resistor / amp_gain;

void getPhaseCurrents(){
    current_ab[0] = analogRead(pinA);// amps
    current_ab[1] = analogRead(pinB);// amps
    //两电流采样
}

void sampling(float *I_in)
{
	I_abc[0] = I_in[0]*_ADC_CONV;
	I_abc[1] = I_in[1]*_ADC_CONV;
}

void offset(float* I_in)
{
	for (int i = 0; i < 1000; i++)
	{
    getPhaseCurrents();
		sampling(I_in);
		I_offset[0] += I_abc[0];
		I_offset[1] += I_abc[1];
	}
	I_offset[0] = I_offset[0]/1000;
	I_offset[1] = I_offset[1]/1000;
  printf("%f,%f\r\n",I_offset[0],I_offset[1]);
}

void CLARKE(float* I_in)
{
	sampling(I_in);
	I_abc[0] = (I_abc[0]-I_offset[0])*gain_a;
	I_abc[1] = (I_abc[1]-I_offset[1])*gain_b;

	I_a = I_abc[0];
	I_b = (I_abc[0] + 2 * I_abc[1]) / 1.732;
}

void PARK(float angle)
{
	Id_Ref = I_a * cos(angle) + I_b * sin(angle);
	Iq_Ref = -I_a * sin(angle) + I_b * cos(angle);
}


void current( float* I_out, float angle)
{
  getPhaseCurrents();
	CLARKE(current_ab);
	PARK(angle);
	I_out[0] = Id_Ref;
	I_out[1] = Iq_Ref;
}