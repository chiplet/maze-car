// global variables used by the helper functions
unsigned int current_sensor = 0;  // used by the timer interrupt service routine to update sensor readings

// print latest distance sensor readings using the Serial connection
void print_sensors()
{
  for (int i = 0; i < n_sensor_pins; i++)
  {
    Serial.print(String() + sensor_readings[i] + ", ");
  }
}

// print magnetometer sensor details using the Serial connection
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
    value += 0.04; // increase right motor speed to compensate for turning
    if (value < 0)
    {
      ain1 = !ain1;
      ain2 = !ain2;
      value = 1 - value;
    }
    digitalWrite(AIN1, ain1);
    digitalWrite(AIN2, ain2);
    analogWrite(PWMA, value * 255);
  }

  if (motor == M_LEFT)
  {
    if (value < 0)
    {
      bin1 = !bin1;
      bin2 = !bin2;
      value = 1 - value;
    }
    digitalWrite(BIN1, bin1);
    digitalWrite(BIN2, bin2);
    analogWrite(PWMB, value * 255);
  }
}

// get the latest measurement from the distance sensor with index index
// returns -1 on failure
int get_distance_sensor(int index)
{
  if (index < 0 || index >= 5)
  {
    Serial.println("Error! Distance sensor index out of bounds! Index needs to be a value between 0 and 4.");
    return -1;
  }
  return sensor_readings[index];
}

// read magnetometer x component
float get_magnetometer_x()
{
  sensors_event_t event;
  mag.getEvent(&event);

  float mx = event.magnetic.x;
  return mx;
}

// read magnetometer y component
float get_magnetometer_y()
{
  sensors_event_t event;
  mag.getEvent(&event);

  float my = event.magnetic.y;
  return my;
}

// read magnetometer z component
float get_magnetometer_z()
{
  sensors_event_t event;
  mag.getEvent(&event);

  float mz = event.magnetic.z;
  return mz;
}

// timer interrupt service routine for updating sensor readings
ISR(TIMER1_COMPA_vect)
{
  // turn current sensor on
  digitalWrite(led_pins[current_sensor], HIGH);
  delay(1);

  // read current sensor
  sensor_readings[current_sensor] = analogRead(sensor_pins[current_sensor]);

  // turn current sensor off
  digitalWrite(led_pins[current_sensor], LOW);

  current_sensor++;

  // select sensor to be used
  if (current_sensor >= 5) current_sensor = 0;
}
