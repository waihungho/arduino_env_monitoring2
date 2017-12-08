// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Arduino Software Version v1.6.12
// Arduino Board UNO
//
// Sensors:
//  1. Data logging shield (SD Card + Clock)
//  2. PMS7003 (PM2.5 / PM1.0 / PM10.0)
//  3. BME280
//  4. TFT Screen
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Program Specification
//    1. setup()
//        initialize all sensors SD Card, Clock, PMS7003, BME280, TFT Screen )
//    2. loop()
//        a. Read data from BME280 (for temperature, humidity, pressure)
//        b. Read data from PMS7003 (for PM2.5, PM1.0, PM10.0)
//        c. Display data on Screen    [displayDataOnScreen()]
//        d. Write to SD Card
//    Problem Solving History:
//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------
//  Software Configuration (BEGIN)
// ------------------------------------------
#define SERIAL_BAUD_RATE 9600
#define WELCOME_MSG_FIRST_LINE "Amber Caffe Lab."
 
#define READING_SENSOR_INTERVAL 800     // Interval to read ALL sensors. 10s interval.
#define ENABLE_SD_CARD 1                 // 1: Enable SD Card, 0; Disable.
#define SD_CARD_FILENAME "MONITOR.TXT"    // Filename "FAT" 8.3 format (XXXXXXXX.XXX) You must use the 8.3 format, so that file names look like AME001.EXT, where AME001 is an 8 character or fewer string, and is a 3 character extension.
// SD Card must be formatted as the "FAT" filesystem first.
// ------------------------------------------
//  Software Configuration (END)
// ------------------------------------------


// ------------------------------------------
//  PIN Configuration (BEGIN)
// ------------------------------------------
// PMS7003 (PM2.5 / PM1.0 / PM10.0) PIN:
#define PMS7003_TXD_PIN A15
#define PMS7003_RXD_PIN A14
//  PMS7003     Arduino(UNO)
//  TXD         Pin A15 (PMS7003_TXD_PIN)
//  RXD         Pin A14 (PMS7003_RXD_PIN)

// BME280 PIN:
//  BME280     Arduino(UNO)
//  SDA         SDA (IIC)
//  SCL         SCL (IIC)

// Clock PIN:
// Data logging shield    Arduino
// SDA                    20
// SCL                    21
// Remark: SDA and SCL pin is different from normal, it locates in "5V GND SDA SCL".
              
// SD Card PIN:
#define SD_CARD_CS_PIN 10  // CS
#define SD_CARD_MOSI_PIN 11
#define SD_CARD_MISO_PIN 12
#define SD_CARD_CLK_PIN 13
//  SD Card Module     Arduino(UNO)
//  CS                 PIN10
//  MOSI               PIN11
//  MISO               PIN12
//  CLK                PIN13

// TFT PIN:     Arduino
// VCC  VCC     VCC  VCC
// 22   23      22   23
// 24   25      24   25
// 26   27      26   27
// ...  ..      ...  ..
// 52   53      52   53
// GND  GND     GND  GND
// Another Side Pin
// VCC          VCC   
// GND          GND

//      3.3V  RESET
// GND  EMPTY
//53  52
//51
//49
//47
//45
//43
//41
//39
//37
//35
//33
//31
//29
//27
//25
//23
//5V  5V


// ------------------------------------------
//  PIN Configuration (END)
// ------------------------------------------


// BME280 Configuration (BEGIN)
#include "cactus_io_BME280_I2C.h"
BME280_I2C bme280(0x76);  // I2C using address 0x76
float temperatureC = 0;
float temperatureF = 0;
float humidityPer = 0;
float pressure = 0;
boolean isSuccesToInitBME = false;  // true: Succes to initialize BME280.
// Pin: SDA, SCL
// BME280 Configuration (END)



