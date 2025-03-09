#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Konfigurasi
#define DHTPIN 4         // Pin data DHT22 (sesuai dengan rangkaian)
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN 34       // Pin sensor cahaya (LDR)

// Inisialisasi
void setup() {
    Serial.begin(115200);
    dht.begin();

    // Inisialisasi OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("SSD1306 tidak ditemukan!");
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void loop() {
    // Baca suhu & kelembapan dari DHT22
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Baca nilai sensor cahaya (LDR)
    int lightIntensity = analogRead(LDR_PIN);
    
    // Debugging ke Serial Monitor
    Serial.print("Suhu: "); Serial.print(temperature); Serial.print(" C | ");
    Serial.print("Kelembapan: "); Serial.print(humidity); Serial.print(" % | ");
    Serial.print("Cahaya: "); Serial.println(lightIntensity);

    // Tampilkan di OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sensor Monitoring");
    display.println("-----------------");
    display.print("Suhu: "); display.print(temperature); display.println(" C");
    display.print("Kelembapan: "); display.print(humidity); display.println(" %");
    display.print("Cahaya: "); display.print(lightIntensity);
    display.display();

    delay(2000); // Tunggu 2 detik sebelum update berikutnya
}
