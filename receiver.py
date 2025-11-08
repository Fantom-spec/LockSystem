import serial
import time
import encryption
import sql
import app

SERIAL_PORT = "COM8" 
BAUD = 9600

ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)
time.sleep(2)
print(f"Listening on {SERIAL_PORT} @ {BAUD} baud...")

def send(result):
    if result:
        app.send_trigger_to_esp32()
while True:
    if ser.in_waiting:
        line = ser.readline().decode(errors='ignore').strip()
        encrypted=encryption.process_message(line)
        print(encrypted)
        send(sql.search(encrypted[0],encrypted[1]))


            
