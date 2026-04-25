#include <Wire.h>
#include <DFRobot_BMI160.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs estándar para el servicio UART de Nordic
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

// Configuración de Pines I2C para ESP32-C3 SuperMini
const int i2c_sda = 8;
const int i2c_scl = 9;

// Instancia del sensor de la nueva librería DFRobot
DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

BLECharacteristic *pCharacteristicTX;
bool deviceConnected = false;
bool sensorOk = false; // Nueva variable para saber si el sensor está funcionando

// Callback de conexión BLE
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      BLEDevice::startAdvertising(); // Reiniciar anuncios de forma segura
    }
};

void setup() {
  Serial.begin(115200);
  
  // 1. Inicializar I2C
  Wire.begin(i2c_sda, i2c_scl);
  
  // 2. Inicializar Sensor BMI160 (DFRobot) con manejo de errores suave
  if (bmi160.softReset() != BMI160_OK || bmi160.I2cInit(i2c_addr) != BMI160_OK) {
    Serial.println("Advertencia: No se encontró el sensor BMI160. Se enviarán datos planos.");
    sensorOk = false;
  } else {
    Serial.println("Sensor BMI160 detectado correctamente.");
    sensorOk = true;
  }

  // 3. Configurar BLE
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

  Serial.println("Sistema listo. Enviando datos a 100Hz...");
}

void loop() {
  static unsigned long lastSampleTime = 0;
  const unsigned long interval = 10; // Reducido a 10ms (100Hz) para mayor resolución temporal

  if (millis() - lastSampleTime >= interval) {
    lastSampleTime = millis();

    float accX = 0.0, accY = 0.0, accZ = 0.0;
    float gyrX = 0.0, gyrY = 0.0, gyrZ = 0.0;

    if (sensorOk) {
      // El sensor está bien, leemos los datos reales
      int16_t accelGyro[6] = {0};
      bmi160.getAccelGyroData(accelGyro);

      // Convertir a valores físicos
      gyrX = accelGyro[0] * (2000.0 / 32768.0);
      gyrY = accelGyro[1] * (2000.0 / 32768.0);
      gyrZ = accelGyro[2] * (2000.0 / 32768.0);

      accX = accelGyro[3] / 16384.0;
      accY = accelGyro[4] / 16384.0;
      accZ = accelGyro[5] / 16384.0;
    } else {
      // El sensor falla: enviamos datos planos para confirmar conexión
      // Ponemos Z a 1.0 para simular la gravedad normal
      accZ = 1.0; 
    }

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
