#include <math.h>
#include <Arduino.h> 
#define _ADC_VOLTAGE 3.3f            //ADC 电压
#define _ADC_RESOLUTION 4095.0f      //ADC 分辨率

// ADC 计数到电压转换比率求解
#define _ADC_CONV ( (_ADC_VOLTAGE) / (_ADC_RESOLUTION) )

extern int pinA;
extern int pinB;

float I_abc[3],I_offset[3];
float I_a, I_b;
float Iq_Ref, Id_Ref;
float _shunt_resistor;
float amp_gain;
extern float current_ab[2];

float volts_to_amps_ratio;

float gain_a;
float gain_b;
float gain_c;

void getPhaseCurrents(){
    current_ab[0] = analogRead(pinA);// amps
    current_ab[1] = analogRead(pinB);// amps
    //两电流采样
}

void sampling(float *I_in)
{
	I_abc[0] = I_in[0]*_ADC_CONV;
	I_abc[1] = I_in[1]*_ADC_CONV;
	// I_abc[2] = I_in[2];
}

void offset(float* I_in)
{
  _shunt_resistor = 0.01;
  amp_gain  = 50;
  volts_to_amps_ratio = 1.0f /_shunt_resistor / amp_gain;
  gain_a = volts_to_amps_ratio;
  gain_b = volts_to_amps_ratio;
  gain_c = volts_to_amps_ratio;
  
	for (int i = 0; i < 1000; i++)
	{
    getPhaseCurrents();
		sampling(I_in);
		I_offset[0] += I_abc[0];
		I_offset[1] += I_abc[1];
		// I_offset[2] += I_abc[2];
	}
	I_offset[0] = I_offset[0]/1000;
	I_offset[1] = I_offset[1]/1000;
	// I_offset[2] = I_offset[2]/1000;
}

void CLARKE(float* I_in)
{
	sampling(I_in);
	I_abc[0] = (I_abc[0]-I_offset[0])*gain_a;
	I_abc[1] = (I_abc[1]-I_offset[1])*gain_b;
	// I_abc[2] += I_offset[2];

	//I_a = I_abc[0] * 2 / 3 - I_abc[1] / 3 - I_abc[2] / 3;
	//I_b = I_abc[1] * 1.732 / 3 - I_abc[2] * 1.732 / 3;

	I_a = I_abc[0];
	I_b = (I_abc[0] + 2 * I_abc[1]) / 1.732;
}

void PARK(float angle)
{
	Id_Ref = I_a * cos(angle) + I_b * sin(angle);
	Iq_Ref =-I_a * sin(angle) + I_b * cos(angle);
}

void current(float* I_in, float* I_out, float angle)
{
  getPhaseCurrents();
	CLARKE(I_in);
	PARK(angle);
	I_out[0] = Id_Ref;
	I_out[1] = Iq_Ref;
}