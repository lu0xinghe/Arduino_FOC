#include "FOC.h"
#include "AS5600.h"
#include "current.h"
#include <Arduino.h>

#define _3PI_2 4.71238898038f
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

int pwmA = 32;
int pwmB = 33;
int pwmC = 25;
int pinA = 39;
int pinB = 36;
int i=0;

float shaft_angle = 0, open_loop_timestamp = 0,zero_electric_angle = 0;
float motor_target=3.14;
float current_ab[2],I_dq[2];

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
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
}

float velocityOpenloop(float target_velocity) {
  unsigned long now_us = micros();  //获取从开启芯片以来的微秒数，它的精度是 4 微秒。 micros() 返回的是一个无符号长整型（unsigned long）的值

  //计算当前每个Loop的运行时间间隔
  float Ts = (now_us - open_loop_timestamp) * 1e-6f;

  //由于 micros() 函数返回的时间戳会在大约 70 分钟之后重新开始计数，在由70分钟跳变到0时，TS会出现异常，因此需要进行修正。如果时间间隔小于等于零或大于 0.5 秒，则将其设置为一个较小的默认值，即 1e-3f
  if (Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;


  // 通过乘以时间间隔和目标速度来计算需要转动的机械角度，存储在 shaft_angle 变量中。在此之前，还需要对轴角度进行归一化，以确保其值在 0 到 2π 之间。
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);
  //以目标速度为 10 rad/s 为例，如果时间间隔是 1 秒，则在每个循环中需要增加 10 * 1 = 10 弧度的角度变化量，才能使电机转动到目标速度。
  //如果时间间隔是 0.1 秒，那么在每个循环中需要增加的角度变化量就是 10 * 0.1 = 1 弧度，才能实现相同的目标速度。因此，电机轴的转动角度取决于目标速度和时间间隔的乘积。

  FOC_act(6,shaft_angle);
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  open_loop_timestamp = now_us;  //用于计算下一个时间间隔

  return 0;
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
  // ledcSetup(0, 30000, 8);  //pwm频道, 频率, 精度
  // ledcSetup(1, 30000, 8);  //pwm频道, 频率, 精度
  // ledcSetup(2, 30000, 8);  //pwm频道, 频率, 精度
  // ledcAttachPin(pwmA, 0);
  // ledcAttachPin(pwmB, 1);
  // ledcAttachPin(pwmC, 2);
  Serial.println("完成PWM初始化设置");
  BeginSensor();

  FOC_act(6, _3PI_2);
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  delay(1000);
  zero_electric_angle = _electricalAngle();
  FOC_act(0, _3PI_2);
  setPwm(T_vector[0],T_vector[1],T_vector[2]);
  Serial.print("0电角度：");
  Serial.println(zero_electric_angle);
  delay(1000);
  // 配置函数
  ADCInline(pinA,pinB);
  // 校准
  offset(current_ab);
}

void loop() {
  float Sensor_Angle = getAngle();
  float Kp = 0.133;
  current(current_ab,I_dq,_electricalAngle());
  velocityOpenloop(3.14*7);
  // FOC_act(_constrain(Kp * (motor_target - DIR * Sensor_Angle) * 180 / PI, -6, 6),_electricalAngle());
  // setPwm(T_vector[0],T_vector[1],T_vector[2]);
  i++;
  if(i==100)
  {
    printf("%f,%f,%f\r\n",zero_electric_angle, I_dq[0],I_dq[1]);
    i=0;
  }
}
