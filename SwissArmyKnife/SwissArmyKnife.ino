#define STEPPER_MIN_DELAY 3
#define STEPPER_MAX_DELAY 20

#include <easyOsc.h>
#include "Stepper.h"
#include "PWM_Servo_DCmotor.h"
#include "Tools.h"

const String SSID = "CAN Bayonet 2000 ";

EasyOsc com(SSID); // <======== configure your own wifi network
// DEFAULT OSC OUTPUT PORT 9999
// DEFAULT OSC INPUT PORT 8888

uint8_t sensorCounter = 0 ;
const uint8_t MAX_SENSOR = 32;
SensorHandler sensors[MAX_SENSOR] ;

uint8_t stepperCounter = 0 ;
const uint8_t MAX_STEPPER = 3;
Stepper steppers[MAX_STEPPER];

uint8_t outputCounter = 0 ;
const uint8_t MAX_OUTPUT = 32;
uint8_t outputs[MAX_OUTPUT];

uint8_t pwmCounter = 0 ;
const uint8_t MAX_PWM = 8;
PWM pwms[MAX_PWM];

uint8_t servoCounter = 0 ;
const uint8_t MAX_SERVO = 8;
Servo servos[MAX_SERVO];

uint8_t dcMotorCounter = 0 ;
const uint8_t MAX_DC_MOTOR = 8;
DCmotor dcMotors[MAX_DC_MOTOR];

