// Importing Libraries //
#include <Wire.h>

// ... for BH1750 // 
#include <BH1750.h>
BH1750 lightMeter(0x23);

// ... for LCD Screen // 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20, 16, 2);

// ... for Button // 
#include <ezButton.h>
ezButton button(7);

// Pin Definitions // -- Subject to Change
byte analogPin = A0; // Analog Pin
int mosfetPin = 11; // PWM Pin

// Function Prototypes // 
float checkLight();
void adjustLight(float);
void checkBattery();

unsigned long buttonCount;

// ************************************************************

// Main (Setup) Function // 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  button.setDebounceTime(50); // 'cooldown time'

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.println("Welcome!            ");
  lcd.setCursor(0, 1);
  lcd.println("Loading...          ");
}

// ************************************************************

// Main (Loop) Function // 
void loop() {
  button.loop();

  if (button.isPressed())
    buttonCount = buttonCount + 1;
  
  lcd.setCursor(0, 0);

  if((buttonCount % 2) != 0)  {     // Manual Mode
    adjustLight(1000);              // Turns OFF
    lcd.print("MODE: Manual Off     ");
  } else  {                     // Automatic Mode (Default)
    float lux = checkLight(); 
    adjustLight(lux);    
    lcd.print("MODE: Automatic     ");
  }
  
  checkBattery();

}

// Definition of 'checkLight' Function //
float checkLight() {
  float lux  = lightMeter.readLightLevel(); // Reading light level (in lux)
  Serial.println(lux);
  return lux;
}

// Definition of 'adjustLight' Function // 
void adjustLight(float lux) {
  if (lux < 50)                     // Setting: Dark
    analogWrite(mosfetPin, 255);    // Light: HIGH (100% of light) -- 0 - 50
  else if (lux < 90 && lux >= 50)   // Setting: Dim
    analogWrite(mosfetPin, 170);    // Light: MEDIUM (67% of light) -- 50 - 90
  else if (lux < 120 && lux >= 90)  // Setting: Lit
    analogWrite(mosfetPin, 85);     // Light: LOW (33% of light) -- 90 - 120
  else                              // Setting: Bright
    analogWrite(mosfetPin, 0);      // Light: OFF (0% of light) -- 120+
}

// Definition of 'checkBattery' Function // 
void checkBattery() {
  float value = analogRead(analogPin); 
  int voltage = map((value * 3), 0, 1023, 0, 1110); // 11.1V Battery
  
  lcd.setCursor(0, 1);  
  if (voltage < 7000)               // Less than 7.0V
    lcd.print("LOW BATTERY!     ");
  else 
    lcd.print("Sufficient Power ");
}
