#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define SS_PIN D8
#define RST_PIN D0
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];
String usuario;

const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";

const char* serverName = "http://mateobernasconivagas.pythonanywhere.com/agua/create/";




//primer trigger, segundo echo
int ultrasonic2[2]={D3, D2};
int motor = D1;
int buzzer = D4;
int minAltura = 4;
int maxAltura = 17;
void setup() {
  Serial.begin(9600);//iniciailzamos la comunicación+
  UltrasonicSetup(ultrasonic2);
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
  
    delay(1000);
    Serial.println("Connecting..");
  
  }
  
  Serial.println(WiFi.localIP());
  // -------------------------------------------

  SPI.begin();        //Iniciamos el Bus SPI
	mfrc522.PCD_Init(); // Iniciamos el MFRC522
	Serial.println("Control de acceso:");

}

void printHex(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], HEX);
 }
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], DEC);
 }
}

void accessoConcedido(){
  int lectura;
  while(true){
    lectura = getUltrasonicDistance(ultrasonic2);
    if(lectura <= maxAltura && lectura > minAltura){
      llenar();
      break;
    }
  }
  Serial.println("Sale loop");
}

bool llenando = false;
int status = 0;

void loop() {
  if(status == 1){
    Serial.println("Entro 1");
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    status = 0;
  }else if(status == 2){
    Serial.println("Entro 2");
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    status = 0;
  }else if(status == 3){
    Serial.println("Entro 3");
    digitalWrite(buzzer, HIGH);
    delay(150);
    digitalWrite(buzzer, LOW);
    status = 0;
  }
  
  if(llenando){
    Serial.println("Acceso concedido");
    accessoConcedido();
    Serial.println("LLenado finalizado");
    llenando = false;
    Serial.println("PARO");
    SPI.begin();        //Iniciamos el Bus SPI
	  mfrc522.PCD_Init(); // Iniciamos el MFRC522
  }

 if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
          Serial.println("Detectada");
          String test ;
  		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i]);   
                          test.concat(String(mfrc522.uid.uidByte[i]));
                  }
                  Serial.println();
                  Serial.println(test); 
                  if(usuario == test){
                    llenando = true;
                    status = 1;
                  }else {
                    status = 2;
                  }
                  if(usuario == ""){
                    usuario = test;
                    status = 3;
                  }
                  mfrc522.PICC_HaltA();
                  // Stop encryption on PCD
                  mfrc522.PCD_StopCrypto1();
                  // Terminamos la lectura de la tarjeta  actual     
            }    
	}


}

// 1l / 53.67
// 1.1179l / min
// 18.632ml / s



void UltrasonicSetup(int ultrasonic1[]){
  pinMode(ultrasonic1[0], OUTPUT); //pin como salida
  pinMode(ultrasonic1[1], INPUT);  //pin como entrada
  digitalWrite(ultrasonic1[0], LOW);//Inicializamos el pin con 0
}

int getUltrasonicDistance(int ultrasonic1[]){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  int intervalos = 4;
  int sum = 0;
  for(int i=0;i<intervalos;i++){
    digitalWrite(ultrasonic1[0], HIGH);
    delayMicroseconds(10);          //Enviamos un pulso de 10us
    digitalWrite(ultrasonic1[0], LOW);
    
    t = pulseIn(ultrasonic1[1], HIGH); //obtenemos el ancho del pulso
    d = t/59;             //escalamos el tiempo a una distancia en cm
    delay(50);
    sum = sum + d;
  }
  return sum/intervalos;
}

void send(double mili){

  double l = mili/1000;

  String litros = String(l);
  String cantidad = "cantidad=";
  String sending = cantidad + litros;

  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
  
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
      // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Token 63cc22aacd5a46737f5267e97a26b09b8bd7e312");
      // Data to send with HTTP POST
    String httpRequestData = sending;           
      // Send HTTP POST request
    int httpResponseCode = http.POST(sending);
    Serial.print(httpResponseCode);

    http.end();
  }
}



void llenar(){
  int timeInicial = millis () ;
  digitalWrite(motor, HIGH);
  int lectura = 0;
  while(true){
    lectura = getUltrasonicDistance(ultrasonic2);
    if(lectura > maxAltura || lectura < minAltura){
      break;
    }
  }
  digitalWrite(motor, LOW);
  int duration =  millis () - timeInicial;
  double mililitros = 0.018632 * duration;
  Serial.print("Se sirvieron ");
  Serial.print(mililitros);
  Serial.print(" mililitros Durante ");
  Serial.print(duration);
  Serial.println(" milisegundos");

  send(mililitros);
}



