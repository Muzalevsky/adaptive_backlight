#include <EEPROM.h>  
#include <FastLED.h>
#include "AB_Protocol.h"

#define LEONARDO_VERSION
#ifndef LEONARDO_VERSION
#define USUAL_ARDUINO
#endif

#define baud_rate 256000
#define LEDPIN 13 // Пин диода на плате для индикации сообщений
#define frame_timeout 4 // Время между фреймами 1 == 50us
#define RX_BUF_LEN 574  // Максимальная длинна приёмного буфера
#define MAX_LEDS 190
#define MAX_BRIGHT 255
#define LEDS_DI 3  // Пин светодиодной ленты

#define SetBit(var, bit_num) ((var) |= (1<<(bit_num)))
#define ClrBit(var, bit_num) ((var) &= (~(1<<(bit_num))))

// Функции
#ifdef USUAL_ARDUINO
static void Timer2Init(void);
#elif defined LEONARDO_VERSION
static void  Timer3Init(void);
#endif
static void LedsInit(void);
static void serial_frame_handler(void);
static void serial_framework(unsigned int input_bytes);

static void all_leds_set_color(byte color_red, byte color_green, byte color_blue);
static void all_leds_set_color_buf(uint8_t *buf);

// Внутренние переменные
static unsigned int counter;
static bool frame_timeout_flag = false;
CRGB leds[MAX_LEDS];

// Приёмный буфер
static byte rx_buf[RX_BUF_LEN];

// Переменные-параметры
static byte device_id;
static uint16_t LEDS_NUM;
static uint16_t BRIGHT;
uint8_t LED1, LED2;
uint8_t BR1, BR2;

// EEPROM ADDRS
#define ADDR_ID 0 
#define ADDR_LED1 1 
#define ADDR_LED2 2 
#define ADDR_BR1 3 
#define ADDR_BR2 4 

void setup() {
  // Настройка таймера 2
  cli(); // отключить глобальные прерывания
#ifdef USUAL_ARDUINO
  Timer2Init();
#elif defined LEONARDO_VERSION
  Timer3Init();
#endif

  sei();  // включить глобальные прерывания

  device_id = EEPROM.read(ADDR_ID);
  LED1 = EEPROM.read(ADDR_LED1);
  LED2 = EEPROM.read(ADDR_LED2);
  BR1 = EEPROM.read(ADDR_BR1);
  BR2 = EEPROM.read(ADDR_BR2);

  LEDS_NUM = LED1*10 + LED2;
  if(LEDS_NUM > MAX_LEDS)LEDS_NUM = MAX_LEDS;
  BRIGHT = BR1*10 + BR2;
  if(BRIGHT > MAX_BRIGHT)BRIGHT = MAX_BRIGHT;

  LedsInit();
  
  // Start serial
  Serial.begin(baud_rate);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  serial_frame_handler();
}

static void serial_frame_handler(void){
  static unsigned int byte_counter = 0;
  if(Serial.available()){
    frame_timeout_flag = true;
    while(frame_timeout_flag){
      if(Serial.available()){
        counter = 0;
        if(byte_counter > RX_BUF_LEN){
          byte_counter = RX_BUF_LEN - 1;
          frame_timeout_flag = 0;
        }
        rx_buf[byte_counter] = Serial.read();
        byte_counter ++;
      }
    }
    serial_framework(byte_counter);
    byte_counter = 0;
  }  
}

static void serial_framework(unsigned int input_bytes){
    uint16_t CRC16_clc, CRC16_in;
    uint8_t tx_buf[10];
    if(input_bytes < 3) return;
    CRC16_in = rx_buf[input_bytes-2] + (rx_buf[input_bytes-1] << 8);
    CRC16_clc = crc16(rx_buf, input_bytes-2);
    
    if(CRC16_in != CRC16_clc){
      memset(rx_buf, 0, sizeof(rx_buf));
      return;
    }
    uint8_t dev_id = rx_buf[0];
    uint8_t cmd = rx_buf[1];
    
    // IF CMD 0x00
    if(dev_id == 0xFF){
      if(cmd == 0x00){
        tx_buf[0] = device_id;
        tx_buf[1] = 0x00;
        CRC16_clc = crc16(tx_buf, 2);
        tx_buf[2] = CRC16_clc & 0xFF;
        tx_buf[3] = ((CRC16_clc & 0xFF00) >> 8);
        serial_tx_msg(tx_buf, 4);
        return;
      }
    }
    // Check device id
    if(dev_id != device_id) return;
    // CMD 0x05 - SET DEVICE ID
    if(cmd == 0x05){
        tx_buf[0] = rx_buf[2];
        tx_buf[1] = 0x05;
        tx_buf[2] = 0x01;
        CRC16_clc = crc16(tx_buf, 3);
        tx_buf[3] = CRC16_clc & 0xFF;
        tx_buf[4] = ((CRC16_clc & 0xFF00) >> 8);
        serial_tx_msg(tx_buf, 5);
        
        EEPROM.update(ADDR_ID, tx_buf[0]);
        device_id = tx_buf[0];
    }
    // CMD 0x01 - SET LEDS NUMBER
    else if(cmd == 0x01){
        tx_buf[0] = device_id;
        tx_buf[1] = 0x01;
        tx_buf[2] = 0x01;
        CRC16_clc = crc16(tx_buf, 3);
        tx_buf[3] = CRC16_clc & 0xFF;
        tx_buf[4] = ((CRC16_clc & 0xFF00) >> 8);
        serial_tx_msg(tx_buf, 5);
        
        EEPROM.update(ADDR_LED1, rx_buf[2]);
        EEPROM.update(ADDR_LED2, rx_buf[3]);
    }
    // CMD 0x02 - SET BRIGHT
    else if(cmd == 0x02){
        tx_buf[0] = device_id;
        tx_buf[1] = 0x02;
        tx_buf[2] = 0x01;
        CRC16_clc = crc16(tx_buf, 3);
        tx_buf[3] = CRC16_clc & 0xFF;
        tx_buf[4] = ((CRC16_clc & 0xFF00) >> 8);
        serial_tx_msg(tx_buf, 5);
        
        EEPROM.update(ADDR_BR1, rx_buf[2]);
        EEPROM.update(ADDR_BR2, rx_buf[3]);
        BRIGHT = rx_buf[2]*10 + rx_buf[3];
        if(BRIGHT > 255)BRIGHT = 255;
        FastLED.setBrightness(BRIGHT);
      
    }
    // CMD 0x03 - GET DEVICE INFO
    else if(cmd == 0x03){
        tx_buf[0] = device_id;
        tx_buf[1] = 0x03;
        tx_buf[2] = LED1;
        tx_buf[3] = LED2;
        tx_buf[4] = BR1;
        tx_buf[5] = BR2;
        CRC16_clc = crc16(tx_buf, 6);
        tx_buf[6] = CRC16_clc & 0xFF;
        tx_buf[7] = ((CRC16_clc & 0xFF00) >> 8);
        serial_tx_msg(tx_buf, 8);
    }
    else if(cmd == 0x04){
      all_leds_set_color_buf(&rx_buf[2]);
      FastLED.show();  
    }
    memset(rx_buf, 0, sizeof(rx_buf));
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}

