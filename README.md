Analizador de Movimiento Cinemático (Wearable DSP)

Este proyecto consiste en un dispositivo wearable equipado con un sensor inercial BMI160 y un microcontrolador ESP32-C3 SuperMini. El sistema transmite datos de movimiento de 6 ejes (Acelerómetro + Giroscopio) a un panel de control web (dashboard.html) a través de USB Serial o Bluetooth LE (BLE) a alta resolución (100Hz).

El panel web incorpora un motor DSP (Procesamiento de Señal Digital) avanzado que permite limpiar la señal, fusionar los sensores y calcular en tiempo real la Componente Principal (PCA) del movimiento o la Magnitud 3D/6D, extrayendo la frecuencia dominante mediante FFT.

🚀 Características Principales

Fusión de Sensores: Capacidad para analizar Acelerómetro, Giroscopio o una fusión 6D de ambos simultáneamente.

Motor DSP en Tiempo Real: Filtros ajustables Pasa-Altos (High-Pass), Pasa-Bajos (Low-Pass) y suavizado de media móvil.

Transmisión Híbrida Optimizada: * BLE: Transmisión de datos en crudo (Binary RAW de 16 bytes) para evitar latencia y pérdida de paquetes.

USB: Transmisión en formato CSV con Auto-Reset de señales DTR/RTS.

Sincronización de Reloj: El microcontrolador envía su propio Timestamp para garantizar un análisis de frecuencia milimétricamente exacto sin importar el lag de la red.

🛠️ 1. Requisitos de Hardware

Microcontrolador: ESP32-C3 SuperMini.

Sensor: Módulo IMU BMI160 (Acelerómetro + Giroscopio).

Cableado: Cables flexibles finos (se recomienda silicona AWG 30).

Conexión al PC: Cable USB-C con soporte para transmisión de datos (no solo carga).

🔌 2. Esquema de Conexión (I2C a 400kHz)

| Pin del BMI160 | Pin del ESP32-C3 SuperMini | Función |
| VCC | 3.3V | Alimentación del sensor |
| GND | GND | Tierra común |
| SCL | GPIO 9 | Reloj de I2C |
| SDA | GPIO 8 | Datos de I2C |

💡 Consejo para Wearables: Suelda los cables directamente a los agujeros de la placa para que el montaje sea de "perfil bajo" y no abulte ni moleste al llevarlo puesto.

💻 3. Configuración del Firmware (Arduino IDE)

Abre Arduino IDE y ve a Archivo > Preferencias. Añade esta URL en el Gestor de URLs Adicionales: https://espressif.github.io/arduino-esp32/package_esp32_index.json

En Herramientas > Placa > Gestor de Tarjetas, instala esp32 de Espressif Systems.

Selecciona tu placa: ESP32C3 Dev Module.

Ve a Programa > Incluir Librería > Administrar Librerías..., busca e instala DFRobot_BMI160.

Asegúrate de que en Herramientas > USB CDC On Boot esté configurado en Enabled.

Pega el código .ino del proyecto y pulsa en Subir.

🌐 4. Uso del Dashboard Web (dashboard.html)

El panel de control es un archivo HTML independiente y responsivo. No requiere instalación ni internet activo (excepto la primera vez para cachear la librería Highcharts). Utiliza Google Chrome o Edge.

Panel de Filtros DSP (Recomendaciones)

Señal Base: Usa PCA para movimientos direccionales o Magnitud para vibraciones generales. Elige el sensor en el desplegable (Fusión recomendada para micromovimientos).

Refresco (s): 0.05 para animaciones fluidas a 20 FPS.

High-Pass (Hz): Ajusta a 0.5 o 1.0 para eliminar la gravedad (1G) y las derivas lentas de la mano.

Low-Pass (Hz): Ajusta a 12 para limpiar el ruido eléctrico manteniendo los temblores humanos reales.

Ponderación: Velocidad (1/f) atenúa el ruido blanco de alta frecuencia resaltando los movimientos físicos reales.

Métodos de Conexión

🔌 Conexión USB: Conecta por cable y selecciona el puerto serie. El panel forzará automáticamente el reinicio de las líneas DTR/RTS para garantizar la lectura fluida del CSV.

🔵 Conexión BLE: Alimenta el guante con batería. El panel cuenta con un sistema de reintentos automáticos de conexión GATT para evitar desconexiones prematuras. (En Android, requiere tener el bluetooth activado).

📊 5. Grabación y Exportación de Datos

Haz clic en el botón rojo 🔴 Grabar.

Realiza el movimiento a medir. Los datos se registrarán a 100Hz en crudo (RAW) sin aplicar los filtros DSP para preservar la calidad original de la señal en las 6 dimensiones.

Haz clic en ⏹️ Stop.

Se descargará automáticamente un archivo CSV con sincronización de reloj (Timestamp, Accel_X, Accel_Y, Accel_Z, Gyro_X, Gyro_Y, Gyro_Z).