void setup() {

  Serial.begin(115200);
  IPAddress ip = com.begin();
  Serial.print("My SSID is : ");
  Serial.println(SSID);

  Serial.print("My IP is : ");
  Serial.println(ip);

  com.onMessage("/ping", [](OSCMessage & msg) {
    com.send("/pong/", (int)millis());
  });

  /*
                     _     _____   _____
                    | |   |_   _| |  __ \
      ___    _   _  | |_    | |   | |__) |
     / _ \  | | | | | __|   | |   |  ___/
    | (_) | | |_| | | |_   _| |_  | |
     \___/   \__,_|  \__| |_____| |_|
    -> /setup/outIP A.B.C.D default is 192.168.4.255
  */
  com.onMessage("/setup/outIP", [](OSCMessage & msg) {
    if (!msg.isInt(0), !msg.isInt(1), !msg.isInt(2), !msg.isInt(3))return;
    uint8_t A = msg.getInt(0);
    uint8_t B = msg.getInt(1);
    uint8_t C = msg.getInt(2);
    uint8_t D = msg.getInt(3);
    com.setOutIP(A, B, C, D);
  });

  /*
                     _                    _     _                              _
                    | |                  | |   (_)                            | |
     _ __     ___   | |_    ___   _ __   | |_   _    ___    _ __ ___     ___  | |_    ___   _ __
    | '_ \   / _ \  | __|  / _ \ | '_ \  | __| | |  / _ \  | '_ ` _ \   / _ \ | __|  / _ \ | '__|
    | |_) | | (_) | | |_  |  __/ | | | | | |_  | | | (_) | | | | | | | |  __/ | |_  |  __/ | |
    | .__/   \___/   \__|  \___| |_| |_|  \__| |_|  \___/  |_| |_| |_|  \___|  \__|  \___| |_|
    | |
    |_|
    -> /setup/potentiometer [PIN]
    <- /potentiometer/status id
    <- /potentiometer VALUE
  */
  com.onMessage("/setup/potentiometer", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;
    com.send("/potentiometer/status", sensorCounter );
    String id = String(sensorCounter);
    uint8_t pin = msg.getInt(0);
    Serial.println("/potentiometer/status " + id);
    sensors[sensorCounter] = {
      [pin, id]() {
        com.send("/potentiometer/" + id, analogRead(pin));
      }
    };
    sensorCounter++;
  });

  /*
     _                           _
    (_)                         | |
     _   _ __    _ __    _   _  | |_
    | | | '_ \  | '_ \  | | | | | __|
    | | | | | | | |_) | | |_| | | |_
    |_| |_| |_| | .__/   \__,_|  \__|
                | |
                |_|
    -> /setup/input [PIN]
    <- /input/status id
    <- /input VALUE
  */
  com.onMessage("/setup/input", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;
    uint8_t pin = msg.getInt(0);
    String id = String(sensorCounter);
    com.send("/input/status", id);
    sensors[sensorCounter] = {
      [pin, id]() {
        pinMode(pin, INPUT);
        com.send("/input/" + id, digitalRead(pin));
      }
    };
    sensorCounter++;
  });

  /*
     _                           _                              _   _
    (_)                         | |                            | | | |
     _   _ __    _ __    _   _  | |_            _ __    _   _  | | | |  _   _   _ __
    | | | '_ \  | '_ \  | | | | | __|          | '_ \  | | | | | | | | | | | | | '_ \
    | | | | | | | |_) | | |_| | | |_           | |_) | | |_| | | | | | | |_| | | |_) |
    |_| |_| |_| | .__/   \__,_|  \__|          | .__/   \__,_| |_| |_|  \__,_| | .__/
                | |                    ______  | |                             | |
                |_|                   |______| |_|                             |_|
    -> /setup/input_pullup [PIN]
    <- /touch/status id
    <- /input_pullup VALUE
  */
  com.onMessage("/setup/input_pullup", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;
    uint8_t pin = msg.getInt(0);
    String id = String(sensorCounter);
    com.send("/input_pullup/status", id);
    sensors[sensorCounter] = {
      [pin, id]() {
        pinMode(pin, INPUT_PULLUP);
        com.send("/input_pullup/" + id, !digitalRead(pin));
      }
    };
    sensorCounter++;
  });

  /*
     _                            _
    | |                          | |
    | |_    ___    _   _    ___  | |__
    | __|  / _ \  | | | |  / __| | '_ \
    | |_  | (_) | | |_| | | (__  | | | |
     \__|  \___/   \__,_|  \___| |_| |_|
    -> /setup/touch [PIN] 4 15 13 12 14 27 33 32  
    <- /touch/status id
    <- /touch/id VALUE
  */
  com.onMessage("/setup/touch", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;
    uint8_t pin = msg.getInt(0);
    String id = String(sensorCounter);

    com.send("/touch/status", id);
    sensors[sensorCounter] = {
      [pin, id]() {
        com.send("/touch/" + id, touchRead(pin));
      }
    };
    sensorCounter++;
  });

  /*
           _
          | |
     ___  | |_    ___   _ __    _ __     ___   _ __
    / __| | __|  / _ \ | '_ \  | '_ \   / _ \ | '__|
    \__ \ | |_  |  __/ | |_) | | |_) | |  __/ | |
    |___/  \__|  \___| | .__/  | .__/   \___| |_|
                       | |     | |
                       |_|     |_|
    -> /setup/stepper [PIN1 - PIN2 - PIN3 - PIN4] OR [DIR_PIN - STEP_PIN]
    <- /stepper/status id
    -> /stepper/id [DIST (>0 Cloclwise <0 CounterClockwise ) - SPEED]
  */
  com.onMessage("/setup/stepper", [](OSCMessage & msg) {
    Serial.println("/setup/stepper");
    
    if (msg.size()== 4 && msg.isInt(0) && msg.isInt(1) && msg.isInt(2) && msg.isInt(3)) {
      uint8_t A = msg.getInt(0);
      uint8_t B = msg.getInt(1);
      uint8_t C = msg.getInt(2);
      uint8_t D = msg.getInt(3);
      steppers[stepperCounter] = Stepper(A, B, C, D);
    }else if (msg.size()== 2 && msg.isInt(0) && msg.isInt(1)) {
      uint8_t A = msg.getInt(0);
      uint8_t B = msg.getInt(1);
      steppers[stepperCounter] = Stepper(A, B);
    }
    
    Serial.println("ok");

    steppers[stepperCounter].begin();
    String id = String(stepperCounter);

    com.send("/stepper/status", stepperCounter );
    Serial.println("/stepper/status " + id);

    com.onMessage("/stepper/speed/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/stepper/speed/", MAX_STEPPER);
      if (id > MAX_STEPPER)return;
      steppers[id].setSpeed(msg.getInt(0));
    });

    com.onMessage("/stepper/dist/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/stepper/dist/", MAX_STEPPER);
      if (id > MAX_STEPPER)return;
      steppers[id].setDist(msg.getInt(0));
    });
    stepperCounter++;
  });

  /*
                     _                     _
                    | |                   | |
      ___    _   _  | |_   _ __    _   _  | |_
     / _ \  | | | | | __| | '_ \  | | | | | __|
    | (_) | | |_| | | |_  | |_) | | |_| | | |_
     \___/   \__,_|  \__| | .__/   \__,_|  \__|
                          | |
                          |_|
    -> /setup/output [PIN]
    <- /output/status id
    -> /output/id [VALUE (0 is LOW larger is HIGH)]
  */
  com.onMessage("/setup/output", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;

    outputs[outputCounter] = msg.getInt(0);
    String id = String(outputCounter);

    com.send("/output/status", outputCounter );
    Serial.println("/output/status " + id);

    com.onMessage("/output/run/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/output/run/", MAX_OUTPUT);
      if (id > MAX_OUTPUT)return;
      pinMode(outputs[id], OUTPUT);
      digitalWrite(outputs[id], msg.getInt(0) == 0 ? LOW : HIGH);
    });
    outputCounter++;
  });

  /*
     _____   __          __  __  __
    |  __ \  \ \        / / |  \/  |
    | |__) |  \ \  /\  / /  | \  / |
    |  ___/    \ \/  \/ /   | |\/| |
    | |         \  /\  /    | |  | |
    |_|          \/  \/     |_|  |_|
    -> /setup/PWM [PIN - CHANNEL] - FREQUENCE : default 1000] - RESOLUTION : default 8 ]
    <- /PWM/status id
    -> /PWM/run/id [VALUE (0, 255)]
  */
  com.onMessage("/setup/PWM", [](OSCMessage & msg) {
    if (!msg.isInt(0))return;
    uint8_t pin = msg.getInt(0);

    pwms[pwmCounter] = PWM(pin);

    String id = String(pwmCounter);

    com.send("/PWM/status", pwmCounter );
    Serial.println("/PWM/status " + id);

    com.onMessage("/PWM/run/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/PWM/run/", MAX_PWM);;
      if (id > MAX_PWM)return;
      pwms[id].setValue(msg.getInt(0));
    });
    pwmCounter++;
  });

  /*
     ___    ___   _ __  __   __   ___
    / __|  / _ \ | '__| \ \ / /  / _ \
    \__ \ |  __/ | |     \ V /  | (_) |
    |___/  \___| |_|      \_/    \___/
    -> /setup/servo [PIN1]
    <- /servo/status id
    -> /servo/pos/id [pos (0, 180)]
    15 4 16 17 5 18 19 21 22 23
  */
  com.onMessage("/setup/servo", [](OSCMessage & msg) {
    Serial.println("/setup/servo");
    if (!msg.isInt(0))return;
    Serial.println("ok");

    servos[servoCounter] = Servo(msg.getInt(0));

    com.send("/servo/status", servoCounter );

    String id = String(servoCounter);
    Serial.println("/servo/status " + id);

    com.onMessage("/servo/pos/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/servo/pos/", MAX_SERVO);
      if (id > MAX_SERVO)return;
      servos[id].setPos(msg.getInt(0));
    });
    servoCounter++;
  });

  /*
     _____     _____                       _
    |  __ \   / ____|                     | |
    | |  | | | |       _ __ ___     ___   | |_    ___    _ __
    | |  | | | |      | '_ ` _ \   / _ \  | __|  / _ \  | '__|
    | |__| | | |____  | | | | | | | (_) | | |_  | (_) | | |
    |_____/   \_____| |_| |_| |_|  \___/   \__|  \___/  |_|
    -> /setup/DCmotor [PIN1 PIN2 SPEED_PIN]
    <- /DCmotor/status id
    -> /DCmotor/speed/id [speed (0, 100)]
    -> /DCmotor/dir/id [dir  (-1 BACKWARD 0 STOP 1 FORWARD)]
  */
  com.onMessage("/setup/DCmotor", [](OSCMessage & msg) {
    if (!msg.isInt(0) || !msg.isInt(1) || !msg.isInt(2))return;
    uint8_t p1 = msg.getInt(0);
    uint8_t p2 = msg.getInt(1);
    uint8_t pSpeed = msg.getInt(2);

    dcMotors[dcMotorCounter] = DCmotor(p1, p2, pSpeed);

    String id = String(dcMotorCounter);
    com.send("/DCmotor/status", dcMotorCounter );
    Serial.println("/DCmotor/status " + id);

    com.onMessage("/DCmotor/speed/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/DCmotor/speed/", MAX_DC_MOTOR);
      if (id > MAX_DC_MOTOR)return;
      dcMotors[id].setSpeed(msg.getInt(0));
    });

    com.onMessage("/DCmotor/dir/" + id, [](OSCMessage & msg) {
      if (!msg.isInt(0))return;
      char buffer[64];
      msg.getAddress(buffer);
      uint8_t id = getIdFromOscAddr(buffer, "/DCmotor/dir/", MAX_DC_MOTOR);
      if (id > MAX_DC_MOTOR)return;

      if (msg.getInt(0) < 0)
        dcMotors[id].setDir(DCmotor::DIRECTION::BACKWARD);
      if (msg.getInt(0) == 0)
        dcMotors[id].setDir(DCmotor::DIRECTION::STOP);
      if (msg.getInt(0) > 0)
        dcMotors[id].setDir(DCmotor::DIRECTION::FORWARD);
    });

    dcMotorCounter++;
  });
}

void loop() {
  com.update();

  for (uint8_t i = 0 ; i < sensorCounter ; i ++) {
    sensors[i].run();
  }

  for (uint8_t i = 0 ; i < stepperCounter ; i ++) {
    steppers[i].run();
  }
}
