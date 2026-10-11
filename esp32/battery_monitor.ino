c++
  //ESP32 EV battery monitoring 
#define BLYNK_TEMPLATE_ID "TMPL3Mx3DaRCl"
#define BLYNK_TEMPLATE_NAME "battery monitoring"
#define BLYNK_AUTH_TOKEN "YBLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "WIFI_NAME";
char pass[] = "PASSWORD";

const int BATTERY_PIN = 34;

const float R1 = 100000.0;
const float R2 = 27000.0;

const float LOW_BATTERY = 9.0;
const float RECOVERY_VOLTAGE = 9.3;

bool lowBatterySent = false;

BlynkTimer timer;

float readBatteryVoltage()
{
  const int samples = 20;
  uint32_t totalMilliVolts = 0;

  for (int i = 0; i < samples; i++)
  {
    totalMilliVolts += analogReadMilliVolts(BATTERY_PIN);
    delay(2);
  }

  float adcVoltage =
      (totalMilliVolts / (float)samples) / 1000.0;

  return adcVoltage * ((R1 + R2) / R2);
}

void sendBatteryData()
{
  float batteryVoltage = readBatteryVoltage();

  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.println(" V");

  if (Blynk.connected())
  {
    Blynk.virtualWrite(V0, batteryVoltage);

    if (batteryVoltage > 2.0)
      Blynk.virtualWrite(V4, "BATTERY CONNECTED");
    else
      Blynk.virtualWrite(V4, "BATTERY DISCONNECTED");

    if (batteryVoltage <= LOW_BATTERY && !lowBatterySent)
    {
      String message = "Battery low: ";
      message += String(batteryVoltage, 2);
      message += " V";

      Blynk.logEvent("low_battery", message);
      lowBatterySent = true;
    }
  }

  if (batteryVoltage >= RECOVERY_VOLTAGE)
    lowBatterySent = false;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 STARTING...");

  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_PIN, ADC_11db);

  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, pass);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 15000)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Wi-Fi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Blynk.config(BLYNK_AUTH_TOKEN);

    Serial.println("Trying Blynk...");

    if (Blynk.connect(5000))
    {
      Serial.println("Blynk connected!");
    }
    else
    {
      Serial.println("Blynk connection failed.");
      Serial.println("Battery monitoring will continue.");
    }
  }
  else
  {
    Serial.println("Wi-Fi connection failed.");
  }

  timer.setInterval(2000L, sendBatteryData);

  Serial.println("System ready!");
}

void loop()
{
  Blynk.run();
  timer.run();
}
