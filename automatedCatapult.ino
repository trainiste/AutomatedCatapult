#include <Servo.h>
#include <math.h>

/*
   DEFINIZIONE VARIABILI DISTANCE SENSOR HC-SR04
*/

#define echoPin 12 // Echo Pin
#define trigPin 10 // Trigger Pin

/*
  DEFINIZIONE PIN COLLEGAMENTI CON ARDUINO
*/

#define gancioPin 2
#define incliPin  4
#define mollaPin  6
#define butPin    8

/*
   DEFINIZIONE VARIABILI ED OFFSET OPERATIVI
*/

#define AGGANCIA    85 //Servo agganciato
#define SGANCIA     0 //Servo sganciato
#define OFFSETMOLLA 30 //Per tirare al minimo la molla
int OFFSETDIST; //Per colpiere in pieno l'obiettivo

/*
   DEFINIZIONE ANGOLI DI LANCIO E DISTANZE
*/

#define MINGIT          30
#define MAXGIT          180
#define CAMBIOGIT       85
#define TRENTA          150
#define QUARANTACINQUE  135

/*
   VARIABILI DI STATO BOTTONE
*/

int initialSet = 1;//Stato iniziale bottone
int stat = 0;//Stato del bottone
int val = 0;//Lettura stato del bottone

/*
   DEFINIZIONE DEI SERVO
*/

Servo gancio;//dichiarazione servo per gancio/sgancio
Servo inclinazione;//dichiarazione servo inclinazione asta
Servo molla;//dichiarazione servo che tira la molla

/*
   SETUP DEL SISTEMA
*/

void setup() {
  gancio.attach(gancioPin);  // servo
  delay(500);
  inclinazione.attach(incliPin);
  delay(500);
  molla.attach(mollaPin);
  pinMode(butPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  delay(500);
}

/*
   DEFINIZIONE DEL LOOP DI ARDUINO
*/

void loop() {
  long dist, all;
  //Stato iniziale del sistema
  if (initialSet == 1) {
    relax();
    initialSet = 0;
  }

  //leggo valore bottone
  val = digitalRead(butPin);

  //controllo bottone
  if (val == LOW) {         // check if the input is HIGH (button released)

    if (stat == 1) {
      relax();
      stat = 0;
      delay(1000);
    }
    else {
      //Serial.println("AGGANCIA");
      gancio.write(AGGANCIA);
      dist = distanza();
      Serial.print("Media Dist: ");
      Serial.println(dist);
      if (dist >= MINGIT && dist <= MAXGIT) {
        if (dist <= CAMBIOGIT) {
          inclinazione.write(TRENTA); //TODO inclinazione 30 gradi
          OFFSETDIST = offset(dist, TRENTA);
          all = allungamento(dist + OFFSETDIST, TRENTA);
          Serial.println("LANCIO A 30 GRADI ");
        } else {
          inclinazione.write(QUARANTACINQUE); //TODO inclinazione 45 gradi
          OFFSETDIST = offset(dist, QUARANTACINQUE);
          all = allungamento(dist + OFFSET, QUARANTACINQUE);
          Serial.print("OFFSET: ");
          Serial.println(OFFSETDIST);
          Serial.println("LANCIO A 45 GRADI");
        };

        Serial.print("CARICA MOLLA A ");
        Serial.println(all);
        molla.write(OFFSETMOLLA + all);
        stat = 1;
      }
      else Serial.println("ERRORE: rilevazione errori fuori dal range");
      delay(1000);
    }
  }
}

/*
   DEFINIZIONE FUNZIONE PER RILASSARE IL SISTEMA
*/

void relax() {
  //inclinazione.write(135);//inclinazione a 45 gradi

  Serial.println("SGANCIA");
  gancio.write(SGANCIA);
  delay(1500);

  Serial.println("RIALASSA MOLLA");
  molla.write(OFFSETMOLLA);
  delay(1000);
}

/*
   FUNZIONE PER CALCOLARE OFFSET DI LANCIO
*/

int offset(long d, int ang){
  int os; //offset
  double a, b, c, err;

  switch(ang){
    case QUARANTACINQUE:
      /*calcolo a 45 gradi*/
      a = 0.0008175;
      b = -0,121;
      c = 21,752;
      err = a * d * d + b * d + c;
      os = round(err);
      break;
    case TRENTA:
      /*calcolo a 30 gradi*/
      if(d>75) os = 5;
      else if(d<=75 && d>=40) os = 3;
      else os = 1;
    break;
  }

  return os;
}

/*
   CALCOLO ALLUNGAMENTO DELLA MOLLA
*/

long allungamento(long x, int ang) {
  long mot;
  double a, b, c, g;

  Serial.print("Fin Dist: ");
  Serial.println(x);

  switch(ang){
    case QUARANTACINQUE:
      /*calcolo a 45 gradi*/
      a = -0.002822;
      b = 1.456;
      c = -89.314;
      break;
    case TRENTA:
      /*calcolo a 30 gradi*/
      a = -0.0007001;
      b = 1.237;
      c = -24.02;
    break;
  }

  g = a * x * x + b * x + c;

  mot = round(g);

  return mot;
}

/*
   CALCOLO VALORI SENSORE DISTANZA
*/

long distanza() {

  long duration, distance;
  long somma, media;
  long nt = 5;
  long nd = 0;
  somma = 0;

  //rilvemento con sensore e media
  for (int i = 0; i < nt; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration / 58.2;

    somma += distance;
    Serial.print("Dist  ");
    Serial.print(": ");
    Serial.println(distance);
  }

  media = somma / nt;
  return media;
}
