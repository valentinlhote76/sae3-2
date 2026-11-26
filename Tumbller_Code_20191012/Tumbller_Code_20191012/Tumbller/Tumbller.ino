/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-09-12 14:51:36
 * @LastEditTime: 2019-10-11 16:39:57
 * @LastEditors: Please set LastEditors
 */
#include <Arduino.h>
#include "PinChangeInt.h"
#include "Pins.h"
#include "mode.h"
#include "Command.h"
#include "BalanceCar.h"
#include "Ultrasonic.h"
#include "voltage.h"

unsigned long start_prev_time = 0;
boolean carInitialize_en = true;

void functionMode()
{
  switch (function_mode)
  {
  case IDLE:
    break;
  case IRREMOTE:
    break;
  case OBSTACLE:
    obstacleAvoidanceMode();
    break;
  default:
    break;
  }
}

void setMotionState()
{
  switch (motion_mode)
  {
  case FORWARD:
    setting_car_speed = 40;
    setting_turn_speed = 0;
    break;
  case BACKWARD:
    setting_car_speed = -40;
    setting_turn_speed = 0;
    break;
  case TURNLEFT:
    setting_car_speed = 0;
    setting_turn_speed = 50;
    break;
  case TURNRIGHT:
    setting_car_speed = 0;
    setting_turn_speed = -50;
    break;
  case STANDBY:
    setting_car_speed = 0;
    setting_turn_speed = 0;
    break;
  case STOP:
    if (millis() - start_prev_time > 1000)
    {
      function_mode = IDLE;
      if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max)
      {
        motion_mode = STANDBY;
      }
    }
    break;
  case START:
    if (millis() - start_prev_time > 2000)
    {
      if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max)
      {
        car_speed_integeral = 0;
        setting_car_speed = 0;
        motion_mode = STANDBY;
      }
      else
      {
        motion_mode = STOP;
        carStop();
      }
    }
    break;
  default:
    break;
  }
}

void keyEventHandle()
{
  if (key_value != '\0')
  {
    key_flag = key_value;

    switch (key_value)
    {
    case 's':
      motion_mode = STANDBY;
      break;
    case 'f':
      motion_mode = FORWARD;
      break;
    case 'b':
      motion_mode = BACKWARD;
      break;
    case 'l':
      motion_mode = TURNLEFT;
      break;
    case 'i':
      motion_mode = TURNRIGHT;
      break;
    case '2':
      function_mode = OBSTACLE;
      obstacle_avoidance_flag = 0;
      obstacle_avoidance_prev_time = millis();
      break;
    case '4':
      function_mode = IDLE;
      motion_mode = STOP;
      carBack(110);
      delay((kalmanfilter_angle - 30) * (kalmanfilter_angle - 30) / 8);
      carStop();
      start_prev_time = millis();
      break;
    case '5':
      if (millis() - start_prev_time > 500 && kalmanfilter_angle >= balance_angle_min)
      {
        start_prev_time = millis();
        motion_mode = START;
      }
      motion_mode = START; 
      break;
    default:
      break;
    }
    if (key_flag == key_value)
    {
      key_value = '\0';
    }
  }
}

void setup()
{
  Serial.begin(9600);
  ultrasonicInit();
  keyInit();
  voltageInit();
  start_prev_time = millis();
  carInitialize();
}
void loop()
{
  getKeyValue();
  keyEventHandle();
  getDistance();
  voltageMeasure();
  setMotionState();
  functionMode();
  checkObstacle();
  static unsigned long print_time;
  if (millis() - print_time > 100)
  {
    print_time = millis();
    Serial.println(kalmanfilter.angle);
  }
  static unsigned long start_time;
  if (millis() - start_time < 10)
  {
    function_mode = IDLE;
    motion_mode = STOP;
    carStop();
  }
  if (millis() - start_time == 2000) // Enter the pendulum, the car balances...
  {
    key_value = '5';
  }
}
