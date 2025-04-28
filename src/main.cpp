#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>          // Ganti dengan ESP8266WiFi.h jika menggunakan ESP8266
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";       // Ganti dengan SSID Wi-Fi kamu
const char* password = "";              // Ganti dengan password Wi-Fi kamu
String apiKey = "aa183d09b9792e4ef1d50d33a2222699";       // API Key dari OpenWeatherMap
String city = "Malang";                 // Kota yang ingin ditampilkan
String units = "metric";                // Untuk Celsius gunakan "metric", untuk Fahrenheit "imperial"
String server = "http://api.openweathermap.org/data/2.5/weather?q=Malang&units=metric&appid=aa183d09b9792e4ef1d50d33a2222699";
//String server = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + units + "&appid=" + apiKey;
//String server = "https://api.openweathermap.org/data/2.5/weather?q=Malang&appid=20ca0ff523294dcdeb424dfc5802e21b";

LiquidCrystal_I2C lcd(0x27, 16, 2);   // Inisialisasi LCD dengan alamat I2C 0x27

String displayText = "";              // Menyimpan teks yang akan discroll
int scrollIndex = 0;                  // Posisi scroll saat ini

unsigned long lastUpdateTime = 0;     // Untuk cek kapan terakhir update data cuaca
unsigned long lastScrollTime = 0;     // Untuk cek kapan terakhir scroll teks
const long updateInterval = 60000;    // 60 detik interval update data cuaca
const long scrollInterval = 300;      // 0.3 detik interval scroll teks LCD

// Deklarasi prototipe fungsi
void updateWeather();
void scrollDisplay();

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Info:");
  
  // Inisialisasi Wi-Fi
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!");
  delay(2000);
  lcd.clear();
  
  // Ambil data cuaca pertama kali setelah koneksi sukses
  updateWeather();
}

void loop() {
  unsigned long currentMillis = millis();

  // Update data cuaca setiap 60 detik
  if (currentMillis - lastUpdateTime >= updateInterval) {
    updateWeather();
    lastUpdateTime = currentMillis;
  }

  // Scroll teks LCD setiap 300ms
  if (currentMillis - lastScrollTime >= scrollInterval) {
    scrollDisplay();
    lastScrollTime = currentMillis;
  }
}

// Fungsi untuk mengambil data cuaca dari API
void updateWeather() {
  if (WiFi.status() == WL_CONNECTED) {  // Check WiFi connection status
    HTTPClient http;
    http.begin(server);     // Tentukan URL
    int httpCode = http.GET();  // Lakukan request HTTP GET
    
    if (httpCode > 0) { // Check apakah request berhasil
      String payload = http.getString();
      Serial.println(payload);   // Tampilkan payload ke Serial Monitor
      
      // Parse data (ambil temperatur)
      int tempIndex = payload.indexOf("temp");
      String temp = payload.substring(tempIndex + 6, payload.indexOf(",", tempIndex));
      
      // Ambil deskripsi cuaca
      int descIndex = payload.indexOf("description");
      String desc = payload.substring(descIndex + 14, payload.indexOf("\"", descIndex + 14));
      
      // Gabungkan data ke teks untuk di scroll
      displayText = "| Temp: " + temp + " C | " + desc;

      // Tampilkan Location di baris atas
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Location: " + city);
      
      // Reset scroll index ke awal
      scrollIndex = 0;
      
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();  // Bebaskan resource HTTP
  }
}

// Fungsi untuk scroll teks di baris kedua LCD
void scrollDisplay() {
  if (displayText.length() > 0) {
    lcd.setCursor(0, 1);
    if (scrollIndex + 16 <= displayText.length()) {
      // Jika masih ada cukup karakter untuk satu baris penuh
      lcd.print(displayText.substring(scrollIndex, scrollIndex + 16));
    } else {
      // Jika sudah mendekati akhir, tampilkan dari awal lagi (efek looping)
      String part1 = displayText.substring(scrollIndex);
      String part2 = displayText.substring(0, 16 - part1.length());
      lcd.print(part1 + part2);
    }
    
    scrollIndex++;
    if (scrollIndex >= displayText.length()) {
      scrollIndex = 0;   // Jika sudah habis, mulai dari awal
    }
  }
}
