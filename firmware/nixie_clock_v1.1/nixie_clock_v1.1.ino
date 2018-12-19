/*
  Скетч к проекту "Часы на ГРИ"
  Страница проекта (схемы, описания): https://alexgyver.ru/nixieclock/
  Исходники на GitHub: https://github.com/AlexGyver/nixieclock/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/
/*
  SET
    - удержали в режиме часов - настройка БУДИЛЬНИКА
    - удержали в режиме настройки будильника - настройка ЧАСОВ
    - двойной клик в режиме будильника - вернуться к часам
    - удержали в настройке часов - возврат к часам с новым временем
    - клик во время настройки - смена настройки часов/минут
  ALARM - вкл/выкл будильник
*/
// ************************** НАСТРОЙКИ **************************
#define BRIGHT 100          // яркость цифр дневная, %
#define BRIGHT_N 20         // яркость ночная, %
#define NIGHT_START 23      // час перехода на ночную подсветку (BRIGHT_N)
#define NIGHT_END 7         // час перехода на дневную подсветку (BRIGHT)
#define FREQ 900            // частота писка будильника

#define CLOCK_TIME 10       // время (с), которое отображаются часы
#define TEMP_TIME 5         // время (с), которое отображается температура и влажность
#define ALM_TIMEOUT 30      // таймаут будильника

// *********************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define BURN_TIME 200       // период обхода в режиме очистки
#define REDRAW_TIME 3000    // время цикла одной цифры, мс
#define ON_TIME 2200        // время включенности одной цифры, мс

// пины
#define PIEZO 3
#define DHT_PIN 2
#define ALARM 11

#define DECODER0 A0
#define DECODER1 A1
#define DECODER2 A2
#define DECODER3 A3

#define KEY0 4    // точка
#define KEY1 10   // часы 
#define KEY2 9    // часы
#define KEY3 5    // минуты
#define KEY4 6    // минуты
#define KEY5 7    // секунды
#define KEY6 8    // секунды

#include "DHT.h"
DHT dht(DHT_PIN, DHT22);

#include "GyverTimer.h"
GTimer_us redrawTimer(REDRAW_TIME);
GTimer_ms modeTimer((long)CLOCK_TIME * 1000);
GTimer_ms dotTimer(500);
GTimer_ms blinkTimer(800);
GTimer_ms almTimer((long)ALM_TIMEOUT * 1000);

#include "GyverButton.h"
GButton btnSet(3, LOW_PULL, NORM_OPEN);
GButton btnUp(3, LOW_PULL, NORM_OPEN);
GButton btnDwn(3, LOW_PULL, NORM_OPEN);

#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;

#include "EEPROMex.h"

int opts[] = {KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6};
byte counter;
byte digitsDraw[7];
boolean dotFlag;
int8_t hrs = 10, mins = 10, secs;
int8_t alm_hrs = 10, alm_mins = 10;
boolean indState;
int8_t mode = 0;    // 0 часы, 1 температура, 2 настройка будильника, 3 настройка часов, 4 аларм
boolean changeFlag;
boolean blinkFlag;
int on_time = ON_TIME;
boolean alm_flag;

void setup() {
  Serial.begin(9600);
  almTimer.stop();
  btnSet.setTimeout(400);
  btnSet.setDebounce(90);
  dht.begin();
  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  DateTime now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(KEY0, OUTPUT);
  pinMode(KEY1, OUTPUT);
  pinMode(KEY2, OUTPUT);
  pinMode(KEY3, OUTPUT);
  pinMode(KEY4, OUTPUT);
  pinMode(KEY5, OUTPUT);
  pinMode(KEY6, OUTPUT);

  pinMode(PIEZO, OUTPUT);
  pinMode(ALARM, INPUT_PULLUP);

  if (EEPROM.readByte(100) != 66) {   // проверка на первый запуск
    EEPROM.writeByte(100, 66);
    EEPROM.writeByte(0, 0);     // часы будильника
    EEPROM.writeByte(1, 0);     // минуты будильника
  }
  alm_hrs = EEPROM.readByte(0);
  alm_mins = EEPROM.readByte(1);

  sendTime();
  changeBright();
}

void sendTime() {
  digitsDraw[1] = (byte)hrs / 10;
  digitsDraw[2] = (byte)hrs % 10;

  digitsDraw[3] = (byte)mins / 10;
  digitsDraw[4] = (byte)mins % 10;

  digitsDraw[5] = (byte)secs / 10;
  digitsDraw[6] = (byte)secs % 10;
}

