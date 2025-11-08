import os
import cv2
import face_recognition
import numpy as np
from enhancer import preprocess_esp32_image

KNOWN_FACES_DIR = "known_faces"
TARGET_SIZE = (112,112)

# ---------------- Load known faces ----------------
known_encodings = []
known_names = []

print("📚 Loading known faces...")
for filename in os.listdir(KNOWN_FACES_DIR):
    if filename.lower().endswith((".jpg", ".png")):
        path = os.path.join(KNOWN_FACES_DIR, filename)
        # Extract name: take everything before the first underscore
        name = os.path.splitext(filename)[0].split("_")[0]
        image = cv2.imread(path)
        rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        encodings = face_recognition.face_encodings(rgb)
        if encodings:
            known_encodings.append(encodings[0])
            known_names.append(name)
            print(f"✅ Loaded {name} from {filename}")
        else:
            print(f"⚠️ No face found in {filename}")

print("🎯 Ready for recognition!")

# ---------------- Recognition ----------------
def recognize_face_from_image(img_path):
    # Enhance image
    img, sharpness = preprocess_esp32_image(img_path, target_size=None)

    # Detect faces
    face_locations = face_recognition.face_locations(img)
    face_encodings = face_recognition.face_encodings(img, face_locations)

    results = []
    for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
        matches = face_recognition.compare_faces(known_encodings, face_encoding)
        name = "Unknown"

        # Find the best matching known face
        face_distances = face_recognition.face_distance(known_encodings, face_encoding)
        if len(face_distances) > 0:
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = known_names[best_match_index]

        results.append({"name": name, "location": (top, right, bottom, left)})

    return name


# ---------------- Run recognition on captures folder ----------------
if __name__ == "__main__":
    CAPTURE_FOLDER = "captures"
    for file in os.listdir(CAPTURE_FOLDER):
        if file.lower().endswith(".jpg"):
            path = os.path.join(CAPTURE_FOLDER, file)
            results = recognize_face_from_image(path)
            # Print just **unique names** per face
            names_seen = set([r['name'] for r in results])
            print(" ".join(names_seen) if names_seen else "None")
