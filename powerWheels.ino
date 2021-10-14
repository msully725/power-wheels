// throttle constants
const float minThrottleVolt = 0.95;
const float maxThrottleVolt = 3.5;
const float minStartingThrottlePercent = 0.1;

// shifter states
const int ShiftStateHigh = 2;
const int ShiftStateLow = 1;
const int ShiftStateReverse = 0;

// current states
float currentThrottlePercent = 0.0;
int currentShifterState = 0;

// pin assignments
int throttleInputPin = A5;

int shifter1InputPin = 2;
int shifter2InputPin = 4;

int forwardLeftMotorPWMPin = 11;
int forwardRightMotorPWMPin = 9;
int reverseLeftMotorPWMPin = 10;
int reverseRightMotorPWMPin = 3;

void setup() {
  Serial.begin(9600);

  pinMode(shifter1InputPin, INPUT_PULLUP);
  pinMode(shifter2InputPin, INPUT_PULLUP);
}

void loop() {
  runThrottleReadIteration();
  runShifterReadIteration();
  runMotorSignalIteration();
    
  delay(25);
}

void runThrottleReadIteration() 
{
  float analogVoltageReading = analogIntToVolt(analogRead(throttleInputPin));
  float throttlePercent = calculateThrottlePercent(analogVoltageReading);

  currentThrottlePercent = throttlePercent;
  
  String message = "Analog Throttle Reading: ";
  message += analogVoltageReading;
  message += "V, ";
  message += throttlePercent;
  message += "%";
  Serial.println(message);
}

float calculateThrottlePercent(float throttleVoltage)
{
  if (throttleVoltage < minThrottleVolt)
    return 0.0;

  if (throttleVoltage > maxThrottleVolt)
    return 1.0;

  float adjustedThrottleVoltage = throttleVoltage - minThrottleVolt;
  float adjustedMaxThrottleVolt = maxThrottleVolt - minThrottleVolt;
  
  return adjustedThrottleVoltage / adjustedMaxThrottleVolt; 
}

void runShifterReadIteration()
{
  int shifterPin1Value = digitalRead(shifter1InputPin);
  int shifterPin2Value = digitalRead(shifter2InputPin);

  currentShifterState = shifterPin1Value + shifterPin2Value;

  String message = "Shifter State: ";
  message += currentShifterState;
  message += ", "; 

  switch(currentShifterState)
  {
    case ShiftStateReverse:
      message += "reverse";
      break;
    case ShiftStateLow:
      message += "low";
      break;
    case ShiftStateHigh:
      message += "high";
      break;
  }

  Serial.println(message); 
}

void runMotorSignalIteration()
{
  float adjustedCurrentThrottlePercent = currentThrottlePercent;
  if (currentThrottlePercent > 0.01 && currentThrottlePercent < minStartingThrottlePercent)
    adjustedCurrentThrottlePercent = minStartingThrottlePercent;
  
  int maxPwm = 255;
  int throttledPwm = maxPwm * adjustedCurrentThrottlePercent;

  
  int forwardPwm = 0;
  int reversePwm = 0;
  if (currentShifterState == ShiftStateReverse)
  {
    reversePwm = throttledPwm;
  }
  else
  {
    forwardPwm = throttledPwm;
  }

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

float analogIntToVolt(int analogIn)
{
  return analogIn * (5.0 / 1023.0);
}