// PMS7003 Configuration (BEGIN)
//https://github.com/MartyMacGyver/PMS7003-on-Particle/blob/master/pms7003-photon-demo-1/pms7003-photon-demo-1.ino
//https://www.taiwaniot.com.tw/shop/module-sensor/%E7%A9%BA%E6%B0%A3%E7%B2%89%E5%A1%B5-pm2-5/%E6%94%80%E8%97%A4-g7-pms7003-pm1-0-pm2-5-pm10-%E7%B2%89%E5%A1%B5%E6%BF%83%E5%BA%A6%E6%84%9F%E6%B8%AC%E5%99%A8/
#include <SoftwareSerial.h>
#define START_1 0x42
#define START_2 0x4d
//-----------------------
#define DATA_LENGTH_H        0
#define DATA_LENGTH_L        1
#define PM1_0_CF1_H          2
#define PM1_0_CF1_L          3
#define PM2_5_CF1_H          4
#define PM2_5_CF1_L          5
#define PM10_CF1_H           6
#define PM10_CF1_L           7
#define PM1_0_ATMOSPHERE_H   8
#define PM1_0_ATMOSPHERE_L   9
#define PM2_5_ATMOSPHERE_H   10
#define PM2_5_ATMOSPHERE_L   11
#define PM10_ATMOSPHERE_H    12
#define PM10_ATMOSPHERE_L    13
#define UM0_3_H              14
#define UM0_3_L              15
#define UM0_5_H              16
#define UM0_5_L              17
#define UM1_0_H              18
#define UM1_0_L              19
#define UM2_5_H              20
#define UM2_5_L              21
#define UM5_0_H              22
#define UM5_0_L              23
#define UM10_H               24
#define UM10_L               25
#define VERSION              26
#define ERROR_CODE           27
#define CHECKSUM             29
unsigned char pms7003ChrRecv;
byte bytCount = 0;
SoftwareSerial myPMS7003(PMS7003_TXD_PIN, PMS7003_RXD_PIN);

unsigned int pm_1 = 0;
unsigned int pm_2_5 = 0;
unsigned int pm_10 = 0;
// PMS7003 Configuration (END)


// Clock Configuration (BEGIN)
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
int isClockOk = true;
// Clock Configuration (END)


// SD Card Module Configuration (BEGIN)
//#include <SPI.h>
//#include <SD.h>

#include <SPI.h>
#include "SdFat.h"
//SdFat sd;
//SdFile sdCardFile;
SdFatSoftSpi<SD_CARD_MISO_PIN, SD_CARD_MOSI_PIN, SD_CARD_CLK_PIN> SD;
File sdCardFile;
//#include <SD.h>

boolean isSuccesToInitSD = false;  // true: Succes to initialize SD Card.
#define SD_FILE_DATA_HEADER "YYYY/MM/DD HH:MN:SS\tTEMP\tHUM\t\tBARO\tPM2.5\tPM10\tPM1.0"
// SD Card Module Configuration (END)

// TFT Display Configuration (BEGIN)
#include <TFT_HX8357.h> // Hardware-specific library
TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library
#define TFT_GREY 0x5AEB
float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = 120, osy = 120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update
int old_analog =  -999; // Value last displayed
int old_digital = -999; // Value last displayed
int value[6] = {0, 0, 0, 0, 0, 0};
int old_value[6] = { -1, -1, -1, -1, -1, -1};
int old_text_value[6] = { -1, -1, -1, -1, -1, -1};
int d = 0;
// TFT Display Configuration (END)



// Software variables(BEGIN)
#define FIRST_LOOP_DELAY 2000  // delay seconds to wait for PMS3003 sensor on first-loop.
boolean firstLoop = true;
boolean writeSDHeader = false;
#define TIME_STR_SIZE 20
#define TEMPERATURE_STR_SIZE 5
#define HUMIDITY_STR_SIZE 5
#define PRESSURE_STR_SIZE 5
char timeStr[TIME_STR_SIZE];
char temperatureStr[TEMPERATURE_STR_SIZE];
char humidityStr[HUMIDITY_STR_SIZE];
char pressureStr[PRESSURE_STR_SIZE];
// Software variables (BEGIN)

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                    FUNCTIONS
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Serial.println("Setup[TFT]");
  initScreen();

  Serial.println("Setup[BME]");
  initBME280();

  Serial.println("Setup[PMS7003]");
  initPMS7003();

  Serial.println("Setup[Clock]");
  initClock();

  Serial.println("Setup[SD]");
  initSDCard();

 //writeClock();

  firstLoop = true;
  Serial.println("Setup[Done]");
}


