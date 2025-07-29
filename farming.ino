#include <DHT.h>

// Pin definitions
#define TRIG_PIN 9
#define ECHO_PIN 10
#define DHT_PIN 2
#define DHT_TYPE DHT21  // DHT21 (AM2301)

// Konstanta kalibrasi
const float TINGGI_TANDON = 40.0;        // tinggi tandon dalam cm
const float JARAK_SENSOR_KE_DASAR = 45.0; // jarak dari sensor ke dasar tandon
const float SPEED_OF_SOUND = 0.0343;     // kecepatan suara cm/microsecond
const unsigned long TIMEOUT_US = 30000;   // timeout 30ms
const int SAMPLE_COUNT = 3;               // jumlah sample untuk averaging

// Threshold untuk peringatan lingkungan
const float TEMP_MIN = 10.0;    // Suhu minimum (°C)
const float TEMP_MAX = 40.0;    // Suhu maksimum (°C)
const float HUMIDITY_MIN = 30.0; // Kelembaban minimum (%)
const float HUMIDITY_MAX = 80.0; // Kelembaban maksimum (%)

// Inisialisasi sensor
DHT dht(DHT_PIN, DHT_TYPE);

// Variabel global
long duration;
float distance_cm;
float filtered_distance;
float temperature, humidity, heat_index;

void setup() {
  Serial.begin(9600);
  
  // Setup pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Inisialisasi DHT sensor
  dht.begin();
  
  // Pastikan trigger dalam keadaan LOW
  digitalWrite(TRIG_PIN, LOW);
  delay(2000); // DHT21 membutuhkan waktu stabilisasi
  
  Serial.println("===== SMART WATER TANK MONITOR =====");
  Serial.println("Sistem monitoring tandon air & lingkungan");
  Serial.print("Tinggi tandon: ");
  Serial.print(TINGGI_TANDON);
  Serial.println(" cm");
  Serial.println("Sensor: HC-SR04 + DHT21");
  Serial.println("Memulai pengukuran...\n");
}

void loop() {
  // Baca sensor lingkungan terlebih dahulu
  readEnvironmentData();
  
  // Baca sensor ultrasonik
  filtered_distance = getFilteredDistance();
  
  // Display header dengan timestamp
  displayHeader();
  
  // Display data lingkungan
  displayEnvironmentData();
  
  // Display data level air
  if (filtered_distance <= 0 || filtered_distance > JARAK_SENSOR_KE_DASAR + 10) {
    displaySensorError();
  } else {
    displayWaterLevel(filtered_distance);
  }
  
  // Display alerts gabungan
  displayCombinedAlerts();
  
  Serial.println("==========================================\n");
  delay(3000); // Delay 3 detik untuk stabilitas DHT21
}

// Fungsi untuk membaca data lingkungan dari DHT21
void readEnvironmentData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Periksa apakah pembacaan berhasil
  if (isnan(temperature) || isnan(humidity)) {
    temperature = -999; // Error flag
    humidity = -999;    // Error flag
    heat_index = -999;
  } else {
    // Hitung heat index (indeks panas)
    heat_index = dht.computeHeatIndex(temperature, humidity, false);
  }
}

// Fungsi untuk mengambil pembacaan sensor yang difilter
float getFilteredDistance() {
  float total = 0;
  int valid_readings = 0;
  
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float reading = measureDistance();
    
    // Hanya gunakan pembacaan yang valid
    if (reading > 0 && reading < 450) {
      total += reading;
      valid_readings++;
    }
    delay(50); // Jeda antar sample
  }
  
  // Kembalikan rata-rata jika ada pembacaan valid
  if (valid_readings > 0) {
    return total / valid_readings;
  }
  return -1; // Error code
}

// Fungsi untuk mengukur jarak dengan sensor ultrasonik
float measureDistance() {
  // Reset trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Kirim pulsa trigger
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Baca durasi echo dengan timeout
  duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
  
  // Hitung jarak (durasi dibagi 2 karena gelombang pulang-pergi)
  if (duration == 0) {
    return -1; // Timeout atau tidak ada echo
  }
  
  return (duration * SPEED_OF_SOUND) / 2.0;
}

