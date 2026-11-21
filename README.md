# Medidor de Fuerza Electrónico mediante Galgas Extensiométricas

## Descripción del Proyecto
Este repositorio contiene el firmware de adquisición y procesamiento de señales para un sistema electrónico de medición de fuerza basado en **galgas extensiométricas (strain gauges)**. 

El sistema acondiciona la señal diferencial proveniente de un puente de Wheatstone, realiza la conversión analógica-digital (ADC) y aplica algoritmos de procesamiento digital (filtrado de ruido, calibración lineal y tara) para obtener mediciones precisas de fuerza en Newtons (N) o kilogramos-fuerza (kgf).

### Características del Firmware
* **Adquisición de datos:** Lectura de alta resolución del sensor de deformación (ej. módulo HX711 o ADC interno con amplificador de instrumentación INA125/AD620).
* **Procesamiento de señal:** Aplicación de filtro de media móvil para reducción de ruido eléctrico, función de tara automática y escalamiento mediante factor de calibración.
* **Salida de datos:** Transmisión de las lecturas procesadas mediante comunicación serial (UART) para su visualización o registro.

## Información Académica
* **Universidad:** Universidad Nacional de Piura (UNP)
* **Facultad / Carrera:** Ingeniería Electrónica y Telecomunicaciones
* **Curso:** Instrumentación Industrial
* **Año:** 2026

## Integrantes
* Barrientos Morante, María
* Trelles Flores, Néstor Iván

## Estructura del Repositorio
```text
├── firmware/       # Código fuente principal de adquisición y procesamiento
├── docs/           # Diagramas de conexión, puente de Wheatstone y hojas de datos
├── .gitignore      # Reglas de exclusión de archivos temporales y compilados
└── README.md       # Documentación general del proyecto