void loop() { // put your main code here, to run repeatedly:

  pm_1 = 0;
  pm_2_5 = 0;
  pm_10 = 0;
  temperatureC = 0;
  temperatureF = 0;
  humidityPer = 0;
  pressure = 0;

  Serial.println("Reading PMS7003");
  readPMS7003();

  Serial.println("Reading BME");
  if ( isSuccesToInitBME ) {
    readBME280();
  }
  
  if ( !firstLoop){ // 2016.11.11: Rule of Thumb: dont log first data, which it is often inaccurate.
    displayDataOnScreen();
    writeToSDCard();
  }
  

  delay(READING_SENSOR_INTERVAL);
  firstLoop = false;
  //initScreen();
}


void initScreen() {
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);
  //analogMeter(); // Draw analogue meter
  analogMeter_V2(0, "%HUM");
  analogMeter_V2(240, "TEMP");
  byte d = 55;
  int w = 50;
  plotLinear("PM 1 ", 0, 160, w);
  plotLinear("PM 2.5", 1 * d, 160, w);
  plotLinear("PM 10", 2 * d, 160, w);

}

void initBME280() {
  if (!bme280.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
   // while (1);
  } else {
    isSuccesToInitBME = true;
  }
  bme280.setTempCal(-1);
}

void initPMS7003() {
  myPMS7003.begin(9600);
}

void initClock() {
  tmElements_t clockTime;
  if (!RTC.read(clockTime)) {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
     
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
      isClockOk = false;
    }
  }
}



tmElements_t writeClockTime;
bool getTime4WriteClock(const char *str) {
  int Hour, Min, Sec;
  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  writeClockTime.Hour = Hour;
  writeClockTime.Minute = Min;
  writeClockTime.Second = Sec;
  return true;
}
bool getDate4WriteClock(const char *str) {
  const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  writeClockTime.Day = Day;
  writeClockTime.Month = monthIndex + 1;
  writeClockTime.Year = CalendarYrToTm(Year);
  return true;
}
void writeClock() {
  bool parse = false;
  bool config = false;

  // get the date and time the compiler was run
  if (getDate4WriteClock(__DATE__) && getTime4WriteClock(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(writeClockTime)) {
      config = true;
    }
  }
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}


void initSDCard() {
  if ( ENABLE_SD_CARD ) {
    pinMode(SD_CARD_CS_PIN, OUTPUT);
    //if (!sd.begin(SD_CARD_CS_PIN, SPI_HALF_SPEED)) {
    if (!SD.begin(SD_CARD_CS_PIN)) {
      //sd.initErrorHalt(); //SdFat specified
      isSuccesToInitSD = false;
      Serial.println("SD Card initialization failed!");
      return;
    } else {
      isSuccesToInitSD = true;
      Serial.println("SD OK");
    }
  }
}


//char displayHumOrig[15], displayTempOrig[15], displayPressureOrig[15], displayPm25Orig[15], displayTimeOrig[TIME_STR_SIZE];

