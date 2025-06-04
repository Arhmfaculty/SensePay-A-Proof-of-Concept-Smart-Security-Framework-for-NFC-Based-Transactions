#define TIMING_DEBUG  // Uncomment to enable timing measurements

#ifdef TIMING_DEBUG
#define TIMING_START(stage) Serial.print("[TIMING]"); Serial.print(stage); Serial.print(":"); Serial.println(micros())
#define TIMING_END(stage) Serial.print("[TIMING]"); Serial.print(stage); Serial.print(":"); Serial.println(micros())
#else
#define TIMING_START(stage)
#define TIMING_END(stage)
#endif 

#include "nfc.h"
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <AESLib.h>

// Device addresses
#define PN532_I2C_ADDRESS 0x24
#define MPU6050_I2C_ADDRESS 0x68

// Global objects
NFC_Module nfc;
MPU6050 mpu;

// MPU6050 DMP variables
bool dmpReady = false;
uint8_t mpuIntStatus, devStatus;
uint16_t packetSize;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3]; // [yaw, pitch, roll]

// NFC communication buffers
uint8_t tx_buf[50] = "Card";
uint8_t tx_len;
uint8_t rx_buf[100];
uint8_t rx_len;

// AES Encryption
uint8_t aes_key[] = { 
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 
  0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C 
};
uint8_t aes_iv[] = { 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F 
};

// Function prototypes
void addPadding(uint8_t* data, size_t &data_len);
void aes128_enc_single(const uint8_t* key, const uint8_t* input, uint8_t* output);
void initializeNFC();
void initializeMPU6050();
void processOrientationData();
void sendEncryptedData(const uint8_t* data, size_t data_len);

// Add PKCS7 padding to data
void addPadding(uint8_t* data, size_t &data_len) {
  size_t padding_len = 16 - (data_len % 16);
  for (size_t i = 0; i < padding_len; i++) {
    data[data_len + i] = padding_len;
  }
  data_len += padding_len;
}

// Encrypt data using AES-128
void aes128_enc_single(const uint8_t* key, const uint8_t* input, uint8_t* output) {
  AES aes;
  aes.set_key(key, 16);
  aes.encrypt(input, output);
}

// Initialize NFC module
void initializeNFC() {
  nfc.begin(PN532_I2C_ADDRESS);
  Serial.println("Payment Device (card)");

  uint32_t versiondata = nfc.get_version();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board at address 0x24");
    while(1);
  }

  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfiguration();
}

// Initialize MPU6050 with DMP
void initializeMPU6050() {
  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while(1);
  }
  Serial.println("MPU6050 connected successfully");

  // Initialize DMP
  devStatus = mpu.dmpInitialize();

  // Factory calibration values
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);

  if (devStatus == 0) {
    // Calibrate the MPU
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    
    // Enable DMP
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
    Serial.println("DMP Ready!");
  } else {
    Serial.print("DMP Initialization failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
    while(1);
  }
}

// Process orientation data and send via NFC
void processOrientationData() {
  if (!dmpReady) return;

  TIMING_START("imu_read_start");
  uint16_t fifoCount = mpu.getFIFOCount();
  TIMING_END("imu_read_end");

  if (fifoCount >= 1024) {
    mpu.resetFIFO();
    Serial.println("FIFO overflow prevented!");
    return;
  }

  if (fifoCount < packetSize) {
    return;
  }

  TIMING_START("dmp_processing_start");
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    TIMING_END("dmp_processing_end");

    float yaw = ypr[0] * 180/M_PI;
    float pitch = ypr[1] * 180/M_PI;
    float roll = ypr[2] * 180/M_PI;

    uint8_t packet[13];
    packet[0] = 0xAA;
    memcpy(&packet[1], &yaw, 4);
    memcpy(&packet[5], &pitch, 4);
    memcpy(&packet[9], &roll, 4);

    TIMING_START("data_encryption_start");
    size_t packet_len = 13;
    addPadding(packet, packet_len);
    
    uint8_t encryptedPacket[16];
    aes128_enc_single(aes_key, packet, encryptedPacket);
    TIMING_END("data_encryption_end");

    TIMING_START("nfc_transmission_start");
    sendEncryptedData(encryptedPacket, sizeof(encryptedPacket));
    TIMING_END("nfc_transmission_end");

    mpu.resetFIFO();
  }
}


// Send encrypted data via NFC
void sendEncryptedData(const uint8_t* data, size_t data_len) {
  memcpy(tx_buf, data, data_len);
  tx_len = data_len;

  Serial.print("Sending encrypted data: ");
  for (int i = 0; i < tx_len; i++) {
    Serial.print(tx_buf[i], HEX); Serial.print(" ");
  }
  Serial.println();

  memset(rx_buf, 0, sizeof(rx_buf));

  if (nfc.P2PTargetTxRx(tx_buf, tx_len, rx_buf, &rx_len)) {
    Serial.print("Received response: ");
    Serial.write(rx_buf, rx_len);
    Serial.println();
  } else {
    Serial.println("Failed to send data to terminal");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  TWBR = ((F_CPU / 400000) - 16) / 2;  // I2C at 400kHz

  initializeNFC();
  initializeMPU6050();
  
  Serial.println("Payment card ready");
}

void loop() {
  if (nfc.P2PTargetInit()) {
    Serial.println("POS Terminal detected");
      
       for (int i = 0; i < 5; i++) {
      processOrientationData();
      delay(2); // Small delay between readings
    }
  
  }
  delay(100);
}

