#include "nfc.h"
#include <AESLib.h>  // Include AES library for decryption

#define MAX_TIME_THRESHOLD 500 // Max legitimate NFC response time in ms

#define MIN_ANGLE 160.0       // Minimum absolute angle (must be close to 180°)
#define PIN_CODE "1111"       // Demo PIN

NFC_Module nfc;
u8 tx_buf[50] = "POS"; // Default message to send
u8 tx_len;
u8 rx_buf[100]; // Buffer to store received data
u8 rx_len;

int anomaly_count = 0;
bool security_locked = false;  // Lock system after 3 anomalies

// AES Key and IV (must match the payment card's key and IV)
uint8_t aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t aes_iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

// Function prototypes
bool request_PIN();
void removePadding(uint8_t* data, size_t &data_len);
void aes128_dec_single(const uint8_t* key, const uint8_t* input, uint8_t* output);
void initializeNFC();
void processTransaction();
bool validateOrientation(float pitch, float roll);
bool validateResponseTime(unsigned long response_time);

// Function to request PIN input
bool request_PIN() {
  Serial.println("🔐 Security Alert: Too many anomalies detected!");
  Serial.println("❗ Please enter your PIN to continue:");

  while (true) {
    String entered_pin = "";
    while (Serial.available() == 0); // Wait for input

    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') break;
      entered_pin += c;
    }

    if (entered_pin == PIN_CODE) {
      Serial.println("✅ Correct PIN. Transaction approved.");
      anomaly_count = 0;  // Reset anomaly counter
      security_locked = false;  // Unlock system
      return true;
    } else {
      Serial.println("❌ Incorrect PIN. Please try again.");
    }
  }
}

// Function to remove PKCS7 padding
void removePadding(uint8_t* data, size_t &data_len) {
  size_t padding_len = data[data_len - 1];
  data_len -= padding_len;
}

// Function to decrypt received data
void aes128_dec_single(const uint8_t* key, const uint8_t* input, uint8_t* output) {
  AES aes;
  aes.set_key(key, 16); // Set AES key (16 bytes for AES-128)
  aes.decrypt(input, output); // Decrypt the input data
}

// Function to initialize NFC module
void initializeNFC() {
  Serial.begin(115200);
  nfc.begin();
  Serial.println("Terminal Initialized.");

  uint32_t versiondata = nfc.get_version();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfiguration();
}

// Function to validate orientation is close to ±180° (minimum ±160°)
bool validateOrientation(float pitch, float roll) {
  float abs_pitch = abs(pitch);
  float abs_roll = abs(roll);
  
  bool pitch_valid = (abs_pitch >= MIN_ANGLE);
  bool roll_valid = (abs_roll >= MIN_ANGLE);

  if (!pitch_valid || !roll_valid) {
    Serial.println("🚨 ALERT: Card not oriented close enough to ±180°!");
    Serial.print("Pitch: "); Serial.print(pitch); 
    Serial.print("° (Must be ≥ ±"); Serial.print(MIN_ANGLE); Serial.println("°)");
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print("° (Must be ≥ ±"); Serial.print(MIN_ANGLE); Serial.println("°)");
    Serial.println("❌ Transaction blocked for security reasons.");
    
    anomaly_count++;
    if (anomaly_count >= 3) {
      security_locked = true;
      if (!request_PIN()) return false;
    }
    return false;
  }
  return true;
}

// Function to validate response time
bool validateResponseTime(unsigned long response_time) {
  if (response_time > MAX_TIME_THRESHOLD) {
    Serial.println("🚨 ALERT: Unusual delay detected in transaction!");
    Serial.println("❌ Transaction blocked for security reasons.");
    anomaly_count++;

    if (anomaly_count >= 3) {
      security_locked = true;
      if (!request_PIN()) return false;
    }
    return false;
  }
  return true;
}

// Function to process a transaction
void processTransaction() {
  Serial.println("\n💳 Waiting for card...");

  // Wait until card is detected
  while (!nfc.P2PInitiatorInit()) {
    delay(100);
  }

  // Start timing
  unsigned long start_time = millis();
  Serial.println("✅ Card detected.");

  // Read NFC Data
  tx_len = strlen((const char *)tx_buf);
  memset(rx_buf, 0, sizeof(rx_buf));

  if (nfc.P2PInitiatorTxRx(tx_buf, tx_len, rx_buf, &rx_len)) {
    unsigned long end_time = millis();
    unsigned long response_time = end_time - start_time;  // Calculate response time

    Serial.print("⏳ Transaction time: "); Serial.print(response_time); Serial.println("ms");

    // Validate response time
    if (!validateResponseTime(response_time)) return;

    // Print the received encrypted data for debugging
    Serial.print("Received Encrypted Data: ");
    for (int i = 0; i < rx_len; i++) {
      Serial.print("0x"); Serial.print(rx_buf[i], HEX); Serial.print(" ");
    }
    Serial.println();

    // Decrypt the received data
    uint8_t decryptedData[16]; // AES block size is 16 bytes
    aes128_dec_single(aes_key, rx_buf, decryptedData);

    // Print the decrypted data for debugging
    Serial.print("Decrypted Data: ");
    for (int i = 0; i < 16; i++) {
      Serial.print("0x"); Serial.print(decryptedData[i], HEX); Serial.print(" ");
    }
    Serial.println();

    // Remove padding
    size_t decrypted_len = 16;
    removePadding(decryptedData, decrypted_len);

    // Check if the decrypted data is valid
    if (decryptedData[0] == 0xAA && decrypted_len == 13) {  // Check for the start marker and correct length
      float yaw, pitch, roll;
      memcpy(&yaw, &decryptedData[1], sizeof(yaw));
      memcpy(&pitch, &decryptedData[5], sizeof(pitch));
      memcpy(&roll, &decryptedData[9], sizeof(roll));

      // Print orientation data for debugging
      Serial.print("Yaw: "); Serial.println(yaw);
      Serial.print("Pitch: "); Serial.println(pitch);
      Serial.print("Roll: "); Serial.println(roll);

      // Validate orientation is close to ±180°
      if (!validateOrientation(pitch, roll)) return;

      // Transaction is valid
      Serial.println("✅ Transaction successful.");
      anomaly_count = 0;  // Reset anomalies
    } else {
      Serial.println("❌ Invalid decrypted data format.");
    }
  } else {
    Serial.println("❌ Failed to receive data from card.");
  }

  Serial.println();
}

void setup(void) {
  initializeNFC();
}

void loop(void) {
  if (security_locked) {
    if (!request_PIN()) return;  // Wait until PIN is provided
  }

  processTransaction();
  delay(1000);  // Add a delay between transactions
}