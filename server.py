from flask import Flask, request
import os
from datetime import datetime
import enhancer
import recognition

app = Flask(__name__)

IMG_FOLDER = "captures"
IP_FILE = "esp_ip.txt"

# Ensure folder exists
os.makedirs(IMG_FOLDER, exist_ok=True)

@app.route('/hello', methods=['POST'])
def hello_from_esp():
    esp32_ip = request.remote_addr
    print(f"✅ ESP32 connected from IP: {esp32_ip}")
    with open(IP_FILE, "w") as f:
        f.write(esp32_ip)
    return "Hello ESP32!", 200

@app.route('/status', methods=['POST'])
def status():
    if request.data:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
        img_file = os.path.join(IMG_FOLDER, f"esp_capture_{timestamp}.jpg")
        with open(img_file, "wb") as f:
            f.write(request.data)
        print(f"📸 Image received and saved as {img_file}")
        enhancer.preprocess_esp32_image(img_file)
        results = recognition.recognize_face_from_image(img_file)
        print(results[0]['name'])

        return "Image received", 200
    return "No data", 400

def send_trigger_to_esp32():
    import requests
    if not os.path.exists(IP_FILE):
        print("⚠️ No ESP32 IP found yet.")
        return
    with open(IP_FILE, "r") as f:
        esp32_ip = f.read().strip()
    try:
        url = f"http://{esp32_ip}/trigger"
        print(f"🚀 Sending trigger to {url}")
        r = requests.post(url, data="blink")
        print(f"✅ Trigger sent, response code: {r.status_code}")
    except Exception as e:
        print("❌ Error sending trigger:", e)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
