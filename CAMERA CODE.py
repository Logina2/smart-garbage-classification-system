import cv2
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import load_model
from tensorflow.keras.applications.resnet50 import preprocess_input

IMG_SIZE = (224, 224)
class_names = ['cardboard', 'glass', 'metal', 'paper', 'plastic', 'trash']

model = load_model("final_model .h5")

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Unable to access the camera.")
    exit()

print("Camera opened successfully. Preparing to capture...")
cv2.waitKey(2000)  # Wait 2 seconds before capturing

ret, frame = cap.read()
cap.release()

if not ret:
    print("Failed to capture image.")
    exit()

# Display the captured image
cv2.imshow("Captured Image", frame)
cv2.waitKey(2000)
cv2.destroyAllWindows()

# Preprocess the image
img_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
img_resized = cv2.resize(img_rgb, IMG_SIZE)
img_array = np.expand_dims(img_resized.astype(np.float32), axis=0)
img_array = preprocess_input(img_array)

# Predict
predictions = model.predict(img_array)
class_idx = np.argmax(predictions)
confidence = predictions[0][class_idx]

# Show the result
print(f"Predicted class: {class_names[class_idx]} ({confidence * 100:.2f}%)")
