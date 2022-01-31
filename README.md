# RemoteCar-ESP32


This is a program of Remote Car which works on ESP32 development board.
___

### **[Used Librarys]**
- [Ps3Controller for ESP32](https://github.com/jvpernis/esp32-ps3/)
- ESP32Servo
___

### **Circuit**
[Circuit]
Comming Soon

___
### **How to connect PS3 controller**
1.Check the MAC Address of ESP32.
```c
void setup(void) {
  Serial.begin(115200);
  uint8_t btmac[6];
  esp_read_mac(btmac, ESP_MAC_BT);
  Serial.printf("[Bluetooth] Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", btmac[0], btmac[1], btmac[2], btmac[3], btmac[4], btmac[5]);
}

void loop() {
}
```
*Serial Output* `[Bluetooth] Mac Address = xx:xx:xx:xx:xx:xx`


2.Make the PS3 controller remember the MAC address
 1. Download this software [Sixaxis Pair Tool](https://sixaxispairtool.software.informer.com/download/)
 2. Connect PS3 Controller with PC.
 3. Run 'Sixaxis Pair Tool' and input the MAC address.Then push "Update" button.So you can change master MAC Address of PS3 Controller.

**Quote source :** http://gijin77.blog.jp/archives/21966552.html

___
### **Controll**
- Left AnalogStick :  GO / BACK
- Right AnalogStick :  LEFT / RIGHT
- L button : Turn Signal on left side.
- R button : Turn Signal on right side.
- ◯ button : Front light. 

