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

// Define Buttons
const int buttonMinutes = A1;  // Define the pin for the button connected to A1
const int buttonHours = A2;  // Define the pin for the button connected to A2

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

  pinMode(buttonMinutes, INPUT); // Set A1 pin as an input
  pinMode(buttonHours, INPUT); // Set A2 pin as an input

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
  int buttonStateMinutes = digitalRead(buttonMinutes);
  int buttonStateHours = digitalRead(buttonHours);

  if (buttonStateMinutes == HIGH) {
    Serial.println("Button connected to A1 is pressed.");
    rtc.adjust(rtc.now() + TimeSpan(0, 0, 1, 0)); // Add 1 minute
  }

  // Check the state of the button connected to A2
  if (buttonStateHours == HIGH) {
    Serial.println("Button connected to A2 is pressed.");
    rtc.adjust(rtc.now() + TimeSpan(0, 1, 0, 0)); // Add 1 hour
  }

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
  //int displayLocalTime = (localDateTime.hour() * 100) + localDateTime.minute();

  // Extract the hours and minutes
  int hours = localDateTime.hour();
  int minutes = localDateTime.minute();

  uint8_t segments[4];

  // Convert hours to 7-segment representation
  if (hours >= 10) {
    segments[0] = display.encodeDigit(hours / 10);  // Tens of hours
  } else {
    segments[0] = 0;  // Leave the segment blank
  }
  segments[1] = display.encodeDigit(hours % 10);  // Units of hours

  // Convert minutes to 7-segment representation
  segments[2] = display.encodeDigit(minutes / 10);  // Tens of minutes
  segments[3] = display.encodeDigit(minutes % 10);  // Units of minutes

  // Display the hour and minute segments
  display.setSegments(segments);


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
  delay(500);
}
