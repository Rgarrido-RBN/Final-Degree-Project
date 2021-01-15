//Añadimos sibrerias necesarias para DHT22
#include <DHT.h>
#define DHTTYPE DHT22
const int DHTPin = 5;
//Decalaracion de variables

char data_labview; //Datos recibidos de Labview
int func_manual; //Cuando la variable está a 1 los accionadores solo funcionan de forma manual
String datoslabview; //Data_labview agrupada en variable tipo string
String temperatura; //Para poder leer los datos del string y pasarlos a 
String humedad;     //las variables int, pero para ello tengo que utilizar toInt()
String PH;


int temp_INT;//Utilizare estas variables de tipo entero para almacenar los valores "meta"
int hum_INT;
int PH_INT;


int b_aspersor=2; //Aspersor
int b_ventanas=3;   //Abrir ventanas
int b_calefactor=4; //Enchufar calefactor

DHT dht(DHTPin, DHTTYPE);//inicializamos el sensor DHT22

//Salidas digitales
int s_ventanas=6; //Salida para las ventanas
int s_calefactor=7; //Salida para el calefactor
int s_phpos=8; //Salida para electrovalvula de PH+
int s_phneg=9; //Salida para electrovalvula de PH-
int s_aspersor=10; //Salida para el aspersor
int s_riego=11; //Salida para el riego





int PH_medido;
int PH_convertido;

int boton_aspersor=0;
int boton_ventana=0;
int boton_calefactor=0;

//Estado Salidas
bool ventana=0;
bool calefactor=0;
bool phpos=0;
bool phneg=0;
bool aspersor=0;
bool riego=0;

void setup() 
{
  Serial.begin(9600);
  dht.begin();//empezamos a leer por el sensor
  pinMode(A0,INPUT);
  pinMode(b_aspersor,INPUT);
  pinMode(b_ventanas,INPUT);
  pinMode(b_calefactor,INPUT);
  pinMode(s_ventanas,OUTPUT);
  pinMode(s_calefactor,OUTPUT);
  pinMode(s_phpos,OUTPUT);
  pinMode(s_phneg,OUTPUT);
  pinMode(s_aspersor,OUTPUT);
}

void loop() 
{
 if (Serial.available()) 
 {
   data_labview=Serial.read(); //Los datos que envia Labview por puerto serie se leen
    datoslabview += data_labview; // Transformamos la "palabra" que envia LAbView en un string
      if (datoslabview[6]=='g'){func_manual=1;}   //Leemos de LabView si estamos funcionando de forma manual
      
      PH_medido=analogRead(A0); //Leemos el PH de 0 a 1023
      //PH_convertido= 
 

      float hum = dht.readHumidity();// Leemos la humedad relativa
      float temp = dht.readTemperature();// Leemos la temperatura en grados centígrados (por defecto)
      int hume= 100*hum;
      int tempe= 100*temp;
    if(data_labview=='\n')
    {
      if (datoslabview[0]=='a') {digitalWrite(s_ventanas,HIGH);ventana=1;}//Opertura manueal de ventanas desde labview
      else if ((hum > hum_INT+ 10) && (func_manual=0)) {digitalWrite(s_ventanas,HIGH);ventana=1;} //Si la humedad es alta se abren las ventanas para ventilar
      else if (digitalRead(b_ventanas)==HIGH) {digitalWrite(s_ventanas,HIGH);ventana=1;}//Si presionamos el boton fisico de abrir ventana
      else {digitalWrite(s_ventanas,LOW);ventana=0;}

      if (datoslabview[1]=='b') {digitalWrite(s_calefactor,HIGH);calefactor=1;}//Enciendo el calefactor de forma manual
      else if ((temp < temp_INT-3) && (func_manual=0)) {digitalWrite(s_calefactor,HIGH);calefactor=1;}//El calefactor se enciende si la temperatura baja mas de 3 grados de la deseada
      else if (digitalRead(b_calefactor)==HIGH) {digitalWrite(s_calefactor,HIGH);calefactor=1;}
      else {digitalWrite(s_calefactor,LOW);calefactor=0;}
      
      if (datoslabview[2]=='c'){digitalWrite(s_phpos,HIGH);phpos=1;}//Opertura de valvula para subir PH manual
      else if ((PH_convertido > PH_INT) && (func_manual=0)){digitalWrite(s_phpos,HIGH);phpos=1;}//La valvula de nivelado se abre para nivelar el PH
      else {digitalWrite(s_phpos,LOW);phpos=0;}

      if (datoslabview[3]=='d'){digitalWrite(s_phneg,HIGH);phneg=1;}//Opertura de valvula para bajar PH manual
      else if ((PH_convertido > PH_INT) && (func_manual=0)){digitalWrite(s_phneg,HIGH);phneg=1;}//La valvula de nivelado se abre para nivelar el PH
      else {digitalWrite(s_phneg,LOW);phneg=0;}

      if (datoslabview[4]=='e'){digitalWrite(s_aspersor,HIGH);aspersor=1;}//Salida aspersor manual
      else if ((hum < hum_INT - 10) && (func_manual=0)){digitalWrite(s_aspersor,HIGH);aspersor=1;}//Si la humedad es muy baja un aspersor se activa
      else if ( digitalRead(b_aspersor)==HIGH ){digitalWrite(s_aspersor,HIGH);aspersor=1;}
      else {digitalWrite(s_aspersor,LOW);aspersor=0;}

  
      for (int i=8;i<12;i++)//Aqui cargo los datos que he pasado del PS del LabView al arduino y los cargo en las variables correspondientes
      {temperatura += datoslabview[i];//siempre va a ocupar 3 bits
       humedad += datoslabview[i+3];//siempre va a ocupar 3 bits
       PH += datoslabview[i+6];}//siempre va a ocupar 3 bits
       temp_INT=temperatura.toInt();//Aqui lo que hago es pasar el string a un entero
       hum_INT=humedad.toInt();
       PH_INT=PH.toInt();

      temperatura="";//Reseteamos las string para que a la siguiente ejecucion del programa este vacia
      humedad="";
      PH="";
      
      datoslabview="";//Se vacia el buffer
      //data_labview="";
      
      Serial.print(PH_LV);//Valor PH
      Serial.print('\t'); 
      Serial.print(hume);//Valor humedad
      Serial.print('\t');
      Serial.print(tempe);//Valor temperatura
      Serial.print('\t');
      Serial.print(ventana);//Estado Ventanas
      Serial.print('\t');
      Serial.print(calefactor);//Estado Calefactor
      Serial.print('\t');
      Serial.print(phpos);// Estado Valvula PH+
      Serial.print('\t');
      Serial.print(phneg);// Estado Valvula PH-
      Serial.print('\t');
      Serial.println(aspersor);// Estado aspersor
      delay(50);
   } 
 }
 else{
  PH_medido=analogRead(A0); //Leemos el PH de 0 a 1023
      float hum = dht.readHumidity();
      float temp = dht.readTemperature();
      if (digitalRead(b_ventanas)==HIGH) {digitalWrite(s_ventanas,HIGH);ventana=1;}
  }
}