void displayDataOnScreen() {
  Serial.println("Display On Screen");

  Serial.print("Time:");
  tmElements_t clockTime = readClock();
  print2digits(clockTime.Hour);
  Serial.write(':');
  print2digits(clockTime.Minute);
  Serial.write(':');
  print2digits(clockTime.Second);
  Serial.print(", Date (D/M/Y) = ");
  Serial.print(clockTime.Day);
  Serial.write('/');
  Serial.print(clockTime.Month);
  Serial.write('/');
  Serial.println(tmYearToCalendar(clockTime.Year));

  Serial.print("PM1.0:");
  Serial.println(pm_1);
  Serial.print("PM2.5:");
  Serial.println(pm_2_5);
  Serial.print("PM10.0:");
  Serial.println(pm_10);

  Serial.print("Temperature:");
  Serial.println(temperatureC);

  Serial.print("Humidity:");
  Serial.println(humidityPer);

  Serial.print("Pressure:");
  Serial.println(pressure);


  d += 4; if (d >= 360) d = 0;

  //value[0] = map(analogRead(A0), 0, 1023, 0, 100); // Test with value form Analogue 0
  //value[1] = map(analogRead(A1), 0, 1023, 0, 100); // Test with value form Analogue 1
  //value[2] = map(analogRead(A2), 0, 1023, 0, 100); // Test with value form Analogue 2
  //value[3] = map(analogRead(A3), 0, 1023, 0, 100); // Test with value form Analogue 3
  //value[4] = map(analogRead(A4), 0, 1023, 0, 100); // Test with value form Analogue 4
  //value[5] = map(analogRead(A5), 0, 1023, 0, 100); // Test with value form Analogue 5

  // Create a Sine wave for testing
  value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);
  value[1] = 50 + 50 * sin((d + 60) * 0.0174532925);
  value[2] = 50 + 50 * sin((d + 120) * 0.0174532925);
  value[3] = 50 + 50 * sin((d + 180) * 0.0174532925);
  value[4] = 50 + 50 * sin((d + 240) * 0.0174532925);
  value[5] = 50 + 50 * sin((d + 300) * 0.0174532925);

  unsigned int DISPLAY_STR_SIZE=15;
  char displayHum[DISPLAY_STR_SIZE], displayTemp[DISPLAY_STR_SIZE], displayPressure[DISPLAY_STR_SIZE], displayPm25[DISPLAY_STR_SIZE], displayTime[TIME_STR_SIZE];
  unsigned int PM25_STR_SIZE = 4;
  char pm2_5Str[PM25_STR_SIZE];
  memset(temperatureStr, ' ', TEMPERATURE_STR_SIZE);
  memset(humidityStr, ' ', HUMIDITY_STR_SIZE);
  memset(pressureStr, ' ', PRESSURE_STR_SIZE);
  memset(pm2_5Str, ' ', PM25_STR_SIZE);
  dtostrf(temperatureC, 2, 1, temperatureStr);  //format the display string (2 digits, 1 dp)  // temperatureStr[4]
  dtostrf(humidityPer, 2, 1, humidityStr);  // humidityStr[4]
  dtostrf(pressure, 4, 0, pressureStr);
  dtostrf(pm_2_5, 3, 0, pm2_5Str);

   strcpy(displayHum,  "HUM  : "); // need to consider DISPLAY_STR_SIZE
   strcat(displayHum, humidityStr);
   strcat(displayHum, " %");

   strcpy(displayTemp,  "TEMP : ");
   strcat(displayTemp, temperatureStr);
   strcat(displayTemp, " C");

   strcpy(displayPressure,  "BARO : ");
   strcat(displayPressure, pressureStr);
   strcat(displayPressure, " hPa");

   strcpy(displayPm25,  "PM 2.5 : ");
   strcat(displayPm25, pm2_5Str);
   strcat(displayPm25, " ug/m3");
   
  tmElements_t currentTime = readClock();
  if ( isClockOk ) {
    memset(displayTime, ' ', TIME_STR_SIZE);
    snprintf(displayTime, sizeof(displayTime), "%04d/%02d/%02d %02d:%02d:%02d", tmYearToCalendar(currentTime.Year), currentTime.Month, currentTime.Day, currentTime.Hour, currentTime.Minute, currentTime.Second); 
  }

//  tft.setTextColor(TFT_BLACK);
//  tft.drawRightString(displayTimeOrig, 450, 150 , 4);
//  tft.drawRightString(displayHumOrig, 450, 180 , 4);
//  tft.drawRightString(displayTempOrig, 450, 210 , 4);
//  tft.drawRightString(displayPressureOrig, 450, 240 , 4);

  unsigned int DISPLAY_X_COR = 200;
  unsigned int DISPLAY_Y_COR_INIT = 140;
  unsigned int EACH_ROW_HEIGHT =30;
  unsigned int rowSize = 0;
  if ( isSuccesToInitSD){
     tft.setTextColor(TFT_PINK, TFT_BLACK);
     tft.drawString("[SD Card]", 410, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize , 2);
  }
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString(WELCOME_MSG_FIRST_LINE, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++ , 4);  

  
  if ( isClockOk ) {
    tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
    tft.drawString(displayTime, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++ , 4);
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(displayHum, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++ , 4);
  tft.drawString(displayTemp, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++, 4);
  tft.drawString(displayPressure, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++, 4);
  tft.drawString(displayPm25, DISPLAY_X_COR, DISPLAY_Y_COR_INIT + EACH_ROW_HEIGHT * rowSize++ , 4);


  
//strcpy(displayHumOrig,  displayHum);
//strcpy(displayTempOrig,  displayTemp);
//strcpy(displayPressureOrig,  displayPressure);
//strcpy(displayPm25Orig,  displayPm25);
//strcpy(displayTimeOrig,  displayTime);
//displayHumOrig =displayHum;
//displayTempOrig =displayTemp;
//displayPressureOrig=displayPressure;
//displayPm25Orig =displayPm25;
//displayTimeOrig=displayTime;

  value[0] = pm_1;
  value[1] = pm_2_5;
  value[2] = pm_10;

  plotPointer(); // It takes aout 3.5ms to plot each gauge for a 1 pixel move, 21ms for 6 gauges
  plotNeedle(temperatureC, 0, 240, "TEMP"); // It takes between 2 and 12ms to replot the needle with zero delay
  plotNeedle(humidityPer, 0, 0, "%HUM"); // It takes between 2 and 12ms to replot the needle with zero delay


}

