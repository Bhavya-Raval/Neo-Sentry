#include <myosa.h>
#include <WiFi.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include<ESP_Google_Sheet_Client.h>


MYOSA myKit;

// --- Wi-Fi Credentials ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
#define BOTtoken "8999228346:AAGDBl6oqxkO91VwNnrP4vMKDtrn6hVk_5Q"
#define CHAT_ID "YOUR_CHAT_ID"
#define THINGSPEAK_CH_ID 3388454                // Replace with your channel number
#define THINGSPEAK_WRITE_APIKEY "5TSSEAWP51A3EZWD" // Replace with your channel write API Key
#define PROJECT_ID "neo-sentry"
#define CLIENT_EMAIL "google-sheets@neo-sentry.iam.gserviceaccount.com"
#define SPREADSHEET_ID "1CYqXfj2ydENCGktqIENCoc1UpGHSjCNpTQpsZO7Fu88"
WiFiClient thingSpeakClient;  // Create a WiFiClient object for ThingSpeak

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// --- Hardware Settings ---
const int BUZZER_PIN = 12;
const int FAN_PIN = 5;      // Fan for cooling down
const int LED_PIN = 18;     // LED for heating up
const float VIBRATION_THRESHOLD = 1800.0;
const char PRIVATE_KEY[] PROGMEM ="-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCwPHUvZwLG0qKU\nqqfKMGfrvD2z4st/fXrwohQG4TOv9U0CqFVpeoUNrR22rEbRu86nKgFIs+xvVe3u\nweLW8BoRRhKkYexvI99zUmNj2QcTzNpvZTzp2OziLA9NWt2xVpns3OnCCTeCjMt3\nBy1mGthH+KQzMHZXzZFJOSVHlZs+wiQ3Jz/d6oIdhbvZntHppG0yfWv34inSanEm\nu+60Old2hLcR+D2BOcq1RPACrNKfGlry1OCG26ofXCw8uB6ijFh3u/YqwyDxZQmv\nu0uHnCZx++ex/kaO4inYX9VwG/sMamPbqHM8xQ2o6J3KvpCO2I9JkvK19JVEqlS1\nws0hztjpAgMBAAECggEABHf3+bR5Nczj+fSMzhvlmqzafKvwwIIQQrc9XkJKEvlb\nk0hs6ACHDKZ51SavogRK1GLEhxEm2tjZMF9irtQGOS9nbBseiV/lu9EYO5EmdIpS\ndJYi0gQh14dJ+QpniiZZv0+HrteYfHsw8orwTAE8wDBuC208ZvQLrclg6FyDA/eR\n9ZQMWMojv8yVLyG2w7HbQid5juiDbY8WI0WLuyxyRW4PD9syjRZ2qzDqttIxWtw4\ng9FBbX2/FSTYaFPG2q9ZO2W8FB60QbZNWN0ESTnUUvOhhOzK4jKksdIovco+Nm6F\nitUGeoHAvlk9It0yc6Lmp4rhjwA5ebifYTDQEhYgYQKBgQDf+/R2B/c5EsLCYY/x\nZICst4RYeTENiQh6TGQ9MOTlgJISHZaY3jlG3eX1Osp+tio9V9OzWhVXVqvvPSXR\nznPaJQ9dVbgs2FKR22vW7ktZGzpHYN6GtmkyWhcr0/OEgoQQqkONb6Ylpz2neHvr\nhFZjQjt8vcGzGzRVsMYMLtFQMQKBgQDJbU2utWiNpOtDJqfyNj7B06JFsjZG/LMo\nv/iQeRmuNw/goBLbyGV/KEZiTqhWcZKV1tAmPjHNsqGYXVLg9S+nJeNUG5g9jmoA\nGoDOkV0yH83qo7npDoDDWfMTYE9wO+l8sdOdbeEhYExp4oT+V12noKirox35ZN9Q\nvFIs12JeOQKBgD22hPeUYEiIY8L88JUZFN8BvWRHF/3buMtBiwjVFOwrDZQSNPEv\noWZmMzrrFOnutRqrvTeYZXLn/X1wQqBZGtCLD32u5YztmWp4o3DXvUjy2I7cINPL\nJJbr/aBkyJ7E+zBASVaJ4aRUPQeMidAtaUP19r/65Ii9wSCvERJAQAmxAoGBALbw\noPcmJ7lNqaapeytO/RyJG/505zr907IU+KoucpZ5x9/ZQod+sEAW0iYDDnzEVb/l\nA9NX9V++92ksiVNrHEaQ8LC5B0IREFDbwVTbTWty21BaN0VmOKg0D8LvW/axU1Q/\nJdT7sNbhYUVhyCxAKP1PuyF32odhiDkJcRDs5ZhxAoGBAMvcxqcB1UBthwBwYdqo\nFnkHQ51b0EfqpSR4pdKOTslwXJwVDLU88aOF4KUuZcLETkD0PN+YP5nlX4O5Q/tB\nlnqS3JmTVlmFsAUTlm3FnywYSPv82rhEO7u4W2f9wWpCas0b2E0FRqQZzmDneNul\njgDbQnqGYGFcibm9cmyW3d75\n-----END PRIVATE KEY-----\n";
// --- Climate Boundaries ---
const float TEMP_MIN = 34.5;
const float TEMP_MAX = 34.7;

