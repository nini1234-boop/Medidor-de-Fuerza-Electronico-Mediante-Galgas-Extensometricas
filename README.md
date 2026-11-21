# Medidor de Fuerza Electrónico mediante Galgas Extensiométricas

## Descripción del Proyecto
Este repositorio contiene el firmware de adquisición y procesamiento de señales para un sistema electrónico de medición de fuerza/masa basado en **galgas extensiométricas**, acondicionamiento mediante amplificador de instrumentación **AD620**, microcontrolador **ESP32** y visualización local en pantalla **LCD 16x2**.

### Características del Firmware
* **Adquisición de datos:** Lectura del ADC interno de 12 bits del ESP32 (`GPIO 34`, atenuación `ADC_11db`) aplicando un filtro de promedio móvil de $N = 64$ muestras para mitigar ruido eléctrico.
* **Calibración Multipunto (Mínimos Cuadrados):** Permite registrar hasta 10 puntos de calibración con pesos patrón mediante comandos por Monitor Serie (`ID:PESO`) y calcula la pendiente óptima de conversión mediante regresión lineal por mínimos cuadrados.
* **Control Físico y Serial:** Gestión de tara (`GPIO 4` o comando `T`) y reinicio del sistema (`GPIO 25` o comando `R`) con filtrado antirrebote (*debounce*) por software.
* **Visualización:** Despliegue en tiempo real de peso (en gramos), voltaje acondicionado y cuentas del ADC en pantalla LCD 16x2 (modo paralelo de 4 bits) y telemetría por UART a 115200 baudios.

## Pinout del Sistema (ESP32)
| Componente | Pin ESP32 | Descripción |
| :--- | :--- | :--- |
| **Salida AD620** | `GPIO 34` | Entrada analógica ADC1 (señal acondicionada) |
| **Botón Tara** | `GPIO 4` | Pulsador con `INPUT_PULLUP` interno |
| **Botón Reset** | `GPIO 25` | Pulsador con `INPUT_PULLUP` interno |
| **LCD 16x2 (RS, E)** | `GPIO 22, 23` | Pines de control de pantalla |
| **LCD 16x2 (D4 - D7)** | `GPIO 5, 18, 19, 21` | Bus de datos de 4 bits |

## Información Académica
* **Universidad:** Universidad Nacional de Piura (UNP)
* **Carrera:** Ingeniería Electrónica y Telecomunicaciones
* **Curso:** Instrumentación Industrial
* **Año:** 2026

## Integrantes
* Barrientos Morante, María
* Trelles Flores, Néstor Iván

## Estructura del Repositorio
```text
├── firmware/
│   └── medidor_fuerza.ino   # Código fuente de adquisición, calibración y procesamiento
├── .gitignore               # Archivos temporales y compilados ignorados por Git
└── README.md                # Documentación técnica y académica del proyecto
