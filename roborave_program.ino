#include <DCMotor.h>
//#include <NewPing.h>
#include <Servo.h>

DCMotor motor_der(8, 14, 15);
DCMotor motor_izq(9, 28, 29);

//NewPing ultra(4, 3, 200);
//Servo myservo;

//estados
#define motores(a,b) motor_izq.setSpeed(a);motor_der.setSpeed(b);

#define seguidor 1
#define caja 2

#define izq 0
#define der 1

//velociades

#define vel_adelante 25
#define giro_menor -40
#define giro_mayor 50

//pines

int PIN_S_DER = 8;
int PIN_S_IZQ = 6;
#define PIN_PULL_DER 19
#define PIN_PULL_IZQ 20

//variables de calibracion

int IZQ_N 340
int DER_N 530

int estado = seguidor;

//prioridad
bool prioridad_izq = 0;
unsigned long resta;

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
  resta = millis();

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

    digitalWrite(17, s_izq < IZQ_N);
    digitalWrite(18, s_der < DER_N);

    int pull_izq = digitalRead(PIN_PULL_IZQ);
    int pull_der = digitalRead(PIN_PULL_DER);

    //Si han pasado 3 segundos desde que encontró la última intersección (O inicio de pasada)
    //Si ve intersección cambia prioridad y guarda el tiempo
    if (abs(millis() - resta) > 3000)
    {
      if (s_izq < IZQ_N - 20 && s_der < DER_N - 20)
      {
        prioridad_izq = !prioridad_izq;
        resta = millis();
      }
    }

    
    if (s_izq < IZQ_N && prioridad_izq)
    {
      motores(giro_menor, giro_mayor);
    }
    else if (s_der < DER_N)
    {
      motores(giro_mayor, giro_menor);
    }
    else if (s_izq < IZQ_N)
    {
      motores(giro_menor, giro_mayor);
    }
    else
    {
      motores(vel_adelante, vel_adelante);
    }

    if (!pull_izq || !pull_der)
    {
      motores(0, 0);
      estado = caja;
    }
  }
  else if (estado == caja)
  {
    for (int i = 1; i <= 4; i++) {
      digitalWrite(17, i % 2);
      digitalWrite(18, i % 2);
      delay(200);
    }
    //Activar servo
    //Descargar pelotitas
    
    
    
    //Volver marcha atrás (Luego de las próximas líneas debería ser simplemente avanzar con el seguidor)

    //Verificar el correcto funcionamiento!!!                            ***********************************************
    motor_der = DCMotor(9, 28, 29, false);
    motor_izq = DCMotor(8, 14, 15, false);

    //PIN_S_IZQ = ;
    //PIN_S_DER = ;
    //IZQ_N =
    //DER_N =
  }
}
