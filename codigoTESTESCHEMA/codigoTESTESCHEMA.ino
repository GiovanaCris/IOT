// --------- PINOS ----------
const int ledPin = 14;     
const int buttonPin = 15;  

bool ledState = false;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // usa resistor interno de pull-up
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  // Detecta clique (HIGH -> LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState; // alterna estado
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    Serial.println(ledState ? "LED LIGADO" : "LED DESLIGADO");
    delay(300); // debounce
  }

  lastButtonState = buttonState;
}