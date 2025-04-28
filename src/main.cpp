#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>            // Ganti dengan ESP8266WiFi.h jika menggunakan ESP8266
#include <HTTPClient.h>
#include <ArduinoJson.h>     // Library untuk parsing JSON

// Konfigurasi Wi-Fi dan API OpenWeatherMap
const char* ssid = "Wokwi-GUEST";  // Ganti dengan SSID Wi-Fi kamu
const char* password = "";         // Ganti dengan password Wi-Fi kamu
String apiKey = "aa183d09b9792e4ef1d50d33a2222699";  // API Key dari OpenWeatherMap
String city = "Malang";            // Kota yang ingin ditampilkan
String units = "metric";           // Untuk Celsius gunakan "metric", untuk Fahrenheit "imperial"
String server = "http://api.openweathermap.org/data/2.5/weather?q=Malang&units=metric&appid=aa183d09b9792e4ef1d50d33a2222699";

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C untuk LCD adalah 0x27, ukuran 16x2 karakter

String displayText = "";             // Teks yang akan ditampilkan dan di-scroll
int scrollIndex = 0;                 // Posisi indeks scroll saat ini

unsigned long lastUpdateTime = 0;    // Waktu terakhir update data cuaca
unsigned long lastScrollTime = 0;    // Waktu terakhir scroll teks
const long updateInterval = 60000;   // Interval update data cuaca setiap 60 detik
const long scrollInterval = 300;     // Interval scroll teks LCD setiap 300ms

// Variabel untuk tombol dan navigasi antar halaman
const int buttonNextPin = 18;        // Pin tombol Next di D18
const int buttonPrevPin = 19;        // Pin tombol Prev di D19
int currentPage = 0;                 // Halaman yang sedang aktif
const int totalPages = 4;            // Jumlah halaman yang tersedia
unsigned long lastDebounceTime = 0;  // Waktu debounce tombol
const long debounceDelay = 200;      // Delay debounce untuk tombol

// Variabel untuk data cuaca
String temp = "";                    // Menyimpan suhu
String desc = "";                    // Menyimpan deskripsi cuaca
String humidity = "";                // Menyimpan kelembaban
String wind = "";                    // Menyimpan kecepatan angin

// Fungsi deklarasi
void updateWeather();
void showPage(int page);
void scrollDisplay();

void setup() {
  Serial.begin(115200);  // Mulai komunikasi serial untuk debug

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Info:");

  // Inisialisasi tombol
  pinMode(buttonNextPin, INPUT_PULLUP);  // Tombol Next menggunakan mode INPUT_PULLUP
  pinMode(buttonPrevPin, INPUT_PULLUP);  // Tombol Prev menggunakan mode INPUT_PULLUP

  // Inisialisasi koneksi Wi-Fi
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
  delay(2000);  // Tunggu 2 detik sebelum lanjut
  lcd.clear();

  // Ambil data cuaca pertama kali setelah Wi-Fi terkoneksi
  updateWeather();
  showPage(currentPage);  // Tampilkan halaman pertama
}

void loop() {
  unsigned long currentMillis = millis();  // Waktu saat ini untuk timer

  // Update data cuaca setiap 60 detik
  if (currentMillis - lastUpdateTime >= updateInterval) {
    updateWeather();
    lastUpdateTime = currentMillis;
    showPage(currentPage);  // Refresh halaman setelah update data
  }

  // Scroll teks LCD setiap 300ms di halaman 2 (deskripsi cuaca)
  if (currentPage == 2) {
    if (currentMillis - lastScrollTime >= scrollInterval) {
      scrollDisplay();
      lastScrollTime = currentMillis;
    }
  }

  // Baca tombol Next (pindah ke halaman berikutnya)
  if (digitalRead(buttonNextPin) == LOW) {
    if (millis() - lastDebounceTime > debounceDelay) {
      currentPage++;
      if (currentPage >= totalPages) currentPage = 0;  // Kembali ke halaman pertama jika sudah di halaman terakhir
      showPage(currentPage);
      lastDebounceTime = millis();
    }
  }

  // Baca tombol Prev (pindah ke halaman sebelumnya)
  if (digitalRead(buttonPrevPin) == LOW) {
    if (millis() - lastDebounceTime > debounceDelay) {
      if (currentPage == 0) currentPage = totalPages - 1;  // Kembali ke halaman terakhir jika di halaman pertama
      else currentPage--;
      showPage(currentPage);
      lastDebounceTime = millis();
    }
  }
}

// Fungsi untuk mengambil data cuaca dari API
void updateWeather() {
  if (WiFi.status() == WL_CONNECTED) {  // Periksa apakah Wi-Fi terhubung
    HTTPClient http;
    http.begin(server);     // Tentukan URL untuk request
    int httpCode = http.GET();  // Lakukan request HTTP GET
    
    if (httpCode > 0) {  // Jika request berhasil
      String payload = http.getString();  // Ambil data dari API
      Serial.println(payload);   // Tampilkan data mentah (JSON) ke Serial Monitor
      
      // Parsing data JSON
      StaticJsonDocument<1024> doc;
      deserializeJson(doc, payload);
      
      // Ambil data dari JSON dan simpan ke variabel
      temp = String(doc["main"]["temp"].as<float>());
      desc = doc["weather"][0]["description"].as<String>();
      humidity = String(doc["main"]["humidity"].as<int>());
      wind = String(doc["wind"]["speed"].as<float>());
      
      // Gabungkan data untuk ditampilkan di layar LCD
      displayText = "| Temp: " + temp + " C | " + desc;
      scrollIndex = 0;  // Reset indeks scroll
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();  // Hapus resource HTTP setelah selesai
  }
}

// Fungsi untuk menampilkan halaman sesuai dengan currentPage
void showPage(int page) {
  lcd.clear();
  switch (page) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Location:");
      lcd.setCursor(0, 1);
      lcd.print(city);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Temperature:");
      lcd.setCursor(0, 1);
      lcd.print(temp + " C");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Weather:");
      lcd.setCursor(0, 1);
      if (desc.length() > 16) {
        lcd.print(desc.substring(0, 16));  // Potong jika deskripsi lebih dari 16 karakter
      } else {
        lcd.print(desc);
      }
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Wind: " + wind + " m/s");
      lcd.setCursor(0, 1);
      lcd.print("Humidity: " + humidity + "%");
      break;
  }
}

// Fungsi untuk scroll teks di halaman deskripsi cuaca
void scrollDisplay() {
  if (displayText.length() > 0) {
    lcd.setCursor(0, 1);
    if (scrollIndex + 16 <= displayText.length()) {
      lcd.print(displayText.substring(scrollIndex, scrollIndex + 16));  // Tampilkan potongan teks
    } else {
      String part1 = displayText.substring(scrollIndex);
      String part2 = displayText.substring(0, 16 - part1.length());
      lcd.print(part1 + part2);  // Efek looping scroll
    }
    
    scrollIndex++;
    if (scrollIndex >= displayText.length()) {
      scrollIndex = 0;  // Jika sudah habis, mulai dari awal lagi
    }
  }
}
