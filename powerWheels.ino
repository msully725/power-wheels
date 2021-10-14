const int loopDelayMs = 25;

// throttle constants
const int MaxPwm = 255;
const float MinThrottleVolt = 0.95;
const float MaxThrottleVolt = 3.5;
const float MaxThrottlePercent = 1.00;
const int StallPreventionMinThrottlePwmPercent = 0.1;
const int ThrottleSmoothingBinLength = 10;
int ThrottleSmoothingBins[ThrottleSmoothingBinLength];
int throttleSmoothingIndex = 0;
int stallPreventionMinThrottlePwm = 0;

// shifter constants
const int ShiftStateHigh = 2;
const int ShiftStateLow = 1;
const int ShiftStateReverse = 0;
const float LowMaxThrottlePercent = 0.66;

// current states
float currentThrottlePercent = 0.0;
int currentShifterState = 0;

// pin assignments
const int ThrottleInputPin = A5;

const int Shifter1InputPin = 2;
const int Shifter2InputPin = 4;

const int ForwardLeftMotorPWMPin = 11;
const int ForwardRightMotorPWMPin = 9;
const int ReverseLeftMotorPWMPin = 10;
const int ReverseRightMotorPWMPin = 3;

void setup() {
  Serial.begin(9600);

  pinMode(Shifter1InputPin, INPUT_PULLUP);
  pinMode(Shifter2InputPin, INPUT_PULLUP);

  initializeThrottleSmoothingBins();
  stallPreventionMinThrottlePwm = MaxPwm * StallPreventionMinThrottlePwmPercent;
}

void loop() {
  runThrottleReadIteration();
  runShifterReadIteration();
  runMotorSignalIteration();
    
  delay(loopDelayMs);
}

void runThrottleReadIteration() 
{
  float analogVoltageReading = analogIntToVolt(analogRead(ThrottleInputPin));
  float throttlePercent = calculateThrottlePercent(analogVoltageReading);

  currentThrottlePercent = throttlePercent * MaxThrottlePercent;
  
  String message = "Analog Throttle Reading: ";
  message += analogVoltageReading;
  message += "V, ";
  message += throttlePercent;
  message += "%";
  Serial.println(message);
}

float calculateThrottlePercent(float throttleVoltage)
{
  if (throttleVoltage < MinThrottleVolt)
    return 0.0;

  if (throttleVoltage > MaxThrottleVolt)
    return 1.0;

  float adjustedThrottleVoltage = throttleVoltage - MinThrottleVolt;
  float adjustedMaxThrottleVolt = MaxThrottleVolt - MinThrottleVolt;
  
  return adjustedThrottleVoltage / adjustedMaxThrottleVolt; 
}

void runShifterReadIteration()
{
  int shifterPin1Value = digitalRead(Shifter1InputPin);
  int shifterPin2Value = digitalRead(Shifter2InputPin);

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
  int throttledPwm = MaxPwm * currentThrottlePercent;

  if (throttledPwm > 0 && throttledPwm < stallPreventionMinThrottlePwm)
    throttledPwm = stallPreventionMinThrottlePwm;
    
  throttledPwm = calculatePwmWithShifterState(throttledPwm);
  int smoothedThrottledPwm = smoothNextThrottlePwm(throttledPwm);

  int forwardPwm = smoothedThrottledPwm > 0 ? smoothedThrottledPwm : 0;
  int reversePwm = smoothedThrottledPwm < 0 ? smoothedThrottledPwm * -1 : 0;

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

  // safety to avoid forward and reverse being active simultaneously.
  if (forwardPwm > 0 && reversePwm > 0)
    return;
  
  analogWrite(ForwardLeftMotorPWMPin, forwardPwm);
  analogWrite(ForwardRightMotorPWMPin, forwardPwm);
  analogWrite(ReverseLeftMotorPWMPin, reversePwm);
  analogWrite(ReverseRightMotorPWMPin, reversePwm);
}

int calculatePwmWithShifterState(int throttledPwm)
{
  int calculatedPwm = 0;
  if (currentShifterState == ShiftStateReverse)
  {
    calculatedPwm = -1 * throttledPwm * LowMaxThrottlePercent;
  }
  else if (currentShifterState == ShiftStateLow)
  {
    calculatedPwm = throttledPwm * LowMaxThrottlePercent;
  } 
  else 
  {
    calculatedPwm = throttledPwm;
  }

  return calculatedPwm;
}

int smoothNextThrottlePwm(int currentThrottlePwm) 
{
  int smoothedThrottlePwm = calculateSmoothedThrottlePwm();
  
  throttleSmoothingIndex = (throttleSmoothingIndex + 1) % ThrottleSmoothingBinLength;
  String message = "throttleSmoothingIndex: ";
  ThrottleSmoothingBins[throttleSmoothingIndex] = currentThrottlePwm;

  return smoothedThrottlePwm;
}

int calculateSmoothedThrottlePwm()
{
  int sum = 0;
  for (int i = 0; i < ThrottleSmoothingBinLength; i++)
  {
    int currentIndex = (throttleSmoothingIndex + i) % ThrottleSmoothingBinLength;
    sum += ThrottleSmoothingBins[currentIndex];
  }

  int calculatedSmoothedThrottlePwm = sum / ThrottleSmoothingBinLength;
  return calculatedSmoothedThrottlePwm;
}

void initializeThrottleSmoothingBins()
{
  for (int i = 0; i < ThrottleSmoothingBinLength; i++)
  {
    ThrottleSmoothingBins[i] = 0;
  }
}

float analogIntToVolt(int analogIn)
{
  return analogIn * (5.0 / 1023.0);
}
