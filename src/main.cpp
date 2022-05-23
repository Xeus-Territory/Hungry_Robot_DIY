#include <Arduino.h>
#include <Servo.h>
#include <string.h>
#include <pitch.h>
#define SERVO_PIN 9 // Servo pin create in PWM mode
#define TRIGGER_PIN 5
#define ECHO_PIN 6
#define BELL_PIN 11
#define INTERRUPT_PIN 2

Servo myservo;
unsigned long time;
extern volatile unsigned long timer0_millis; // Get the obj timer0_millis from hardware/cores/arduino/wiring.c
bool first = true;                           // first time (Prevent the servo to move when the program starts)
int distance_to_object = 5; //default distance
unsigned int time_delay = 3000; // default time to servo
unsigned int time_adding_delay = 3501; // default time to servo
int rhythmNotes[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,

    NOTE_A4, NOTE_A4,
    // Repeat of first part
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,
    // End of Repeat

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4,

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4}; // Hook
int rhythmBeats[] = {
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,

    250, 125,
    // Rpeat of First Part
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,
    // End of Repeat

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500,

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500};
const float songSpeed = 1.0;

void playMusic()
{
  int rhythmDuration;
  const int rhythmLength = sizeof(rhythmNotes) / sizeof(int);
  for (int i = 0; i < rhythmLength; i++)
  {
    rhythmDuration = rhythmBeats[i] / songSpeed;
    if (rhythmNotes[i] == 0)
    {
      noTone(BELL_PIN);
    }
    else
    {
      tone(BELL_PIN, rhythmNotes[i], rhythmDuration);
    }
    delay(rhythmDuration);
    if (Serial.available() > 0)
    {
      break;
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  time = millis();
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BELL_PIN, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  myservo.attach(SERVO_PIN);
  myservo.write(145);
}

float scan_obj()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void ring_Bell()
{
  digitalWrite(BELL_PIN, HIGH);
  delay(1000);
  digitalWrite(BELL_PIN, LOW);
}

// Function to reset the timer assign for millis()
void setMillis(unsigned long new_millis)
{
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis = new_millis;
  SREG = oldSREG;
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0)
  {
    int method = 0;
    String x = Serial.readString();
    if (strstr(x.c_str(), "L") != NULL)
    {
      x = x.substring(1);
      distance_to_object = x.toInt();
    }
    if (strstr(x.c_str(), "T") != NULL)
    {
      x = x.substring(1);
      time_delay = x.toInt();
      time_adding_delay = time_delay + 501;
    }
    if (strstr(x.c_str(), "L") == NULL && strstr(x.c_str(), "T") == NULL)
    {
      method = x.toInt();
    }
    switch (method)
    {
    case 1:
    {
      ring_Bell();
      myservo.write(40);
      delay(500);
      myservo.write(145);
      delay(500);
    }
    break;

    case 2:
      playMusic();
      break;

    case 3:
    {
      while (true)
      {
        float distance = scan_obj(); // scan object
        Serial.print(String(distance_to_object) + " " + String(time_delay) + " " + String(distance) + " ");
        if (distance < distance_to_object && first == true)
        {
          first = false;
          continue;
        }
        if (distance < distance_to_object && first != true && (unsigned long)(millis() - time) > time_delay)
        {
          ring_Bell();
          myservo.write(40);
          delay(500);
          myservo.write(145);
          time = 0;
          setMillis(0);
        }
        if ((unsigned long)(millis() - time) > time_adding_delay)
        {
          setMillis(0);
        }
        delay(500);
        if (Serial.available() > 0)
        {
          break;
        }
      }
    }
    break;
    }
  }
}