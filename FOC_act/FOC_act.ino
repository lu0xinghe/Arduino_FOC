#include "FOC.h"
#include "AS5600.h"
#include "current.h"
#include "PID_control.h"
#include <Arduino.h>

#define _3PI_2 4.71238898038f
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

PID PID_electricity;
PID *pid_e = &PID_electricity;

int pwmA = 32;
int pwmB = 33;
int pwmC = 25;
int pinA = 39;
int pinB = 36;
int i=0;
unsigned long now_us;
float shaft_angle = 0, open_loop_timestamp = 0,zero_electric_angle = 0;
float motor_target=3.14;
float current_ab[2],I_dq[2];
float Ts;

int PP = 7, DIR = 1;
float _electricalAngle() {
  return _normalizeAngle((float)(DIR * PP) * getAngle_Without_track()-zero_electric_angle);
}

// 归一化角度到 [0,2PI]
float _normalizeAngle(float angle) {
  float a = fmod(angle, 2 * PI);  //取余运算可以用于归一化，列出特殊值例子算便知
  return a >= 0 ? a : (a + 2 * PI);
  //三目运算符。格式：condition ? expr1 : expr2
  //其中，condition 是要求值的条件表达式，如果条件成立，则返回 expr1 的值，否则返回 expr2 的值。可以将三目运算符视为 if-else 语句的简化形式。
  //fmod 函数的余数的符号与除数相同。因此，当 angle 的值为负数时，余数的符号将与 _2PI 的符号相反。也就是说，如果 angle 的值小于 0 且 _2PI 的值为正数，则 fmod(angle, _2PI) 的余数将为负数。
  //例如，当 angle 的值为 -PI/2，_2PI 的值为 2PI 时，fmod(angle, _2PI) 将返回一个负数。在这种情况下，可以通过将负数的余数加上 _2PI 来将角度归一化到 [0, 2PI] 的范围内，以确保角度的值始终为正数。
}

void setPwm(float Ua, float Ub, float Uc) {
  //写入PWM到PWM 0 1 2 通道
  ledcWrite(pwmA, Ua * 255);
  ledcWrite(pwmB, Ub * 255);
  ledcWrite(pwmC, Uc * 255);
}

void ADCInline(const int pinA,const int pinB){
  pinMode(pinB, INPUT);
  pinMode(pinA, INPUT);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //PWM设置
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);  //V4电机使能

  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(pwmC, OUTPUT);
  ledcAttach(pwmA, 30000, 8);
  ledcAttach(pwmB, 30000, 8);
  ledcAttach(pwmC, 30000, 8);
  Serial.println("完成PWM初始化设置");
  BeginSensor();

  FOC_act( 6, 0, 0);
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  delay(1000);
  zero_electric_angle = _electricalAngle();
  FOC_act( 0, 0, 0);
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  Serial.print("0电角度：");
  Serial.println(zero_electric_angle);
  Serial.println(getAngle_Without_track());
  
  delay(1000);
  // 配置函数
  ADCInline(pinA,pinB);
  // 校准
  Serial.print("校准");
  setPwm(0.5,0.5,0.5);
  offset(current_ab);
  delay(1000);

  PID_init(pid_e,0.5,0.1,0,100,0.3);
}

void loop() {
  float angle_e = _electricalAngle();
  float value = PID_position_control(pid_e,I_dq[0]);
  FOC_act(0,6,angle_e);   // 传递电角度
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  current(I_dq, angle_e); // 传递电角度

  i++;
  if(i==10){
  // printf("%f,%f,%f,%f,%f,",_electricalAngle(),zero_electric_angle,T_vector[0],T_vector[1],T_vector[2]);
  printf("%f,%f,",pid_e->err[0],pid_e->inter);
  printf("%f,%f,%f,%f\r\n",value,current_ab[1]*(3.3f/4096),I_dq[0],I_dq[1]);
  i=0;
  }

}
