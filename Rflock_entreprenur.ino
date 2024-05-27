#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 15
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// Binary configurations for each pin
#define MOTOR_PIN_MASK    B00000100  // Binary: 0000 0100, powersource_motor = pin 2
#define TRANSISTOR_PIN_MASK B10000000 // Binary: 1000 0000, powersource_transistor = pin 8
#define VERIFIKASI_RFID_PIN_MASK B00000000 // Binary: 0000 0000, not used
#define OKELIGHT_PIN_MASK B00000010  // Binary: 0000 0010, okelight = pin 9
#define NOLIGHT_PIN_MASK  B00000001  // Binary: 0000 0001, nolight = pin 1

void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  // Set pins as outputs using binary configuration
  DDRD |= MOTOR_PIN_MASK;         // Set pin 2 as output
  DDRB |= TRANSISTOR_PIN_MASK;    // Set pin 8 as output
  DDRD |= OKELIGHT_PIN_MASK;      // Set pin 9 as output
  DDRB |= NOLIGHT_PIN_MASK;       // Set pin 1 as output

  // Initialize motors
  PORTD |= MOTOR_PIN_MASK;        // Set power source for motor high
}

void loop()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "C3 D5 A6 A6") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();

    // Turn on the corresponding pins using binary configuration
    PORTD |= OKELIGHT_PIN_MASK;      // Turn on okelight
    PORTB &= ~NOLIGHT_PIN_MASK;      // Turn off nolight
    PORTB |= TRANSISTOR_PIN_MASK;    // Turn on transistor
    PORTB |= VERIFIKASI_RFID_PIN_MASK; // Turn on verifikasi_RFID (unused pin)
    delay(10000);
  }
  else {
    Serial.println("Access denied");
    
    // Turn off the corresponding pins using binary configuration
    PORTB |= NOLIGHT_PIN_MASK;       // Turn on nolight
    PORTD &= ~OKELIGHT_PIN_MASK;     // Turn off okelight
    PORTB &= ~TRANSISTOR_PIN_MASK;   // Turn off transistor
    PORTB &= ~VERIFIKASI_RFID_PIN_MASK; // Turn off verifikasi_RFID (unused pin)
    delay(2000);
  }

  // Reset pins
  PORTD &= ~OKELIGHT_PIN_MASK;
  PORTB &= ~NOLIGHT_PIN_MASK;
  PORTB &= ~VERIFIKASI_RFID_PIN_MASK;
  PORTB &= ~TRANSISTOR_PIN_MASK;
}
