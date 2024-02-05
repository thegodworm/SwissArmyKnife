
#include <pwmWrite.h>

Pwm pwm = Pwm();


class PWM {
  public :
    PWM() {};
    PWM(uint8_t pin)
      : pin(pin)
    {
      ch = pwm.attach(pin);
      if (ch % 2 == 0) { // even ch
        pwm.mem[ch + 1].pin = pin;
      } else { // odd ch
        pwm.mem[ch - 1].pin = pin;
      }
    };
    void setValue(int value) {
      pwm.write(pin, value);
    }
  private :
    uint8_t pin;
    int ch;

};


class Servo {
  public :
    Servo() {};
    Servo(uint8_t pin)
      : pin(pin) {
      ch = pwm.attachServo(pin);
      if (ch % 2 == 0) { // even ch
        pwm.mem[ch + 1].pin = pin;
      } else { // odd ch
        pwm.mem[ch - 1].pin = pin;
      }
    };
    void setPos(int pos) {
      pwm.writeServo(pin, pos);
    }
  private :
    uint8_t pin;
    int ch;
};

class DCmotor {
  public :
    enum DIRECTION { FORWARD, BACKWARD, STOP };
    DCmotor() {};
    DCmotor(uint8_t pinA, uint8_t pinB, uint8_t pinSpeed)
      : pinA(pinA),
        pinB(pinB),
        pinS(pinSpeed) {
      ch = pwm.attach(pinSpeed);
      if (ch % 2 == 0) { // even ch
        pwm.mem[ch + 1].pin = pinSpeed;
      } else { // odd ch
        pwm.mem[ch - 1].pin = pinSpeed;
      }
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
    };
    void setDir(DIRECTION dir) {
      switch (dir) {
        case FORWARD:
          digitalWrite(pinA, LOW);
          digitalWrite(pinB, HIGH);
          break;
        case BACKWARD:
          digitalWrite(pinA, HIGH);
          digitalWrite(pinB, LOW);
          break;
        case STOP:
          digitalWrite(pinA, LOW);
          digitalWrite(pinB, LOW);
          break;
      }
    }
    void setSpeed(int speed) {
      pwm.write(pinS, (int)floor(speed * 2.55f));
    }
  private :
    uint8_t pinA;
    uint8_t pinB;
    uint8_t pinS;
    int ch;

};
