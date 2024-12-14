#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

#define TRIGGER_PIN 19

#define NEOPIXEL_PIN 48
#define NEOPIXEL_COUNT 1

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile uint32_t buttonHighSince = 0;
volatile uint32_t lastTriggerMicros = 0;
IRAM_ATTR void triggerIsr()
{
  if (micros() - lastTriggerMicros < 1000)
  {
    return;
  }

  lastTriggerMicros = micros(); // Debouncing

  if (!digitalRead(TRIGGER_PIN))
  {
    buttonHighSince = 0;
    return;
  }

  if (buttonHighSince == 0)
  {
    buttonHighSince = millis();
  }

  return;
}

void setup()
{
  Serial.begin(115200);

  strip.begin();
  strip.clear();
  strip.setBrightness(255);
  strip.show();

  strip.setPixelColor(0, 0x0000FF);
  strip.show();

  WiFi.begin("Omars", "12345678");
  while (WiFi.status() != WL_CONNECTED)
  {
    printf("Connecting...\n");
    delay(500);
  }

  printf("Connected\n");

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  attachInterrupt(TRIGGER_PIN, triggerIsr, CHANGE);
}

void loop()
{
  if (millis() - buttonHighSince >= 3000 && buttonHighSince != 0)
  {
    delay(100);

    printf("Sending request. Status: %d\n", digitalRead(TRIGGER_PIN));
    HTTPClient http;
    http.begin(String("https://emailtrigger.maisbilisim.com.tr/api/trigger?data=") + String(digitalRead(TRIGGER_PIN)));
    int code = http.GET();
    if (code < 0)
    {
      printf("HTTP failed.\n");
      http.end();
      return;
    }

    printf("HTTP request successful. Code: %d\n", code);

    String payload = http.getString();
    printf("Received payload: %s\n", payload.c_str());
    http.end();

    buttonHighSince = 0;
  }

  strip.setPixelColor(0, digitalRead(TRIGGER_PIN) ? 0xFF0000 : 0x00FF00);
  strip.show();

  delay(100);
}
