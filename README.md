# AI-Powered-Matchbox

AI-Powered Matchbox: The Smallest AI Image Identifier (Part 1)
🚀 An ESP32-S3-based matchbox-sized AI device for real-time image recognition.

Overview
This project is an AI-powered matchbox that captures images using an ESP32-S3, processes them via an AI model, and displays the results on a 1.3” SH1106 OLED screen. It’s one of the smallest AI-powered vision devices, making it portable and easy to integrate into various applications.

This is Part One of the project, where I focus on:
✔️ Capturing images using an ESP32-S3 camera module
✔️ Processing images with OpenAI’s GPT-4o API
✔️ Displaying results on a 1.3” OLED screen
✔️ Creating a sleek user interface with animations and borders

Future parts will add more features, including offline AI processing, improved image recognition, and expanded use cases. 🚀

Hardware Components
Xiao ESP32-S3 🖥️
OV2640 Camera Module 📷
1.3” SH1106 OLED Display 🖥️
Push Button for Image Capture 🔘
3.7V Li-ion Battery (Apple Watch Series 9 Battery used here) 🔋
TP4056 Battery Charging Module ⚡
3D-Printed/Matchbox Enclosure 📦
How It Works
1️⃣ Press the button to capture an image
2️⃣ The ESP32-S3 sends the image to OpenAI’s GPT-4o API
3️⃣ The AI analyzes the image and returns a one-word result
4️⃣ The result is displayed on the OLED screen with a stylish UI
5️⃣ The system waits for the next input

Software & Libraries
ESP32 Camera Library (for image capture)
WiFi & HTTPClient (to send images to API)
Base64 Encoding (to format image data)
ArduinoJson (to handle API responses)
Adafruit GFX & SH110X Libraries (for OLED graphics)
