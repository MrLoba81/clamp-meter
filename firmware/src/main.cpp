#include <Arduino.h>
#include <EmonLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_I2C_ADDRESS 0x3C

#define VOLTAGE_CALIBRATION 260
#define CURRENT_CALIBRATION 19

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
EnergyMonitor emon;

double watt, realWatt;

void draw();
void printHCentered(const char *buffer, int16_t y);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
  }
  Serial.println("Setup the device");

  // For 12BIT ADC Resolution analogReadResolution(ADC_BITS);

  emon.voltage(A1, VOLTAGE_CALIBRATION, 1.7);
  emon.current(A0, CURRENT_CALIBRATION);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS))
  {
    Serial.println("SSD1306 allocation failed");
    delay(1000);
    abort();
  }

  // Print the header
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.fillRect(0, 0, 127, 16, SSD1306_WHITE);
  printHCentered("Clamp Meter", 11);
  // Draw the info box and set the fonts
  display.drawRect(0, 17, 127, 47, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  // Print the Author
  printHCentered("Made by", 20);
  display.setFont(&FreeMonoBold12pt7b);
  printHCentered("MrLoba81", 45);
  display.setFont();
  printHCentered("instagram/mrloba81", 52);
  display.display();

  delay(2000);

  Serial.println("End Setup");
}

void loop()
{
  emon.calcVI(60, 2000);
  Serial.print("CalcVI: ");
  emon.serialprint();

  watt = emon.apparentPower;
  realWatt = emon.realPower;
  if (realWatt < 0)
  {
    watt = 0;
    realWatt = 0;
    emon.Irms = 0;
    emon.powerFactor = 0;
  }
  /*   kwh = (realWatt / 3600) / 1000;
    Serial.print("KWh: ");
    Serial.println(kwh, 8); */

  draw();
}

void draw()
{
  char buffer[19];
  char doubleBuffOne[8];
  char doubleBuffTwo[8];

  // Clear information box
  display.fillRect(1, 18, 125, 45, SSD1306_BLACK);

  dtostrf(emon.Irms, 5, 2, doubleBuffOne);
  dtostrf(emon.powerFactor, 5, 2, doubleBuffTwo);
  sprintf(buffer, "A:%s PF:%s", doubleBuffOne, doubleBuffTwo);
  printHCentered(buffer, 24);

  const char *strToPrint;
  if (watt >= 1000.0)
  {
    strToPrint = "K:";
    watt = watt / 1000.0;
    realWatt = realWatt / 1000.0;
  }
  else
  {
    strToPrint = "W:";
  }

  dtostrf(realWatt, 5, 2, doubleBuffOne);
  dtostrf(watt, 5, 2, doubleBuffTwo);
  sprintf(buffer, "%s%s ap:%s", strToPrint, doubleBuffOne, doubleBuffTwo);
  printHCentered(buffer, 38);

  dtostrf(emon.Vrms, 5, 2, doubleBuffOne);
  sprintf(buffer, "V: %s", doubleBuffOne);
  printHCentered(buffer, 52);

  display.display();
}

void printHCentered(const char *buffer, int16_t y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buffer, 0, y, &x1, &y1, &w, &h);
  int16_t x = ((SCREEN_WIDTH - 2) / 2) - (w / 2);
  display.setCursor(x, y);
  display.print(buffer);
}