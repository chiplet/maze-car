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
#define DIST_LED_4 A1
#define DIST_LED_5 A0
#define DIST_LED_6 A2

// distance sensor signal pins
#define DIST_SIG_1 A5
#define DIST_SIG_2 A6
#define DIST_SIG_3 A7
#define DIST_SIG_4 A4
#define DIST_SIG_5 A3
#define DIST_SIG_6 A2


/* pin number arrays for convenience */

// distance sensor signal pins
const int sensor_pins[] = {DIST_SIG_1, DIST_SIG_2, DIST_SIG_3, DIST_SIG_4, DIST_SIG_5, DIST_SIG_6};
const int n_sensor_pins = sizeof(sensor_pins)/sizeof(sensor_pins[0]);

// distance sensor LED pins
const int led_pins[] = {DIST_LED_1, DIST_LED_2, DIST_LED_3, DIST_LED_4, DIST_LED_5, DIST_LED_6};
const int n_led_pins = sizeof(led_pins)/sizeof(led_pins[0]);

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
  // Serial debug connection
  Serial.begin(115200);
  
  digitalWrite(AIN1, ain1);
  digitalWrite(AIN2, ain2);
  digitalWrite(BIN1, bin1);
  digitalWrite(BIN2, bin2);
  digitalWrite(STBY, HIGH);
}

void loop() {
  const float freq = 0.1f;
  float a = sin(2*PI*millis()/1000.0f*freq);
  
  set_motor_speed(M_LEFT, a);
  set_motor_speed(M_RIGHT, -a);
}
