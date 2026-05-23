#include "myosa.h"
#include <WiFi.h>
#include <Wire.h>

MYOSA myKit;

// --- Self-Hosted Local Wi-Fi Credentials ---
const char* ap_ssid     = "Neo-Sentry-Hub";
const char* ap_password = "password123"; 

WiFiServer server(80);

// --- Hardware Settings ---
const int BUZZER_PIN = 12;
const int FAN_PIN = 5;      //  Fan for cooling down
const int LED_PIN = 18;     //  LED for heating up
const float VIBRATION_THRESHOLD = 1800.0;

// --- Climate Boundaries ---
const float TEMP_MIN = 34.5;
const float TEMP_MAX = 34.7;

// --- Custom Non-Blocking Timers ---
unsigned long lastMpuRead = 0;       const long mpuInterval = 500;    
unsigned long lastBmpRead = 0;       const long bmpInterval = 2000;   
unsigned long lastApdsRead = 0;      const long apdsInterval = 2000;  
unsigned long lastOledUpdate = 0;     const long oledInterval = 1000;  
unsigned long fanActiveStartTime = 0; 
unsigned long ledActiveStartTime = 0;
const unsigned long ACTUATOR_TIMEOUT = 5000;

// --- Progressive Lifecycle Tracking States ---
unsigned long lastMotionTime = 0;
bool buzzerIsActive = false;
bool fanIsActive = false;
bool ledIsActive = false;

