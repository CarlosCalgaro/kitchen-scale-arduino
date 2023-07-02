#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define endereco 0x27
#define colunas 16
#define linhas 2
#define SENSOR_PORT 3
#define FREQ_AMOSTRAGEM 1000

#define BUFFER_TENSAO_TAMANHO 10

LiquidCrystal_I2C lcd(endereco, colunas, linhas);
int estado_atual = 0;
double offset = 0;
double buffer_tensao_media[BUFFER_TENSAO_TAMANHO];

double tensaoBalanca(){
  double sensorValue = analogRead(A3);
  double voltage = sensorValue * (5.0 / 1024.0);
  return voltage;
}

double equacaoPrimeiraOrdem(double x){
  
  return 2384.0269277845796*(x) + (-2395.4643002855996); // Auferido com água e uma balança de cozinha, começa em 1.00v
  return 1428.5714285714273*(x) + (-2414.285714285712); // Pesos ucs, comeca em 1.69v
}

double lerPeso(){
  double peso_offset = equacaoPrimeiraOrdem(offset);
  double valor = equacaoPrimeiraOrdem(tensaoBalanca()) - peso_offset;
  return valor;
}

void leituraPesoMedia(){
  double soma_parcial = 0;
  for(int i = 0;  i < FREQ_AMOSTRAGEM; i++){
    soma_parcial += tensaoBalanca();
  }
  double tensao_media = soma_parcial/FREQ_AMOSTRAGEM;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tensao_media);
  lcd.setCursor(4, 0);
  lcd.print(" volts");
  lcd.setCursor(0, 1);
  lcd.print((int)floor(equacaoPrimeiraOrdem(tensao_media)));
  lcd.setCursor(4, 1);
  lcd.print(" gramas");
}

void leituraPesoMediaMovel(){
  double soma_parcial = 0;
  for(int i = 0;  i < FREQ_AMOSTRAGEM; i++){
    soma_parcial += tensaoBalanca();
  }
  double tensao_media = soma_parcial/FREQ_AMOSTRAGEM;
  for(int j = BUFFER_TENSAO_TAMANHO-1; j > 0; j--){
    buffer_tensao_media[j] = buffer_tensao_media[j-1];
  }
  buffer_tensao_media[0] = tensao_media;
  lcd.clear();
  // lcd.setCursor(5, 4);
  // lcd.print((int)(floor(equacaoPrimeiraOrdem(tensao_media))));

  soma_parcial = 0;
  for(int i = 0; i < BUFFER_TENSAO_TAMANHO; i++){
    soma_parcial += buffer_tensao_media[i];
  }
  tensao_media = soma_parcial/BUFFER_TENSAO_TAMANHO;
  int peso = (int)(floor(equacaoPrimeiraOrdem(tensao_media)));

  lcd.setCursor(0, 0);
  lcd.print(tensao_media);
  lcd.setCursor(8, 0);
  lcd.print(" volts");
  lcd.setCursor(0, 1);
  lcd.print(peso);
  lcd.setCursor(8, 4);
  lcd.print(" gramas");
}

double tensao_media_movel =0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  lcd.print("Calibrando...");

  double soma_parcial = 0;
  for(int j = 0 ; j < BUFFER_TENSAO_TAMANHO; j++){
    soma_parcial = 0;
    for(int i = 0; i < FREQ_AMOSTRAGEM; i++){
      soma_parcial += tensaoBalanca();
    }
    offset = soma_parcial/FREQ_AMOSTRAGEM;
    buffer_tensao_media[j]=offset;
    tensao_media_movel += offset;
  Serial.println(offset);
  }
  offset = tensao_media_movel / BUFFER_TENSAO_TAMANHO;
  // attachInterrupt(digitalPinToInterrupt(BOTAO), cliqueTara, FALLING);
}

void loop() {
  switch (estado_atual) {
    case 0:
      leituraPesoMediaMovel();
      delay(400);

  }
  // put your main code here, to run repeatedly:

}