// Fungsi untuk menampilkan header dengan info waktu
void displayHeader() {
  Serial.println("📊 SMART TANK MONITORING SYSTEM");
  Serial.print("⏰ Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" detik");
}

// Fungsi untuk menampilkan data lingkungan
void displayEnvironmentData() {
  Serial.println("\n🌡️  DATA LINGKUNGAN:");
  
  if (temperature == -999 || humidity == -999) {
    Serial.println("   ❌ ERROR: Tidak dapat membaca sensor DHT21");
    Serial.println("   Periksa koneksi dan coba lagi");
  } else {
    Serial.print("   Suhu        : ");
    Serial.print(temperature, 1);
    Serial.println(" °C");
    
    Serial.print("   Kelembaban  : ");
    Serial.print(humidity, 1);
    Serial.println(" %");
    
    Serial.print("   Heat Index  : ");
    Serial.print(heat_index, 1);
    Serial.println(" °C");
    
    // Status suhu
    displayTemperatureStatus();
    
    // Status kelembaban
    displayHumidityStatus();
  }
}

// Fungsi untuk menampilkan status suhu
void displayTemperatureStatus() {
  Serial.print("   Status Suhu : ");
  
  if (temperature < TEMP_MIN) {
    Serial.println("🟦 Terlalu Dingin");
  } else if (temperature > TEMP_MAX) {
    Serial.println("🟥 Terlalu Panas");
  } else if (temperature >= 25 && temperature <= 30) {
    Serial.println("🟢 Ideal");
  } else {
    Serial.println("🟡 Normal");
  }
}

// Fungsi untuk menampilkan status kelembaban
void displayHumidityStatus() {
  Serial.print("   Status RH   : ");
  
  if (humidity < HUMIDITY_MIN) {
    Serial.println("🟨 Terlalu Kering");
  } else if (humidity > HUMIDITY_MAX) {
    Serial.println("🔵 Terlalu Lembab");
  } else if (humidity >= 50 && humidity <= 65) {
    Serial.println("🟢 Ideal");
  } else {
    Serial.println("🟡 Normal");
  }
}

// Fungsi untuk error sensor ultrasonik
void displaySensorError() {
  Serial.println("\n💧 STATUS TANDON AIR:");
  Serial.println("   ❌ SENSOR ERROR: Pembacaan tidak valid");
  Serial.print("   Jarak terukur: ");
  Serial.print(filtered_distance);
  Serial.println(" cm");
  Serial.println("   Periksa koneksi sensor HC-SR04");
}

// Fungsi untuk menampilkan informasi level air
void displayWaterLevel(float jarak_ke_air) {
  // Hitung tinggi air dari dasar tandon
  float tinggi_air = JARAK_SENSOR_KE_DASAR - jarak_ke_air;
  
  // Batasi nilai dalam range yang logis
  if (tinggi_air < 0) tinggi_air = 0;
  if (tinggi_air > TINGGI_TANDON) tinggi_air = TINGGI_TANDON;
  
  // Hitung persentase
  float persentase = (tinggi_air / TINGGI_TANDON) * 100;
  
  // Display informasi
  Serial.println("\n💧 STATUS TANDON AIR:");
  Serial.print("   Jarak ke permukaan : ");
  Serial.print(jarak_ke_air, 1);
  Serial.println(" cm");
  
  Serial.print("   Tinggi air         : ");
  Serial.print(tinggi_air, 1);
  Serial.println(" cm");
  
  Serial.print("   Persentase isi     : ");
  Serial.print(persentase, 1);
  Serial.println("%");
  
  // Status bar visual
  displayStatusBar(persentase);
  
  // Alert level air
  displayWaterAlert(persentase);
}

// Fungsi untuk menampilkan status bar visual
void displayStatusBar(float persentase) {
  Serial.print("   Level: [");
  
  int bars = (int)(persentase / 10); // 10 bar maksimal
  
  for (int i = 0; i < 10; i++) {
    if (i < bars) {
      Serial.print("█");
    } else {
      Serial.print("░");
    }
  }
  Serial.print("] ");
  Serial.print(persentase, 0);
  Serial.println("%");
}

// Fungsi untuk alert level air
void displayWaterAlert(float persentase) {
  Serial.print("   Status             : ");
  
  if (persentase >= 95) {
    Serial.println("🚨 HAMPIR PENUH");
  } else if (persentase >= 80) {
    Serial.println("✅ TERISI BAIK");
  } else if (persentase >= 30) {
    Serial.println("⚠️  LEVEL SEDANG");
  } else if (persentase >= 10) {
    Serial.println("🟡 MULAI SEDIKIT");
  } else {
    Serial.println("🔴 HAMPIR HABIS");
  }
}

// Fungsi untuk alert gabungan sistem
void displayCombinedAlerts() {
  Serial.println("\n🚨 SISTEM ALERTS:");
  
  bool has_alert = false;
  
  // Alert suhu
  if (temperature != -999) {
    if (temperature < TEMP_MIN) {
      Serial.println("   ⚠️  Suhu terlalu rendah - risiko pembekuan");
      has_alert = true;
    } else if (temperature > TEMP_MAX) {
      Serial.println("   🔥 Suhu terlalu tinggi - risiko penguapan berlebih");
      has_alert = true;
    }
  }
  
  // Alert kelembaban
  if (humidity != -999) {
    if (humidity > HUMIDITY_MAX) {
      Serial.println("   💧 Kelembaban tinggi - risiko kondensasi/jamur");
      has_alert = true;
    } else if (humidity < HUMIDITY_MIN) {
      Serial.println("   🏜️  Kelembaban rendah - penguapan meningkat");
      has_alert = true;
    }
  }
  
  // Alert level air (dari perhitungan sebelumnya)
  if (filtered_distance > 0) {
    float tinggi_air = JARAK_SENSOR_KE_DASAR - filtered_distance;
    if (tinggi_air < 0) tinggi_air = 0;
    float persentase = (tinggi_air / TINGGI_TANDON) * 100;
    
    if (persentase >= 95) {
      Serial.println("   🚨 Tandon hampir penuh - hentikan pengisian");
      has_alert = true;
    } else if (persentase <= 10) {
      Serial.println("   🆘 Air hampir habis - segera isi ulang!");
      has_alert = true;
    }
  }
  
  // Alert kombinasi suhu dan kelembaban
  if (temperature > 35 && humidity > 70) {
    Serial.println("   🌡️💧 Kondisi panas & lembab - pantau kualitas air");
    has_alert = true;
  }
  
  if (!has_alert) {
    Serial.println("   ✅ Semua sistem normal");
  }
}