void writeToSDCard() {
  if ( ENABLE_SD_CARD && isSuccesToInitSD ) { 

    //if ( sdCardFile.open(SD_CARD_FILENAME, O_RDWR | O_CREAT | O_AT_END) ) {
    sdCardFile = SD.open(SD_CARD_FILENAME, FILE_WRITE);
    if ( sdCardFile ) {
      //Serial.println(String("Writing to ") + SD_CARD_FILENAME);
      tmElements_t currentTime = readClock();
      memset(timeStr, ' ', TIME_STR_SIZE);
      snprintf(timeStr, sizeof(timeStr), "%04d/%02d/%02d %02d:%02d:%02d", tmYearToCalendar(currentTime.Year), currentTime.Month, currentTime.Day, currentTime.Hour, currentTime.Minute, currentTime.Second); // timeStr[20]
      memset(temperatureStr, ' ', TEMPERATURE_STR_SIZE);
      memset(humidityStr, ' ', HUMIDITY_STR_SIZE);
      memset(pressureStr, ' ', PRESSURE_STR_SIZE);
      dtostrf(temperatureC, 2, 1, temperatureStr);  //format the display string (2 digits, 1 dp)  // temperatureStr[4]
      dtostrf(humidityPer, 2, 1, humidityStr);  // humidityStr[4]
      dtostrf(pressure, 4, 0, pressureStr);
      if (!writeSDHeader) {
        sdCardFile.println(SD_FILE_DATA_HEADER);  // Write Header
        writeSDHeader = true;
      }

      // Modify SD_FILE_DATA_HEADER, if modify this data format
      if (pm_1 == 0 && pm_2_5 == 0 && pm_10 == 0) { // 2016.11.11 (pmat25==0 && pmat100==0 && pmat10==0) means PMS7003 did not provide good data.
        sdCardFile.println(timeStr + String("\t") + temperatureStr + "\t" + humidityStr + "\t" + pressureStr);
      } else {
        sdCardFile.println(timeStr + String("\t") + temperatureStr + "\t" + humidityStr + "\t" + pressureStr + "\t" + pm_2_5 + "\t" + pm_10 + "\t" + pm_1);
      }
      sdCardFile.close();   // flush(), if keep opening file, use flush() to write data immediately.
      
    } else {

      Serial.println("ERR: Cannot open ENV_MONITORING.txt...");
    }
  } 
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
tmElements_t readClock() {
  tmElements_t clockTime;
  if (RTC.read(clockTime)) {
    //    Serial.print("Ok, Time = ");
    //    print2digits(clockTime.Hour);
    //    Serial.write(':');
    //    print2digits(clockTime.Minute);
    //    Serial.write(':');
    //    print2digits(clockTime.Second);
    //    Serial.print(", Date (D/M/Y) = ");
    //    Serial.print(clockTime.Day);
    //    Serial.write('/');
    //    Serial.print(clockTime.Month);
    //    Serial.write('/');
    //    Serial.print(tmYearToCalendar(clockTime.Year));
    //    Serial.println();
    isClockOk = true;
  } else {
    isClockOk = false;
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    //delay(9000);
  }
  return clockTime;
}


unsigned int getPMS7003Data(unsigned char chrSrc[], byte bytHigh, byte bytLow)
{
  return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];
}
void readPMS7003() {
  boolean isFound = false;
  int roundTry = 0;
  while (!isFound && roundTry <= 500) {
    unsigned char chrData[30];
    roundTry++;
    if (myPMS7003.available())
    {
      pms7003ChrRecv = myPMS7003.read();

      if (pms7003ChrRecv == START_1 && bytCount == 0)  bytCount = 1;
      if (pms7003ChrRecv == START_2 && bytCount == 1)  bytCount = 2;

      if (bytCount == 2)
      {
        bytCount = 0;
        isFound = true;
        for (int i = 0; i < 30; i++)
        {
          chrData[i] = myPMS7003.read();
          //Serial.print(String(chrData[i], HEX));
          //Serial.print("-");
        } // end for(bytCount = 0; bytCount < 30; bytCount++)
        //Serial.println("");

        //         Serial.println("===================================================");
        //         Serial.print("Data Length = ");
        //         Serial.print((unsigned int) (chrData[DATA_LENGTH_H] << 8) + chrData[DATA_LENGTH_L]);
        //         Serial.print(", Device Version = ");
        //         Serial.print((unsigned int) chrData[VERSION]);
        //         Serial.print(" (Src=");  Serial.print(chrData[VERSION]);   Serial.print(" HEX="); Serial.print(String(chrData[VERSION], HEX));
        //         Serial.print("), Error Code = ");
        //         Serial.println((unsigned int) chrData[ERROR_CODE]);
        //         Serial.println("");

        pm_1 = getPMS7003Data(chrData, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L);
        pm_2_5 = getPMS7003Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);
        pm_10 = getPMS7003Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);
        //         Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        //         Serial.println("                   Atmosphere (ug/m^3)");
        //         Serial.println("  PM 1.0     PM 2.5     PM 10");
        //         Serial.println("---------------------------------------------------");
        //         Serial.print("  ");
        //         Serial.print(pm_1);
        //         Serial.print("          ");
        //         Serial.print(pm_2_5);
        //         Serial.print("          ");
        //         Serial.println(pm_10);
        //         Serial.println("");

        //         Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        //         Serial.println("                   CF = 1 (ug/m^3)");
        //         Serial.println("  PM 1.0     PM 2.5     PM 10");
        //         Serial.println("---------------------------------------------------");
        //         Serial.print("  ");
        //         Serial.print(getPMS7003Data(chrData, PM1_0_CF1_H, PM1_0_CF1_L));
        //         Serial.print("          ");
        //         Serial.print(getPMS7003Data(chrData, PM2_5_CF1_H, PM2_5_CF1_L));
        //         Serial.print("          ");
        //         Serial.println(getPMS7003Data(chrData, PM10_CF1_H, PM10_CF1_L));
        //         Serial.println("");

        //         Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        //         Serial.println("                   Dusty Count");
        //         Serial.println("  >um0.3     >0.5um     >1.0um     >2.5um     >5.0um     >10um");
        //         Serial.println("--------------------------------------------------------------");
        //         Serial.print("  ");
        //         Serial.print(getPMS7003Data(chrData, UM0_3_H, UM0_3_L));
        //         Serial.print("        ");
        //         Serial.print(getPMS7003Data(chrData, UM0_5_H, UM0_5_L));
        //         Serial.print("        ");
        //         Serial.print(getPMS7003Data(chrData, UM1_0_H, UM1_0_L));
        //         Serial.print("        ");
        //         Serial.print(getPMS7003Data(chrData, UM2_5_H, UM2_5_L));
        //         Serial.print("        ");
        //         Serial.print(getPMS7003Data(chrData, UM5_0_H, UM5_0_L));
        //         Serial.print("        ");
        //         Serial.print(getPMS7003Data(chrData, UM10_H, UM10_L));
        //         Serial.print("        ");
        //
        //         Serial.println("");


      }
    }
  }
}



