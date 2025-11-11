import os
import cv2
import face_recognition
import numpy as np
from enhancer import preprocess_esp32_image

KNOWN_FACES_DIR = "known_faces"
TARGET_SIZE = (112, 112)

# ---------------- Load known faces ----------------
known_encodings = []
known_names = []

for filename in os.listdir(KNOWN_FACES_DIR):
    if filename.lower().endswith((".jpg", ".png")):
        path = os.path.join(KNOWN_FACES_DIR, filename)
        name = os.path.splitext(filename)[0].split("_")[0]
        image = cv2.imread(path)
        rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        encodings = face_recognition.face_encodings(rgb)
        if encodings:
            known_encodings.append(encodings[0])
            known_names.append(name)

# ---------------- Recognition ----------------
def recognize_face_from_image(img_path):
    # Enhance image
    img, _ = preprocess_esp32_image(img_path, target_size=None)

    # Detect faces and encode
    face_locations = face_recognition.face_locations(img)
    face_encodings = face_recognition.face_encodings(img, face_locations)

    for face_encoding in face_encodings:
        matches = face_recognition.compare_faces(known_encodings, face_encoding)
        face_distances = face_recognition.face_distance(known_encodings, face_encoding)
        if len(face_distances) > 0:
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                return known_names[best_match_index]
    return "Unknown"

# ---------------- Test (optional) ----------------
if __name__ == "__main__":
    CAPTURE_FOLDER = "captures"
    for file in os.listdir(CAPTURE_FOLDER):
        if file.lower().endswith(".jpg"):
            path = os.path.join(CAPTURE_FOLDER, file)
            name = recognize_face_from_image(path)
            print(name)
