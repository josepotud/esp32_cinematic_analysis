#include <Wire.h>
#include <BMI160Gen.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs estándar para el servicio UART de Nordic (compatibles con el Dashboard)
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

// Configuración de Pines I2C para ESP32-C3 SuperMini
const int i2c_sda = 8;
const int i2c_scl = 9;

BLECharacteristic *pCharacteristicTX;
bool deviceConnected = false;

// Callback de conexión BLE
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      // Reiniciar anuncios para permitir reconexión
      pServer->getAdvertising()->start();
    }
};

void setup() {
  Serial.begin(115200);
  
  // 1. Inicializar I2C y BMI160
  Wire.begin(i2c_sda, i2c_scl);
  if (!BMI160.begin(BMI160_I2C_MODE, 0x69)) {
    Serial.println("Error: No se encontró el sensor BMI160.");
    while (1);
  }

  // 2. Configurar BLE
  BLEDevice::init("ESP32-C3-Serial");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristicTX = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_TX,
                       BLECharacteristic::PROPERTY_NOTIFY
                     );
  
  pCharacteristicTX->addDescriptor(new BLE2902());
  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("Sistema listo. Enviando datos a 30Hz...");
}

void loop() {
  static unsigned long lastSampleTime = 0;
  const unsigned long interval = 33; // ~30Hz (1000ms / 30)

  if (millis() - lastSampleTime >= interval) {
    lastSampleTime = millis();

    int ax, ay, az, gx, gy, gz;
    
    // Leer datos del sensor
    BMI160.readMotion6(ax, ay, az, gx, gy, gz);

    // Convertir a valores físicos (Escalas por defecto: +/- 2G y +/- 2000°/s)
    float accX = ax / 16384.0;
    float accY = ay / 16384.0;
    float accZ = az / 16384.0;
    float gyrX = gx * (2000.0 / 32768.0);
    float gyrY = gy * (2000.0 / 32768.0);
    float gyrZ = gz * (2000.0 / 32768.0);

    // Construir trama JSON para el Dashboard
    char payload[128];
    snprintf(payload, sizeof(payload), 
             "{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}",
             accX, accY, accZ, gyrX, gyrY, gyrZ);

    // Enviar por USB Serial
    Serial.println(payload);

    // Enviar por Bluetooth LE
    if (deviceConnected) {
      pCharacteristicTX->setValue(payload);
      pCharacteristicTX->notify();
    }
  }
}
