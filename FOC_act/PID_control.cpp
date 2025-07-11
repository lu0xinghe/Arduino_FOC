#include <math.h>
#include "PID_control.h"

float PID_init(PID* pid, float kp, float ki, float kd, float limit, float target)
{
	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;

	pid->target = target;

	pid->limit = limit;
}

float PID_increment_control(PID* pid, float actual)//increment����ʽPID
{
	static float Output_1 = 0;

	pid->err[0] = pid->target - actual;

	Output_1 += pid->Kp * (pid->err[0] - pid->err[1]) +
			        pid->Ki * pid->err[0] + 
			        pid->Kd * (pid->err[0] - 2 * pid->err[1] + pid->err[2]);

	pid->err[1] = pid->err[0];
	pid->err[2] = pid->err[1];

	return Output_1;
}

float PID_position_control(PID* pid, float actual)//positionλ��ʽPID
{
	static float Output_2 = 0;

	pid->err[0] = pid->target - actual;
	pid->inter += pid->err[0];
  if(pid->inter > pid->limit) pid->inter = pid->limit;
	else if(pid->inter < -pid->limit) pid->inter = -pid->limit;

	Output_2 = pid->Kp * pid->err[0] +
				      pid->Ki * pid->inter +
				      pid->Kd * (pid->err[0] - pid->err[1]);

	pid->err[1] = pid->err[0];

	return Output_2;
}
