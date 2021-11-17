#include "ESP32Wiimote.h"

ESP32Wiimote wiimote;


// Motor Pins
// motor B
int IN3 = 13;  //M2  Motor B+
int IN4 = 12;  //M1  Motor B-
// motor A
int IN1 = 17;  //M4  Motor A+
int IN2 = 16;  //M3  Motor A-

//
int enB = 18; //GPIO18
int enA = 19; //GPIO19
// virtual channel
int channel_for_enA = 1;
int channel_for_enB = 0;

int motorSpeedA = 255;
int motorSpeedB = 255;

const int LEDC_TIMER_BIT = 8;   // PWM (8bit:0〜255、10bit:0〜1023)
const int LEDC_BASE_FREQ = 100; // Freqency(Hz)
const int VALUE_MAX = 255;      // Max value for PWM


void setup ( ) {

  Serial.begin(115200);
  wiimote.init();
  wiimote.addFilter(ACTION_IGNORE, FILTER_NUNCHUK_ACCEL);

  // Pins to control motors as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // make A,B motor enable pin as output too
  pinMode(enB, OUTPUT);
  pinMode(enA, OUTPUT);
  //
  ledcSetup(channel_for_enA, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(channel_for_enB, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  // link the virtual channel to each enable pin
  ledcAttachPin(enB, channel_for_enB);
  ledcAttachPin(enA, channel_for_enA);

}

// Left wheel
void leftBackward(int volume) {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH );
  ledcWrite(channel_for_enB, volume);
  delay(10);
}

void leftForward(int volume) {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(channel_for_enB, volume);
  delay(10);
}

// Right wheel
void rightBackward(int volume) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(channel_for_enA, volume);
  delay(10);
}
void rightForward(int volume) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(channel_for_enA, volume);
  delay(10);
}

void loop () {
  wiimote.task();

  if (wiimote.available() > 0) {
    uint16_t button = wiimote.getButtonState();
    Serial.printf("%04x\n", button);
    if (button == ESP32Wiimote::BUTTON_A) {
      Serial.println("A button");
      leftForward(0);
      rightForward(0);
      Serial.println("set speed to 0");
    }
   

    NunchukState nunchuk = wiimote.getNunchukState();
    Serial.printf("nunchuk:");
    Serial.printf(" X-Stick: %d", nunchuk.xStick);
    Serial.printf(" Y-Stick: %d", nunchuk.yStick);
    Serial.printf(" X-Axis: %d", nunchuk.xAxis);
    Serial.printf(" Y-Axis: %d", nunchuk.yAxis);
    Serial.printf(" Z-Axis: %d", nunchuk.zAxis);
    Serial.printf(" C-Button: %02x", nunchuk.cBtn);
    Serial.printf(" Z-Button: %02x", nunchuk.zBtn);
    Serial.printf("\n");
    int xStickPosition = nunchuk.xStick - 127;
    int yStickPosition = nunchuk.yStick - 128;
    int left_speed = 0;
    int right_speed = 0;
    // forward or backward
    if (yStickPosition == 0) {
      // left or right
      if (xStickPosition == 0) {
        left_speed = 0;
        right_speed = 0;
        leftForward(left_speed);
        rightForward(right_speed);
      } else if (xStickPosition > 0) {
        // turn right
        int turn_speed =  xStickPosition * 2 - 1;
        leftBackward(turn_speed);
        rightForward(turn_speed);
      } else if (xStickPosition < 0) {
        // turn left
        int turn_speed = (-xStickPosition ) * 2 - 1;
        leftForward(turn_speed);
        rightBackward(turn_speed);
      }
    } else if (yStickPosition > 0) {
      left_speed = yStickPosition;
      right_speed = yStickPosition;

      leftForward(yStickPosition * 2 - 1);
      rightForward(yStickPosition * 2 - 1);

    } else if (yStickPosition < 0) {
      // smaller than 0 means backward
      left_speed = -yStickPosition;
      right_speed = -yStickPosition;

      leftBackward(yStickPosition * 2 - 1);
      rightBackward(yStickPosition * 2 - 1);
    }

  }
  delay(10);

}
