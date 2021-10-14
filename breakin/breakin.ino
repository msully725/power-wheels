const int loopDelayMs = 25;

const float breakInThrottlePercent = 30;

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
  float adjustedCurrentThrottlePercent = currentThrottlePercent;
  if (currentThrottlePercent > 0.01 && currentThrottlePercent < minStartingThrottlePercent)
    adjustedCurrentThrottlePercent = minStartingThrottlePercent;
  
  int maxPwm = 255;
  int throttledPwm = maxPwm * breakInThrottlePercent;
  
  int forwardPwm = 0;
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
  
  analogWrite(forwardLeftMotorPWMPin, forwardPwm);
  analogWrite(forwardRightMotorPWMPin, forwardPwm);
  analogWrite(reverseLeftMotorPWMPin, reversePwm);
  analogWrite(reverseRightMotorPWMPin, reversePwm);
}