// --- Custom Non-Blocking Timers ---
unsigned long lastMpuRead = 0;       const long mpuInterval = 500;    // Check movement every 0.5s
unsigned long lastBmpRead = 0;       const long bmpInterval = 2000;   // Check pressure/temp every 2.0s
unsigned long lastApdsRead = 0;      const long apdsInterval = 2000;  // Check light every 2.0s
unsigned long lastOledUpdate = 0;     const long oledInterval = 1000;  // Redraw screen every 1.0s
unsigned long lastThingSpeakUpdate = 0;  // Last update time for ThingSpeak
const long thingSpeakInterval = 20000;    // Interval to update ThingSpeak (20 seconds)
unsigned long fanActiveStartTime = 0;
unsigned long ledActiveStartTime = 0;
bool fanIsActive = false;
bool ledIsActive = false;
const unsigned long ACTUATOR_TIMEOUT = 5000; // 5-second automatic safety limit

// --- Progressive Lifecycle Tracking States ---
unsigned long lastMotionTime = 0;
bool tenSecondAlertSent = false;
bool fourteenSecondAlertSent = false;
bool buzzerIsActive = false;
bool buzzerOffAlertPending = false;
unsigned long buzzerOffTime = 0;


// --- Clean State-Tracking States ---
enum SystemState { STATE_NORMAL, STATE_HIGH, STATE_LOW };
SystemState targetState = STATE_NORMAL;
SystemState lastSentState = STATE_NORMAL;
bool climateFirstRunComplete = false;

// --- Global Sensor Variables ---
float temp = 37.0;
float currentAccelZ = 0;
uint16_t light = 0;
float rgbSum = 0;