static void serial_tx_msg(uint8_t *buf, uint16_t buf_length){
  for(int i=0; i < buf_length; i++){
    Serial.write(*(buf+i));
  }
}

static uint16_t crc16(uint8_t *buf, uint16_t buf_length){
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buf_length--) {
        i = crc_hi ^ *buf++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

// Выставить все диоды в цвет (RGB)
void all_leds_set_color(byte color_red, byte color_green, byte color_blue){
    for (int i = 0; i < LEDS_NUM; i++) {
      leds[i] = CRGB(color_red, color_green, color_blue);
    }
}
// Выставить все диоды в цвет из указанного буфера
void all_leds_set_color_buf(uint8_t *buf){
    for (int i = 0; i < LEDS_NUM; i++) {
      leds[i] = CRGB(buf[i*3], buf[3*i+1], buf[3*i+2]);
//        leds[i] = CRGB(rx_buf[2], rx_buf[3], rx_buf[4]);

    }
}

static void LedsInit(void){
  // -----Иницилизация ленты-----
  // ... добавить ленту
  FastLED.addLeds<WS2812, LEDS_DI, GRB>(leds, LEDS_NUM).setCorrection( TypicalLEDStrip );
  // Ограничения по току и напряжению
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  // ... ставим яркость
  FastLED.setBrightness(BRIGHT);
  // ... очищаем
  FastLED.clear();
  // ... проморгать диодами
  for(int i=0; i < 100; i++){
    all_leds_set_color((i+1)*2, (i+1)*2, (i+1)*2);
    FastLED.show();    
    delay(10);
  }
  for(int i=100; i > 0; i--){
    all_leds_set_color((i-1)*2, (i-1)*2, (i-1)*2);
    FastLED.show();    
    delay(10);
  }
}

#ifdef USUAL_ARDUINO
static void Timer2Init(void){
  // Настройка на режим совпадения(СТС)
  ClrBit(TCCR2A, WGM20);
  SetBit(TCCR2A, WGM21);
  ClrBit(TCCR2B, WGM22);
  
  // Настройка предделителя и периода таймера  
//  // Мод 1------------------------------------
//  // Конфигурация на Fosc/1024 (1 такт 64 мкс)
//  SetBit(TCCR2B, CS20);
//  SetBit(TCCR2B, CS21);
//  SetBit(TCCR2B, CS22);
//  // Предел для регистра сравнения
//  OCR2A = 15; // 15 ~ 1 мс
//  // -----------------------------------------
  // Мод 2------------------------------------
  // Конфигурация на Fosc/32 (1 такт 2 мкс)
  SetBit(TCCR2B, CS20);
  SetBit(TCCR2B, CS21);
  ClrBit(TCCR2B, CS22);
  // Предел для регистра сравнения
  OCR2A = 50; // 50 ~ 100 мкс
  // -----------------------------------------
  
  // Включение прерывания по совпадению (СТС)
  SetBit(TIMSK2,  OCIE2A);   
}
#endif
#ifdef LEONARDO_VERSION
static void  Timer3Init(void){
  // Настройка на режим совпадения(СТС)
  ClrBit(TCCR3A, WGM30);
  SetBit(TCCR3A, WGM31);
  ClrBit(TCCR3B, WGM32);
  // Мод 2------------------------------------
  // Конфигурация на Fosc/32 (1 такт 2 мкс)
  SetBit(TCCR3B, CS30);
  SetBit(TCCR3B, CS31);
  ClrBit(TCCR3B, CS32);
  // Предел для регистра сравнения
  OCR3A = 50; // 50 ~ 100 мкс
  // -----------------------------------------
  // Включение прерывания по совпадению (СТС)
  SetBit(TIMSK3,  OCIE3A); 
  
}
#endif

#ifdef USUAL_ARDUINO
ISR(TIMER2_COMPA_vect){
#endif
#ifdef LEONARDO_VERSION
ISR(TIMER3_COMPA_vect){
#endif
  if(frame_timeout_flag){
    if (counter > frame_timeout){
      frame_timeout_flag = 0;
      counter = 0;
    }
    else counter ++;
  }
  else counter = 0;
}
