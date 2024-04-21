//https://github.com/KurtE/Arduino_GIGA-stuff/blob/main/sketches/USBHost_SerialDevice_test/USBHost_SerialDevice_test.ino
#include <Arduino_USBHostMbed5.h>
#include "USBHostSerialDevice.h"
#define USB_DEBUG_H
#define DEBUG 3 /*INFO,ERR,WARN*/

REDIRECT_STDOUT_TO(Serial)

USBHostSerialDevice hser;

// packets
// Page 77
unsigned char pkt_resetsoftcfg[] = {0x61, 0x00, 0x07, 0x00, 0x34, 0x05, 0x00, 0x00, 0x07, 0x00, 0x10, 0x61, 0x00, 0x03, 0x00, 0x05, 0x10, 0x40};

// SPEECH RATE 2400bps FEC 1200bps RCW 0 0x0130 RCW 1 0x0763 RCW 2 0x4000 RCW 3 0x0000 RCW 4 0x0000 RCW 5 0x0048 (Page 95)
unsigned char pkt_ratep[] = {0x61, 0x00, 0x0d, 0x00, 0x0a, 0x01, 0x30, 0x07, 0x63, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48};

// Ready packet as the resetsoftcfg doesn't return a specific response, just a ready
unsigned char pkt_ready[] = {0x61, 0x00, 0x01, 0x00, 0x39};

// Page 65
unsigned char pkt_prodid[] = {0x61, 0x00, 0x01, 0x00, 0x30};

// Page 65
unsigned char pkt_verstring[] = {0x61, 0x00, 0x01, 0x00, 0x31};

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {}

  Serial.println("Starting USB host Serial device test...");

  // Enable the USBHost
  pinMode(PA_15, OUTPUT);
  digitalWrite(PA_15, HIGH);

  while (!hser.connect()) {
    Serial.println("No USB host Serial device connected");
    delay(5000);
  }

  Serial.println("USB host Serial device connected trying begin");
  hser.begin(460800);
  writeBytes(pkt_resetsoftcfg, sizeof(pkt_resetsoftcfg)); // Send the soft reset packet
}

// Function to write an array of bytes to the USBHostSerialDevice
void writeBytes(const unsigned char* buffer, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    hser.write(buffer[i]);

    // for debug
    Serial.print(" 0x");           // Prefix to indicate hexadecimal
    if (buffer[i] < 0x10) {       // Add leading zero for values less than 0x10
      Serial.print("0");
    }
    Serial.print(buffer[i], HEX); // Print the byte in hexadecimal

    delay(10); // Optional: delay to avoid overrunning the UART buffer
  }
  Serial.print("\n");
}

void loop() {
  while (hser.available())
    Serial.println(hser.read());
}
