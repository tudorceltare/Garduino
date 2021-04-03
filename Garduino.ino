
#include <Wire.h> 
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <dht11.h>

#define DHT11PIN 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
  
dht11 DHT11;
//LiquidCrystal_I2C lcd(0x27,20,4);

// variables
float humi;
float tempC;
float tempF;
int light;
int soilHumidity;
int mode = 0;
int buttonState = 0;
int lastButtonState = 0;
int lightSensor = 0;

// pins
int buttonPin = 8;
int temperatureSensorPin = A0;
int lightSensorPin = A1;
int relayPin = 3;
int pumpButtonPin = 7;

int updateValuesTime = 0;
int updateValuesTimePump = 0;

void setup() {
  Serial.begin(9600);
  // oled
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    } // obligatoriu!! altfel nu merge ecranul
  display.clearDisplay();
  display.display();
  
  // button
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  // set up the LCD's number of columns and rows:
//  lcd.init();
//  lcd.backlight();
  // Print a message to the LCD.
//  lcd.print("Cycle w/ button");


  
  tempC = 0.0f;
  humi = 0.0f;
  light = 0;
  soilHumidity = 0;
  
  mode = 0;
  
}

//void lcdPrintText(int mode){
//  switch(mode){
//    case 1: // temperature
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Teperature:");
//      break;
//    
//    case 2: // humidity
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Air Humidity:");
//      break;
//
//    case 3: // humidity
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Soil Humidity:");
//      break;
//    
//    case 4: // light
//      // light will be written in percentages
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Light:");
//      break;
//    
//    default:
//      lcd.clear();
//      lcd.setCursor(0, 0);
//        lcd.print("Cycle w/ button");
//      break;
//    
//  }
//}


void oledPrintText(int mode){
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  // obligatoriu!! pt text
  int i = 0;
  char source[] = "Loading";
    switch(mode){
    
    case 1: // air humidity and temperature
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Humidity");
      display.setCursor(60, 0);
      display.print("Temperature");
      break;

    case 2: // soil humidity
      display.clearDisplay();
      display.println("Soil Humidity:");
      break;
    
    case 3: // light
      // light will be written in percentages
      display.clearDisplay();
      display.println("Light:");
      break;
    
    default:
//      int i,j = 0;
//      char source[] = "Loading...";
//      for(j = 0; j< 3; j++){
//        for(i = 0; i < strlen(source); i++){
//          display.write(source[i]);
//          display.display();
//          delay(30);
//        }  
//      }
      display.print("Cycle with the button");
      break;
    }
}

//void lcdPrintValues(int mode){
//  switch(mode){
//    case 0:
//      break;
//    
//    case 1: // temperature
//      lcd.setCursor(0, 1);
//      lcd.print("      ");
//      lcd.setCursor(0, 1);
//      lcd.print(temperature);
//      lcd.print("C");
//      break;
//    
//    case 2: // humidity
//      lcd.setCursor(0, 1);
//      lcd.print("      ");
//      lcd.setCursor(0, 1);
//      lcd.print(humidity);
//      lcd.print("%");
//      break;
//
//    case 3: // humidity
//      lcd.setCursor(0, 1);
//      lcd.print("      ");
//      lcd.setCursor(0, 1);
//      lcd.print(soilHumidity);
//      lcd.print("%");
//      break;
//    
//    case 4: // light
//      // light will be written in percentages
//      lcd.setCursor(0, 1);
//      lcd.print("      ");
//      lcd.setCursor(0, 1);
//      lcd.print(light);
//      lcd.print("%");
//      break;
//    
//    default:
//      lcd.clear();
//      lcd.setCursor(0, 0);
//        lcd.print("values error");
//      break;
//  }
//}

void oledPrintValues(int mode){
  Serial.println("Enters function oledPrintValues");
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 1);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  // obligatoriu!! pt text
  
  switch(mode){
    case 0:
      break;
    
    case 1: // air humidity
      oledDisplay(3,5,28,humi,"%");
      oledDisplay(2,70,16,tempC,"C");
      oledDisplay(2,70,44,tempF,"F");
      break;

    case 2: // soil humidity
      oledDisplay(3,5,28,soilHumidity,"%");
      break;
    
    case 3: // light
      // light will be written in percentages
      oledDisplay(3,5,28,light,"%");
      break;
    
    default:
      display.print("values error");
      break;
  }
}

void controlPump(){
  digitalWrite(relayPin, LOW);
  delay(1000);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if((buttonState != lastButtonState) && (buttonState == HIGH)){
    mode++;
    if(mode >= 4)
      mode = 1;
    Serial.print("mode: ");
    Serial.println(mode);
    delay(50);
  }
  lastButtonState = buttonState;
  

// Reads values from sensors
  //reads temperature
  DHT11.read(DHT11PIN);
  tempC = (float)DHT11.temperature;
  tempF = tempC * 1.8 + 32;

  //reads humidity
  humi = (float)DHT11.humidity;

  //read soil humidity
  soilHumidity = analogRead(A0);
  soilHumidity = map(soilHumidity,1024,0,0,100);
  
  //reads light
  lightSensor = analogRead(lightSensorPin);
  light = map(lightSensor, 1024, 0, 0, 100);

// Display Everything
  if (updateValuesTime == 10){
    display.clearDisplay();
    oledPrintText(mode);
    oledPrintValues(mode);
    
    updateValuesTime = 0;
  }
  else
    updateValuesTime++;
    
  display.display();

// Pump control Automatic, watering in 9 minutes cycle
  if (updateValuesTimePump == 10000){
    if(soilHumidity < 50){
      controlPump();
    }
    updateValuesTimePump = 0;
  }
  else
    updateValuesTimePump++;

// Pump control Manual
  if(digitalRead(pumpButtonPin) == HIGH){
    controlPump();
  }
  return;
}

void oledDisplay(int size, int x,int y, float value, String unit){
 int charLen=12;
 int xo=x+charLen*3.2;
 int xunit=x+charLen*3.6;
 int xval = x; 
 display.setTextSize(size);
 display.setTextColor(WHITE);
 
 if (unit=="%"){
   display.setCursor(x, y);
   display.print(value,0);
   display.print(unit);
 } else {
   if (value>99){
    xval=x;
   } else {
    xval=x+charLen;
   }
   display.setCursor(xval, y);
   display.print(value,0);
   display.drawCircle(xo, y+2, 2, WHITE);  // print degree symbols (  )
   display.setCursor(xunit, y);
   display.print(unit);
 }
}
