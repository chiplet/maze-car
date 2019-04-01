// TODO: Debug why sensors 4 and 5 don't work

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
#define DIST_LED_6 13

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
}

unsigned int current_sensor = 0;

// interrupt service routine for timer1
ISR(TIMER1_COMPA_vect)
{
  // select sensor to be used
  if (current_sensor >= 6) current_sensor = 0;

  // read current sensor
  sensor_readings[current_sensor] = analogRead(sensor_pins[current_sensor]);
  delay(1);

  // turn current sensor off
  digitalWrite(led_pins[current_sensor], LOW);

  // turn next sensor on
  digitalWrite(led_pins[current_sensor+1 <= 5 ? current_sensor+1 : 0], HIGH);

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

void flush_serial(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}   

// frame format, start byte 'b' followed by four bytes indicating key states (ascii '1' for on, anything else for off

float left_speed = 0.0f;
float right_speed = 0.0f;

char buf[10];

void loop() {
  while (!Serial.available());
  char state = Serial.read();

  char w = state & 1 << 3;
  char a = state & 1 << 2;
  char s = state & 1 << 1;
  char d = state & 1 << 0;

  char spd = state >> 4;
  float f_speed = (float)spd/16.0f;

  /*
  Serial.print(w ? '1' : '0');
  Serial.print(a ? '1' : '0');
  Serial.print(s ? '1' : '0');
  Serial.print(d ? '1' : '0');
  Serial.println();
  */

  flush_serial();

  left_speed = 0.0f;
  right_speed = 0.0f;
  if (w)
  {
    left_speed += f_speed;
    right_speed += f_speed;
  }
  if (a)
  {
    left_speed -= f_speed;
    right_speed += f_speed;
  }
  if (s)
  {
    left_speed -= f_speed;
    right_speed -= f_speed;
  }
  if (d)
  {
    left_speed += f_speed;
    right_speed -= f_speed;
  }
  
  set_motor_speed(M_LEFT, left_speed);
  set_motor_speed(M_RIGHT, right_speed);

  delay(1);
}
