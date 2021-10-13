int accumulator = 0;
float minThrottleVolt = 1.0;
float maxThrottleVolt = 4.0;
float minStartingThrottlePercent = 0.1;

// shifter states
const int ShiftStateHigh = 0;
const int ShiftStateLow = 1;
const int ShiftStateReverse = 2;

// current states
float currentThrottlePercent = 0.0;
int currentShifterState = 0;

// pin assignments
int throttleInputPin = A0;

int shifter1InputPin = 2;
int shifter2InputPin = 4;

int forwardLeftMotorPWMPin = 11;
int forwardRightMotorPWMPin = 6;
int reverseLeftMotorPWMPin = 10;
int reverseRightMotorPWMPin = 5;

void setup() {
  Serial.begin(9600);

  pinMode(shifter1InputPin, INPUT_PULLUP);
  pinMode(shifter2InputPin, INPUT_PULLUP);
}

void loop() {
  accumulator++;

  runThrottleReadIteration();
  runShifterReadIteration();
  runMotorSignalIteration();
    
  delay(250);
}

void runThrottleReadIteration() 
{
  float analogVoltageReading = analogIntToVolt(analogRead(throttleInputPin));
  float throttlePercent = calculateThrottlePercent(analogVoltageReading);

  currentThrottlePercent = throttlePercent;
  
  String message = "Analog In 0 Reading #";
  message += accumulator + ": ";
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

  String message = "currentShifterState: " + currentShifterState;
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

  float percentPwm = throttledPwm / 255.0 * 100.0;
  String message = "Sending ";
  message += percentPwm;
  message += "% PWM, ";
  message += throttledPwm + " int PWM (0 - 255)";
  Serial.println(message);
  
  analogWrite(forwardLeftMotorPWMPin, throttledPwm);
  analogWrite(forwardRightMotorPWMPin, throttledPwm);
  analogWrite(reverseLeftMotorPWMPin, 0);
  analogWrite(reverseRightMotorPWMPin, 0);
}

float analogIntToVolt(int analogIn)
{
  return analogIn * (5.0 / 1023.0);
}