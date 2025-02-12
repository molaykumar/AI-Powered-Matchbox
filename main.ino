#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Base64.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// OLED Display Configuration
#define OLED_SDA 9
#define OLED_SCL 8
#define OLED_RST -1
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

// Button Configuration
#define BUTTON_PIN 2  // Change if needed

// Camera Pins Configuration
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13
#define LED_GPIO_NUM      21

// WiFi Credentials
const char* ssid = "*****";
const char* password = "******";


// API Configuration
const char* apiKey = "API key";  
const char* apiUrl = "https://api.openai.com/v1/chat/completions";

// Function to show startup animation
void startupAnimation() {
    display.clearDisplay();

    // Display "Welcome" at the center
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(30, 20);
    display.println("Welcome");
    display.display();
    delay(1000);

    // Clear and show "AI-powered Matchbox"
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5, 20);
    display.println("AI-powered");
    display.setCursor(20, 45);
    display.println("Matchbox");
    display.display();
    delay(2000);

    display.clearDisplay();
}

// Function to display status messages
void displayStatus(const String& message) {
    Serial.println("[OLED] " + message);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(5, 5);
    display.println("AI-Powered Matchbox");
    display.drawRect(5, 15, 118, 20, SH110X_WHITE);
    display.setCursor(10, 22);
    display.println(message);
    display.display();
}

// Function to display results
void displayResult(const String& result) {
    Serial.println("[RESULT] " + result);
    display.clearDisplay();

    // Title
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(30, 5);
    display.println("RESULT:");

    // Draw frame
    display.drawRect(5, 15, 118, 40, SH110X_WHITE);

    // Result text
    display.setTextSize(2);
    display.setCursor(5, 30);
    display.println(result);

    display.display();
}

// Function to set up the camera
void setupCamera() {
    Serial.println("[INFO] Initializing Camera...");
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_QQVGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 15;
    config.fb_count = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.println("[ERROR] Camera Init Failed!");
        displayStatus("Cam Init Failed!");
        while (1);
    }
    Serial.println("[SUCCESS] Camera Initialized.");
}

// Function to capture an image and send it to the API
String processImage() {
    Serial.println("[INFO] Capturing Image...");
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[ERROR] Image Capture Failed!");
        displayStatus("Capture Failed!");
        return "";
    }

    Serial.println("[INFO] Encoding Image to Base64...");
    String base64Image = base64::encode(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    Serial.println("[INFO] Sending Image to API...");
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));

    DynamicJsonDocument doc(4096);
    doc["model"] = "gpt-4o";
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject message = messages.createNestedObject();
    message["role"] = "user";
    JsonArray content = message.createNestedArray("content");

    JsonObject textContent = content.createNestedObject();
    textContent["type"] = "text";
    textContent["text"] = "Identify in one word.";

    JsonObject imageContent = content.createNestedObject();
    imageContent["type"] = "image_url";
    imageContent["image_url"]["url"] = "data:image/jpeg;base64," + base64Image;

    doc["max_tokens"] = 10;

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    int httpCode = http.POST(jsonPayload);
    String result = "Error";

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("[INFO] API Response Received.");
        DynamicJsonDocument resDoc(1024);
        deserializeJson(resDoc, payload);
        result = resDoc["choices"][0]["message"]["content"].as<String>();
    } else {
        Serial.println("[ERROR] API Request Failed. HTTP Code: " + String(httpCode));
        result = "HTTP Error: " + String(httpCode);
    }

    http.end();
    return result;
}

void setup() {
    Serial.begin(115200);
    Serial.println("[INFO] Starting ESP32-S3...");

    Wire.begin(OLED_SDA, OLED_SCL);
    display.begin(0x3C, true);
    
    startupAnimation();
    
    displayStatus("Initializing...");
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    setupCamera();

    Serial.println("[INFO] Connecting to WiFi...");
    displayStatus("Connecting WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        displayStatus("Connecting...");
    }

    Serial.println("\n[SUCCESS] WiFi Connected.");
    displayStatus("Click, To capture");
}

void loop() {
    static unsigned long lastPress = 0;

    if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 2000) {
        Serial.println("[INFO] Button Pressed. Capturing Image...");
        lastPress = millis();

        displayStatus("Capturing...");
        String result = processImage();

        if (result.length() > 0) {
            displayResult(result);
        } else {
            displayStatus("API Error");
        }

        delay(3000);
        displayStatus("Press Button");
    }
}
