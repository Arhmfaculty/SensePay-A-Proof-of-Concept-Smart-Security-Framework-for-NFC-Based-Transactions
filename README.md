# SensePay-A-Proof-of-Concept-Smart-Security-Framework-for-NFC-Based-Transactions

![image](https://github.com/user-attachments/assets/36f17fbd-2e20-4660-87f8-d18bc587987d)

---

## Overview

**SensePay** is a real-time fraud detection system designed to secure contactless NFC-based payments against relay attacks. The system introduces a **lightweight, on-device orientation verification mechanism** using a 6-axis **IMU sensor**, combined with a **real-time One-Class SVM machine learning model** embedded at the Point-of-Sale terminal.

This research-driven project demonstrates a viable **proof-of-concept** implementation that integrates IMU-based proximity sensing and ML anomaly detection into existing EMV-compliant payment systems with minimal latency, power, and hardware overhead.

---

## Core Features

-  **IMU + NFC Fusion**: Real-time orientation sensing (pitch, roll) during transactions.
-  **Local ML Inference**: Lightweight One-Class SVM model deployed on ESP32 (via TensorFlow Lite → C-array).
- ⏱ **Sub-300ms Transaction Latency**: Low-latency secure processing that meets EMV performance standards.
-  **Detection Accuracy**: Achieved **96.7% accuracy**, outperforming traditional statistical models.
-  **Attack Mitigation**: Proven detection against **delay-based** and **orientation-based** relay attacks.

---

##  System Architecture
```
Payment Device
├── PN532 NFC Module
├── MPU6050 IMU Sensor
└── ESP32 Microcontroller
└── Sensor Fusion + NFC Packet Encryption

POS Terminal
├── NFC Reader
├── ESP32-S3
│ └── Model Inference (fraudModel.h)
└── TFT LCD Display for Orientation Prompt & Feedback
```
---
```
## 📂 Repository Structure

SensePay/
├── firmware/
│ ├── card_device/
│ │ └── main.ino # Code for payment device (IMU + NFC + AES)
│ ├── pos_terminal/
│ │ └── terminal.ino # Code for POS terminal (decryption + inference)
│ └── fraudModel.h # C-array of trained TFLite model
├── ml_model/
│ ├── ocsvm_train.ipynb # Model training and tuning (Python)
│ ├── autoencoder_train.ipynb
│ └── model.tflite # Optimized TFLite model for deployment
├── data/
│ └── orientation_logs.csv # Collected sensor data (Yaw, Pitch, Roll, Response Time)
├── hardware/
│ ├── schematics/ # Fritzing + PDF schematics
│ ├── images/
│ └── 3D_printables/ # Optional enclosures
├── simulation/
│ └── wokwi_simulation.json
├── presentation/
│ └── SensePay_Capstone_Slides.pdf
└── README.md
```
---

## Experimental Results

| Metric               | Statistical Threshold | OC-SVM Model |
|----------------------|-----------------------|--------------|
| Accuracy             | 79%                   | **96.7%**    |
| False Rejection Rate | 30%                   | **4%**       |
| False Acceptance Rate| 8%                    | **3%**       |
| Avg Latency          | 285ms                 | 285ms        |

> **ROC-AUC**: 0.96 for ML vs 0.81 for Threshold Model  
> **McNemar’s Test**: χ² = 10.2, *p* = 0.0014 → Statistically significant improvement.

---

##  Getting Started

### 1. Firmware Setup

- Program the **payment device** and **POS terminal** ESP32 boards using `Arduino IDE`.
- Upload `fraudModel.h` to the terminal firmware for onboard ML inference.

### 2. Hardware Connections

- Connect **MPU6050** and **PN532** via I2C (addressed `0x68` and `0x24` respectively).
- Follow schematic files in `hardware/schematics/`.

### 3. Model Deployment

- Train OC-SVM using `ml_model/ocsvm_train.ipynb`.
- Convert using:
```
xxd -i model.tflite > fraudModel.h
```
Include fraudModel.h in the terminal firmware.

## Research Context
This project was developed as a B.Sc. Computer Engineering Capstone at Ashesi University, Ghana. It is grounded in security research and applies embedded systems, anomaly detection, and contactless payment architecture to propose real-time, deployable, and resource-efficient countermeasures against proximity spoofing attacks.

## Future Work
Develop a testing environment to simulate relay attacks with orientation spoofing.

Optimize firmware for energy harvesting cards using ultra-low-power IMUs.

Integrate with existing EMV RRP protocols for hybrid verification.

## 👨‍🔬 Author
Maxwell Bosiako Antwi
B.Sc. Computer Engineering (2025)
Ashesi University
📧 maxwellantwibosiako@domain.com
