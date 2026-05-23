#include <Arduino.h>
#include "BluetoothSerial.h"

// เลือกเปิดใช้ Driver ให้ถูกตามหน้าจอของคุณ (ปลดคอมเมนต์แค่อันเดียว)
#include "SSD1306Wire.h"        // สำหรับจอ 0.96" หรือ 0.91" (ชิป SSD1306)
// #include "SH1106Wire.h"      // สำหรับจอ 1.3" (ชิป SH1106)

// ประกาศขา I2C ของ ESP32 (ปกติ NodeMCU-32S ขามาตรฐานคือ SDA=21, SCL=22)
#define I2C_SDA 21
#define I2C_SCL 22
#define DISPLAY_ADDRESS 0x3C   // ที่อยู่มาตรฐานของจอ OLED ส่วนใหญ่

// สร้าง Object สำหรับควบคุมหน้าจอ (หากใช้จอ 1.3 นิ้ว ให้เปลี่ยนคำว่า SSD1306Wire เป็น SH1106Wire)
SSD1306Wire display(DISPLAY_ADDRESS, I2C_SDA, I2C_SCL); 

BluetoothSerial SerialBT;

int cpuVal = 0;
int ramVal = 0;

// ฟังก์ชันสำหรับวาด UI หน้าจอ
void drawMonitorUI(int cpu, int ram) {
  display.clear(); // เคลียร์หน้าจอก่อนวาดเฟรมใหม่

  // --- ส่วนของ CPU ---
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "CPU: " + String(cpu) + "%");
  // วาดกรอบหลอด (x, y, width, height)
  display.drawRect(0, 13, 128, 10);
  // คำนวณความยาวแถบข้างในหลอด (ความกว้างสูงสุด 124 พิกเซล เว้นขอบข้างละ 2 พิกเซล)
  int cpuBarWidth = map(cpu, 0, 100, 0, 124);
  display.fillRect(2, 15, cpuBarWidth, 6);

  // --- ส่วนของ RAM ---
  display.drawString(0, 28, "RAM: " + String(ram) + "%");
  display.drawRect(0, 41, 128, 10);
  int ramBarWidth = map(ram, 0, 100, 0, 124);
  display.fillRect(2, 43, ramBarWidth, 6);

  display.display(); // สั่งให้หน้าจอแสดงผลตามข้อมูลที่วาดข้างบน
}

void setup() {
  Serial.begin(115200);
  
  // เริ่มทำงานหน้าจอ
  display.init();
  display.flipScreenVertically(); // กลับหัวหน้าจอ 180 องศา (ปรับตามความสะดวกตอนจัดวาง)
  
  // หน้าจอตอนเปิดเครื่องแสดงสถานะรอการเชื่อมต่อ
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Waiting for Bluetooth...");
  display.display();

  // เปิดใช้งาน Bluetooth ตั้งชื่อสัญญาณให้คอมพิวเตอร์ค้นหา
  SerialBT.begin("ESP32_PC_Monitor");
  Serial.println("Bluetooth พร้อมทำงานในชื่อ: ESP32_PC_Monitor");
}

void loop() {
  // เช็กว่ามีข้อมูล Bluetooth ส่งเข้ามาและสิ้นสุดด้วยการขึ้นบรรทัดใหม่ (\n) หรือไม่
  if (SerialBT.available()) {
    String dataIn = SerialBT.readStringUntil('\n');
    dataIn.trim(); // ตัดเว้นวรรคหรือขยะหัวท้ายออก
    
    if (dataIn.length() > 0) {
      // ค้นหาตำแหน่งของเครื่องหมายคอมมา ',' เพื่อแยกคำ
      int commaIndex = dataIn.indexOf(',');
      
      if (commaIndex != -1) {
        // แยกข้อความตัวหน้าเป็น CPU ตัวหลังเป็น RAM
        String cpuStr = dataIn.substring(0, commaIndex);
        String ramStr = dataIn.substring(commaIndex + 1);
        
        // แปลงข้อความสตริงเป็นตัวเลขจำนวนเต็ม (Integer)
        cpuVal = cpuStr.toInt();
        ramVal = ramStr.toInt();
        
        // สั่งอัปเดตหน้าจอ UI ทันทีที่ได้ค่าใหม่
        drawMonitorUI(cpuVal, ramVal);
        
        Serial.printf("Updated -> CPU: %d%%, RAM: %d%%\n", cpuVal, ramVal);
      }
    }
  }
  delay(10); // หน่วงเวลาสั้นๆ เพื่อให้ชิปไม่ทำงานหนักเกินไป
}