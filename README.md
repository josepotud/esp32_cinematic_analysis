Analizador de Movimiento Cinemático (Wearable)
Este proyecto consiste en un guante (o dispositivo wearable) equipado con un sensor inercial BMI160 y un microcontrolador ESP32-C3 SuperMini. El sistema transmite los datos de movimiento (acelerómetro y giroscopio de 6 ejes) a un panel de control web (dashboard.html) a través de USB Serial o Bluetooth LE (BLE) a alta resolución (100Hz).
El panel web incorpora un motor DSP avanzado que permite limpiar la señal antes de calcular en tiempo real la Componente Principal (PCA) del movimiento y extraer la frecuencia dominante (Hz) y su intensidad mediante una FFT en un rango configurable.
🛠️ 1. Requisitos de Hardware
Microcontrolador: ESP32-C3 SuperMini.
Sensor: Módulo IMU BMI160 (Acelerómetro + Giroscopio).
Cableado: Cables flexibles finos (se recomienda silicona AWG 30) para evitar que el guante quede rígido.
Conexión al PC: Cable USB-C con soporte para transmisión de datos (no solo carga).
🔌 2. Esquema de Conexión (Hardware)
El sensor BMI160 se comunica con el ESP32-C3 utilizando el protocolo I2C. Debes soldar o conectar los pines de la siguiente manera:
Pin del BMI160
Pin del ESP32-C3 SuperMini
Función
VCC
3.3V
Alimentación del sensor
GND
GND
Tierra común
SCL
GPIO 9
Reloj de I2C
SDA
GPIO 8
Datos de I2C

💡 Consejo para Wearables: Intenta evitar los "pines negros" (cabezales). Suelda los cables directamente a los agujeros de la placa para que el montaje sea de "perfil bajo" y no abulte ni moleste en el guante.
💻 3. Configuración del Software (Arduino IDE)
Para cargar el código en el ESP32-C3, sigue estos pasos:
A. Preparar el Arduino IDE
Abre Arduino IDE y ve a Archivo > Preferencias.
En Gestor de URLs Adicionales de Tarjetas, añade: https://espressif.github.io/arduino-esp32/package_esp32_index.json
Ve a Herramientas > Placa > Gestor de Tarjetas, busca esp32 e instala la versión oficial de Espressif Systems.
Selecciona tu placa: Herramientas > Placa > ESP32 Arduino > ESP32C3 Dev Module (o similar).
B. Instalar las Librerías
Ve a Programa > Incluir Librería > Administrar Librerías...
Busca BMI160.
Localiza e instala la librería llamada DFRobot_BMI160 (por DFRobot).
C. Subir el Código
Pega el código .ino del proyecto.
Asegúrate de que en Herramientas > USB CDC On Boot esté configurado en Enabled (necesario en algunas placas C3 para que funcione el Serial Monitor por el USB nativo).
Pulsa en Subir.
🌐 4. Uso del Panel de Control Web (dashboard.html)
El panel de control es un archivo HTML independiente. No requiere instalación ni internet activo (excepto la primera vez para cachear la librería de gráficos de Highcharts).
Filtros DSP (Nuevas Funcionalidades)
En la barra lateral izquierda, encontrarás el panel de ajustes DSP para limpiar la señal en tiempo real:
Suavizado: Aplica un filtro de media móvil. Valores mayores suavizan la señal, ideal para eliminar picos erráticos de choque.
High-Pass (Hz): Elimina el componente continuo (DC), incluyendo la aceleración de la gravedad (1G). Recomendado en 0.5 Hz.
Low-Pass (Hz): Bloquea ruidos eléctricos de alta frecuencia. Recomendado por debajo del límite de Nyquist (< 50 Hz).
Márgenes FFT: Define la ventana de frecuencias en la que el sistema buscará la vibración dominante, ignorando los artefactos fuera de ese rango.
Métodos de Conexión
Conexión USB: Conecta la placa por cable, pulsa el botón 🔌 USB y selecciona el puerto serie ("Dispositivo Serie USB" o "JTAG/serial debug unit"). Recomendado para máxima estabilidad a 100Hz en PC.
Conexión BLE: Alimenta el guante con batería, pulsa el botón 🔵 BLE y selecciona ESP32-C3-Serial. Nota para Android: Requiere tener activada la Ubicación (GPS) por políticas del navegador.
📊 5. Grabación y Extracción de Datos (CSV)
Con el dispositivo transmitiendo datos, haz clic en el botón rojo 🔴 Grabar.
Realiza el movimiento a medir. Los datos se registrarán en crudo (RAW) sin aplicar los filtros para preservar la calidad original.
Haz clic en el botón ⏹️ Stop.
Se descargará automáticamente un archivo .csv con las columnas: Timestamp, Accel_X, Accel_Y, Accel_Z, Gyro_X, Gyro_Y, Gyro_Z.
