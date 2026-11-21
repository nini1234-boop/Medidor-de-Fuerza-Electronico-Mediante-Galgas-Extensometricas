#include <LiquidCrystal.h>

// Definición de pines para la LCD 16x2 en modo paralelo (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(22, 23, 5, 18, 19, 21);

// Pines de Hardware
const int ADC_PIN = 34;         // Pin analógico del ESP32
const int BTN_TARA_PIN = 4;     // Botón físico para hacer la Tara (Pin 4)
const int BTN_RESET_PIN = 25;   // Botón físico para Reiniciar (Pin 25)

const float VREF = 3.3;
const int ADC_MAX = 4095;
const int N = 64;
const int MAX_PUNTOS = 10;      // Capacidad máxima de puntos de calibración

// Arreglos dinámicos para almacenar los pesos y lecturas
float pesosKg[MAX_PUNTOS] = {0};
float lecturas[MAX_PUNTOS] = {0};
int totalPuntosRegistrados = 0;

float ceroADC = 0;
float cuentasPorKg = 0;
bool taraLista = false;
bool calibracionLista = false;

// Variables para antirrebote de botones físicos
unsigned long ultimoDebounceTara = 0;
unsigned long ultimoDebounceReset = 0;
const unsigned long debounceDelay = 250;

// Función para leer el ADC promediando N muestras
float leerADC() {
  long suma = 0;
  for (int i = 0; i < N; i++) {
    suma += analogRead(ADC_PIN);
    delay(3);
  }
  return (float)suma / N;
}

// Función para realizar la Tara
void realizarTara() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tarando...");
  
  ceroADC = leerADC();
  taraLista = true;
  calibracionLista = false;
  totalPuntosRegistrados = 0;

  for (int i = 0; i < MAX_PUNTOS; i++) {
    pesosKg[i] = 0;
    lecturas[i] = 0;
  }

  Serial.println("\n=== TARA REALIZADA ===");
  Serial.print("Tara ADC: ");
  Serial.println(ceroADC, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tara Lista!");
  delay(1200);
  lcd.clear();
}

// Función para reiniciar el sistema
void reiniciarSistema() {
  taraLista = false;
  calibracionLista = false;
  ceroADC = 0;
  totalPuntosRegistrados = 0;
  for (int i = 0; i < MAX_PUNTOS; i++) {
    pesosKg[i] = 0;
    lecturas[i] = 0;
  }

  Serial.println("\n=== SISTEMA RESETEADO ===");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Reiniciado");
  delay(1200);
  lcd.clear();
}

// Función para calcular la calibración usando todos los puntos registrados (Mínimos Cuadrados)
void calcularCalibracion() {
  if (totalPuntosRegistrados == 0) {
    Serial.println("Error: No hay puntos registrados para calibrar.");
    return;
  }

  float numerador = 0;
  float denominador = 0;

  for (int i = 0; i < totalPuntosRegistrados; i++) {
    float delta = lecturas[i] - ceroADC;
    numerador += pesosKg[i] * delta;
    denominador += delta * delta;
  }

  if (denominador > 0 && numerador != 0) {
    cuentasPorKg = denominador / numerador;
    calibracionLista = true;

    Serial.println("\n=== CALIBRACION COMPLETADA ===");
    Serial.print("Puntos usados: "); Serial.println(totalPuntosRegistrados);
    Serial.print("Cuentas por kg: ");
    Serial.println(cuentasPorKg, 3);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibrado OK!");
    delay(1500);
    lcd.clear();
  } else {
    Serial.println("Error: senal ADC insuficiente.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error Calibracion");
    delay(1500);
    lcd.clear();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  // Configurar botones físicos con resistencia pull-up interna
  pinMode(BTN_TARA_PIN, INPUT_PULLUP);
  pinMode(BTN_RESET_PIN, INPUT_PULLUP);

  // Configuración del ADC del ESP32
  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db);

  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();

  Serial.println("\n--- SISTEMA DE PESAJE LIBRE INICIADO ---");
  Serial.println("1. Haz Tara con Boton 1 (Pin 4) o escribe 'T'.");
  Serial.println("2. Envía puntos con formato ID:PESO (Ej: 1:0.22, 2:3.565).");
  Serial.println("3. Envía 'FIN' cuando termines de registrar.");
  Serial.println("4. Envía 'R' o usa Boton 2 (Pin 25) para Reiniciar.");
}

