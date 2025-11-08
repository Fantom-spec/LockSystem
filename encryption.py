import base64
import hashlib
import re
from Crypto.Cipher import AES

# ------ AES Config (must match Arduino) ------
AES_KEY = bytes.fromhex("603deb1015ca71be2b73aef0857d7781")
AES_IV  = bytes.fromhex("000102030405060708090a0b0c0d0e0f")
# ---------------------------------------------

def decrypt_message_safe(encrypted_b64: str) -> bytes:
    """
    Base64 decode + AES-CBC decrypt.
    Returns raw decrypted bytes (may contain padding).
    """
    encrypted_bytes = base64.b64decode(encrypted_b64)
    cipher = AES.new(AES_KEY, AES.MODE_CBC, AES_IV)
    decrypted = cipher.decrypt(encrypted_bytes)
    return decrypted

def extract_message_text(decrypted_bytes: bytes) -> str:
    """
    Convert decrypted bytes into cleaned string:
    - Strip PKCS7 padding
    - Keep printable ASCII + pipe
    """
    pad_len = decrypted_bytes[-1]
    if 1 <= pad_len <= 16:
        decrypted_bytes = decrypted_bytes[:-pad_len]
    s = decrypted_bytes.decode('latin1')
    s = re.sub(r'[^0-9A-Za-z\|\-_:., ]', '', s)
    return s.strip()

def encrypt_identifier(identifier: str) -> str:
    """
    AES-CBC encrypt identifier and return Base64.
    """
    pad_len = 16 - (len(identifier) % 16)
    padded = identifier + chr(pad_len) * pad_len
    cipher = AES.new(AES_KEY, AES.MODE_CBC, AES_IV)
    encrypted_bytes = cipher.encrypt(padded.encode('utf-8'))
    return base64.b64encode(encrypted_bytes).decode('utf-8')

def hash_pin(pin: str) -> str:
    """
    Returns SHA-256 hash of the PIN in hex.
    """
    return hashlib.sha256(pin.encode('utf-8')).hexdigest()

def process_message(encrypted_message: str) -> list:
    """
    Takes the full encrypted message from Arduino, returns:
    [encrypted_identifier, hashed_pin]
    """
    decrypted_bytes = decrypt_message_safe(encrypted_message)
    text = extract_message_text(decrypted_bytes)
    if '|' not in text:
        raise ValueError("Decrypted message does not contain '|' separator.")
    
    identifier, pin = text.split('|', 1)
    identifier = identifier.strip()
    pin = pin.strip()

    encrypted_identifier = encrypt_identifier(identifier)
    pin_hash = hash_pin(pin)

    return [encrypted_identifier, pin_hash]

