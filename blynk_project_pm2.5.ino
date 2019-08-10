#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include "PMS.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

PMS pms(Serial);
PMS::DATA data;

#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "c7c5eb78b6224efa8a274a0d7234887d";

char ssid[] = "p 2.4G";
char pass[] = "0804074610A";
char host[] = "blynk.honey.co.th";

BlynkTimer get_dht;
BlynkTimer get_pms7003;
BlynkTimer show_lcd;

int alarm = D5;
int pm2_5 = 0;
float temp = 0;
float humi = 0;

void read_dht() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  temp = t;
  humi = h;

  Blynk.virtualWrite(V0 , temp);
  Blynk.virtualWrite(V1 , humi);
}


void read_pms7003() {
  if (pms.read(data)) {
    pm2_5 = data.PM_AE_UG_2_5;
  }

  if (pm2_5 >= 30) {
    digitalWrite(alarm , HIGH);
  }
  else {
    digitalWrite(alarm , LOW);
  }

  Blynk.virtualWrite(V2 , pm2_5);
}


void send_lcd() {
  lcd.setCursor(0 , 0);
  lcd.print("T = " + String(temp) + "  ");
  lcd.setCursor(0 , 1);
  lcd.print("H = " + String(humi) + "  ");
  lcd.setCursor(0 , 2);
  lcd.print("PM2.5 = " + String(pm2_5) + "  ");
}


void setup() {
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial1.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)
  Blynk.begin(auth , ssid , pass , host , 8080);
  lcd.begin();
  lcd.backlight();
  get_dht.setInterval(1000L , read_dht);
  get_pms7003.setInterval(1000L ,  read_pms7003);
  show_lcd.setInterval(1000L ,  send_lcd);
  pinMode(alarm , OUTPUT);
}


void loop() {
  Blynk.run();
  get_dht.run();
  get_pms7003.run();
  show_lcd.run();
}
