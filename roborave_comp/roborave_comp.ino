#include <DCMotor.h>
#include <Servo.h>

#define motores(a,b) motor_izq.setSpeed(a);motor_der.setSpeed(b);

#define seguidor 1
#define caja 2

#define izq 0
#define der 1

//pines
#define PIN_S_DER 11
#define PIN_S_IZQ 13
#define PIN_DER_EXT 9
#define PIN_IZQ_EXT 8
#define PIN_PULL_DER 40
#define PIN_PULL_IZQ 27
#define PIN_SWITCH 53
#define PIN_SERVO 2

//servo
#define p_cerrada 60
#define p_abierta 140

//valores sensores seguimiento linea
////////////////////////////////////////////////////////////
//202	||	171	||	218	||	125
#define IZQ_N 440
#define DER_N 450
#define MIN_IZQ 290
#define MIN_DER 290

#define IZQ_E_N 253
#define DER_E_N 140

//velociades seguidor
#define vel_adelante 40
#define giro_menor -55
#define giro_mayor 80

//valore sensores para interseccion
#define DER_VAL_1 440
#define DER_VAL_2 465
#define DER_VAL_3 435

#define IZQ_VAL_1 375
#define IZQ_VAL_2 400
#define IZQ_VAL_3 350

//velociades interseccion
#define VEL_INTERSEC_AD 35
#define VEL_GIRO_INTERSEC_NEG -55
#define VEL_GIRO_INTERSEC_POS 90

#define DELTA_MOTOR_IZQ 5 //diferencia de velocidad para el motor izq

#define ON_OFF_LEDS LOW

//Zona de variables
DCMotor motor_izq(4, 17, 16);
DCMotor motor_der(5, 30, 41);
Servo puerta;

int estado = seguidor;

boolean prioridad_izq = 0;
unsigned long t_gracia_interseccion = millis();

int s_izq;
int s_der;

void setup()
{
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_PULL_DER, INPUT);
  pinMode(PIN_PULL_IZQ, INPUT);

  digitalWrite(22,prioridad_izq);
  digitalWrite(23,!prioridad_izq);
  digitalWrite(25, ON_OFF_LEDS);
}

void loop()
{
  if (estado == seguidor)
  {
    if (!digitalRead(PIN_PULL_IZQ) || !digitalRead(PIN_PULL_DER))
    {
      //choque detectado
      estado = caja;
      motores(0, 0);
    }
    else {
      //siguiendo linea...
      //Si ve intersección cambia prioridad y guarda el tiempo
      if (((analogRead(PIN_IZQ_EXT) < IZQ_E_N && prioridad_izq) || (analogRead(PIN_DER_EXT) < DER_E_N && !prioridad_izq)) && (millis() - t_gracia_interseccion) > 1000)
      {
        interseccion();
        t_gracia_interseccion = millis();
      }
      else
      {
        s_izq = analogRead(PIN_S_IZQ);
        s_der = analogRead(PIN_S_DER);
        if (s_der < DER_N)
        {
          //motores(giro_mayor + 40, -20 - 25 * (s_der < MIN_DER + 45));
          motores(giro_mayor+DELTA_MOTOR_IZQ, -20 - 25 * (s_der < MIN_DER));
          //motores(giro_mayor+DELTA_MOTOR_IZQ,-40);
        }
        else if (s_izq < IZQ_N)
        {
          //motores(-20 - 25 * (s_izq < MIN_IZQ + 50), giro_mayor + 45);
          motores(-20 - 25 * (s_izq < MIN_IZQ) - DELTA_MOTOR_IZQ, giro_mayor);
          //motores(-40-DELTA_MOTOR_IZQ,giro_mayor);
        }
        else
        {
          motores(vel_adelante + DELTA_MOTOR_IZQ, vel_adelante);
        }
      }
    }
  }
  else if (estado == caja)
  {
    delay(1000);

    puerta.attach(2);
    puerta.write(p_abierta);
    
    //y comienza el puerteo!!!
    int tiempo = millis();
    int i=0;
    int inv = 1;
    while((millis()-tiempo) < (2000 * ((digitalRead(PIN_SWITCH) * 1000) + 1)))
    {
      motores(vel_adelante*inv,vel_adelante*inv);
      if((i%1000)==0){
        inv*=-1;
      }
      i++;
    }

    motores(0, 0);
    puerta.write(p_cerrada);
    delay(500);
    puerta.detach();

    motores(-40, -40);
    delay(500);
    motores(50, -43);
    delay(1000);
    s_izq = analogRead(PIN_S_IZQ);
    while (s_izq > IZQ_N)
    {
      s_izq = analogRead(PIN_S_IZQ);
    }
    motores(0, 0);

    estado = seguidor;
  }
}

void interseccion()
{
  if (prioridad_izq)
  {
    s_izq = analogRead(PIN_S_IZQ);

    motores(VEL_INTERSEC_AD+DELTA_MOTOR_IZQ, VEL_INTERSEC_AD);
    while (s_izq < IZQ_N)
      s_izq = analogRead(PIN_S_IZQ);

    s_der = analogRead(PIN_S_DER);
    motores(VEL_GIRO_INTERSEC_NEG-DELTA_MOTOR_IZQ, VEL_GIRO_INTERSEC_POS);
    while (s_der > DER_VAL_1)
      s_der = analogRead(PIN_S_DER);
    while (s_der < DER_VAL_2)
      s_der = analogRead(PIN_S_DER);
    while (s_der > DER_VAL_3)
      s_der = analogRead(PIN_S_DER);
  }
  else
  {
    s_der = analogRead(PIN_S_DER);

    motores(VEL_INTERSEC_AD+DELTA_MOTOR_IZQ, VEL_INTERSEC_AD);
    while (s_der < DER_N)
      s_der = analogRead(PIN_S_DER);

    s_izq = analogRead(PIN_S_IZQ);
    motores(VEL_GIRO_INTERSEC_POS+DELTA_MOTOR_IZQ, VEL_GIRO_INTERSEC_NEG);
    while (s_izq > IZQ_VAL_1)
      s_izq = analogRead(PIN_S_IZQ);
    while (s_izq < IZQ_VAL_2)
      s_izq = analogRead(PIN_S_IZQ);
    while (s_izq > IZQ_VAL_3)
      s_izq = analogRead(PIN_S_IZQ);

  }

  prioridad_izq = !prioridad_izq;
  digitalWrite(22,prioridad_izq);
  digitalWrite(23,!prioridad_izq);

  motores(0, 0);
}