void changeBright() {
  // установка яркости от времени суток
  if ( (hrs >= NIGHT_START && hrs <= 23)
       || (hrs >= 0 && hrs <= NIGHT_END) ) on_time = (float)ON_TIME * BRIGHT_N / 100;
  else on_time = (float)ON_TIME * BRIGHT / 100;
}

void loop() {
  if (redrawTimer.isReady()) showDigits();
  if (dotTimer.isReady() && (mode == 0 || mode == 1)) calculateTime();
  buttonsTick();
  if (!alm_flag) modeTick();

}

void buttonsTick() {
  int analog = analogRead(7);

  btnSet.tick(analog <= 1023 && analog > 1000);
  btnUp.tick(analog <= 820 && analog > 690);
  btnDwn.tick(analog <= 280 && analog > 120);

  if (mode == 2 || mode == 3) {
    if (btnUp.isClick()) {
      if (mode == 2) {
        if (!changeFlag) {
          alm_mins++;
          if (alm_mins > 59) {
            alm_mins = 0;
            alm_hrs++;
          }
          if (alm_hrs > 23) alm_hrs = 0;
        } else {
          alm_hrs++;
          if (alm_hrs > 23) alm_hrs = 0;
        }
      } else {
        if (!changeFlag) {
          mins++;
          if (mins > 59) {
            mins = 0;
            hrs++;
          }
          if (hrs > 23) hrs = 0;
        } else {
          hrs++;
          if (hrs > 23) hrs = 0;
        }
      }
    }

    if (btnDwn.isClick()) {
      if (mode == 2) {
        if (!changeFlag) {
          alm_mins--;
          if (alm_mins < 0) {
            alm_mins = 59;
            alm_hrs--;
          }
          if (alm_hrs < 0) alm_hrs = 23;
        } else {
          alm_hrs--;
          if (alm_hrs < 0) alm_hrs = 23;
        }
      } else {
        if (!changeFlag) {
          mins--;
          if (mins < 0) {
            mins = 59;
            hrs--;
          }
          if (hrs < 0) hrs = 23;
        } else {
          hrs--;
          if (hrs < 0) hrs = 23;
        }
      }
    }

    if (blinkTimer.isReady()) {
      if (blinkFlag) blinkTimer.setInterval(800);
      else blinkTimer.setInterval(200);
      blinkFlag = !blinkFlag;
    }

    if (mode == 2) {
      digitsDraw[1] = alm_hrs / 10;
      digitsDraw[2] = alm_hrs % 10;
      digitsDraw[3] = alm_mins / 10;
      digitsDraw[4] = alm_mins % 10;
    } else {
      digitsDraw[1] = hrs / 10;
      digitsDraw[2] = hrs % 10;
      digitsDraw[3] = mins / 10;
      digitsDraw[4] = mins % 10;
    }

    digitsDraw[5] = 0;  // секунды
    digitsDraw[6] = 0;  // секунды

    if (blinkFlag) {      // горим
      if (changeFlag) {
        digitsDraw[1] = 10;
        digitsDraw[2] = 10;
      } else {
        digitsDraw[3] = 10;
        digitsDraw[4] = 10;
      }
    }
  }

  if (mode == 1 && btnSet.isClick()) {
    mode = 0;
    modeTimer.setInterval((long)CLOCK_TIME * 1000);
  }

  if (mode == 0 && btnSet.isHolded()) {
    mode = 2;
  }

  if (mode == 2 && btnSet.isHolded()) {
    mode = 3;
  }

  if (mode == 2 && btnSet.isDouble()) {
    sendTime();

    EEPROM.updateByte(0, alm_hrs);
    EEPROM.updateByte(1, alm_mins);
    mode = 0;
    modeTimer.setInterval((long)CLOCK_TIME * 1000);
  }

  if (mode == 3 && btnSet.isHolded()) {
    sendTime();
    mode = 0;
    secs = 0;
    EEPROM.updateByte(0, alm_hrs);
    EEPROM.updateByte(1, alm_mins);
    rtc.adjust(DateTime(2014, 1, 21, hrs, mins, 0));

    changeBright();
    modeTimer.setInterval((long)CLOCK_TIME * 1000);
  }

  if ((mode == 2 || mode == 3) && btnSet.isClick()) {
    changeFlag = !changeFlag;
  }

}

