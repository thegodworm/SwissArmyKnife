#define STEPPER_MIN_DELAY 3
#define STEPPER_MAX_DELAY 20


class Stepper {

  public :
    Stepper() {}
    Stepper(int PIN_1, int PIN_2, int PIN_3, int PIN_4) :
      STEPPER_PIN_1(PIN_1),
      STEPPER_PIN_2(PIN_2),
      STEPPER_PIN_3(PIN_3),
      STEPPER_PIN_4(PIN_4),
      TWO_PIN_CONTROL(false)
    {
      Serial.println("4 PIN STEPPER");
      
        
    }
    
    Stepper(int DIR_PIN, int STEP_PIN) :
      STEPPER_PIN_1(DIR_PIN),
      STEPPER_PIN_2(STEP_PIN),
      TWO_PIN_CONTROL(true)
    {
      Serial.println("2 PIN STEPPER");
    }

    void setDist(int value) {
      dir = value > 0;
      dist = abs(value);
    }

    void setSpeed(int value) {
      value = 100 - abs(value);
      speed = int(STEPPER_MIN_DELAY + (value * 0.01f) * (STEPPER_MAX_DELAY - STEPPER_MIN_DELAY));
      speed = min(STEPPER_MAX_DELAY, max(STEPPER_MIN_DELAY, speed));
    }

    void begin() {
      pinMode(STEPPER_PIN_1, OUTPUT);
      pinMode(STEPPER_PIN_2, OUTPUT);
      if (!TWO_PIN_CONTROL) pinMode(STEPPER_PIN_3, OUTPUT);
      if (!TWO_PIN_CONTROL) pinMode(STEPPER_PIN_4, OUTPUT);

//      Serial.print(TWO_PIN_CONTROL);
//      Serial.print(" : ");
//      Serial.print(STEPPER_PIN_1);
//      Serial.print(" : ");
//      Serial.println(STEPPER_PIN_2);
      
    }

    void run() {
      if (dist > 0) {
        OneStep(dir);
        dist--;
        delay(speed);
      }
    }

  private :
    uint8_t step_number = 0;
    uint8_t STEPPER_PIN_1 = 255;
    uint8_t STEPPER_PIN_2 = 255;
    uint8_t STEPPER_PIN_3 = 255;
    uint8_t STEPPER_PIN_4 = 255;
    bool TWO_PIN_CONTROL = false;
    bool dir = 0;
    long dist = 0;
    int speed = 10;

    void OneStep(bool dir) {
      if (TWO_PIN_CONTROL) {
//        Serial.println(dist);
        digitalWrite(STEPPER_PIN_1, dir ? HIGH : LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        delayMicroseconds(speed);
        digitalWrite(STEPPER_PIN_2, LOW);
        delayMicroseconds(speed);
        return;
      }
      if (dir) {
        switch (step_number) {
          case 0:
            digitalWrite(STEPPER_PIN_1, HIGH);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, LOW);
            break;
          case 1:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, HIGH);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, LOW);
            break;
          case 2:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, HIGH);
            digitalWrite(STEPPER_PIN_4, LOW);
            break;
          case 3:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, HIGH);
            break;
        }
      } else {
        switch (step_number) {
          case 0:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, HIGH);
            break;
          case 1:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, HIGH);
            digitalWrite(STEPPER_PIN_4, LOW);
            break;
          case 2:
            digitalWrite(STEPPER_PIN_1, LOW);
            digitalWrite(STEPPER_PIN_2, HIGH);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, LOW);
            break;
          case 3:
            digitalWrite(STEPPER_PIN_1, HIGH);
            digitalWrite(STEPPER_PIN_2, LOW);
            digitalWrite(STEPPER_PIN_3, LOW);
            digitalWrite(STEPPER_PIN_4, LOW);
        }
      }
      step_number++;
      step_number %= 4;
    }
};