void loop() {
  unsigned long tiempoActual = millis();

  // --- 1. LECTURA DE BOTONES FÍSICOS ---
  if (digitalRead(BTN_TARA_PIN) == LOW) {
    if (tiempoActual - ultimoDebounceTara > debounceDelay) {
      realizarTara();
      ultimoDebounceTara = tiempoActual;
    }
  }

  if (digitalRead(BTN_RESET_PIN) == LOW) {
    if (tiempoActual - ultimoDebounceReset > debounceDelay) {
      reiniciarSistema();
      ultimoDebounceReset = tiempoActual;
    }
  }

  // --- 2. CONTROL POR MONITOR SERIE ---
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("T")) {
      realizarTara();
    } 
    else if (cmd.equalsIgnoreCase("R")) {
      reiniciarSistema();
    }
    else if (cmd.equalsIgnoreCase("FIN")) {
      if (!taraLista) {
        Serial.println("¡Primero realiza la tara!");
      } else if (totalPuntosRegistrados == 0) {
        Serial.println("¡Debes registrar al menos un peso antes de finalizar!");
      } else {
        calcularCalibracion();
      }
    }
    else {
      // Procesar formato ID:PESO (ej: 1:0.22)
      int colonIndex = cmd.indexOf(':');
      if (colonIndex != -1) {
        String idStr = cmd.substring(0, colonIndex);
        String pesoStr = cmd.substring(colonIndex + 1);
        
        int objNum = idStr.toInt(); 
        float pesoVal = pesoStr.toFloat(); 

        if (!taraLista) {
          Serial.println("¡Primero realiza la tara enviando 'T' o presionando el Boton 1!");
        } else if (totalPuntosRegistrados >= MAX_PUNTOS) {
          Serial.println("¡Has alcanzado el límite máximo de puntos de calibración!");
        } else {
          // Guardar en la posición correspondiente del arreglo dinámico
          pesosKg[totalPuntosRegistrados] = pesoVal; 

          Serial.print("Registrando punto [");
          Serial.print(objNum);
          Serial.print("] con peso: ");
          Serial.print(pesoVal);
          Serial.println(" kg");

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Reg Punto ");
          lcd.print(objNum);
          lcd.setCursor(0, 1);
          lcd.print("W: ");
          lcd.print(pesoVal);
          lcd.print("kg");

          lecturas[totalPuntosRegistrados] = leerADC();
          totalPuntosRegistrados++;
          delay(1000);

          Serial.print("ADC leido y guardado. Total puntos: ");
          Serial.println(totalPuntosRegistrados);
        }
      } else {
        Serial.println("Comando no reconocido. Use 'T', 'R', 'FIN' o el formato ID:PESO (Ej: 1:0.22)");
      }
    }
  }

  // --- 3. MEDICIÓN Y ACTUALIZACIÓN EN TIEMPO REAL ---
  float adc = leerADC();
  float voltaje = (adc * VREF / ADC_MAX) * 1.621828; // Factor de ajuste

  // --- ACTUALIZAR PANTALLA LCD ---
  lcd.setCursor(0, 0);
  if (!taraLista) {
    lcd.print("1. Balanza Vacia ");
    lcd.setCursor(0, 1);
    lcd.print("Pulse Tara (Btn1)");
  } 
  else if (taraLista && !calibracionLista) {
    // Muestra cuántos puntos lleva registrados y recuerda enviar FIN
    lcd.setCursor(0, 0);
    lcd.print("Puntos Reg: ");
    lcd.print(totalPuntosRegistrados);
    lcd.setCursor(0, 1);
    lcd.print("Envie FIN para cal");
  } 
  else {
    // Modo de pesaje activo (calibrado)
    float deltaADC = adc - ceroADC;
    float pesoKg = deltaADC / cuentasPorKg;
    float pesoGramos = pesoKg * 1000;

    lcd.setCursor(0, 0);
    lcd.print("Peso:           ");
    lcd.setCursor(6, 0);
    lcd.print(pesoGramos, 1);
    lcd.print(" g   ");

    lcd.setCursor(0, 1);
    lcd.print("V:");
    lcd.print(voltaje, 2);
    lcd.print(" A:");
    lcd.print((int)adc);
    lcd.print("   ");
  }

  // Depuración por Monitor Serie
  Serial.print("ADC: ");
  Serial.print(adc, 1);
  Serial.print(" | Voltaje: ");
  Serial.print(voltaje, 3);
  if (calibracionLista) {
    float pesoGramos = ((adc - ceroADC) / cuentasPorKg) * 1000;
    Serial.print(" | Peso: ");
    Serial.print(pesoGramos, 1);
    Serial.print(" g");
  }
  Serial.println();

  delay(200);
}