void modeTick() {
  if (modeTimer.isReady()) {
    if (mode == 0) {
      for (byte i = 1; i < 7; i++) digitsDraw[i] = 10;
      mode = 1;
      dotFlag = false;
      byte temp = dht.readTemperature();
      byte hum = dht.readHumidity();
      digitsDraw[1] = temp / 10;
      digitsDraw[2] = temp % 10;
      digitsDraw[3] = 10;
      digitsDraw[4] = 10;
      digitsDraw[5] = hum / 10;
      digitsDraw[6] = hum % 10;
      modeTimer.setInterval((long)TEMP_TIME * 1000);
    } else if (mode == 1) {
      for (byte i = 1; i < 7; i++) digitsDraw[i] = 10;
      mode = 0;
      modeTimer.setInterval((long)CLOCK_TIME * 1000);
    }
  }
}

void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag) {
    secs++;
    if (secs > 59) {
      secs = 0;
      mins++;

      if (mins == 1 || mins == 30) {      // каждые полчаса
        burnIndicators();                 // чистим чистим!
        DateTime now = rtc.now();         // синхронизация с RTC
        secs = now.second();
        mins = now.minute();
        hrs = now.hour();
      }

      if (!alm_flag && alm_mins == mins && alm_hrs == hrs && !digitalRead(ALARM)) {
        mode = 0;
        alm_flag = true;
        almTimer.start();
        almTimer.reset();
      }
    }
    if (mins > 59) {
      mins = 0;
      hrs++;
      if (hrs > 23) hrs = 0;
      changeBright();
    }


    if (mode == 0) sendTime();

    if (alm_flag) {
      if (almTimer.isReady() || digitalRead(ALARM)) {
        alm_flag = false;
        almTimer.stop();
        mode = 0;
        noTone(PIEZO);
        modeTimer.setInterval((long)CLOCK_TIME * 1000);
      }
    }
  }

  // мигать на будильнике
  if (alm_flag) {
    if (!dotFlag) {
      noTone(PIEZO);
      for (byte i = 1; i < 7; i++) digitsDraw[i] = 10;
    } else {
      tone(PIEZO, FREQ);
      sendTime();
    }
  }

}

void burnIndicators() {
  for (byte ind = 0; ind < 7; ind++) {
    digitalWrite(opts[ind], 1);
  }
  for (byte dig = 0; dig < 10; dig++) {
    setDigit(dig);
    delayMicroseconds(BURN_TIME);
  }
}

void showDigits() {
  if (indState) {
    indState = false;
    redrawTimer.setInterval(on_time);   // переставляем таймер, столько индикаторы будут светить
    counter++;                          // счётчик бегает по индикаторам (0 - 6)
    if (counter > 6) counter = 0;

    if (counter != 0) {                   // если это не точка
      setDigit(digitsDraw[counter]);      // отображаем ЦИФРУ в её ИНДИКАТОР
      digitalWrite(opts[counter], 1);     // включаем текущий индикатор
    } else {                              // если это точка
      if (dotFlag)
        if (mode != 1) digitalWrite(opts[counter], 1);   // включаем точку
        else
          digitalWrite(opts[counter], 0);   // выключаем точку
    }

  } else {
    indState = true;
    digitalWrite(opts[counter], 0);                 // выключаем текущий индикатор
    //setDigit(10);
    redrawTimer.setInterval(REDRAW_TIME - on_time); // переставляем таймер, столько индикаторы будут выключены
  }
}

// настраиваем декодер согласно отображаемой ЦИФРЕ
void setDigit(byte digit) {
  switch (digit) {
    case 0: setDecoder(0, 0, 0, 0);
      break;
    case 1: setDecoder(1, 0, 0, 0);
      break;
    case 2: setDecoder(0, 0, 1, 0);
      break;
    case 3: setDecoder(1, 0, 1, 0);
      break;
    case 4: setDecoder(0, 0, 0, 1);
      break;
    case 5: setDecoder(1, 0, 0, 1);
      break;
    case 6: setDecoder(0, 0, 1, 1);
      break;
    case 7: setDecoder(1, 0, 1, 1);
      break;
    case 8: setDecoder(0, 1, 0, 0);
      break;
    case 9: setDecoder(1, 1, 0, 0);
      break;
    case 10: setDecoder(0, 1, 1, 1);    // выключить цифру!
      break;
  }
}

// функция настройки декодера
void setDecoder(boolean dec0, boolean dec1, boolean dec2, boolean dec3) {
  digitalWrite(DECODER0, dec0);
  digitalWrite(DECODER1, dec1);
  digitalWrite(DECODER2, dec2);
  digitalWrite(DECODER3, dec3);
}
