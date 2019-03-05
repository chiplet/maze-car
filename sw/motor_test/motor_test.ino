// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define AIN1 7
#define AIN2 6
#define PWMA 5
#define BIN1 8
#define BIN2 9
#define PWMB 10

int ain1 = 1;
int ain2 = 0;
int bin1 = 1;
int bin2 = 0;

void setup() {
  // put your setup code here, to run once:

  digitalWrite(AIN1, ain1);
  digitalWrite(AIN2, ain2);
  digitalWrite(BIN1, bin1);
  digitalWrite(BIN2, bin2);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  analogWrite(PWMA, 64);
  analogWrite(PWMB, 64);

  ain1 = !ain1;
  ain2 = !ain2;
  bin1 = !bin1;
  bin2 = !bin2;
  digitalWrite(AIN1, ain1);
  digitalWrite(AIN2, ain2);
  digitalWrite(BIN1, bin1);
  digitalWrite(BIN2, bin2);
}
