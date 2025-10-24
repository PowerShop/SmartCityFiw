# SmartCityFiw 📡

เอกสารนี้เป็นคำอธิบายและขั้นตอนการใช้งานโปรเจกต์ SmartCityFiw (ESP8266) — หน้าเว็บ (option), เซนเซอร์ และการอัปโหลด

## สรุปสั้น ๆ 🚀
- บอร์ด: ESP8266 (esp12e)
- เซนเซอร์: DHT11 (Temp/Hum), MQ-2 (Gas analog), MH-B (IR avoidance digital), HC‑SR04 (Ultrasonic)
- ตัวเลือกเพิ่มเติม: KY‑037 (Sound) — รองรับทั้ง DO (digital) และ AO (analog) แต่ AO จะแชร์กับ A0 ของ ESP8266

## ขา (Pin) ที่ตั้งค่าใน `src/main.cpp` 🔌
- DHT11: GPIO14 (D5) — `PIN_DHT = 14`
- MQ‑2: A0 (ADC) — `PIN_MQ2_A = A0`
- MH‑B (IR): GPIO2 (D4) — `PIN_MHB = 2`
- HC‑SR04: TRIG = GPIO12 (D6), ECHO = GPIO13 (D7)
- (ถ้ามี KY‑037 แบบ digital) DO -> GPIO16 (D0) หรือ AO -> A0 ถ้าต้องการ analog (ดูข้อควรระวัง)

⚠️ หมายเหตุสำคัญ: ESP8266 มี ADC เพียงพอร์ตเดียว (A0). ถาคุณใช้ KY‑037 AO บน A0 จะทำให้ MQ‑2 อ่านค่า analog ไม่ได้พร้อมกัน — ต้องใช้ ADC ภายนอก หรือ multiplexer เพื่ออ่านหลาย analog

## การต่อไฟ
- VCC ของเซนเซอร์ต่อกับ 3.3V (หากอุปกรณ์ระบุ 5V ให้ตรวจสอบความทนทานของสัญญาณ) และ GND ต่อ GND
- HC‑SR04 มักต้องใช้ 5V Vcc แต่ ECHO เป็น 5V — ต้องต่อผ่านตัวลดแรงดัน (voltage divider) ก่อนเข้าขา ECHO ของ ESP8266

## คำสั่งสำคัญ (PowerShell) 🛠️
- อัปโหลดไฟล์ในโฟลเดอร์ `data/` ไปยัง LittleFS (ถ้าใช้ UI แบบเว็บ):
```powershell
pio run -t uploadfs
```
- คอมไพล์และอัปโหลดเฟิร์มแวร์:
```powershell
pio run -t upload
```
- เปิด Serial Monitor (ค่าเริ่มต้นในโปรเจกต์คือ 115200):
```powershell
pio device monitor -b 115200
```

## Serial boot message (ข้อสังเกต) 📝
- ESP8266 ROM/bootloader พิมพ์ข้อความตอนบูตที่ความเร็ว 74880 baud — ถาคุณเห็นตัวอักษรกวน ๆ ให้ลองเปิด Monitor ที่ 74880 เพื่อดูข้อความบูต
```powershell
pio device monitor -b 74880
```
- เมื่อสเก็ตช์เริ่มทำงานแล้ว โค้ดจะใช้ Serial ที่ 115200 ตาม `Serial.begin(115200)`

## การปรับแต่งที่น่าสนใจ 🔧
- เปิดใช้งาน KY‑037 แบบ analog (AO->A0): กำหนดค่าคอมไพล์ `-DKY037_AS_ANALOG=1` หรือแก้โค้ดค่า `KY037_AS_ANALOG` ใน `src/main.cpp` แต่ถ้าทำเช่นนี้ MQ‑2 จะไม่สามารถอ่านได้บน A0 พร้อมกัน
- คืนค่า webserver/UI: ถ้าคุณต้องการให้บอร์ดให้หน้าเว็บอีกครั้ง ให้คืน `ESP8266WebServer` และ `LittleFS` โค้ดใน `src/main.cpp` ที่ลบไป และอัปโหลด `data/index.html` ด้วย `uploadfs`
- เพิ่ม MQTT: หากต้องการส่งข้อมูลแบบรีโมท แนะนำใช้ MQTT broker และไลบรารี PubSubClient

## ปัญหาที่อาจเจอและการแก้ไขเบื้องต้น 🧭
- DHT อ่าน NaN: ตรวจสอบสาย, ตรวจสอบประเภท DHT (DHT11) และเวลาที่อ่าน (DHT แพช้า)
- หน้าจอเว็บไม่โหลด: ตรวจสอบว่าคุณอัปโหลด LittleFS (`uploadfs`) และอัปโหลดเฟิร์มแวร์แล้ว
- HC‑SR04 คืนค่า -1: ตรวจสอบการต่อสายและ voltage divider บน ECHO

## ข้อเสนอแนะเพิ่มเติม 😊
- ถ้าต้องการ ผมสามารถ: เพิ่ม MQTT, คืน web UI, เพิ่มหน้า config (WiFi STA), หรือต่อ external ADC (MCP3008) ให้ครบหลาย analog

ขอบคุณที่ใช้ SmartCityFiw — ถ้าต้องการให้ผมเพิ่มหรือเปลี่ยนอะไร บอกมาได้เลยครับ! 💡
