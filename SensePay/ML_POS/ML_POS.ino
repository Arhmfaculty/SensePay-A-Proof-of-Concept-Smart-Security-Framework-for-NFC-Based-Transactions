#include "nfc.h"
#include <AESLib.h>
#include "fraudModel.h"  // Your trained model (1=legitimate, 0=fraud)

#define PIN_CODE "1111"       // Demo PIN
#define MAX_ANOMALIES 30      // Lock after 3 fraud attempts

NFC_Module nfc;
u8 tx_buf[50] = "POS";      // Default message to send
u8 tx_len;
u8 rx_buf[100];             // Buffer for received data
u8 rx_len;

int anomaly_count = 0;
bool security_locked = false;

// AES Key and IV
uint8_t aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t aes_iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };


// Function prototypes
bool request_PIN();
void removePadding(uint8_t* data, size_t &data_len);
void aes128_dec_single(const uint8_t* key, const uint8_t* input, uint8_t* output);
void initializeNFC();
void processTransaction();
bool isLegitimate(float yaw, float pitch, float roll, unsigned long response_time);


// --- Helper Functions ---
bool request_PIN() {
  Serial.println("🔐 Security Locked! Enter PIN:");
  while (true) {
    String entered_pin = "";
    while (Serial.available() == 0); // Wait for input
    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') break;
      entered_pin += c;
    }
    if (entered_pin == PIN_CODE) {
      Serial.println("✅ PIN Correct. System unlocked.");
      anomaly_count = 0;
      security_locked = false;
      return true;
    } else {
      Serial.println("❌ Incorrect PIN. Try again.");
    }
  }
}

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
   Serial.println("🚀 POS Terminal Ready (ML Fraud Detection)");

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


// --- ML Fraud Prediction ---
bool isLegitimate(float yaw, float pitch, float roll, unsigned long response_time) {
  float features[4] = {yaw, pitch, roll, (float)response_time};
  int prediction = fraudClassifier.predict(features);  // 1=legitimate, 0=fraud
  return (prediction == 1);
}

// --- Transaction Processing ---
void processTransaction() {
  Serial.println("\n=== NEW TRANSACTION ===");
  Serial.println("💳 Waiting for card...");

  // Wait for card
  while (!nfc.P2PInitiatorInit()) delay(100);

  // Start timer
  unsigned long start_time = millis();
  Serial.println("✅ Card detected.");

  // Read NFC data
  tx_len = strlen((const char *)tx_buf);
  memset(rx_buf, 0, sizeof(rx_buf));

  if (nfc.P2PInitiatorTxRx(tx_buf, tx_len, rx_buf, &rx_len)) {
    unsigned long response_time = millis() - start_time;
    Serial.print("⏱️ Response Time: "); Serial.print(response_time); Serial.println("ms");

    // Decrypt data
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

    // Extract orientation and time
    if (decryptedData[0] == 0xAA && decrypted_len == 13) {
      float yaw, pitch, roll;
      memcpy(&yaw, &decryptedData[1], sizeof(yaw));
      memcpy(&pitch, &decryptedData[5], sizeof(pitch));
      memcpy(&roll, &decryptedData[9], sizeof(roll));

      // ML Fraud Check (Only Security Gate)
      if (!isLegitimate(abs(yaw), abs(pitch), abs(roll), response_time)) {
        Serial.println("🚨 ALERT: Fraud Detected by ML Model!");
        Serial.print("Yaw: "); Serial.print(yaw);
        Serial.print(", Pitch: "); Serial.print(pitch);
        Serial.print(", Roll: "); Serial.print(roll);
        Serial.print(", Time: "); Serial.print(response_time); Serial.println("ms");
        
        anomaly_count++;
        if (anomaly_count >= MAX_ANOMALIES) security_locked = true;
        return;
      }

      // Success Case
      Serial.println("✅ Transaction Approved (ML-Checked).");
      Serial.print("Yaw: "); Serial.print(yaw);
      Serial.print(", Pitch: "); Serial.print(pitch);
      Serial.print(", Roll: "); Serial.print(roll);
      anomaly_count = 0;
    } else {
      Serial.println("❌ Invalid decrypted data format.");
    }
  } else {
    Serial.println("❌ Failed to receive data from card.");
  
  }
  Serial.println("======================");
}

// --- Main Loop ---
void setup() {
  initializeNFC();
}

void loop() {
  if (security_locked) request_PIN();
  processTransaction();
  delay(1000);
}