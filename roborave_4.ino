#include <DCMotor.h>
//#include <NewPing.h>
#include <Servo.h>

DCMotor motor_izq(4, 17, 16);
DCMotor motor_der(5, 30, 41);

//NewPing ultra(4, 3, 200);
Servo puerta;

//estados
#define motores(a,b) motor_izq.setSpeed(a);motor_der.setSpeed(b);

#define seguidor 1
#define caja 2

#define izq 0
#define der 1

//velociades

#define vel_adelante 30
#define giro_menor -55
#define giro_mayor 50

//pines

int PIN_S_DER = 11;
int PIN_S_IZQ = 13;
#define PIN_DER_EXT 9
#define PIN_IZQ_EXT 8
#define PIN_PULL_DER 40
#define PIN_PULL_IZQ 27

//servo

#define p_cerrada 50
#define p_abierta 140
#define PIN_SERVO 2

//variables de calibracion

int IZQ_N = 415;
int DER_N = 525;
#define IZQ_E_N 490
#define DER_E_N 300

int estado = seguidor;

//prioridad
boolean prioridad_izq = 0;
unsigned long resta;

//sensores

int s_izq;
int s_der;
int s_del;
int pull_izq;
int pull_der;
int s_izq_e;
int s_der_e;

int a = 0;
////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(25,OUTPUT);
  digitalWrite(25,1);
  pinMode(PIN_PULL_DER, INPUT);
  pinMode(PIN_PULL_IZQ, INPUT);
  resta = millis();
  puerta.attach(2);
  puerta.write(p_cerrada);
  delay(500);
  puerta.detach();
}

void loop()
{
  //Agregar switch. 
  //Modo 0: tiempo corto de descarga. 
  //Modo 1: Tiempo largo de descarga.

  if (estado == seguidor)
  {
    s_izq = analogRead(PIN_S_IZQ);
    s_der = analogRead(PIN_S_DER);
    s_izq_e = analogRead(PIN_IZQ_EXT);
    s_der_e = analogRead(PIN_DER_EXT);

    digitalWrite(22, prioridad_izq);
    digitalWrite(23, !prioridad_izq);

    pull_izq = digitalRead(PIN_PULL_IZQ);
    pull_der = digitalRead(PIN_PULL_DER);

    //Si han pasado 3 segundos desde que encontró la última intersección (O inicio de pasada)
    //Si ve intersección cambia prioridad y guarda el tiempo
    if (s_izq_e < IZQ_E_N || s_der_e < DER_E_N)
    {
      interseccion();
    }
    if (s_izq < IZQ_N && prioridad_izq)
    {
      motores(giro_menor-10, giro_mayor);
    }
    else if (s_der < DER_N)
    {
      motores(giro_mayor+10, giro_menor);
    }
    else if(s_izq < IZQ_N)
    {
      motores(giro_menor-10, giro_mayor);
    }
    else
    {
      motores(vel_adelante+10, vel_adelante);
    }

    if (!pull_der || !pull_izq)
    {
      estado = caja;
      motores(0,0);
    }
  }
  else if (estado == caja)
  {
    while(a < 4)
    {
      if(a == 0)
      {
        delay(1000);
        a++;
      }
      else if(a == 1)
      {
        puerta.attach(2);
        puerta.write(p_abierta);
        delay(3000);
        a++;
      }
      else if(a == 2)
      {
        puerta.write(p_cerrada);
        delay(500);
        puerta.detach();
        a++;
      }
      else if(a == 3)
      {
        motores(-40,-40);
        delay(500);
        motores(50,-43);
        delay(1000);
        s_izq = analogRead(PIN_S_IZQ);
        while(s_izq > IZQ_N)
        {
          s_izq = analogRead(PIN_S_IZQ);
        }
        motores(0,0);
        a++;
      }
    }
    resta = millis();
    estado = seguidor;

    //Volver marcha atrás (Luego de las próximas líneas debería ser simplemente avanzar con el seguidor)

    //Verificar el correcto funcionamiento!!!                            ***********************************************
    //motor_der = DCMotor(9, 28, 29, false);
    //motor_izq = DCMotor(8, 14, 15, false);

    //PIN_S_IZQ = ;
    //PIN_S_DER = ;
    //IZQ_N =
    //DER_N =
  }
}

void interseccion()
{
  if(prioridad_izq)
  {
    s_izq_e = analogRead(PIN_IZQ_EXT);
    while(s_izq_e < IZQ_E_N+2)
    {
      motores(35,35);
      s_izq_e = analogRead(PIN_IZQ_EXT);
    }
    s_izq = analogRead(PIN_S_IZQ);
    while(s_izq < 480)
    {
      s_izq = analogRead(PIN_S_IZQ);
    }
    while(s_izq > 460)
    {
      motores(-47,70);
      s_izq = analogRead(PIN_S_IZQ);
    }
    while(s_izq < 480)
    {
      s_izq = analogRead(PIN_S_IZQ);
    }
    s_der = analogRead(PIN_S_DER);
    while(s_der > 590)
    {
      s_der = analogRead(PIN_S_DER);
    }
  }
  else
  {
    s_der_e = analogRead(PIN_DER_EXT);
    while(s_der_e < DER_E_N+2)
    {
      motores(35,35);
      s_der_e = analogRead(PIN_DER_EXT);
    }
    s_der = analogRead(PIN_S_DER);
    while(s_der < 530)
    {
      s_der = analogRead(PIN_S_DER);
    }
    while(s_der > 500)
    {
      motores(70,-40);
      s_der = analogRead(PIN_S_DER);
    }
    while(s_der < 520)
    {
      s_der = analogRead(PIN_S_DER);
    }
    s_izq = analogRead(PIN_S_IZQ);
    while(s_izq > 420)
    {
      s_izq = analogRead(PIN_S_IZQ);
    }
  }
  prioridad_izq = !prioridad_izq;
  motores(0,0);
}









