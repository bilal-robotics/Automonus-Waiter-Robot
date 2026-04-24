// === Libraries for OLED ===                tabel no 2
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === OLED Setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === IR Sensors ===
#define IR_LEFT   A0
#define IR_CENTER A1
#define IR_RIGHT  A2

// === Motor Driver L298N ===
#define ENA 9  // Right Motor PWM
#define IN1 2
#define IN2 3
#define ENB 10 // Left Motor PWM
#define IN3 4
#define IN4 5

// === Buzzer Pin ===
#define BUZZER 12

// === Base Motor Speed ===
int baseSpeed = 55;

// === PID Constants ===
float Kp = 20.0;
float Ki = 0.01;
float Kd = 60.0;

// === PID Variables ===
float error = 0, lastError = 0;
float P = 0, I = 0, D = 0;
float PIDvalue = 0;

// === Steering Trim ===
int steerTrim = 3;

// === Turn Counter & Mission Vars ===
int leftTurnCounter = 0;
int finalDestination = 5;   // max junctions for second pattern
int patternMode = 1;        // 1 = Pattern A, 2 = Pattern B
bool missionComplete = false;

// === Blink Vars ===
unsigned long lastBlink = 0;
bool eyeOpen = true;

// === Beep Function ===
void beep(int duration) {
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
}

// === Motor Functions ===
void setMotor(int leftSpeed, int rightSpeed) {
  // Left Motor
  if (leftSpeed > 0) {
    analogWrite(ENB, leftSpeed);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (leftSpeed < 0) {
    analogWrite(ENB, -leftSpeed);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    analogWrite(ENB, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Right Motor
  if (rightSpeed > 0) {
    analogWrite(ENA, rightSpeed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (rightSpeed < 0) {
    analogWrite(ENA, -rightSpeed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// === OLED Eyes Draw Function (Updated) ===
void drawEyes(bool open) {
  display.clearDisplay();

  int eyeWidth = 30;                   
  int eyeHeight = open ? 25 : 5;       
  int gap = 20;                        

  int totalWidth = (2 * eyeWidth) + gap;
  int startX = (SCREEN_WIDTH - totalWidth) / 2;
  int yPos = (SCREEN_HEIGHT - eyeHeight) / 2; 

  display.fillRect(startX, yPos, eyeWidth, eyeHeight, SSD1306_WHITE);
  display.fillRect(startX + eyeWidth + gap, yPos, eyeWidth, eyeHeight, SSD1306_WHITE);

  display.display();
}

void setup() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  drawEyes(true);

  // === Beep at Startup ===
  beep(200);
  delay(100);
  beep(200);
}

void loop() {
  if (millis() - lastBlink > 500) {  
    eyeOpen = !eyeOpen;
    drawEyes(eyeOpen);
    lastBlink = millis();
  }

  if (missionComplete) {
    stopMotors();
    return;
  }

  int left   = digitalRead(IR_LEFT);
  int center = digitalRead(IR_CENTER);
  int right  = digitalRead(IR_RIGHT);

  Serial.print("L:"); Serial.print(left);
  Serial.print(" C:"); Serial.print(center);
  Serial.print(" R:"); Serial.println(right);

  // === Final Destination Check ===
  if (left == 1 && center == 1 && right == 1 && leftTurnCounter == finalDestination) {
    stopMotors();
    Serial.println("Final destination reached!");
    delay(5000);

    setMotor(-70, 70);
    delay(825);  //adjust according to tour line
    stopMotors();
    delay(1000);

    if (patternMode == 1) {
      patternMode = 2;
      leftTurnCounter = 0;
      Serial.println("Switching to Pattern 2...");
      beep(500);   // === Mission 1 Complete Beep ===
    } else {
      missionComplete = true;
      Serial.println("All missions completed - permanent stop!");
      beep(2000);  // === Mission 2 Complete Final Beep ===
    }
    return;
  }

  // === Junction Detection ===
  if (left == 1 && center == 1 && leftTurnCounter < finalDestination) {
    leftTurnCounter++;
    Serial.print("Junction #"); Serial.println(leftTurnCounter);

    if (patternMode == 1) {
      if (leftTurnCounter == 1 || leftTurnCounter == 3) {
        setMotor(70, 70); delay(150);
        setMotor(-70, 70); delay(350);
      } else if (leftTurnCounter == 2) {
        setMotor(baseSpeed, baseSpeed); delay(150);
      }
    } else if (patternMode == 2) {
      if (leftTurnCounter == 1 || leftTurnCounter == 2 || leftTurnCounter == 3 ) {
        setMotor(70, 70); delay(150);
        setMotor(-70, 70); delay(210);
      } else if (leftTurnCounter == 4 || leftTurnCounter == 5) {
        setMotor(baseSpeed, baseSpeed); delay(150);
      }
    }

    return;
  }

  if (left == 0 && center == 0 && right == 0) {
    stopMotors();
    return;
  }

  if (center == 1) error = 0;
  else if (left == 1) error = -1;
  else if (right == 1) error = +1;

  P = error;
  I = I + error;
  I = constrain(I, -30, 30);
  D = error - lastError;
  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  lastError = error;

  float rawLeft  = baseSpeed - PIDvalue;
  float rawRight = baseSpeed + PIDvalue;

  rawLeft  -= steerTrim;
  rawRight += steerTrim;

  int leftMotorSpeed  = constrain((int)rawLeft, 0, 255);
  int rightMotorSpeed = constrain((int)rawRight, 0, 255);

  setMotor(leftMotorSpeed, rightMotorSpeed);

  delay(10);
}



//finalize robot with tabel 2