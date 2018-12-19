![PROJECT_PHOTO](https://github.com/AlexGyver/nixieclock/blob/master/proj_img.jpg)
# Часы на газоразрядных индикаторах и Arduino
* [Описание проекта](#chapter-0)
* [Папки проекта](#chapter-1)
* [Схемы подключения](#chapter-2)
* [Материалы и компоненты](#chapter-3)
* [Как скачать и прошить](#chapter-4)
* [FAQ](#chapter-5)
* [Полезная информация](#chapter-6)
[![AlexGyver YouTube](http://alexgyver.ru/git_banner.jpg)](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)

<a id="chapter-0"></a>
## Описание проекта
Часы на советских газоразрядных индикаторах под управлением платформы Arduino. 
Страница проекта на сайте: https://alexgyver.ru/nixieclock/
  
Имеют три кнопки управления, датичк температуры и влажности и будильник. 
- SET
    - удержали в режиме часов - настройка БУДИЛЬНИКА
    - удержали в режиме настройки будильника - настройка ЧАСОВ
    - двойной клик в режиме будильника - вернуться к часам
    - удержали в настройке часов - возврат к часам с новым временем
    - клик во время настройки - смена настройки часов/минут
- ALARM - вкл/выкл будильник 


<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии (в этом проекте внешних библиотек нет)
- **firmware** - прошивки для Arduino
- **schemes** - схемы подключения компонентов

<a id="chapter-2"></a>
## Схемы
Схемы на EasyEDA: https://easyeda.com/beragumbo/nixieclock
	- Общая
	- ИН-14/ИН-16
	- ИН-14
	- ИН-12
Гербер файлы уже в архиве!  
Список компонентов тоже есть в архиве!
![SCHEME](https://github.com/AlexGyver/NixieClock/blob/master/schemes/scheme.jpg)

<a id="chapter-3"></a>
## Материалы и компоненты
### Ссылки оставлены на магазины, с которых я закупаюсь уже не один год
- Arduino NANO http://ali.ski/GVQzs  http://ali.ski/3SJf_
- RTC DS3231 http://ali.ski/2iTBXy
- DHT22 (датчик) http://ali.ski/lgwhky
- Пищалка http://ali.ski/lgwhky
- Кнопки http://ali.ski/6prO3r  http://ali.ski/zdU4eJ
- Ступенчатое сверло http://ali.ski/OdKec
- Проводочки http://ali.ski/uUR_r  http://ali.ski/uUR_r
- Корпус https://hobbybazza.ru/zagotovki-iz-mdf/2345-schkatulka-bolschaya-s-kruglym-kraem.html
- Ещё вариант корпуса https://hobbybazza.ru/derevyannye-zagotovki/3853-kupyurnica-pod-evro.html

## Вам скорее всего пригодится
* [Всё для пайки (паяльники и примочки)](http://alexgyver.ru/all-for-soldering/)
* [Недорогие инструменты](http://alexgyver.ru/my_instruments/)
* [Все существующие модули и сенсоры Arduino](http://alexgyver.ru/arduino_shop/)
* [Электронные компоненты](http://alexgyver.ru/electronics/)
* [Аккумуляторы и зарядные модули](http://alexgyver.ru/18650/)

<a id="chapter-4"></a>
## Как скачать и прошить
* [Первые шаги с Arduino](http://alexgyver.ru/arduino-first/) - ультра подробная статья по началу работы с Ардуино, ознакомиться первым делом!
* Скачать архив с проектом
> На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**
* Установить библиотеки в  
`C:\Program Files (x86)\Arduino\libraries\` (Windows x64)  
`C:\Program Files\Arduino\libraries\` (Windows x86)
* **Подключить внешнее питание 5 Вольт**
* Подключить Ардуино к компьютеру
* Запустить файл прошивки (который имеет расширение .ino)
* Настроить IDE (COM порт, модель Arduino, как в статье выше)
* Настроить что нужно по проекту
* Нажать загрузить
* Пользоваться  

## Настройки в коде
    // ************************** НАСТРОЙКИ **************************
    BRIGHT 100          // яркость цифр дневная, %
    BRIGHT_N 20         // яркость ночная, %
    NIGHT_START 23      // час перехода на ночную подсветку (BRIGHT_N)
    NIGHT_END 7         // час перехода на дневную подсветку (BRIGHT)
    FREQ 900            // частота писка будильника

    CLOCK_TIME 10       // время (с), которое отображаются часы
    TEMP_TIME 5         // время (с), которое отображается температура и влажность
    ALM_TIMEOUT 30      // таймаут будильника

    // *********************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
    BURN_TIME 200       // период обхода в режиме очистки
    REDRAW_TIME 3000    // время цикла одной цифры, мс
    ON_TIME 2200        // время включенности одной цифры, мс
	
<a id="chapter-5"></a>
## FAQ
### Основные вопросы
В: Как скачать с этого грёбаного сайта?  
О: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**

В: Скачался какой то файл .zip, куда его теперь?  
О: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.

В: Я совсем новичок! Что мне делать с Ардуиной, где взять все программы?  
О: Читай и смотри видос http://alexgyver.ru/arduino-first/

В: Вылетает ошибка загрузки / компиляции!
О: Читай тут: https://alexgyver.ru/arduino-first/#step-5

В: Сколько стоит?  
О: Ничего не продаю.

### Вопросы по этому проекту

<a id="chapter-6"></a>
## Полезная информация
* [Мой сайт](http://alexgyver.ru/)
* [Основной YouTube канал](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
* [YouTube канал про Arduino](https://www.youtube.com/channel/UC4axiS76D784-ofoTdo5zOA?sub_confirmation=1)
* [Мои видеоуроки по пайке](https://www.youtube.com/playlist?list=PLOT_HeyBraBuMIwfSYu7kCKXxQGsUKcqR)
* [Мои видеоуроки по Arduino](http://alexgyver.ru/arduino_lessons/)