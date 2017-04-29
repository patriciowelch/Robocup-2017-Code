#include <DCMotor.h>
#include <NewPing.h>
#include <Servo.h>

DCMotor motor_der(8, 14, 15);
DCMotor motor_izq(9, 28, 29);

NewPing ultra(4, 3, 200);
//Servo myservo;

//estados

#define seguidor 1
#define caja 2

#define izq 0
#define der 1

//velociades

#define vel_adelante 25
#define giro_menor -40
#define giro_mayor 50

//pines

#define PIN_S_DEL 7
#define PIN_S_DER 8
#define PIN_S_IZQ 6
#define PIN_PULL_DER 19
#define PIN_PULL_IZQ 20

//variables de calibracion

#define IZQ_N 300
#define DER_N 490
#define DEL_N 580

int estado = seguidor;

//prioridad

int prioridad = izq;
int prioridad_prox = izq;

//sensores

int s_izq;
int s_der;
int s_del;

////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(PIN_PULL_DER, INPUT);
  pinMode(PIN_PULL_IZQ, INPUT);
}

void loop()
{
  if (estado == seguidor)
  {
    s_izq = analogRead(PIN_S_IZQ);
    s_der = analogRead(PIN_S_DER);
    s_del = analogRead(PIN_S_DEL);

    digitalWrite(17, s_izq < IZQ_N);
    digitalWrite(18, s_der < DER_N);
    unsigned long timer = millis();
    unsigned long resta;
    unsigned long timer_real = timer - resta;

    int pull_izq = digitalRead(PIN_PULL_IZQ);
    int pull_der = digitalRead(PIN_PULL_DER);

    if (!pull_izq || !pull_der)
    {
      motores(0, 0);
      estado = caja;
    }
    if (timer_real > 3000 && prioridad_prox == der)
    {
      motores(0, 0);
      prioridad = der;
    }
    if (s_izq < IZQ_N - 20 && s_der < DER_N - 20)
    {
      if (prioridad == izq)
      {
        resta = timer;
        prioridad_prox = der;
      }
    }
    if (s_izq < IZQ_N && prioridad == izq)
    {
      motores(giro_menor, giro_mayor);
    }
    else if (s_der < DER_N)
    {
      motores(giro_mayor, giro_menor);
    }
    else if (s_izq < IZQ_N && prioridad == der)
    {
      motores(giro_menor, giro_mayor);
    }
    else
    {
      motores(vel_adelante, vel_adelante);
    }
  }
  if (estado == caja)
  {
    motores(0,0);
    digitalWrite(17, 1);
    digitalWrite(18, 1);
    delay(200);
    digitalWrite(17, 0);
    digitalWrite(18, 0);
    delay(200);
  }
}

void motores(int vel1, int vel2)
{
  motor_izq.setSpeed(vel1);
  motor_der.setSpeed(vel2);
}
