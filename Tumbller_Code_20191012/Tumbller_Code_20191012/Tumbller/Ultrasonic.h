/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-08 09:35:07
 * @LastEditTime: 2019-10-10 14:24:57
 * @LastEditors: Please set LastEditors
 */
char measure_flag = 0;
double distance_value = 0;
unsigned long measure_prev_time = 0;
unsigned long get_distance_prev_time = 0;
char obstacle_avoidance_flag = 0;
unsigned long obstacle_avoidance_prev_time = 0;
int follow_flag = 0;
unsigned long follow_prev_time = 0;
long int turn_count = 0;

void ultrasonicInit()
{
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
}

void obstacleAvoidanceMode()
{
  unsigned int super_min = 3;
  unsigned int distance_min = 10;
  unsigned int distance_max = 45;
  unsigned int super_max = 400;

    switch (obstacle_avoidance_flag)
    {
    case 0:
      if (distance_value < distance_max || distance_value > super_max)
      {
        obstacle_avoidance_flag = 1;
        motion_mode = STANDBY;
      }
      else
      {
        motion_mode = FORWARD;
      }
      break;
    case 1:
      if (distance_value >= distance_max && distance_value <= super_max)
      {
        obstacle_avoidance_flag = 0;
        motion_mode = FORWARD;
      }
      else if (distance_value >= super_min && distance_value <= distance_min)
      {
        motion_mode = BACKWARD;
      }
      else
      {
        if (turn_count >= 0)
        {
          motion_mode = TURNRIGHT;
          turn_count++;
        }
        else
        {
          motion_mode = TURNLEFT;
          turn_count--;
        }
      }
      break;
    default:
      break;
    }
  }

void measureDistance()
{
  if (measure_flag == 0)
  {
    measure_prev_time = micros();
    attachPinChangeInterrupt(ECHO_PIN, measureDistance, FALLING);
    measure_flag = 1;
  }
  else if (measure_flag == 1)
  {
    distance_value = (micros() - measure_prev_time) * 0.017; //340.29 m/s / 2 -> (340.29*100 cm) /(1000*1000 us) / 2 = 0.0170145
    measure_flag = 2;
  }
}

void getDistance()
{
  if (millis() - get_distance_prev_time > 50)
  {
    get_distance_prev_time = millis();
    measure_flag = 0;
    attachPinChangeInterrupt(ECHO_PIN, measureDistance, RISING);
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
  }
}
