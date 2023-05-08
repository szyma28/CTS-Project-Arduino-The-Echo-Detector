#include <VirtualWire.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

//Define variables 
#define I2C_ADDR          0x3F       //Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7
#define BUZZER_PIN         9


//Initialise the LCD
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

unsigned long previousMillis = 0; // Variable to store the last time the buzzer was reset
const long interval = 1000; // Interval at which to reset the buzzer

void setup() {
  //Define the LCD as 16 column by 2 rows 
  lcd.begin (16, 2);

  //Define the buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  //Define the receiver pin and rate
  vw_set_rx_pin(2);       //Sets pin D12 as the RX Pin
  vw_setup(2000);	       // Bits per sec
  vw_rx_start();           // Start the receiver PLL running

  Serial.begin(9600);
}

const int DISTANCE_MIN[3] = {1, 30, 60};
const int DISTANCE_MAX[3] = {29, 59, 100};

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) {
    // Convert the received message to a float
    char msg[buflen + 1];
    memcpy(msg, buf, buflen);
    msg[buflen] = '\0';
    float distance = atof(msg);

    // Print the received distance on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance is:");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");



    if (distance < 100) {
      int frequency;
      int loudness;

      if (distance >= 60) {
        frequency = map(distance, 60, 100, 100, 500);
        loudness = map(distance, 60, 100, 30, 60);
      } else if (distance >= 30) {
        frequency = map(distance, 30, 59, 500, 1000);
        loudness = map(distance, 30, 59, 60, 90);
      } else {
        frequency = map(distance, 1, 29, 1000, 2000);
        loudness = map(distance, 1, 29, 90, 100);
      }

      tone(BUZZER_PIN, frequency, loudness);
      digitalWrite(BUZZER_PIN, HIGH);
      previousMillis = millis(); // Reset the timer when the buzzer is turned on
    } else {
      // Turn off the buzzer if the timer has expired
      if (millis() - previousMillis >= interval) {
        digitalWrite(BUZZER_PIN, LOW);
      }
    }
  } else {
    // Turn off the buzzer if the timer has expired
    if (millis() - previousMillis >= interval) {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}
