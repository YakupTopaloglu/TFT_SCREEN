#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// TFT Shield pin tanımlamaları
MCUFRIEND_kbv tft;

// Touch screen pin tanımlamaları (çoğu Arduino TFT shield için standart)
#define YP A3  // Y+ analog pin
#define XM A2  // X- analog pin
#define YM 9   // Y- digital pin
#define XP 8   // X+ digital pin

// Touch screen kalibrasyonu - Bu değerleri ayarlamanız gerekebilir!
#define TS_MINX 120
#define TS_MINY 70
#define TS_MAXX 900
#define TS_MAXY 920

// Touch screen basınç değerleri
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Renkler
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
#define PURPLE  0x8010

// Değişkenler
int sayac = 0;
int renk_index = 0;
unsigned long onceki_zaman = 0;
int x_pos = 20;
int y_pos = 80;
int yon = 1;
bool animasyon_aktif = true;

uint16_t renkler[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, PURPLE};

// Fonksiyon prototipleri (ÖNEMLİ!)
void ekranCiz();
void cizButon(int x, int y, int w, int h, uint16_t renk, String metin);
void durumMesaji(String mesaj, uint16_t renk);
void butonAnimasyon(int x, int y, int w, int h, uint16_t renk);

void setup() {
  Serial.begin(9600);
  
  // TFT başlat
  uint16_t ID = tft.readID();
  Serial.print("TFT ID: 0x");
  Serial.println(ID, HEX);
  
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(0); // Dikey mod (240x320)
  
  ekranCiz();
}

void ekranCiz() {
  // İlk ekran
  tft.fillScreen(BLACK);
  
  // Başlık
  tft.fillRect(0, 0, 240, 40, BLUE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 12);
  tft.println("TFT Demo");
  
  // Sayaç alanı çerçevesi
  tft.drawRect(10, 50, 220, 50, WHITE);
  
  // Animasyon alanı çerçevesi
  tft.drawRect(10, 110, 220, 80, WHITE);
  
  // Durum mesajı alanı
  tft.drawRect(10, 200, 220, 30, CYAN);
  
  // Butonları çiz
  cizButon(20, 240, 60, 35, GREEN, "START");
  cizButon(90, 240, 60, 35, RED, "STOP");
  cizButon(160, 240, 60, 35, BLUE, "RESET");
  
  // Alt bilgi
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(50, 285);
  tft.println("Touch Aktif!");
  tft.setCursor(60, 300);
  tft.println("240x320 Ekran");
}

void loop() {
  unsigned long simdiki_zaman = millis();
  
  // Her 500ms'de bir güncelleme
  if (simdiki_zaman - onceki_zaman >= 500 && animasyon_aktif) {
    onceki_zaman = simdiki_zaman;
    
    // Sayaç göster
    tft.fillRect(15, 55, 210, 40, BLACK);
    tft.setTextColor(YELLOW);
    tft.setTextSize(3);
    tft.setCursor(20, 65);
    tft.print("Sayac:");
    tft.print(sayac);
    sayac++;
    
    // Hareket eden top
    if (animasyon_aktif) {
      tft.fillCircle(x_pos, y_pos, 10, BLACK);
      x_pos += (4 * yon);
      
      if (x_pos >= 220 || x_pos <= 20) {
        yon = -yon;
        renk_index = (renk_index + 1) % 8;
      }
      
      tft.fillCircle(x_pos, y_pos, 10, renkler[renk_index]);
    }
  }
  
  // Touch kontrolü - DÜZELTİLDİ!
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  
  // Pin modlarını geri ayarla (ÖNEMLİ!)
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    
    // Ham değerleri göster (kalibrasyon için)
    Serial.print("Ham X: "); Serial.print(p.x);
    Serial.print(" Ham Y: "); Serial.print(p.y);
    Serial.print(" Basinc: "); Serial.println(p.z);
    
    // Koordinatları dönüştür (rotation 0 için)
    int px = map(p.y, TS_MINY, TS_MAXY, 0, 240);
    int py = map(p.x, TS_MINX, TS_MAXX, 320, 0);
    
    Serial.print("Donusturulmus X: "); Serial.print(px);
    Serial.print(" Y: "); Serial.println(py);
    
    // START butonu kontrolü
    if (px >= 20 && px <= 80 && py >= 240 && py <= 275) {
      Serial.println("START BUTONU BASILDI!");
      animasyon_aktif = true;
      durumMesaji("START basildi!", GREEN);
      butonAnimasyon(20, 240, 60, 35, GREEN);
      delay(200);
    }
    
    // STOP butonu kontrolü
    else if (px >= 90 && px <= 150 && py >= 240 && py <= 275) {
      Serial.println("STOP BUTONU BASILDI!");
      animasyon_aktif = false;
      durumMesaji("STOP basildi!", RED);
      butonAnimasyon(90, 240, 60, 35, RED);
      delay(200);
    }
    
    // RESET butonu kontrolü
    else if (px >= 160 && px <= 220 && py >= 240 && py <= 275) {
      Serial.println("RESET BUTONU BASILDI!");
      sayac = 0;
      x_pos = 20;
      renk_index = 0;
      animasyon_aktif = true;
      durumMesaji("RESET basildi!", BLUE);
      butonAnimasyon(160, 240, 60, 35, BLUE);
      
      // Animasyon alanını temizle
      tft.fillRect(11, 111, 218, 78, BLACK);
      delay(200);
    }
    
    delay(100); // Debounce için bekleme
  }
}

void cizButon(int x, int y, int w, int h, uint16_t renk, String metin) {
  tft.fillRoundRect(x, y, w, h, 5, renk);
  tft.drawRoundRect(x, y, w, h, 5, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  // Metni ortala
  int metin_uzunluk = metin.length() * 6;
  int metin_x = x + (w - metin_uzunluk) / 2;
  int metin_y = y + (h - 8) / 2;
  
  tft.setCursor(metin_x, metin_y);
  tft.println(metin);
}

void durumMesaji(String mesaj, uint16_t renk) {
  tft.fillRect(11, 201, 218, 28, BLACK);
  tft.setTextColor(renk);
  tft.setTextSize(1);
  
  // Mesajı ortala
  int metin_uzunluk = mesaj.length() * 6;
  int metin_x = 120 - (metin_uzunluk / 2);
  
  tft.setCursor(metin_x, 211);
  tft.println(mesaj);
}

void butonAnimasyon(int x, int y, int w, int h, uint16_t renk) {
  // Buton basma efekti
  tft.fillRoundRect(x, y, w, h, 5, WHITE);
  delay(50);
  tft.fillRoundRect(x, y, w, h, 5, renk);
  tft.drawRoundRect(x, y, w, h, 5, WHITE);
}