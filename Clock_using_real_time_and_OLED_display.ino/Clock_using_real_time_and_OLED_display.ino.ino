#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int pause=1000;

DS3231 clock;
RTCDateTime dt;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1// Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
Serial.begin(9600);
    clock.begin();
  // Set sketch compiling time
    clock.setDateTime(__DATE__, __TIME__);
 
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display(); //display initial Adafruit logo
    delay(2000); 

  // Clear the buffer
  display.clearDisplay(); 
  display.display();

}

String DayOfTheWeek(uint8_t Day){
  String DayText;
  if (Day==1)  DayText="Dushanba";
  if (Day==2)  DayText="Seshanba";
  if (Day==3)  DayText="Chorshanba";
  if (Day==4)  DayText="Payshanba";
  if (Day==5)  DayText="Juma";
  if (Day==6)  DayText="Shanba";
  if (Day==7)  DayText="Yakshanba";
  return DayText;
}



String DayMonthYear(uint8_t Day,uint8_t Month,uint16_t Year){
  String DayMonthYearText;
  if (Month==1)  DayMonthYearText="YAN ";
  if (Month==2)  DayMonthYearText="FEV ";
  if (Month==3)  DayMonthYearText="MAR ";
  if (Month==4)  DayMonthYearText="APR ";
  if (Month==5)  DayMonthYearText="MAY ";
  if (Month==6)  DayMonthYearText="IYUN ";
  if (Month==7)  DayMonthYearText="IYUL ";
  if (Month==8)  DayMonthYearText="AVG ";
  if (Month==9)  DayMonthYearText="SEN ";
  if (Month==10)  DayMonthYearText="OKT ";
  if (Month==11)  DayMonthYearText="NOY ";
  if (Month==12)  DayMonthYearText="DEK ";

  DayMonthYearText=DayMonthYearText+Day;
  if (Day==1)DayMonthYearText=DayMonthYearText+"-";
  if (Day==2)DayMonthYearText=DayMonthYearText+"-";
  if (Day>2)DayMonthYearText=DayMonthYearText+"-";

  DayMonthYearText=DayMonthYearText+Year;
  
  return DayMonthYearText;
}

String AddLeadingZero(uint8_t x){
  String AddLeadingZeroText;
  if(x<10) AddLeadingZeroText="0"; 
  else AddLeadingZeroText=""; 
  AddLeadingZeroText=AddLeadingZeroText+x;
  return AddLeadingZeroText;
}

String CurrentTime(uint8_t H, uint8_t I ){
  String CurrentTimeText="";
  CurrentTimeText=CurrentTimeText + AddLeadingZero(H) +":"+AddLeadingZero(I);
  return CurrentTimeText;
}

void loop() {
  dt = clock.getDateTime();

  display.fillRect(0,0,128,16,SSD1306_WHITE);
  display.fillRect(0,17,128,16,SSD1306_BLACK);
  display.fillRect(0,31,128,33,SSD1306_WHITE);

    display.setCursor(1,1); 
    display.setTextSize(2);
    display.setTextColor(SSD1306_BLACK); 
    display.println(DayOfTheWeek(dt.dayOfWeek));

  display.setCursor(1,18); 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); 
  display.println(DayMonthYear(dt.day,dt.month,dt.year));

    display.setCursor(3,35); 
    display.setTextSize(3);  
    display.setTextColor(SSD1306_BLACK); 
    display.println(CurrentTime(dt.hour,dt.minute));

  display.setCursor(100,35); 
  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK); 
  display.println(AddLeadingZero(dt.second));

  clock.forceConversion();
  display.setCursor(85,18); 
  display.setTextSize(1);
display.setTextColor(SSD1306_WHITE); 
display.print(clock.readTemperature());
display.setCursor(117,16); 
display.print("o");


  display.display();
  delay(1000);
}  