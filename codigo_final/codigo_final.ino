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
#define PIN_RUN 47
#define PIN_SWITCH 53
#define PIN_SERVO 2
#define PIN_MOVELA 6

//servo
#define p_cerrada 85
#define p_abierta 170

//valores sensores seguimiento linea
////////////////////////////////////////////////////////////
//172	||	160	||	186	||	111

#define IZQ_N 390//400
#define DER_N 410//410
#define MIN_IZQ 185//290
#define MIN_DER 215//290

#define IZQ_E_N 253
#define DER_E_N 140

#define GIRO_TORRE_N 320

//valore sensores para interseccion
#define DER_VAL_1 500//440
#define DER_VAL_2 525//465
#define DER_VAL_3 450//435

#define IZQ_VAL_1 405//375   //mientras blanco
#define IZQ_VAL_2 425//400   //mientras negro
#define IZQ_VAL_3 375//350   //mientras blanco

//velociades seguidor
#define vel_adelante 50//40
#define giro_menor -55
#define giro_mayor 80

//velociades interseccion
#define VEL_INTERSEC_AD 35
#define VEL_GIRO_INTERSEC_NEG -55
#define VEL_GIRO_INTERSEC_POS 90

#define DELTA_MOTOR_IZQ 10 //diferencia de velocidad para el motor izq

#define ON_OFF_LEDS HIGH

//Zona de variables
DCMotor motor_izq(4, 17, 16);
DCMotor motor_der(5, 30, 41);
Servo puerta;
Servo mueve_bolitas;

int estado = seguidor;

boolean prioridad_izq = 1;
unsigned long t_gracia_interseccion = millis();

int s_izq;
int s_der;

void setup()
{
  mueve_bolitas.attach(PIN_MOVELA);
  mueve_bolitas.write(90);
  delay(400);
    mueve_bolitas.detach();
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(PIN_RUN,INPUT_PULLUP);
  pinMode(PIN_SWITCH,INPUT);
  pinMode(PIN_PULL_DER, INPUT);
  pinMode(PIN_PULL_IZQ, INPUT);

  while(digitalRead(PIN_RUN))
  {

  }
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
      if (((analogRead(PIN_IZQ_EXT) < IZQ_E_N) || (analogRead(PIN_DER_EXT) < DER_E_N)) && (millis() - t_gracia_interseccion) > 1000)
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
          motores(giro_mayor+DELTA_MOTOR_IZQ, -20 - 25 * (s_der < MIN_DER));
        }
        else if (s_izq < IZQ_N)
        {
          motores(-20 - 25 * (s_izq < MIN_IZQ) - DELTA_MOTOR_IZQ, giro_mayor);
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
    delay(1000);
    //dale a las pelotas movimiento macarena!!!
    int tiempo = millis();
    
    mueve_bolitas.attach(PIN_MOVELA);
    while((millis()-tiempo) < (1500 * ((digitalRead(PIN_SWITCH) * 1000) + 1)))
    {
      if(digitalRead(PIN_SWITCH))
      {
        mueve_bolitas.write(170);
        delay(400);
        mueve_bolitas.write(10);
        delay(400);
      }
    }

    motores(0, 0);
    puerta.write(p_cerrada);
    mueve_bolitas.write(90);
    delay(500);
    mueve_bolitas.detach();
    puerta.detach();

    motores(-40, -40);
    delay(400);
    motores(50+DELTA_MOTOR_IZQ, -55);
    delay(900);
    s_izq = analogRead(PIN_S_IZQ);
    while (s_izq > GIRO_TORRE_N)
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
    s_der = analogRead(PIN_S_DER);
    motores(VEL_GIRO_INTERSEC_NEG-DELTA_MOTOR_IZQ, VEL_GIRO_INTERSEC_POS-(digitalRead(PIN_SWITCH)*10));
    while (s_der > DER_VAL_1-((!digitalRead(PIN_SWITCH))*35))
      s_der = analogRead(PIN_S_DER);
    while (s_der < DER_VAL_2-((!digitalRead(PIN_SWITCH))*35))
      s_der = analogRead(PIN_S_DER);
    while (s_der > DER_VAL_3-((!digitalRead(PIN_SWITCH))*35))
      s_der = analogRead(PIN_S_DER);
  }
  else
  {
    s_izq = analogRead(PIN_S_IZQ);
    motores(VEL_GIRO_INTERSEC_POS+DELTA_MOTOR_IZQ-(digitalRead(PIN_SWITCH)*10), VEL_GIRO_INTERSEC_NEG);
    while (s_izq > IZQ_VAL_1-((!digitalRead(PIN_SWITCH))*35))
      s_izq = analogRead(PIN_S_IZQ);
    while (s_izq < IZQ_VAL_2-((!digitalRead(PIN_SWITCH))*35))
      s_izq = analogRead(PIN_S_IZQ);
    while (s_izq > IZQ_VAL_3-((!digitalRead(PIN_SWITCH))*35))
      s_izq = analogRead(PIN_S_IZQ);

  }

  prioridad_izq = !prioridad_izq;
  digitalWrite(22,prioridad_izq);
  digitalWrite(23,!prioridad_izq);

  motores(0, 0);
}
