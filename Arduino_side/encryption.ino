#include <AESLib.h>
#include <SHA256.h>

AESLib aesLib;
SHA256 sha256;

byte aes_key[16] = { 
  0x60,0x3d,0xeb,0x10,
  0x15,0xca,0x71,0xbe,
  0x2b,0x73,0xae,0xf0,
  0x85,0x7d,0x77,0x81 
};

byte aes_iv[16] = { 
  0x00,0x01,0x02,0x03,
  0x04,0x05,0x06,0x07,
  0x08,0x09,0x0a,0x0b,
  0x0c,0x0d,0x0e,0x0f 
};

// Convert byte array to hex
String toHex(const uint8_t *buf, size_t len) {
  String s;
  s.reserve(len * 2);
  const char hex[] = "0123456789abcdef";
  for (size_t i = 0; i < len; ++i) {
    s += hex[(buf[i] >> 4) & 0xF];
    s += hex[buf[i] & 0xF];
  }
  return s;
}

String hashPin(String pin) {
  uint8_t hash[32];
  sha256.reset();
  sha256.update((const uint8_t*)pin.c_str(), pin.length());
  sha256.finalize(hash, sizeof(hash));
  return toHex(hash, 32);
}

String encryptMessage(String message) {
  char encrypted[128];
  byte iv_copy[16];
  memcpy(iv_copy, aes_iv, 16);
  aesLib.encrypt64((byte*)message.c_str(), message.length(), encrypted, aes_key, 128, iv_copy);
  return String(encrypted);
}

String encryptAndReturn(String uid, String pin) {
  String pinHash = hashPin(pin);
  String message = uid + "|" + pinHash;
  return encryptMessage(message);
}
