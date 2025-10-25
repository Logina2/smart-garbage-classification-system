import requests 
import json 
# add ai model to return a number between 0 and 6 
# 1 -> trash 
# 2 -> glass 
# 3 -> plastic 
# 4 -> paper 
# 5 -> cardboard 
# 6 -> metal 
# Replace with the actual IP of your ESP32 
esp32_ip = "192.168.4.1"  # Default when using softAP 
url = f"http://{esp32_ip}/control" 
data = { 
"gpioValue": 6  # Change to the integer you want to send 
} 
headers = {'Content-Type': 'application/json'} 
try: 
response = requests.post(url, data=json.dumps(data), headers=headers) 
print("Response Code:", response.status_code) 
print("Response Body:", response.text) 
except requests.exceptions.RequestException as e: 
print("Error communicating with ESP32:", e)