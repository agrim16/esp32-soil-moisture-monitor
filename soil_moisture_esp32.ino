/*
  ESP32 Soil Moisture Monitor
  - Bare 2-pin resistive soil moisture probe, no resistors needed
  - Live web dashboard at http://soil.local (or the IP printed in Serial Monitor)

  Wiring:
    Probe pin A -> GPIO 4   (powers the probe only while measuring)
    Probe pin B -> GPIO 32  (analog read, internal pull-down)
  The two probe pins are interchangeable.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASSWORD";

const int probePower = 4;   // powers the probe only while measuring
const int soilPin = 32;     // reads the probe (internal pull-down)

// Calibrate these for your probe (see README)
const int DRY = 100;    // raw value in open air
const int WET = 2500;   // raw value with prongs in water

WebServer server(80);
int raw = 0, percent = 0;

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Soil Moisture</title>
<style>
  body{font-family:sans-serif;background:#111;color:#eee;text-align:center;padding:20px}
  .card{max-width:340px;margin:auto;background:#1e1e1e;border-radius:16px;padding:24px}
  #pct{font-size:72px;font-weight:bold;margin:8px 0}
  .bar{height:24px;background:#333;border-radius:12px;overflow:hidden}
  #fill{height:100%;width:0;background:#3b82f6;transition:width .5s,background .5s}
  #status{font-size:22px;margin-top:16px}
  small{color:#888}
</style></head><body>
<div class="card">
  <h2>🌱 Soil Moisture</h2>
  <div id="pct">--%</div>
  <div class="bar"><div id="fill"></div></div>
  <div id="status">Loading...</div>
  <p><small>raw: <span id="raw">--</span></small></p>
</div>
<script>
async function update(){
  try{
    const r = await fetch('/data'); const d = await r.json();
    document.getElementById('pct').textContent = d.percent + '%';
    document.getElementById('raw').textContent = d.raw;
    const f = document.getElementById('fill');
    f.style.width = d.percent + '%';
    let s, c;
    if(d.percent < 30){s='Dry - water me! 🏜️'; c='#ef4444';}
    else if(d.percent < 70){s='Happy 🌿'; c='#22c55e';}
    else {s='Very wet 💧'; c='#3b82f6';}
    f.style.background = c;
    document.getElementById('status').textContent = s;
  }catch(e){
    document.getElementById('status').textContent = 'Disconnected';
  }
}
update(); setInterval(update, 2000);
</script></body></html>
)rawliteral";

void readSoil() {
  digitalWrite(probePower, HIGH);
  delay(20);
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    pinMode(soilPin, INPUT_PULLDOWN); // re-apply before each read
    sum += analogRead(soilPin);
    delay(5);
  }
  digitalWrite(probePower, LOW);
  raw = sum / 10;
  percent = constrain(map(raw, DRY, WET, 0, 100), 0, 100);
  Serial.printf("raw=%d percent=%d\n", raw, percent);
}

void setup() {
  Serial.begin(115200);
  pinMode(probePower, OUTPUT);
  digitalWrite(probePower, LOW);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(300);
  Serial.println(WiFi.localIP());
  MDNS.begin("soil"); // http://soil.local

  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.on("/data", []() {
    server.send(200, "application/json",
      "{\"raw\":" + String(raw) + ",\"percent\":" + String(percent) + "}");
  });
  server.begin();
}

unsigned long last = 0;
void loop() {
  server.handleClient();
  if (millis() - last > 1000) { readSoil(); last = millis(); }
}