// --- Global Sensor Variables ---
float temp = 37.0;
float currentAccelZ = 0;
uint16_t light = 0;
float rgbSum = 0;
uint16_t eco2Val = 450; 

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- NEO-SENTRY: CLINICAL ANALYTICS MIGRATION ---");

  pinMode(BUZZER_PIN, INPUT); 
  // Initialize new hardware pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); 
  WiFi.softAP(ap_ssid, ap_password);
  
  delay(2000);
  server.begin();

  Wire.begin();
  myKit.Pr.begin();
  myKit.Ag.begin();
  myKit.Ag.setSleep(false);
  myKit.Ag.setFullScaleAccelRange(MPU_ACCEL_CONFIG_FS_SEL_2g);
  myKit.Lpg.begin();
  myKit.Th.begin();
  myKit.Lpg.enablePower();
  myKit.Lpg.enableAmbientLightSensor();
  myKit.display.begin();

  delay(2000);
  lastMotionTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. EVALUATE ACCELEROMETER MOTION ENGINE
  if (currentMillis - lastMpuRead >= mpuInterval) {
    lastMpuRead = currentMillis;
    currentAccelZ = myKit.Ag.getAccelZ();
    if (currentAccelZ >= VIBRATION_THRESHOLD) {
      if (!buzzerIsActive) {
        lastMotionTime = currentMillis; 
      }
    }
  }

  // 2. RUN CLIMATE DATA ENGINE
 if (currentMillis - lastBmpRead >= bmpInterval) {
    lastBmpRead = currentMillis;
    temp = myKit.Pr.getTempC();
    
    // Evaluate temperature triggers
    // 2. RUN CLIMATE DATA ENGINE
  if (currentMillis - lastBmpRead >= bmpInterval) {
    lastBmpRead = currentMillis;
    temp = myKit.Pr.getTempC();
    
    // Evaluate temperature triggers
    if (temp > TEMP_MAX) {
      if (!fanIsActive) {
        digitalWrite(FAN_PIN, HIGH);
        fanIsActive = true;
        fanActiveStartTime = currentMillis;
        Serial.println("[ACTUATOR] Temperature HIGH. Fan ON.");
      }
    } 
    else if (temp < TEMP_MIN) {
      if (!ledIsActive) {
        digitalWrite(LED_PIN, HIGH);
        ledIsActive = true;
        ledActiveStartTime = currentMillis;
        Serial.println("[ACTUATOR] Temperature LOW. LED ON.");
      }
    }

    // Calculate normalized eco2 scale derived from environmental tracking vectors
    eco2Val = 400 + (int)(myKit.Th.getRelativeHumdity() * 2) + (int)(abs(currentAccelZ) / 300);
    if(eco2Val > 2000) eco2Val = 1910;
  }

  // --- Safety Engine: 5-Second Automatic Shutoff ---
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
  
  // 3. APDS9960 FLASHLIGHT TRACKER
  if (currentMillis - lastApdsRead >= apdsInterval) {
    lastApdsRead = currentMillis;
    light = myKit.Lpg.getAmbientLight();
    rgbSum = myKit.Lpg.getRedProportion() + myKit.Lpg.getGreenProportion() + myKit.Lpg.getBlueProportion();

    if (buzzerIsActive && (light > 1500 || rgbSum > 2.5)) {
      buzzerIsActive = false;
      digitalWrite(BUZZER_PIN, LOW);
      pinMode(BUZZER_PIN, INPUT); 
      lastMotionTime = currentMillis;
    }
  }

  unsigned long timeStill = currentMillis - lastMotionTime;

  // 4. LOCAL HARDWARE ACTION ENGINE
  if (buzzerIsActive) {
    pinMode(BUZZER_PIN, OUTPUT); 
    if (currentMillis % 400 < 200) {
      digitalWrite(BUZZER_PIN, HIGH); 
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else if (timeStill >= 15000) {
    buzzerIsActive = true; 
  }

  // 5. STAGGERED OLED SCREEN MANAGER
  if (currentMillis - lastOledUpdate >= oledInterval) {
    lastOledUpdate = currentMillis;
    myKit.display.clearDisplay();
    myKit.display.setCursor(0, 0);
    myKit.display.println("   Neo-Sentry AP    ");
    myKit.display.println("--------------------");
    myKit.display.printf("Temp:  %.1f C\n", temp);
    myKit.display.printf("Still: %lu s\n", timeStill / 1000);
    myKit.display.print("IP: 192.168.4.1\n");
    myKit.display.display();
  }

  // 6. LOCAL WEB DASHBOARD ENGINE
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            
            // HTTP JSON API Route
            if (request.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.print("{\"temperature\":" + String(temp, 1) + ",");
              client.print("\"stillness\":" + String(timeStill / 1000) + ",");
              client.print("\"pressure\":" + String(myKit.Pr.getPressurePascal() / 100.0, 0) + ","); 
              client.print("\"humidity\":" + String(myKit.Th.getRelativeHumdity(), 1) + ",");
              client.print("\"eco2\":" + String(eco2Val) + ","); 
              client.print("\"accelZ\":" + String(currentAccelZ / 100.0, 1) + ","); // Scaled to readable coordinate bounds
              client.print("\"alarm\":" + String(buzzerIsActive ? "true" : "false") + ",");
              
              String statusStr = "Stable";
              if (buzzerIsActive) statusStr = "Critical";
              else if (temp > TEMP_MAX || temp < TEMP_MIN) statusStr = "Unstable";
              
              client.print("\"status\":\"" + statusStr + "\"}");
            } 
            // Premium Clinical + Advanced Real-Time Analytics UI Route
            else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html; charset=utf-8"); 
              client.println("Connection: close");
              client.println();
              
              client.println("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>");
              client.println("<title>iNICU Monitor Control Hub</title>");
              client.println("<style>");
              client.println("body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; background: #f4f7fa; color: #334155; padding: 12px; margin: 0; }");
              client.println(".wrapper { max-width: 540px; margin: 0 auto; background: #ffffff; border-radius: 16px; box-shadow: 0 10px 25px rgba(0,0,0,0.05); overflow: hidden; border: 1px solid #e2e8f0; }");
              
              // Top Medical Header Styling
              client.println(".navbar { background: #ffffff; padding: 14px 20px; border-bottom: 2px solid #edf2f7; display: flex; align-items: center; justify-content: space-between; }");
              client.println(".navbar-brand { font-size: 20px; font-weight: 800; color: #2563eb; display: flex; align-items: center; gap: 6px; }");
              client.println(".navbar-menu { font-size: 11px; font-weight: 600; color: #64748b; letter-spacing: 0.5px; text-transform: uppercase; }");
              
              client.println(".main-panel { padding: 20px; }");
              client.println(".bed-card { background: #ffffff; border: 1px solid #e2e8f0; border-radius: 14px; padding: 16px; text-align: center; margin-bottom: 20px; position: relative; }");
              client.println(".bed-title { font-size: 16px; font-weight: 700; color: #1e293b; margin: 0; }");
              client.println(".bed-meta { font-size: 11px; color: #64748b; margin: 2px 0 12px 0; }");
              
              // Status Pill Indicators
              client.println(".status-pill { padding: 6px 18px; border-radius: 20px; font-weight: 700; font-size: 11px; display: inline-block; text-transform: uppercase; letter-spacing: 0.5px; border: 1px solid transparent; }");
              client.println(".pill-stable { background: rgba(16, 185, 129, 0.12); color: #10b981; border-color: rgba(16, 185, 129, 0.2); }");
              client.println(".pill-warning { background: rgba(245, 158, 11, 0.12); color: #f59e0b; border-color: rgba(245, 158, 11, 0.2); }");
              client.println(".pill-critical { background: rgba(239, 68, 68, 0.12); color: #ef4444; border-color: rgba(239, 68, 68, 0.3); animation: pulse 1.5s infinite; }");
              
              // Grid Mapping Parameters
              client.println(".metric-matrix { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 16px; }");
              client.println(".metric-box { background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 12px; padding: 12px 14px; text-align: left; position: relative; }");
              client.println(".box-danger { border-color: #fca5a5; background: #fff5f5; }");
              client.println(".metric-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 4px; }");
              client.println(".metric-label { font-size: 10px; font-weight: 700; color: #64748b; text-transform: uppercase; }");
              client.println(".metric-value { font-size: 24px; font-weight: 800; color: #0f172a; display: flex; align-items: baseline; }");
              client.println(".metric-unit { font-size: 12px; font-weight: 600; color: #94a3b8; margin-left: 3px; }");
              client.println(".metric-limit { font-size: 9px; color: #94a3b8; margin-top: 4px; }");
              
              // Embedded Vector Icons Style
              client.println(".medical-icon { width: 18px; height: 18px; display: inline-block; vertical-align: middle; }");
              
              // Analytics Real-Time Graphics Container Panels Layout
              client.println(".analytics-panel { background: #ffffff; border: 1px solid #e2e8f0; border-radius: 14px; padding: 16px; margin-bottom: 16px; }");
              client.println(".analytics-title { font-size: 11px; font-weight: 700; color: #1e293b; text-transform: uppercase; letter-spacing: 0.5px; margin-bottom: 10px; display: flex; justify-content: space-between; align-items: center; }");
              client.println(".canvas-box { width: 100%; height: 110px; background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 8px; overflow: hidden; }");
              client.println("canvas { width: 100%; height: 100%; display: block; }");
              
              // Footer Logger Panel
              client.println(".log-section { background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 12px; padding: 12px; }");
              client.println(".log-title { font-size: 10px; font-weight: 700; color: #475569; text-transform: uppercase; margin-bottom: 6px; }");
              client.println(".log-row { display: flex; justify-content: space-between; font-size: 11px; padding: 5px 0; border-bottom: 1px solid #e2e8f0; color: #334155; }");
              client.println(".log-row:last-child { border-bottom: none; }");
              client.println(".log-time { color: #94a3b8; font-weight: 600; }");
              client.println(".log-tag { font-weight: 700; text-transform: uppercase; font-size: 9px; padding: 1px 5px; border-radius: 3px; }");
              client.println("@keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.6; } 100% { opacity: 1; } }");
              client.println("</style></head><body>");
              
              // Interface Layout Shell Delivery
              client.println("<div class='wrapper'><div class='navbar'>");
              client.println("<div class='navbar-brand'>👶 iNICU Central Dashboard</div>");
              client.println("<div class='navbar-menu'>Room No. 1</div></div>");
              
              client.println("<div class='main-panel'><div class='bed-card'>");
              client.println("<h2 class='bed-title'>Incubator Unit Station 1</h2>");
              client.println("<p class='bed-meta'>Ecosystem Telemetry Matrix &bull; Continuous Medical Diagnostics</p>");
              client.println("<div id='pPill' class='status-pill pill-stable'>Stable</div></div>");
              
              // 2x2 Parameters Panel Matrix with Embedded Medical Symbols (SVGs)
              client.println("<div class='metric-matrix'>");
              
              // Block 1: Temperature with Medical Thermometer Symbol
              client.println("<div class='metric-box' id='tBox'><div class='metric-header'><div class='metric-label'>HR / TEMP</div>");
              client.println("<svg class='medical-icon' viewBox='0 0 24 24' fill='none' stroke='#2563eb' stroke-width='2.5'><path d='M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 1 1 4 0Z'/></svg></div>");
              client.println("<div class='metric-value'><span id='tV'>--.-</span><span class='metric-unit'>°C</span></div><div class='metric-limit'>Target: 36.5 - 37.5</div></div>");
              
              // Block 2: eCO2 with Gas / Respiration Symbol
              client.println("<div class='metric-box' id='eco2Box'><div class='metric-header'><div class='metric-label'>eCO2 LEVEL</div>");
              client.println("<svg class='medical-icon' viewBox='0 0 24 24' fill='none' stroke='#10b981' stroke-width='2.5'><path d='M12 2a3 3 0 0 0-3 3v7a3 3 0 0 0 6 0V5a3 3 0 0 0-3-3Z'/><path d='M19 10v1a7 7 0 0 1-14 0v-1'/><line x1='12' y1='19' x2='12' y2='22'/></svg></div>");
              client.println("<div class='metric-value'><span id='eco2V'>---</span><span class='metric-unit'>ppm</span></div><div class='metric-limit'>Limit: Normal &lt; 1000</div></div>");
              
              // Block 3: Stillness Timer with Watch Symbol
              client.println("<div class='metric-box' id='sBox'><div class='metric-header'><div class='metric-label'>⏳ STILLNESS</div>");
              client.println("<svg class='medical-icon' viewBox='0 0 24 24' fill='none' stroke='#ef4444' stroke-width='2.5'><circle cx='12' cy='12' r='10'/><polyline points='12 6 12 12 16 14'/></svg></div>");
              client.println("<div class='metric-value'><span id='sV'>--</span><span class='metric-unit'>sec</span></div><div class='metric-limit'>Threshold: Max 15s</div></div>");
              
              // Block 4: Humidity with Fluid Drop Symbol
              client.println("<div class='metric-box'><div class='metric-header'><div class='metric-label'>💧 HUMIDITY</div>");
              client.println("<svg class='medical-icon' viewBox='0 0 24 24' fill='none' stroke='#06b6d4' stroke-width='2.5'><path d='M12 2s8 7.5 8 11a8 8 0 1 1-16 0c0-3.5 8-11 8-11Z'/></svg></div>");
              client.println("<div class='metric-value'><span id='hV'>--.-</span><span class='metric-unit'>%</span></div><div class='metric-limit'>Atmosphere balance content</div></div>");
              
              client.println("</div>");

              // Expanded secondary multi grid layer blocks
              client.println("<div class='metric-matrix' style='grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 20px;'>");
              client.println("<div class='metric-box'><div class='metric-label'>🌀 Pressure</div><div class='metric-value'><span id='pV'>----</span><span class='metric-unit'>hPa</span></div></div>");
              client.println("<div class='metric-box'><div class='metric-label'>📉 Motion Vector (Z)</div><div class='metric-value'><span id='azV'>-.-</span><span class='metric-unit'>g</span></div></div></div>");
              
              // 📊 THREE SEPARATE ANALYTICS GRAPH CHANNELS
              // Graph Card 1: Core Climate Trends
              client.println("<div class='analytics-panel'><div class='analytics-title'>🌡️ Temperature & Stillness Trends <span style='color:#64748b; font-size:9px;'>Blue: Temp | Red: Still</span></div>");
              client.println("<div class='canvas-box'><canvas id='climateGraph'></canvas></div></div>");
              
              // Graph Card 2: eCO2 Atmosphere Trends
              client.println("<div class='analytics-panel'><div class='analytics-title'>🌬️ Gas Concentration Analytics <span style='color:#10b981; font-weight:800;'>eCO2 (ppm)</span></div>");
              client.println("<div class='canvas-box'><canvas id='gasGraph'></canvas></div></div>");

              // Graph Card 3: Kinematic Waveform Trends
              client.println("<div class='analytics-panel'><div class='analytics-title'>📉 Acceleration Z Waveform <span style='color:#8b5cf6; font-weight:800;'>Kinematic Vector</span></div>");
              client.println("<div class='canvas-box'><canvas id='motionGraph'></canvas></div></div>");

              client.println("<div class='log-section'><div class='log-title'>Nursery Diagnostic Event Monitor</div><div id='logContainer'></div></div></div></div>");
              
              // Client Browser JavaScript Runtime Layout Controller
              client.println("<script>let previousState = false; let telemetryLog = [];");
              
              // Canvas contexts generation hooks
              client.println("const c1 = document.getElementById('climateGraph'); const ctx1 = c1.getContext('2d');");
              client.println("const c2 = document.getElementById('gasGraph');     const ctx2 = c2.getContext('2d');");
              client.println("const c3 = document.getElementById('motionGraph');  const ctx3 = c3.getContext('2d');");
              
              client.println("function initCanvas(c) { c.width = c.parentElement.clientWidth; c.height = c.parentElement.clientHeight; }");
              client.println("function setupAll() { initCanvas(c1); initCanvas(c2); initCanvas(c3); }");
              client.println("window.addEventListener('resize', setupAll); setupAll();");
              
              // Advanced Multi Trace Script plotter function logic
              client.println("function processGraphs() {");
              client.println("  ctx1.clearRect(0,0,c1.width,c1.height); ctx2.clearRect(0,0,c2.width,c2.height); ctx3.clearRect(0,0,c3.width,c3.height);");
              client.println("  if(telemetryLog.length < 2) return;");
              client.println("  let step = c1.width / 29;");
              
              client.println("  for(let i=1; i<telemetryLog.length; i++) {");
              client.println("    let x1 = (i-1)*step; let x2 = i*step;");
              
              // Plot Panel 1 lines (Temp & Stillness values)
              client.println("    let yt1 = c1.height - ((telemetryLog[i-1].t - 34)/(40 - 34))*c1.height; let yt2 = c1.height - ((telemetryLog[i].t - 34)/(40 - 34))*c1.height;");
              client.println("    ctx1.strokeStyle='#2563eb'; ctx1.lineWidth=2; ctx1.beginPath(); ctx1.moveTo(x1,yt1); ctx1.lineTo(x2,yt2); ctx1.stroke();");
              client.println("    let ys1 = c1.height - (telemetryLog[i-1].s / 20)*c1.height; let ys2 = c1.height - (telemetryLog[i].s / 20)*c1.height;");
              client.println("    ctx1.strokeStyle='#ef4444'; ctx1.lineWidth=1.5; ctx1.beginPath(); ctx1.moveTo(x1,ys1); ctx1.lineTo(x2,ys2); ctx1.stroke();");
              
              // Plot Panel 2 lines (eCO2 values)
              client.println("    let yg1 = c2.height - ((telemetryLog[i-1].e - 300)/(1800 - 300))*c2.height; let yg2 = c2.height - ((telemetryLog[i].e - 300)/(1800 - 300))*c2.height;");
              client.println("    ctx2.strokeStyle='#10b981'; ctx2.lineWidth=2; ctx2.beginPath(); ctx2.moveTo(x1,yg1); ctx2.lineTo(x2,yg2); ctx2.stroke();");
              
              // Plot Panel 3 lines (Acceleration Waveform vectors)
              client.println("    let ym1 = c3.height - ((telemetryLog[i-1].a - (-30))/(30 - (-30)))*c3.height; let ym2 = c3.height - ((telemetryLog[i].a - (-30))/(30 - (-30)))*c3.height;");
              client.println("    ctx3.strokeStyle='#8b5cf6'; ctx3.lineWidth=2; ctx3.beginPath(); ctx3.moveTo(x1,ym1); ctx3.lineTo(x2,ym2); ctx3.stroke();");
              client.println("  }");
              client.println("}");
              
              // Core continuous monitoring parser request worker loop
              client.println("function monitorSuite(){fetch('/data').then(r=>r.json()).then(d=>{");
              client.println("  document.getElementById('tV').innerText = d.temperature.toFixed(1);");
              client.println("  document.getElementById('sV').innerText = d.stillness;");
              client.println("  document.getElementById('pV').innerText = d.pressure;");
              client.println("  document.getElementById('hV').innerText = d.humidity.toFixed(1);");
              client.println("  document.getElementById('eco2V').innerText = d.eco2;");
              client.println("  document.getElementById('azV').innerText = d.accelZ.toFixed(1);");
              
              client.println("  telemetryLog.push({t: d.temperature, s: d.stillness, e: d.eco2, a: d.accelZ});");
              client.println("  if(telemetryLog.length > 30) telemetryLog.shift(); processGraphs();");
              
              client.println("  let pill = document.getElementById('pPill');");
              client.println("  if(d.temperature > 37.5 || d.temperature < 36.5){ document.getElementById('tBox').classList.add('box-danger'); } else { document.getElementById('tBox').classList.remove('box-danger'); }");
              client.println("  if(d.eco2 > 1000){ document.getElementById('eco2Box').classList.add('box-danger'); } else { document.getElementById('eco2Box').classList.remove('box-danger'); }");
              
              client.println("  if(d.alarm){");
              client.println("    pill.className='status-pill pill-critical'; pill.innerText='Critical';");
              client.println("    document.getElementById('sBox').classList.add('box-danger');");
              client.println("    if(!previousState){ addEvent('CRITICAL','Stillness parameter limit exceeded'); previousState=true; }");
              client.println("  } else {");
              client.println("    document.getElementById('sBox').classList.remove('box-danger');");
              client.println("    if(d.temperature > 37.5 || d.temperature < 36.5 || d.eco2 > 1000){ pill.className='status-pill pill-warning'; pill.innerText='Unstable'; }");
              client.println("    else { pill.className='status-pill pill-stable'; pill.innerText='Stable'; }");
              client.println("    if(previousState){ addEvent('RECOVERY','Ecosystem parameters stabilized'); previousState=false; }");
              client.println("  }");
              client.println("});}");
              
              client.println("function addEvent(tag, desc){");
              client.println("  const container = document.getElementById('logContainer'); const now = new Date();");
              client.println("  const tStr = String(now.getHours()).padStart(2,'0')+':'+String(now.getMinutes()).padStart(2,'0')+':'+String(now.getSeconds()).padStart(2,'0');");
              client.println("  const badgeColor = tag === 'CRITICAL' ? 'background:#fef2f2;color:#ef4444;border:1px solid #fca5a5' : 'background:#f0fdf4;color:#10b981;border:1px solid #bbf7d0';");
              client.println("  const html = `<div class='log-row'><span class='log-time'>${tStr}</span><span class='log-tag' style='${badgeColor}'>${tag}</span><span style='width:55%; text-align:right;'>${desc}</span></div>`;");
              client.println("  container.insertAdjacentHTML('afterbegin', html);");
              client.println("  if(container.children.length > 3){ container.removeChild(container.lastChild); }");
              client.println("}");
              
              client.println("addEvent('BOOT','Clinical Monitor Core Engine Started');");
              client.println("setInterval(monitorSuite,1000); monitorSuite();");
              client.println("</script></body></html>");
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }

  yield(); 
}