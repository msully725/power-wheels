void setup() {
  Serial.begin(9600);
}

int accumulator = 0;
int throttlePin = A0;
int motorPWMPin = 9;
float minThrottleVolt = 1.0;
float maxThrottleVolt = 4.0;
float currentThrottlePercent = 0.0;
float minStartingThrottlePercent = 0.1;

void loop() {
  accumulator++;

  runThrottleReadIteration();
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
  float analogVoltageReading = analogIntToVolt(analogRead(throttlePin));
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
  
  analogWrite(motorPWMPin, throttledPwm);
}

float analogIntToVolt(int analogIn)
{
  return analogIn * (5.0 / 1023.0);
}
