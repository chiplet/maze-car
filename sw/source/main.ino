/* includes */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* pin definitions */

// motor controller
#define PWMA 5
#define AIN2 6
#define AIN1 7
#define STBY 8
#define BIN1 9
#define BIN2 10
#define PWMB 11

// distance sensor IR leds
#define DIST_LED_1 2
#define DIST_LED_2 3
#define DIST_LED_3 4
#define DIST_LED_4 12
#define DIST_LED_5 13

// distance sensor signal pins
#define DIST_SIG_1 A0
#define DIST_SIG_2 A1
#define DIST_SIG_3 A2
#define DIST_SIG_4 A6
#define DIST_SIG_5 A7

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

/* pin number arrays for convenience */
// distance sensor signal pins
const int sensor_pins[] = {DIST_SIG_1, DIST_SIG_2, DIST_SIG_3, DIST_SIG_4, DIST_SIG_5};
const int n_sensor_pins = sizeof(sensor_pins)/sizeof(sensor_pins[0]);

// distance sensor LED pins
const int led_pins[] = {DIST_LED_1, DIST_LED_2, DIST_LED_3, DIST_LED_4, DIST_LED_5};
const int n_led_pins = sizeof(led_pins)/sizeof(led_pins[0]);

// array for holding latest distance sensor readings
int sensor_readings[n_sensor_pins] = {0};

enum motors
{
  M_LEFT,
  M_RIGHT
};

int ain1 = 1;
int ain2 = 0;
int bin1 = 1;
int bin2 = 0;

void setup() {
  /* configure timer interrupt */
  // disable interrupts
  cli();

  // set timer1 to interrupt at 8*60 Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;  // init counter to 0
  // set compare match register for 60hz increments
  // OCR1A = 33332; // = (16*10^6) / (1*8) - 1 (must be <65536)
  OCR1A = 2499; // = (16*10^6) / (1*8) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  // enable interrupts
  sei();

  
  // Serial debug connection
  Serial.begin(115200);

  for (int i = 0; i < n_sensor_pins; i++)
  {
    pinMode(sensor_pins[i], INPUT);
  }

  for (int i = 0; i < n_led_pins; i++)
  {
    pinMode(led_pins[i], OUTPUT);
  }
  
  digitalWrite(AIN1, ain1);
  digitalWrite(AIN2, ain2);
  digitalWrite(BIN1, bin1);
  digitalWrite(BIN2, bin2);
  digitalWrite(STBY, HIGH);

  /* magnetometer */
  /* Enable auto-gain */
  mag.enableAutoRange(true);

  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }

  /* Display some basic information on this sensor */
  // displaySensorDetails();
}

unsigned int current_sensor = 0;

// interrupt service routine for timer1
ISR(TIMER1_COMPA_vect)
{
  // select sensor to be used
  if (current_sensor >= 5) current_sensor = 0;

  // read current sensor
  sensor_readings[current_sensor] = analogRead(sensor_pins[current_sensor]);
  delay(1);

  // turn current sensor off
  digitalWrite(led_pins[current_sensor], LOW);

  // turn next sensor on
  digitalWrite(led_pins[current_sensor+1 <= 4 ? current_sensor+1 : 0], HIGH);

  current_sensor++;
}

int min_index(int* arr, int arrlen)
{
  int min_index = 0;
  for (int i = 0; i < arrlen; i++)
  {
    if (arr[i] < arr[min_index]) min_index = i;
  }
  return min_index;
}

int max_index(int* arr, int arrlen)
{
  int max_index = 0;
  for (int i = 0; i < arrlen; i++)
  {
    if (arr[i] > arr[max_index]) max_index = i;
  }
  return max_index;
}

float mean(int* arr, int arrlen)
{
  float sum = 0;
  for (int i = 0; i < arrlen; i++) sum += arr[i];
  return sum / arrlen;
}


void loop() {  
  // set_motor_speed(M_LEFT, 0.3f);
  // set_motor_speed(M_RIGHT, 0.3f);

  sensors_event_t event;
  mag.getEvent(&event);

  float mx = event.magnetic.x;
  float my = event.magnetic.y;
  float mz = event.magnetic.z;

  float heading = -1; // error

  if (mx >= 0 && my > 0)
  {
    heading = atan(mx/my)*180.0f/PI;
  }
  else if (mx > 0 && my <= 0)
  {
    heading = 90 + atan(-my/mx)*180.0f/PI;
  }
  else if (mx <= 0 && my < 0)
  {
    heading = 180 + atan(mx/my)*180.0f/PI;
  }
  else if (mx < 0 && my >= 0)
  {
    heading = 270 + atan(my/-mx)*180.0f/PI;
  }

  print_sensors();
  Serial.println(heading);
  
  delay(10);
}
