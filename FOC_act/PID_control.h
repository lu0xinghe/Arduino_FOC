#ifndef PID_CONTROL_H
#define PID_CONTROL_H

typedef struct PID_Control
{
	float Kp;
	float Ki;
	float Kd;

	float err[3];

	float inter;
	float limit;

	float target;
}PID;

float PID_init(PID* pid, float kp, float ki, float kd, float limit, float target);//PID��ʼ��
float PID_increment_control(PID* pid, float actual);//increment����ʽPID
float PID_position_control(PID* pid, float actual);//positionλ��ʽPID

#endif
