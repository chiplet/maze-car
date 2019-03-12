#include <ArduinoSTL.h>

// print adc readings from all distance sensor pins
void print_sensors()
{
  printf("Distance sensor readings:\n");
  for (int i = 0; i < n_sensor_pins; i++)
  {
    printf("  [%d] = %d\n", i, analogRead(sensor_pins[i]));
  }
  printf("\n");
}

// set given motor to spin at given rate
// positive values -> forward, negative values -> backward
void set_motor_speed(enum motors motor, float value)
{
  // restrict value to range [-1, 1]
  if (value < -1.0f) value = -1.0f;
  if (value > 1.0f) value = 1.0f;
  
  // pin states for forward movement
  int ain1 = 0;
  int ain2 = 1;
  int bin1 = 0;
  int bin2 = 1; 
  
  if (motor == M_LEFT)
  {
    if (value < 0)
    {
      ain1 = !ain1;
      ain2 = !ain2;
      value = 1-value;
    }
    digitalWrite(AIN1, ain1);
    digitalWrite(AIN2, ain2);
    analogWrite(PWMA, value*255);
  }

  if (motor == M_RIGHT)
  {
    if (value < 0)
    {
      bin1 = !bin1;
      bin2 = !bin2;
      value = 1-value;
    }
    digitalWrite(BIN1, bin1);
    digitalWrite(BIN2, bin2);
    analogWrite(PWMB, value*255);
  }
}
