#define ULTRASONIC_ECHO 5 // PC6 
#define ULTRASONIC_TRIG 10 // PB6

void setup() { 
    Serial.begin (9600); 
    pinMode(ULTRASONIC_TRIG, OUTPUT); 
    pinMode(ULTRASONIC_ECHO, INPUT); 
}

void loop() { 
    int duration, cm; 
    digitalWrite(ULTRASONIC_TRIG, LOW); 
    delayMicroseconds(2); 
    digitalWrite(ULTRASONIC_TRIG, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(ULTRASONIC_TRIG, LOW); 
    duration = pulseIn(ULTRASONIC_ECHO, HIGH); 
    cm = duration / 29 / 2;

    Serial.print(cm); 
    Serial.println(" cm"); 
    delay(100); 
}
