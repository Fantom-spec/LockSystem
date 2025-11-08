# LockSystem

A project combining Flask, facial recognition, encryption, and hardware controls (ESP32/Arduino) to build a secure access control system.

## 🚀 Project Overview  
LockSystem allows access control using face recognition + encryption, interfacing with ESP32/Arduino hardware for physical locking mechanisms. It includes:  
- A Python Flask backend for face capture, recognition & key generation.  
- Encryption modules for securing communications.  
- Hardware side code (ESP32 / Arduino) to actuate a lock or signal.  
- A MySQL database interface for storing known face data or events.

## 🧩 Features  
- Face recognition using `face_recognition` library.  
- Data encryption with AES (`Crypto.Cipher`).  
- Serial communication with Arduino/ESP32 (`pyserial`).  
- Modular design for backend, encryption, recognition, and hardware.  
- Structured folders for known faces, captures, and residual data.

## 🏃‍♂️ How to Run  
1. Create required directories in the project root:  
    mkdir captures
    mkdir known_faces
2. Run server.py and receiver.py in different terminals

