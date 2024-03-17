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


  /* Serial debug connection */
  Serial.begin(115200);

  /* initialise GPIO pins */
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

  /* initialise magnetometer */
  // enable auto gain
  mag.enableAutoRange(true);
  
  if(!mag.begin())
  {
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

// status flags
bool moving = true; // is the car moving through a corridor
bool right_open = false;
bool left_open = false;
bool bright_open = false;
bool bleft_open = false;

// global vars
float left_speed = 0.2f;
float right_speed = 0.2f;

// tunnel driving
float centering_coeff = 0.0003f;

// edge detection
int previous_readings[5] = {512, 512, 512, 512, 512};
int prev_millis = 0;

// save previous readings
void save_previous()
{
  for (int i = 0; i < 5; i++)
  {
    previous_readings[i] = sensor_readings[i];
  }
}

void loop() {
  // experimental edge detector
  int dt = millis() - prev_millis;
  prev_millis = millis();

  int diff1 = (sensor_readings[2] - previous_readings[2])/(float(prev_millis)/1000.0f)*100;

  save_previous();
  
  print_sensors();
  Serial.println(diff1);

  if (moving)
  { 
    // try to centre the car in a tunnel
    int left_dist = sensor_readings[0];
    int right_dist = sensor_readings[2];
    if (left_dist > right_dist)
    {
      float diff_dist = (left_dist - right_dist)*centering_coeff;
      set_motor_speed(M_LEFT, left_speed+diff_dist);
      set_motor_speed(M_RIGHT, right_speed-diff_dist);
    }
    else if (right_dist > left_dist)
    {
      float diff_dist = (right_dist - left_dist)*centering_coeff;
      set_motor_speed(M_LEFT, left_speed-diff_dist);
      set_motor_speed(M_RIGHT, right_speed+diff_dist);
    }
  }

    /* detect branches */
  if (sensor_readings[2] < 600) right_open = true;
  if (sensor_readings[0] < 600) left_open = true;
  if (sensor_readings[4] < 600) bright_open = true;
  if (sensor_readings[3] < 600) bleft_open = true;

  // select tunnel
  if (right_open || left_open)
  {
    // move forward until back sensor agrees
    
  }

  // PANIC!
  if (sensor_readings[1] > 900)
  {
    Serial.println("PANIC!!!");
    set_motor_speed(M_LEFT, 0.0f);
    set_motor_speed(M_RIGHT, 0.0f);
  }
  
}
