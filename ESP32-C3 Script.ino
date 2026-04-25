#include <Wire.h>
#include <DFRobot_BMI160.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

const int i2c_sda = 8;
const int i2c_scl = 9;

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

BLECharacteristic *pCharacteristicTX;
bool deviceConnected = false;
bool sensorOk = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      BLEDevice::startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);
  Wire.begin(i2c_sda, i2c_scl);
  
  if (bmi160.softReset() != BMI160_OK || bmi160.I2cInit(i2c_addr) != BMI160_OK) {
    Serial.println("Advertencia: No se encontró el sensor BMI160. Se enviarán datos planos.");
    sensorOk = false;
  } else {
    Serial.println("Sensor BMI160 detectado correctamente.");
    sensorOk = true;
  }

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

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Sistema listo. Enviando datos crudos a 100Hz...");
}

void loop() {
  static unsigned long lastSampleTime = 0;
  const unsigned long interval = 10; // 100Hz

  if (millis() - lastSampleTime >= interval) {
    lastSampleTime = millis();

    // Las lecturas crudas del sensor son enteros de 16 bits (int16_t)
    int16_t raw_ax = 0, raw_ay = 0, raw_az = 16384; // 16384 = 1G en la escala por defecto
    int16_t raw_gx = 0, raw_gy = 0, raw_gz = 0;

    if (sensorOk) {
      int16_t accelGyro[6] = {0};
      bmi160.getAccelGyroData(accelGyro);
      
      // Mapeo según la librería DFRobot: [0,1,2] = Gyro, [3,4,5] = Accel
      raw_gx = accelGyro[0];
      raw_gy = accelGyro[1];
      raw_gz = accelGyro[2];
      raw_ax = accelGyro[3];
      raw_ay = accelGyro[4];
      raw_az = accelGyro[5];
    }

    // 1. ENVÍO POR USB (Formato CSV simple: mucho más rápido que JSON)
    Serial.printf("%d,%d,%d,%d,%d,%d\n", raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz);

    // 2. ENVÍO POR BLUETOOTH (Formato BINARIO: 12 bytes en total, entra en 1 solo paquete BLE)
    if (deviceConnected) {
      int16_t blePayload[6] = {raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz};
      // Enviamos el array directamente como bytes de memoria
      pCharacteristicTX->setValue((uint8_t*)blePayload, 12);
      pCharacteristicTX->notify();
    }
  }
}
