#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(1000);

  Wire.begin(21, 22); // SDA = GPIO21, SCL = GPIO22
  Serial.println("Scanner I2C iniciado...");
}

void loop() {
  byte count = 0;

  Serial.println("Procurando dispositivos no barramento I2C...");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo encontrado no endereco 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      count++;
    }
  }

  if (count == 0) {
    Serial.println("Nenhum dispositivo encontrado. Confira fiacao e alimentacao.");
  } else {
    Serial.print(count);
    Serial.println(" dispositivo(s) encontrado(s).");
  }

  Serial.println("---");
  delay(3000);
}
