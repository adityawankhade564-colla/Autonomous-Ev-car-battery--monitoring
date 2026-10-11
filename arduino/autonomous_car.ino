c++
  // Arduino UNO - Autonomous EV car
#include <Servo.h>

// -------- ULTRASONIC --------
#define trigPin 3
#define echoPin 2

// -------- MOTOR DRIVER --------
#define IN1 10
#define IN2 9
#define IN3 8
#define IN4 7

#define ENA 11
#define ENB 6

// -------- SERVO --------
#define servoPin 5
Servo myServo;

// -------- SPEED --------
int speedLeft = 140;
int speedRight = 140;

// -------- SETUP --------
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(90);

  stopCar();
  delay(1000);
}

// -------- LOOP --------
void loop() {

  int frontDist = getDistance();

  if (frontDist > 25) {
    forward();
  } 
  else {

    // 🛑 STOP
    stopCar();
    delay(100);

    // ⬅ SMALL BACKWARD (create turning space)
    backward();
    delay(350);   // adjust if needed (200–400)

    stopCar();
    delay(100);

    // 🔍 SCAN LEFT & RIGHT
    int rightDist = lookRight();
    int leftDist  = lookLeft();

    delay(100);

    // ↪ DECIDE DIRECTION
    if (rightDist > leftDist) {
      turnRight();
    } else {
      turnLeft();
    }

    stopCar();
    delay(100);
  }
}

// -------- ULTRASONIC --------
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 25000);

  if (duration == 0) return 200;

  return duration * 0.034 / 2;
}

// -------- SCAN SERVO --------
int lookRight() {
  myServo.write(10);
  delay(300);
  int d = getDistance();
  myServo.write(90);
  delay(200);
  return d;
}

int lookLeft() {
  myServo.write(170);
  delay(300);
  int d = getDistance();
  myServo.write(90);
  delay(200);
  return d;
}

// -------- MOVEMENTS --------
void forward() {
  analogWrite(ENA, speedRight);
  analogWrite(ENB, speedLeft);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  analogWrite(ENA, speedRight);
  analogWrite(ENB, speedLeft);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight() {
  analogWrite(ENA, speedRight);
  analogWrite(ENB, speedLeft);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(400);
}

void turnLeft() {
  analogWrite(ENA, speedRight);
  analogWrite(ENB, speedLeft);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(400);
}

void stopCar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
