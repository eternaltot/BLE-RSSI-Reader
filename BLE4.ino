#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>
#include <WiFi.h>
#include <HTTPClient.h>
BLEScan* pBLEScan;

const char* host = "35.208.13.22";
const int httpPort = 80;
const char* url =  "http://35.208.13.22/push";


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      std::string deviceName = advertisedDevice.getAddress().toString();
    }
};

void setup() {
  const char* ssid     = "AndroidAPTOT";
  const char* password = "gpnt7788";
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);  // less or equal setInterval value

}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(1, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  int count = foundDevices.getCount();
  std::stringstream ss;
//  ss << "[";
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);
    if ("3c:a3:08:97:e4:e3" == d.getAddress().toString()) {
      if (i > 0) {
        ss << ",";
      }

      ss << "{\"node\":\"Beacon4\",\"address\":\"" << d.getAddress().toString() << "\",\"rssi\":" << d.getRSSI();
      ss << "}";
    }
  }
//  ss << "]";
  Serial.println(ss.str().c_str());
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  HTTPClient http;   
  if(ss.rdbuf()->in_avail() == 0){
    Serial.println("no data true");
    return;  
   }
   http.begin(url);
   http.addHeader("Content-Type", "application/json");
   
   int httpResponseCode = http.POST(ss.str().c_str()); 
 
   if(httpResponseCode>0){
    String response = http.getString(); 
    Serial.println(httpResponseCode);
    Serial.println(response);
   }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
   http.end();

  // Read all the lines of the reply from server and print them to Serial
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  ss.str("");
  delay(500);
}