void readBME280() {
  bme280.readSensor();
  temperatureC = bme280.getTemperature_C();
  temperatureF = bme280.getTemperature_F();
  humidityPer = bme280.getHumidity();
  pressure = bme280.getPressure_MB();

  //  Serial.print("temperatureC:");
  //  Serial.println(temperatureC);
  //  Serial.print("temperatureF:");
  //  Serial.println(temperatureF);
  //  Serial.print("humidityPer:");
  //  Serial.println(humidityPer);
  //  Serial.print("pressure:");
  //  Serial.println(pressure);
}



// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(float value, byte ms_delay, int x_adj, char* header)
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  char buf[8]; dtostrf(value, 4, 1, buf);
  tft.drawRightString(buf, 40 + x_adj, 119 - 20, 2);

  if (value < -10) value = -10; // Limit value to emulate needle end stops
  if (value > 110) value = 110;

  // Move the needle util new value reached
  //  while (!(value == old_analog)) {
  //    if (old_analog < value) old_analog++;
  //    else old_analog--;
  //
  //    if (ms_delay == 0) old_analog = value; // Update immediately id delay is 0

  float sdeg = map(value, -10, 110, -150, -30); // Map value to angle
  // Calcualte tip of needle coords
  float sx = cos(sdeg * 0.0174532925);
  float sy = sin(sdeg * 0.0174532925);

  // Calculate x delta of needle start (does not start at pivot point)
  float tx = tan((sdeg + 90) * 0.0174532925);

  // Erase old needle image
  tft.drawLine(120 + 20 * ltx - 1 + x_adj, 140 - 20, osx - 1 + x_adj, osy, TFT_WHITE);
  tft.drawLine(120 + 20 * ltx + x_adj, 140 - 20, osx + x_adj, osy, TFT_WHITE);
  tft.drawLine(120 + 20 * ltx + 1 + x_adj, 140 - 20, osx + 1 + x_adj, osy, TFT_WHITE);

  // Re-plot text under needle
  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString(header, 120 + x_adj, 70, 4); // // Comment out to avoid font 4

  // Store new needle end coords for next erase
  ltx = tx;
  osx = sx * 98 + 120;
  osy = sy * 98 + 140;

  // Draw the needle in the new postion, magenta makes needle a bit bolder
  // draws 3 lines to thicken needle
  tft.drawLine(120 + 20 * ltx - 1 + x_adj , 140 - 20, osx - 1 + x_adj, osy, TFT_RED);
  tft.drawLine(120 + 20 * ltx + x_adj, 140 - 20 , osx + x_adj, osy, TFT_MAGENTA);
  tft.drawLine(120 + 20 * ltx + 1 + x_adj , 140 - 20, osx + 1 + x_adj, osy, TFT_RED);

  //    tft.drawLine(120 + (x_adj+20) * ltx - 1 + x_adj, 140 - 20, osx - 1+x_adj, osy, TFT_RED);
  //    tft.drawLine(120 + (x_adj+20) * ltx + x_adj, 140 - 20 , osx+x_adj, osy, TFT_MAGENTA);
  //    tft.drawLine(120 + (x_adj+20) * ltx + 1 + x_adj, 140 - 20, osx + 1+x_adj, osy, TFT_RED);

  // Slow needle down slightly as it approaches new postion
  //    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;
  //
  //    // Wait before next update
  //    delay(ms_delay);
  //  }
}

