#include <ArduinoSTL.h>

void print_sensors()
{
  for (int i = 0; i < n_sensor_pins; i++)
  {
    printf("%d, ", sensor_readings[i]);
  }
}

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
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
  
  if (motor == M_RIGHT)
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

  if (motor == M_LEFT)
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

// TODO change implementation to store recorded values to an array
// call this function from a timer interffupt (restructure for that) so that it is continuously updating the distance values
// the measure_distance functions should then just get the corresponding value from the array
void test_sensors(const int* sensor_pins, const int* led_pins, int sensor_index, int n_sensor_pins)
{
  for (int i = 0; i < n_sensor_pins; i++)
  {
    digitalWrite(led_pins[i], HIGH);
    delay(10);
    int reading = analogRead(sensor_pins[i]);
    delay(40);
    digitalWrite(led_pins[i], LOW);
    delay(200);
  }
}
