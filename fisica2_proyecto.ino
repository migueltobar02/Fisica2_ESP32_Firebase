
#include "WiFi.h"
#include "FirebaseESP32.h"  // librerias a utilizar 
#include "time.h"
 
const char* ssid = "meto tem";      // usuario de la red movil
const char* password =  "tobar2000"; // contraseña de la red movil 

const char* ntpServer = "pool.ntp.org"; //cliente de servicio 
const long  gmtOffset_sec =  -18000;    // zona horaria 
const int   daylightOffset_sec = 0;     // horario de verano

const int sensor = 33 ;    //#pin sensor 
const int rele =  26;     //#pin rele
int sensorState = 0;
int lastSensorState=0;
int timestamp;
int day;
int month;
int year;
int hour ;
int minn;
int sec ;


#define FIREBASE_HOST "appfisica2-ce55e-default-rtdb.firebaseio.com"  //poner direccion de la database de Firebase sin https://  ni el ultimo /
#define FIREBASE_AUTH "sWmgbz8vxFzpU0Ihc9Y63UUGy6eoCxVqmVCR54Qr"// poner codigo secreto. Configuracion proyecto-cuentas servicio-secretos de la base de datos
FirebaseData firebaseData;
String node="/registro"; //nombre del nodo en Firebase
String node1="/Bombillo"; //nombre del nodo en Firebase

/* La funcion printLocalTime se encarga de obtener la hora local del servidor*/
String printLocalTime(){
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        }

    hour = timeinfo.tm_hour;
    minn = timeinfo.tm_min;
    sec = timeinfo.tm_sec;
    day = timeinfo.tm_mday;
    month = timeinfo.tm_mon + 1;
    year = timeinfo.tm_year +1900;
    String date = String(day)+"/"+ String(month)+"/"+String(year)+" "+String(hour)+":"+String(minn)+":"+String(sec);//String date = String(hour,minn,sec,day,month,year);
    return date;
 }

/* La funcion setup configura las entradas y salidas de los pines del ESP32 
y la conexion a wifi y conexion a Firebase
*/
void setup() {
  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  pinMode(sensor, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

/* La funcion loop se encarga de leer y activar los pines del ESP32 
 * y envia los datos de encendido, apagado, hora y fecha  del bombillo a Firebase
 */
void loop() {
  sensorState = digitalRead(sensor);
  FirebaseJson json;
  if ( sensorState!= lastSensorState) {
      if (sensorState == HIGH) {
        Firebase.setString(firebaseData, node1,"Encendido");
        json.add("estado", "encendido");
        digitalWrite(rele, HIGH);
      } else {
          Firebase.setString(firebaseData, node1,"Apagado");
          json.add("estado", "apagado");
          digitalWrite(rele, LOW);
      }
      json.add("fecha", printLocalTime());
      Firebase.pushJSON(firebaseData, node, json);
    }
    lastSensorState = sensorState; 
  }
