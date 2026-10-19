#include <Arduino.h>
#include <HX711.h>
#include <Servo.h>

// =======================
// Konfigurasi Pin
// =======================
#define DOUT A1
#define CLK A0
#define TRIG_PIN 7
#define ECHO_PIN 6
#define SERVO_PIN 10
#define LED_PIN 13
#define BUZZER_PIN 9

// =======================
// Objek
// =======================
HX711 scale;
Servo servo;

// =======================
// Ambang Batas Berat (kg)
// =======================
#define BATAS_LED 4.0     // LED menyala jika berat >= 4kg
#define BATAS_BUZZER 5.0  // Buzzer menyala jika berat >= 5kg

// =======================
// Kalibrasi Manual
// =======================
bool modeKalibrasi = false;     // true = kalibrasi
float faktorSkala = 84;         // Ganti sesuai hasil kalibrasi

// =======================
// Fungsi Baca Berat Rata-Rata
// =======================
float getAverageWeight(int times) {
  float total = 0;
  for (int i = 0; i < times; i++) {
    total += scale.get_units();
    delay(100);
  }
  return total / times;
}

// =======================
// Fungsi Baca Jarak Rata-Rata (Tanpa NewPing)
// =======================
float bacaJarakRata(int trigPin, int echoPin, int jumlahPembacaan) {
  float total = 0;
  for (int i = 0; i < jumlahPembacaan; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    float faktorKalibrasi = 1.0092;
    long durasi = pulseIn(echoPin, HIGH, 25000); // Timeout 25ms
    float jarak = durasi * 0.034 / 2 * faktorKalibrasi;
    if (jarak <= 0 || jarak > 200) jarak = 200;

    total += jarak;
    delay(30);
  }
  return total / jumlahPembacaan;
}

// =======================
// Setup
// =======================
void setup() {
  Serial.begin(9600);

  // Inisialisasi HX711
  scale.begin(DOUT, CLK);
  scale.set_scale(84);  // Default sementara
  scale.tare();

  if (modeKalibrasi) {
    Serial.println("=== MODE KALIBRASI ===");
    Serial.println("Taruh beban yang diketahui, lihat hasil output.");
  } else {
    scale.set_scale(faktorSkala);
    Serial.println("Sistem siap digunakan.");
  }

  // Inisialisasi Pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  servo.attach(SERVO_PIN);
  servo.write(0);  // Tutup awal
}

// =======================
// Loop
// =======================
void loop() {
  if (modeKalibrasi) {
    float beratKalibrasi = getAverageWeight(10);
    Serial.print("Berat rata-rata: ");
    Serial.println(beratKalibrasi, 2);
    delay(1000);
    return;
  }

  float berat = scale.get_units();
  if (berat < 0) berat = 0;

  float jarak = bacaJarakRata(TRIG_PIN, ECHO_PIN, 5);

  Serial.print("Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");

  Serial.print("Berat: ");
  Serial.print(berat, 2);
  Serial.println(" kg");

  // Deteksi tangan untuk buka tutup
  if (jarak > 0 && jarak < 20) {
    Serial.println("Tangan terdeteksi, buka tutup...");
    servo.write(90);
    delay(5000);
    servo.write(0);
  }

  // LED nyala jika berat >= 4 kg dan < 5 kg
  digitalWrite(LED_PIN, (berat >= BATAS_LED && berat < BATAS_BUZZER));

  // Buzzer nyala jika berat >= 5 kg
  if (berat >= BATAS_BUZZER) {
    tone(BUZZER_PIN, 1000);  // Bunyikan buzzer
  } else {
    noTone(BUZZER_PIN);      // Matikan buzzer
  }

  delay(5000);
}