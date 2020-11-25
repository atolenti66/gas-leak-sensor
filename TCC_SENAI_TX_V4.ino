

/* 
  =======================================
  Modulo de Projeto - TCCtx - v4
  
  Autor: Alberto Tolentino
  Criado em: 12/10/2020
  Modificado em: 23/10/2020
  
  Demonstra como utilizar trasmissor RF
  
  Criado para demonstrar os elementos
  chave do TCC do SENAI Eletroeletrônica
    
  Grupo 5: Alberto, Deisnard, Lindomar e Thomas
  =======================================    
*/


/* V4 Utiliza VirtualWire para comunicação RF
 * V3 Utiliza RCSwitch para comunicação RF
 */
/*

PINAGEM ARDUINO

10 - TX

*/

#include <VirtualWire.h>

const int led_pin = 13;
const int transmit_pin = 10;

void setup() {

  Serial.begin(9600);
  pinMode (led_pin,OUTPUT);

  
// Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);   // Bits per sec
  
}

void loop() {
  digitalWrite (led_pin,HIGH);  
  delay(1000);
  send("Fumaça Detectada");
  Serial.println("Enviando Codigo: Fumaça Detectada");
  digitalWrite (led_pin,LOW);
  delay(1000);
}

void send (char *message){
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx(); // Wait until the whole message is gone
}
