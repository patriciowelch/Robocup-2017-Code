#include <DCMotor.h>
//#include <NewPing.h>
#include <Servo.h>

DCMotor motor_der(8, 14, 15);
DCMotor motor_izq(9, 28, 29);

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

int PIN_S_DER = 8;
int PIN_S_IZQ = 6;
#define PIN_PULL_DER 19
#define PIN_PULL_IZQ 20

//servo

#define p_cerrada 50
#define p_abierta 140
#define PIN_SERVO 2

//variables de calibracion

int IZQ_N = 300;
int DER_N = 340;

int estado = seguidor;

//prioridad
boolean prioridad_izq = 1;
unsigned long resta;

//sensores

int s_izq;
int s_der;
int s_del;

int a = 0;

////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
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

    digitalWrite(17, prioridad_izq);
    digitalWrite(18, !prioridad_izq);

    int pull_izq = digitalRead(PIN_PULL_IZQ);
    int pull_der = digitalRead(PIN_PULL_DER);

    //Si han pasado 3 segundos desde que encontró la última intersección (O inicio de pasada)
    //Si ve intersección cambia prioridad y guarda el tiempo
    if (abs(millis() - resta) > 3000)
    {
      if (s_izq < IZQ_N - 10 && s_der < DER_N - 10)
      {
        prioridad_izq = !prioridad_izq;
        resta = millis();
      }
    }

    if (s_izq < IZQ_N - 15 && s_der < DER_N - 15)
    {
      if(prioridad_izq)
      {
        while(s_der < 480)
        {
          s_der = analogRead(PIN_S_DER);
          motores(-35,55);
        }
        while(s_der > 450)
        {
          s_der = analogRead(PIN_S_DER);
        }
      }
      else
      {
        while(s_izq < 340)
        {
          s_izq = analogRead(PIN_S_IZQ);
          motores(55,-35);
        }
        while(s_izq > 300)
        {
          s_izq = analogRead(PIN_S_IZQ);
        }
      }
    }
    if (s_izq < IZQ_N && prioridad_izq)
    {
      motores(giro_menor, giro_mayor);
    }
    else if (s_der < DER_N-prioridad_izq*20)
    {
      motores(giro_mayor, giro_menor);
    }
    else if (s_izq < IZQ_N-(1-prioridad_izq)*20)
    {
      motores(giro_menor, giro_mayor);
    }
    else
    {
      motores(vel_adelante, vel_adelante);
    }

    /*if (!pull_izq || !pull_der)
    {
      estado = caja;
      motores(0,0);
    }*/
  }
  else if (estado == caja)
  {
    puerta.attach(2);
    motores(0,0);
    while(a < 2)
    {
      if(a == 0)
      {
        for(int pos = p_cerrada; pos < p_abierta; pos += 1)  // goes from 0 degrees to 180 degrees 
        {                                  // in steps of 1 degree 
          puerta.write(pos);              // tell servo to go to position in variable 'pos' 
          delay(15);                       // waits 15ms for the servo to reach the position 
        }
      }
      else if(a == 1)
      {
        for(int pos = p_abierta; pos>=p_cerrada; pos-=1)     // goes from 180 degrees to 0 degrees 
        {                                
          puerta.write(pos);              // tell servo to go to position in variable 'pos' 
          delay(15);                       // waits 15ms for the servo to reach the position 
        }
      }
      delay(5000);
      a++;
    }


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





