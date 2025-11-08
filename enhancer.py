import cv2
import numpy as np

def preprocess_esp32_image(img_path, target_size=(112,112), denoise=True, sharpen=True, sharpen_strength=1.5):
    """
    Enhance ESP32 image for recognition.

    Args:
        img_path (str): Path to ESP32 image
        target_size (tuple): Resize after enhancement
        denoise (bool): Apply denoising
        sharpen (bool): Apply sharpening
        sharpen_strength (float): Strength of sharpening

    Returns:
        img (numpy array): Enhanced RGB image
        sharpness (float): Variance of Laplacian (measure of sharpness)
    """
    img = cv2.imread(img_path)
    if img is None:
        raise FileNotFoundError(f"Image not found: {img_path}")

    # Convert to RGB
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

    # Denoise
    if denoise:
        img = cv2.fastNlMeansDenoisingColored(img, None, 10,10,7,21)

    # CLAHE
    img_yuv = cv2.cvtColor(img, cv2.COLOR_RGB2YUV)
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
    img_yuv[:,:,0] = clahe.apply(img_yuv[:,:,0])
    img = cv2.cvtColor(img_yuv, cv2.COLOR_YUV2RGB)

    # Sharpen
    if sharpen:
        kernel = np.array([[0, -1, 0], [-1, 5 * sharpen_strength, -1], [0, -1, 0]])
        img = cv2.filter2D(img, -1, kernel)

    # Resize
    if target_size:
        img = cv2.resize(img, target_size, interpolation=cv2.INTER_CUBIC)

    # Sharpness metric
    gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    sharpness = cv2.Laplacian(gray, cv2.CV_64F).var()

    return img, sharpness