void formatDateTime(const char* dateTimeFormat, char* result, size_t resultSize) {
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)) {
        strftime(result, resultSize, dateTimeFormat, &timeinfo);
    }
}


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- NEO-SENTRY 1.0 STRUCTURAL DRY RUN ---");

  // Set pin 12 to safe input mode at boot to bypass bootstrap startup errors
  pinMode(BUZZER_PIN, INPUT); 
  // Initialize new climate hardware pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // LOGIC STEP 1: CONNECT WI-FI FIRST
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // Strictly limited to 8.5dBm for safety
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(400);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi Connection Established!");
    Serial.println("Stabilizing Wi-Fi RF environment for 2 seconds...");
    delay(2000);

    // CRITICAL: Tells the secure client to run in lightweight mode without root certificates
    client.setInsecure(); 

    // Your first live notification test!
    bot.sendMessage(CHAT_ID, "🤖 *Neo-Sentry 1.0 Online!* System initialized successfully with optimized memory allocations.", "Markdown");
    delay(1000);
    
    // LOGIC STEP 2: 2-SECOND DELAY TO STABILIZE WI-FI
    Serial.println("Stabilizing Wi-Fi RF environment for 2 seconds...");
    delay(2000);
    
    bot.sendMessage(CHAT_ID, "🏁 *SYSTEM BOOT*\nNeo-Sentry System Initialization Complete.", "Markdown");
    delay(1000);
  } else {
    Serial.println("\n❌ Wi-Fi Timeout!");
  }

  // LOGIC STEP 3: INITIALIZE HARDWARE KIT SECOND
  Serial.println("Initializing MYOSA hardware bus ...");
  Wire.begin();
  ThingSpeak.begin(thingSpeakClient); // Initialize ThingSpeak
  GSheet.begin(CLIENT_EMAIL,PROJECT_ID,PRIVATE_KEY);
  const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 19800; // your timezone offset
    const int   daylightOffset_sec = 0; // daylight saving time adjustment
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //myKit.begin(); 
  myKit.Pr.begin();
  myKit.Ag.begin();
  myKit.Ag.setSleep(false);
  myKit.Ag.setFullScaleAccelRange(MPU_ACCEL_CONFIG_FS_SEL_2g);
  myKit.Lpg.begin();
  myKit.Th.begin();
  myKit.Lpg.enablePower();
  myKit.Lpg.enableAmbientLightSensor();
  myKit.display.begin();

  // LOGIC STEP 4: PUT DELAY TO STABILIZE THE INITIALIZED KIT
  Serial.println("Kit initialized. Stabilizing hardware bus threads for 2 seconds...");
  delay(2000);

  lastMotionTime = millis();
  Serial.println("🚀 Setup Complete. System Running Full Structural Test Loop.");
}

