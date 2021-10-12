int accumulator = 0;
float minThrottleVolt = 1.0;
float maxThrottleVolt = 4.0;
float minStartingThrottlePercent = 0.1;

// shifter states
int shiftStateLow = 0;
int shiftStateHigh = 1;
int shiftStateReverse = 2;

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
  String beginning = "Analog In 0 Reading #";
  String join = ": ";
  String comma = ", ";
  String voltEnd = "V";
  String percentEnd = "%";
  String message = "";
  float analogVoltageReading = analogIntToVolt(analogRead(throttleInputPin));
  float throttlePercent = calculateThrottlePercent(analogVoltageReading);

  currentThrottlePercent = throttlePercent;
  
  message = beginning 
    + accumulator 
    + join 
    + analogVoltageReading 
    + voltEnd
    + comma
    + throttlePercent * 100
    + percentEnd;
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

void runMotorSignalIteration()
{
  float adjustedCurrentThrottlePercent = currentThrottlePercent;
  if (currentThrottlePercent > 0.01 && currentThrottlePercent < minStartingThrottlePercent)
    adjustedCurrentThrottlePercent = minStartingThrottlePercent;
  
  int maxPwm = 255;
  int throttledPwm = maxPwm * adjustedCurrentThrottlePercent;
  String beginning = "Sending ";
  float percentPwm = throttledPwm / 255.0 * 100.0;
  String percentEnding = "% PWM";
  String comma = ", ";
  String intPwmEnding = " int PWM (0 - 255)";
  String message = "";
  message = beginning
    + percentPwm 
    + percentEnding 
    + comma 
    + throttledPwm 
    + intPwmEnding; 
  Serial.println(message);
  
  analogWrite(forwardLeftMotorPWMPin, throttledPwm);
  analogWrite(forwardRightMotorPWMPin, throttledPwm);
}

float analogIntToVolt(int analogIn)
{
  return analogIn * (5.0 / 1023.0);
}

void runShifterReadIteration()
{
  int shifterPin1Value = digitalRead(shifter1InputPin);
  int shifterPin2Value = digitalRead(shifter2InputPin);

  String message = "shifterPin1: ";
  message += shifterPin1Value; 
  message += ", shifterPin2: ";
  message += shifterPin2Value;

  Serial.println(message); 
}
