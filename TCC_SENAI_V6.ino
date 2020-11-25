/* 
  =======================================
  Modulo de Projeto - TCC - v6
  
  Autor: Alberto Tolentino
  Criado em: 12/10/2020
  Modificado em: 24/10/2020
  
  Demonstra como utilizar o OLED Display
  sensor de gás, acionamento de Relé
  e Receptor RF
  
  Criado para demonstrar os elementos
  chave do TCC do SENAI Eletroeletrônica
    
  Grupo 5: Alberto, Deisnard, Lindomar e Thomas
  =======================================    
*/

/* V6 - Reorganizando as bibliotecas e utilizando Display 16x2 ao invés de OLED
 * V5 - Utilizando VirtualWire para comunicação RF
 * V4 - Utilizando ECSwitch para comunicação RF
 */

/*

PINAGEM ARDUINO

A2 - Data Sensor Gas
A4 - SDA OLED Display
A5 - SCL OLED Display
08 - Relay
10 - RX
05 - Reset Button (pullup)

*/

// Bibliotecas a serem carregadas
#include <VirtualWire.h>
#include <string.h>
#include <LiquidCrystal_I2C.h>

//Declarações dos Sensores e Variáveis

const int led_pin = 13;
const int transmit_pin = 12;
const int receive_pin = 10;
const int transmit_en_pin = 3;
const int buttonPin = 5;     // O numero da porta para o botão Reset
const int RELAY = 8;          // O número da porta do Relay
const int MQ2pin = A2;        // O número da porta para o sensor de Gás

byte message[VW_MAX_MESSAGE_LEN];         // um buffer para amrazenar a mensagem recebida
byte messageLength = VW_MAX_MESSAGE_LEN;  // o tamanho da mensagem

float sensorValue;            // Variável que recebe os dados do sensor de Gás
int GasThreshold = 470;       // Variável que contém o limite que determina o evento GAS PRESENTE

String SmokePresent = "Fumaça Detectada";       // Variável que contém o valor a ser recebido pelo transmissor e representa o evento FUMAÇA PRESENTE

int ResetButtonState = 1;     // Variável que recebe o estado do Botão Reset. 0 = Botão pressionado
int OperationState = 0;       // Variável que armazena o estado de operação. 0 = Normal e 1 = Incidente (fogo, fumaça ou gás)

String TXMsg = "";            // Variável que recebe o valor transmitido.

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

void setup()
{
  delay(1000); //Aguardando os dispositivos se inicializarem

  // Inicializando o LCD
  lcd.init(); 
  lcd.backlight();
  lcd.clear();

  Serial.begin(9600);  // Debugging only
  Serial.println("Inicializando...");
  pinMode(buttonPin, INPUT_PULLUP);   //Declara que a porta do botão Reset é entrada mas com resistor Pull Up
  pinMode(RELAY,OUTPUT);              //Declara que a porta do Relay é saída
  Serial.println("Portas definidas...");


  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec

  vw_rx_start();       // Start the receiver PLL running

  Serial.println("Device is ready");
}

void loop(){
// Obtém o estado do Botão Reset
  ResetButtonState = digitalRead(buttonPin);

  if (ResetButtonState == LOW) {        // Se Botão Reset pressionado limpa as váriaves para estado de operação Normal
    Serial.println("*** RESET Pressionado ***");
    OperationState = 0; 
    TXMsg = "";
    sensorValue = 0;
  }

// Obtem os valores do sensor de Gás
  sensorValue = analogRead(MQ2pin); // Lê o sensor de Gas
  Serial.print("Leitura Gas: ");
  Serial.println(sensorValue);

// Le a trasmissão RF
    
if (vw_get_message(message, &messageLength)){ // Non-blocking
 Serial.print("Recebido: ");
 TXMsg = String((char *)message);
 Serial.print(TXMsg);
 Serial.println();
}else{
 Serial.println("Nada Recebido"); 
 }

//Toma decisao com base nos valores obtidos pelos sensores e estado de operação
if (OperationState == 0){
  // Começa limpando o OLED Display
    //oled.clear();
    
  if(sensorValue >= GasThreshold){          // Avalia a presença de Gás
    digitalWrite(RELAY,HIGH);               // Desliga a Solenoide
    Serial.print(sensorValue);
    Serial.println(" |GAS DETECTADO");
    lcd.clear();                            //Limpa o LCD
    lcd.setCursor(0,0);                     //Primeira linha do Display
    lcd.print("GAS PRESENTE    ");
    lcd.setCursor(0,1);                     //Segunda linha do Display
    lcd.print("SOLENOIDE OFF   ");
    OperationState = 1; 
  }
  else{
    if (TXMsg == SmokePresent) {            // Avalia a presença de Fogo e Fumaça
    digitalWrite(RELAY,HIGH);               // Desliga a Solenoide
    Serial.println("FUMAÇA/FOGO DETECTADO");
    lcd.clear();                            //Limpa o LCD
    lcd.setCursor(0,0);                     //Primeira linha do Display
    lcd.print("FUMACA PRESENTE ");
    lcd.setCursor(0,1);                     //Segunda linha do Display
    lcd.print("SOLENOIDE OFF   ");
    OperationState = 1; 
    TXMsg="";
  }else {
     digitalWrite(RELAY,LOW);               // Nada detectado...tudo normal então liga a Solenoide
    //Serial.print("Valor do Sensor: ");
    //Serial.println(sensorValue);
    Serial.println("Tudo Normal");
    lcd.clear();                            //Limpa o LCD
    lcd.setCursor(0,0);                     //Primeira linha do Display
    lcd.print("TUDO NORMAL     ");
    lcd.setCursor(0,1);                     //Segunda linha do Display
    lcd.print("SOLENOIDE ON    ");
    OperationState = 0;                     // Declara que estado de operação é Normal
  }
  
}
}
  delay(2000);
}
