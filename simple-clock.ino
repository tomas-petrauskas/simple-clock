// Libraries:
// https://github.com/adafruit/RTClib (also Adafruit BusIO as a dependency - https://github.com/adafruit/Adafruit_BusIO)
// https://github.com/avishorp/TM1637
// https://github.com/JChristensen/Timezone (also Time as a dependency - https://playground.arduino.cc/Code/Time/)

#include "RTClib.h"
#include <TM1637Display.h>
#include <Timezone.h> // Include the Timezone library

// Define the connections pins for TM1637 4 digit 7 segment display
#define CLK 9
#define DIO 8

// Change to "true" if you want Arduino to output time to serial port
#define SERIAL_DEBUG true

// Create rtc and display object
RTC_DS3231 rtc;
// TM1637Display display = TM1637Display(CLK, DIO);
TM1637Display display(CLK, DIO);

// Define the Europe/Vilnius timezone with DST rules
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 3, 180}; // Central European Summer Time
TimeChangeRule CET = {"CET", Last, Sun, Oct, 4, 120};    // Central European Time
Timezone tz(CEST, CET);

void setup() {

  // Configure 7 Segment Display
  display.setBrightness(7);
  display.clear();

  // Setup serial communication (Terminal) at a baud rate of 9600
  if (SERIAL_DEBUG) {
    Serial.begin(9600);
  }

  // Check if RTC is connected correctly
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // These lines contains 2 time points which are 10 seconds before Daylight Saving event
  // If you want to adjust current RTC date, uncomment one of these lines:
  // rtc.adjust(DateTime(2023, 10, 29, 0, 59, 50));
  // rtc.adjust(DateTime(2024, 3, 31, 0, 59, 50));
}

void loop() {
  // Get current date and time in UTC
  DateTime rtcDateTime = rtc.now();

  // Convert UTC time to Europe/Vilnius time
  TimeChangeRule *tcr;
  time_t localTime = tz.toLocal(rtcDateTime.unixtime(), &tcr);
  time_t utcTime = rtcDateTime.unixtime();

  // Convert the local time to a DateTime object
  DateTime localDateTime = DateTime(localTime);
  DateTime utcDateTime = DateTime(utcTime);
  
  // Create Integer for 7 Segment Display (Local Time)
  int displayLocalTime = (localDateTime.hour() * 100) + localDateTime.minute();

  // Output Current time to 7 Segment Display (Local Time)
  display.showNumberDec(displayLocalTime);
  
  if (SERIAL_DEBUG) {
    // Create strings for Terminal
    String displayLocalTimeString = String(localDateTime.hour()) + ":" + String(localDateTime.minute()) + ":" + String(localDateTime.second());
    String displayLocalDateString = String(localDateTime.year()) + "-" + String(localDateTime.month()) + "-" + String(localDateTime.day());
    String displayUTCTimeString = String(utcDateTime.hour()) + ":" + String(utcDateTime.minute()) + ":" + String(utcDateTime.second());
    String concatenatedString = "Local Date/Time: " + displayLocalTimeString + " (" + displayLocalDateString + ") | UTC Time: " + displayUTCTimeString;

    // Send to Terminal and add blank line after
    Serial.println(concatenatedString);
    Serial.println();
  }
  
  // Wait 1 second
  delay(1000);
}