// #########################################################################
//  Draw a linear meter on the screen
// #########################################################################
void plotLinear(char *label, int x, int y, int w)
{
  //int w = 36;
  tft.drawRect(x, y, w, 155, TFT_GREY);
  tft.fillRect(x + 2, y + 19, w - 3, 155 - 38, TFT_WHITE);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(label, x + w / 2, y + 2, 2);

  for (int i = 0; i < 110; i += 10)
  {
    tft.drawFastHLine(x + 20, y + 27 + i, 6, TFT_BLACK);
  }

  for (int i = 0; i < 110; i += 50)
  {
    tft.drawFastHLine(x + 20, y + 27 + i, 9, TFT_BLACK);
  }

  tft.fillTriangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 - 5, TFT_RED);
  tft.fillTriangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 + 5, TFT_RED);

  tft.drawCentreString("    ", x + w / 2, y + 155 - 18, 2);
}

// #########################################################################
//  Adjust 6 linear meter pointer positions
// #########################################################################
void plotPointer(void)
{
  int dy = 187;
  byte pw = 16;

  int w = 55;

  // restore to black color.
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  for (int i = 0; i < 3; i++)
  {
    if ( old_text_value[i] >= 0 ) {
      char buf[8]; dtostrf(old_text_value[i], 4, 0, buf);
      tft.drawRightString(buf, i * w + 36 - 5, 187 - 27 + 155 - 18, 2);
      old_text_value[i] = value[i];
    }
  }
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // Move the 6 pointers one pixel towards new value
  for (int i = 0; i < 3; i++)
  {
    char buf[8]; dtostrf(value[i], 4, 0, buf);
    tft.drawRightString(buf, i * w + 36 - 5, 187 - 27 + 155 - 18, 2);

    int dx = 3 + w * i;
    if (value[i] < 0) value[i] = 0; // Limit value to emulate needle end stops
    if (value[i] > 100) value[i] = 100;

    while (!(value[i] == old_value[i])) {
      dy = 187 + 100 - old_value[i];
      if (old_value[i] > value[i])
      {
        tft.drawLine(dx, dy - 5, dx + pw, dy, TFT_WHITE);
        old_value[i]--;
        tft.drawLine(dx, dy + 6, dx + pw, dy + 1, TFT_RED);
      }
      else
      {
        tft.drawLine(dx, dy + 5, dx + pw, dy, TFT_WHITE);
        old_value[i]++;
        tft.drawLine(dx, dy - 6, dx + pw, dy - 1, TFT_RED);
      }
    }
  }
}


