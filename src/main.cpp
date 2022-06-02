  #include <IRremote.h>
  int IRsig = 12;
  IRrecv IR(IRsig);
  decode_results cmd;
  const byte pingPin1 = 2;
  const byte echoPin1 = 3;
  const byte pingPin2 = 4;
  const byte echoPin2 = 5;
  const byte pingPin3 = 6;
  const byte echoPin3 = 7;           
  long duration1, inches1, cm1;
  long duration2, inches2, cm2;
  long duration3, inches3, cm3;
  
  const byte IN1 = 8;
  const byte IN2 = 9;
  const byte IN3 = 10;  
  const byte IN4 = 11;

  boolean Follow, Navigate;

  class blink  {
    byte ledPin;
    int onTime;
    int offTime;
    int ledState; //initial led state
    unsigned long previousMillis;
    
    public : blink() {
      ledState = LOW; 
      previousMillis = 0;
      void upDate(byte pin, int on, int off );
    }
      
     void upDate(byte pin, int on, int off ) {
      ledPin = pin;
      pinMode(ledPin, OUTPUT);
      onTime = on;
      offTime = off;
      unsigned long currentMillis = millis();
      if ((ledState == HIGH) && (currentMillis - previousMillis >= onTime)) {
        ledState = LOW;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState); 
      } else if((ledState == LOW) && (currentMillis - previousMillis >= offTime)) { 
        ledState = HIGH;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
      }
    }
  };

  class motorDelay {
    int activeTime;
    int deadTime;
    int motorState;
    int IN1S;
    int IN2S;
    int IN3S;
    int IN4S;
    unsigned long previousMillis;

    public : motorDelay() {
       previousMillis = 0;
       motorState = LOW;
       void upDate(int IN1S, int IN2S, int IN3S, int IN4S, int activeTime, int deadTime);
    }

    void upDate(int IN1S, int IN2S, int IN3S, int IN4S, int activeTime, int deadTime) {
        unsigned long currentMillis = millis();
        if((motorState == LOW) && (currentMillis - previousMillis >= activeTime)) {
            motorState = HIGH;
            previousMillis = currentMillis;
            digitalWrite(IN1, IN1S);
            digitalWrite(IN2, IN2S);
            digitalWrite(IN3, IN3S);
            digitalWrite(IN4, IN4S); 
        } else if ((motorState == HIGH) && (currentMillis - previousMillis >= deadTime)) {
            motorState = LOW;
            previousMillis = currentMillis;
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
        }
    }
  };

  motorDelay motorD;
  blink led;
  
  void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    IR.enableIRIn();
    pinMode(13, OUTPUT);
    pinMode(pingPin1, OUTPUT);
    pinMode(pingPin2, OUTPUT);
    pinMode(pingPin3, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(echoPin2, INPUT);
    pinMode(echoPin3, INPUT);
    Serial.println("Hello");
    for(int j = 0; j <= 10; j++) {
      digitalWrite(13, HIGH);
      delay(50);
      digitalWrite(13, LOW);
      delay(50);
    };
  }
  
  void loop() {
    // put your main code here, to run repeatedly:
     while(IR.decode(&cmd)){
       Serial.println(cmd.value, DEC);
       IR.resume();
     };
     led.upDate(13,500,500);
     digitalWrite(pingPin1,LOW);
     delayMicroseconds(2);
     digitalWrite(pingPin1, HIGH);
     delayMicroseconds(10);
     digitalWrite(pingPin1, LOW);
     duration1= pulseIn(echoPin1, HIGH);
     inches1 = duration1 / 74 / 2;
     cm1 = duration1 / 29 / 2;
    
     digitalWrite(pingPin2,LOW);
     delayMicroseconds(2);
     digitalWrite(pingPin2, HIGH);
     delayMicroseconds(10);
     digitalWrite(pingPin2, LOW);
     duration2= pulseIn(echoPin2, HIGH);
     inches2 = duration2 / 74 / 2;
     cm2 = duration2 / 29 / 2;
    
     digitalWrite(pingPin3,LOW);
     delayMicroseconds(2);
     digitalWrite(pingPin3, HIGH);
     delayMicroseconds(10);
     digitalWrite(pingPin3, LOW);
     duration3= pulseIn(echoPin3, HIGH);
     inches3 = duration3 / 74 / 2;
     cm3 = duration3 / 29 / 2;
  
     int LPD = cm1 - cm3;
     int RPD = cm3 - cm1;
     
     Serial.print("LEFT:");
     Serial.print(cm1); 
     Serial.print(" | "); 
     Serial.print(LPD);
     Serial.print("cm   ");
     Serial.print("CENTER:");
     Serial.print(cm2);
     Serial.print("cm   ");
     Serial.print("RIGHT:");
     Serial.print(cm3);
     Serial.print(" | "); 
     Serial.print(RPD);
     Serial.println("cm");
       
    if(cmd.value == 802421760) {
      Follow = true;
      Navigate = false;
    } else if (cmd.value == 802417680) {
      Navigate = true;
      Follow = false;
    };

    /*if((cmd.value != 802421760) || (cmd.value !=802417680)) {
      Navigate = false;
      Follow = false;
    };*/
       
    // NAVIGATE MODE
    if(Navigate == true) {
      Serial.println("Navigating...");
      if (cm1 <= 10) {
          //turn right
          motorD.upDate(HIGH, LOW, LOW, HIGH, 25, 100); 
      } else if (cm3 <= 10) {
          //turn left
          motorD.upDate(LOW, HIGH, HIGH, LOW, 25, 100); 
      } else {
        // move forward
         motorD.upDate(LOW, HIGH, LOW, HIGH, 50, 50);
      }
    };
    
    if (Follow == true){
        // FOLLOW MODE
        if(( cm2 >= 11) && (cm2 <= 25) ) {
           Serial.println("Following...");
           // move forward
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, HIGH);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, HIGH);
         } else if((cm2 < 6)){
           //move backwards
           digitalWrite(IN1, HIGH);
           digitalWrite(IN2, LOW);
           digitalWrite(IN3, HIGH);
           digitalWrite(IN4, LOW);
         } else {
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, LOW);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, LOW);
         };
         if((cm1 <= 25) || (cm3 <=25)) {
            if((LPD >= 6)) {
              // turn right
              motorD.upDate(HIGH, LOW, LOW, HIGH, 25, 25);
            } else if((RPD >= 6)) {
                //turn left
                motorD.upDate(LOW, HIGH, HIGH, LOW, 25, 25);
            }
         }
     }
     
  }