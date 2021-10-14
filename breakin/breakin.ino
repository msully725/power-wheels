const int loopDelayMs = 10000;
const int maxPwm = 255;

const float breakInThrottlePercent = .3;

const int forwardLeftMotorPWMPin = 11;
const int forwardRightMotorPWMPin = 9;
const int reverseLeftMotorPWMPin = 10;
const int reverseRightMotorPWMPin = 3;

void setup() {
  Serial.begin(9600);
}

void loop() {
  runMotorBreakinSignalIteration();
    
  delay(loopDelayMs);
}

void runMotorBreakinSignalIteration()
{
  int throttledPwm = maxPwm * breakInThrottlePercent;
  
  int forwardPwm = throttledPwm;
  int reversePwm = 0;

  float percentPwm = throttledPwm / 255.0 * 100.0;
  String message = "Sending ";
  message += percentPwm;
  message += "% PWM, ";
  message += throttledPwm;
  message += " int PWM (0 - 255)";
  message += "\nforwardPWM: ";
  message += forwardPwm;
  message += ", reversePWM: ";
  message += reversePwm;
  Serial.println(message);

  if (forwardPwm > 0 && reversePwm > 0) return;
  
  analogWrite(forwardLeftMotorPWMPin, forwardPwm);
  analogWrite(forwardRightMotorPWMPin, forwardPwm);
  analogWrite(reverseLeftMotorPWMPin, reversePwm);
  analogWrite(reverseRightMotorPWMPin, reversePwm);
}