void analogMeter_V2(int x_adj, char* header)
{
  // Meter outline
  tft.fillRect(0 + x_adj, 0, 239, 126, TFT_GREY); // X, Y, width, height
  tft.fillRect(5 + x_adj, 3, 230, 119, TFT_WHITE);

  tft.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coodinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (100 + tl) + 120;
    uint16_t y0 = sy * (100 + tl) + 140;
    uint16_t x1 = sx * 100 + 120;
    uint16_t y1 = sy * 100 + 140;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (100 + tl) + 120;
    int y2 = sy2 * (100 + tl) + 140;
    int x3 = sx2 * 100 + 120;
    int y3 = sy2 * 100 + 140;

    // Yellow zone limits
    //if (i >= -50 && i < 0) {
    //  tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
    //  tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    //}

    // Green zone limits
    if (i >= 0 && i < 25) {
      tft.fillTriangle(x0 + x_adj, y0, x1 + x_adj, y1, x2 + x_adj, y2, TFT_GREEN);
      tft.fillTriangle(x1 + x_adj, y1, x2 + x_adj, y2, x3 + x_adj, y3, TFT_GREEN);
    }

    // Orange zone limits
    if (i >= 25 && i < 50) {
      tft.fillTriangle(x0 + x_adj, y0, x1 + x_adj, y1, x2 + x_adj, y2, TFT_ORANGE);
      tft.fillTriangle(x1 + x_adj, y1, x2 + x_adj, y2, x3 + x_adj, y3, TFT_ORANGE);
    }

    // Short scale tick length
    if (i % 25 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (100 + tl) + 120;
    y0 = sy * (100 + tl) + 140;
    x1 = sx * 100 + 120;
    y1 = sy * 100 + 140;

    // Draw tick
    tft.drawLine(x0 + x_adj, y0, x1 + x_adj, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 25 == 0) {
      // Calculate label positions
      x0 = sx * (100 + tl + 10) + 120;
      y0 = sy * (100 + tl + 10) + 140;
      switch (i / 25) {
        case -2: tft.drawCentreString("0", x0 + x_adj, y0 - 12, 2); break;
        case -1: tft.drawCentreString("25", x0 + x_adj, y0 - 9, 2); break;
        case 0: tft.drawCentreString("50", x0 + x_adj, y0 - 6, 2); break;
        case 1: tft.drawCentreString("75", x0 + x_adj, y0 - 9, 2); break;
        case 2: tft.drawCentreString("100", x0 + x_adj, y0 - 12, 2); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * 100 + 120;
    y0 = sy * 100 + 140;
    // Draw scale arc, don't draw the last part
    if (i < 50) tft.drawLine(x0 + x_adj, y0, x1 + x_adj, y1, TFT_BLACK);
  }


  tft.drawCentreString(header, 120 + x_adj, 70, 4); // Comment out to avoid font 4
  tft.drawRect(5 + x_adj, 3, 230, 119, TFT_BLACK); // Draw bezel line

  plotNeedle(0, 0, 240, header); // Put meter needle at 0
}