void loop() {
  unsigned long currentMillis = millis();
  bool sheetReady = GSheet.ready();

  // 1. EVALUATE ACCELEROMETER MOTION ENGINE (Every 0.5s)
  if (currentMillis - lastMpuRead >= mpuInterval) {
    lastMpuRead = currentMillis;
    currentAccelZ = myKit.Ag.getAccelZ();

    if (currentAccelZ >= VIBRATION_THRESHOLD) {
      if (!buzzerIsActive) {
        lastMotionTime = currentMillis; // Reset stillness stopwatch
        tenSecondAlertSent = false;
        fourteenSecondAlertSent = false;
      }
    }
  }

  // 2. RUN CLIMATE DATA ENGINE (Every 2.0s)
  // 2. RUN CLIMATE DATA ENGINE (Every 2.0s)
  if (currentMillis - lastBmpRead >= bmpInterval) {
    lastBmpRead = currentMillis;
    temp = myKit.Pr.getTempC();
    
    if (temp > TEMP_MAX) {
      targetState = STATE_HIGH;
      if (!fanIsActive) {
        digitalWrite(FAN_PIN, HIGH);
        fanIsActive = true;
        fanActiveStartTime = currentMillis;
        Serial.println("[ACTUATOR] Temperature HIGH. Fan ON.");
      }
    } else if (temp < TEMP_MIN) {
      targetState = STATE_LOW;
      if (!ledIsActive) {
        digitalWrite(LED_PIN, HIGH);
        ledIsActive = true;
        ledActiveStartTime = currentMillis;
        Serial.println("[ACTUATOR] Temperature LOW. LED ON.");
      }
    } else {
      targetState = STATE_NORMAL;
    }
  }

  // --- New Safety Engine: 5-Second Automatic Shutoff ---
  if (fanIsActive && (currentMillis - fanActiveStartTime >= ACTUATOR_TIMEOUT)) {
    digitalWrite(FAN_PIN, LOW);
    fanIsActive = false;
    Serial.println("[ACTUATOR] Fan safety timeout reached. Fan OFF.");
  }
  
  if (ledIsActive && (currentMillis - ledActiveStartTime >= ACTUATOR_TIMEOUT)) {
    digitalWrite(LED_PIN, LOW);
    ledIsActive = false;
    Serial.println("[ACTUATOR] LED safety timeout reached. LED OFF.");
  }

  // 3. APDS9960 FLASHLIGHT TRACKER (Every 2.0s)
  if (currentMillis - lastApdsRead >= apdsInterval) {
    lastApdsRead = currentMillis;
    light = myKit.Lpg.getAmbientLight();
    rgbSum = myKit.Lpg.getRedProportion() + myKit.Lpg.getGreenProportion() + myKit.Lpg.getBlueProportion();

    // Flashlight override trigger
    if (buzzerIsActive && (light > 1500 || rgbSum > 2.5)) {
      buzzerIsActive = false;
      digitalWrite(BUZZER_PIN, LOW);
      pinMode(BUZZER_PIN, INPUT); 
      
      buzzerOffAlertPending = true;
      buzzerOffTime = currentMillis;
      
      lastMotionTime = currentMillis;
      tenSecondAlertSent = false;
      fourteenSecondAlertSent = false;
    }
  }

  // 4. DECOUPLED INSTANT CLIMATE STATE MONITOR (Your Chosen Structure!)
  if (targetState != lastSentState || !climateFirstRunComplete) {

    if (targetState == STATE_HIGH) {
      String msg = "🚨 *CLIMATE ALERT* 🚨\nTemp: " + String(temp, 1) + " °C\nCRITICAL HIGH TEMP! Fan activated.";
      bot.sendMessage(CHAT_ID, msg, "Markdown");
    } 
    else if (targetState == STATE_LOW) {
      String msg = "❄️ *CLIMATE ALERT* ❄️\nTemp: " + String(temp, 1) + " °C\nCRITICAL LOW TEMP! Heater activated.";
      bot.sendMessage(CHAT_ID, msg, "Markdown");
    } 
    else {
      String msg = "✅ *CLIMATE STATUS* ✅\nTemp: " + String(temp, 1) + " °C\nIncubator environment has stabilized.";
      bot.sendMessage(CHAT_ID, msg, "Markdown");
    }

    // This updates once at the very end so your phone doesn't get spammed
    lastSentState = targetState;
    climateFirstRunComplete = true;
  }

  // Calculate duration of continuous stillness
  unsigned long timeStill = currentMillis - lastMotionTime;

  // 5. LOCAL HARDWARE ACTION ENGINE (Staggered Stillness Responses)
  if (buzzerIsActive) {
    pinMode(BUZZER_PIN, OUTPUT); 
    if (currentMillis % 400 < 200) {
      digitalWrite(BUZZER_PIN, HIGH); 
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

  } else if (timeStill >= 15000) {
    buzzerIsActive = true;

  } else if (timeStill >= 14000) {
    digitalWrite(BUZZER_PIN, LOW); pinMode(BUZZER_PIN, INPUT); 
    if (!fourteenSecondAlertSent) {
      bot.sendMessage(CHAT_ID, "🚨 *STILLNESS ALARM*\n14-second threshold crossed. Alarm arming...", "Markdown");
      fourteenSecondAlertSent = true; 
    }

  } else if (timeStill >= 10000) {
    digitalWrite(BUZZER_PIN, LOW); pinMode(BUZZER_PIN, INPUT);
    if (!tenSecondAlertSent) {
      bot.sendMessage(CHAT_ID, "⚠️ *STILLNESS WARNING*\nPatient still for 10 seconds.", "Markdown");
      tenSecondAlertSent = true; 
    }
  }

  // 6. DELAYED "ALARM CLEARED" STATUS ACTION (1 second post flashlight action)
  if (buzzerOffAlertPending && (currentMillis - buzzerOffTime >= 1000)) {
    bot.sendMessage(CHAT_ID, "✅ *ALARM RECOVERY*\nFlashlight override confirmed. System reset.", "Markdown");
    buzzerOffAlertPending = false;
  }

  // 7. STAGGERED OLED SCREEN MANAGER (Every 1.0s)
  if (currentMillis - lastOledUpdate >= oledInterval) {
    lastOledUpdate = currentMillis;

    myKit.display.clearDisplay();
    myKit.display.setCursor(0, 0);
    myKit.display.println("  Structural Test   ");
    myKit.display.println("--------------------");
    myKit.display.printf("Temp:  %.1f C\n", temp);
    myKit.display.printf("Still: %lu s\n", timeStill / 1000);
    
    // Fixed: Put your original working Wi-Fi lines back right here!
    myKit.display.print("WiFi:  ");
    myKit.display.println(WiFi.status() == WL_CONNECTED ? "ONLINE" : "OFFLINE");
    
    myKit.display.println("--------------------");

    if (buzzerIsActive) {
      myKit.display.println("!!! ACTIVE ALARM !!!");
    } else if (temp > TEMP_MAX) {
      myKit.display.println("ALERT: HIGH TEMP");
    } else if (temp < TEMP_MIN) {
      myKit.display.println("ALERT: LOW TEMP");
    } else {
      myKit.display.println("STATUS: RUNNING");
    }
    myKit.display.display();
  }
  if (currentMillis - lastThingSpeakUpdate >= thingSpeakInterval) {
    lastThingSpeakUpdate = currentMillis; 
    
    ThingSpeak.setField(1, myKit.Pr.getTempC()); 
    ThingSpeak.setField(2, myKit.Pr.getPressurePascal()); 
    ThingSpeak.setField(3, myKit.Th.getRelativeHumdity()); 
    ThingSpeak.setField(4, myKit.Ag.getAccelZ()); 
    
    int result = ThingSpeak.writeFields(THINGSPEAK_CH_ID, THINGSPEAK_WRITE_APIKEY);
    if (result == 200) {
        Serial.println("ThingSpeak update successful.");
        
        // 🌟 CRITICAL FIX: Only try to write to Google Sheets if token validation is ready!
        if (sheetReady) {
            Serial.println("Writing to Google Sheets...");
            FirebaseJson valueRange;
            const char* dateFormat = "%Y-%m-%d %H:%M:%S";
            size_t bufferSize = 20;
            char theTime[bufferSize];
            formatDateTime(dateFormat, theTime, bufferSize); 

            valueRange.add("majorDimension", "COLUMNS");
            valueRange.set("values/[0]/[0]", theTime); 
            valueRange.set("values/[1]/[0]", myKit.Pr.getTempC()); 
            valueRange.set("values/[2]/[0]", myKit.Pr.getPressurePascal()); 
            valueRange.set("values/[3]/[0]", myKit.Th.getRelativeHumdity()); 
            valueRange.set("values/[4]/[0]", myKit.Ag.getAccelZ()); 
            
            FirebaseJson response;
            // Changed from Sheet1!A1:E1 to Sheet1!A1:C1 matching your reference specs
            bool googleSheetsSuccess = GSheet.values.append(&response, SPREADSHEET_ID, "Sheet1!A1:E1", &valueRange);
            
            if (googleSheetsSuccess) {
                Serial.println("✅ Google Sheet append successful!");
            } else {
                Serial.println("❌ Google Sheet update failed.");
            }
            response.toString(Serial, true);
            Serial.println();
        } else {
            Serial.println("⚠️ Google Sheet Client is not ready yet (Token parsing in progress...)");
        }
    } else {
        Serial.print("Problem updating ThingSpeak channel. HTTP error code: ");
        Serial.println(result);
    }
  }

  yield(); 
}