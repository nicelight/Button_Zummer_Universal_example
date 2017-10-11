#define PIN_TONE 5 // номер порта зуммера

#define BUT1 22 // кнопки сенсорные
#define BUT2 23
#define BUT3 24
#define BUT4 25
#define HOWKEYS 4 // сколько кнопок задействовано
#define ACTIVE 1 // если нажатие кнопки формирует на пине единичку( сенсорная кнопка)
#define SHORT 1 // глобальные флаги пользовательского состояния нажатых кнопок ( длясвязи с другими автоматами)
#define LONG 2
#define NONE 0

unsigned long ms = 0; // время от старта программы

byte menu = 0; // автомат отображения менюшки
unsigned long menuMs = 0;

byte zummer = 0; // avtomat zumera
bool longZum = NONE; // глобальные флаги для пищания зумером
bool shortZum[] = {NONE, NONE, NONE, NONE};
unsigned long zummerMs = 0;


byte n = 0; // количество кнопок, эту переменную менять не надо 
byte button[] = {0, 0, 0, 0}; // автоматы обработок кнопок
byte buttonPrev[] = {0, 0, 0, 0}; // автоматы для отладки 
unsigned long buttonMs[] = {0, 0, 0, 0}; // для ориентации автомата во времени
unsigned long buttonTimeCounter[] = {0, 0, 0, 0};  // временные счетчики удержания кнопок
int key[] = {BUT1, BUT2, BUT3, BUT4}; // пины на которых кнопки висят
byte KeyDetected[] = {NONE, NONE, NONE, NONE}; // глобальные ( для всех автоматов) флаги состояний кнопок



///// функция парсинга команд из серийноого порта
//формат команд: "arg1=arg2;"
// пример "state=;"
void executeRequest(String* request){
  String command = request->substring(0, request->indexOf(';') );
   command.trim();
  String arg1 = "";
  String arg2 = "";
  if (command.indexOf('=') > 0){
    arg1 = command.substring(0, command.indexOf('=') );
    arg2 = command.substring(command.indexOf('=') + 1, command.indexOf(';') );
  
  if (arg1 == "state"){
    Serial.print(" First argument of serial request is: ");
    Serial.println(arg1);
    Serial.print(" Second argument of serial request is: ");
    Serial.println(arg2);
    
  } else if (arg1 == "zum"){
    Serial.print("zummer=");  Serial.println(arg2);
    tone(PIN_TONE, arg2.toInt(), 500); // Включаем звук, определенной частоты
    delay(500);
    noTone(PIN_TONE); // Останавливаем звук
  } else if (arg1 == "volt"){
    Serial.print(" raw voltage of A7 =");
    //Serial.println(rawVolt);
  } else if (arg1 == "dis"){
    //do actions here
  }// else 
 }//else indexOf =
}//execute quest

  
void setup()   {                
  Serial.begin(115200);
  delay(10);
  pinMode(PIN_TONE, OUTPUT);  // Настраиваем контакт на выход  
  pinMode(BUT1, INPUT); // сенсорная кнопка 
  pinMode(BUT2, INPUT); // сенсорная кнопка 
  pinMode(BUT3, INPUT); // сенсорная кнопка 
  pinMode(BUT4, INPUT); // сенсорная кнопка 
}//setup


void loop() {
  ms = millis();
// парсинг сериала
if (Serial.available()){
    delay(10);
    String  request = Serial.readString();
    Serial.println(" request accepted ");
    if (request != "") executeRequest(&request); // функция парсинга uart строки (введенной с клавиатуры)
}// make com port

// обобщенный автомат обработки HOWKEYS-ного количества кнопок (на долгое и длинное нажатие)
for(n = 0; n < HOWKEYS; n++){ 
switch(button[n]){
  case 0:
  buttonMs[n] = ms;
  button[n] = 5;
  break;
  case 5: // ловим момент первого контакта нажатой
  if(digitalRead(key[n])== ACTIVE){
    buttonMs[n] = ms;
    button[n] = 10;
    buttonTimeCounter[n] = ms; // начинаем отсчет времени нажатой кнопки 
  }//if digRead
  break;
  case 10: // подождем дребезг нажатия
  if((ms - buttonMs[n]) > 0){ // if hard keys need at least 10
    buttonMs[n] = ms;
    if(digitalRead(key[n])== ACTIVE){
      button[n] = 13;
    }//if not active
    else { // иначе это наводка, игнор
      button[n] = 5; // на новый опрос
    }
  }//if ms
  break;
  case 13: // ловим когда кнопку отпустят
    if(digitalRead(key[n])!= ACTIVE){
      buttonMs[n] = ms;
      button[n] = 16;
    }//if not active
  break;
  case 16: // подождем дребезг отпускания и делаем вывод какое нажатие
  if((ms - buttonMs[n]) > 0){ // if hard keys need at least 10
    buttonMs[n] = ms;
    if(digitalRead(key[n])!= ACTIVE){
      if((ms - buttonTimeCounter[n]) < 300){ // если короткое нажатие
        KeyDetected[n] = SHORT;
        shortZum[n] = 1; // пищать 
        //button[n] = 20; // GO
      } else if((ms - buttonTimeCounter[n]) < 3000){ // если удлинненное нажатие
        KeyDetected[n] = LONG;
        longZum = 1; // пищать
        //button[n] = 25; // GO
      } else { // если дольше 3 секунд
        // пока ничего 
      }
      button[n] = 0; // на исходную
    }//if not active
  }//if ms
  break;
}//switch(buttons)
} //for(n = 0; n < HOWKEYS; n++)

switch(zummer){
                  // звучание громкое на 900 и 1400
                  //tone(PIN_TONE, 1400, 500); // Включаем звук, определенной частоты
                  //noTone(PIN_TONE); // Останавливаем звук    
  case 0:
  zummerMs = ms;
  zummer = 5;
  break;
  case 5:
  if(shortZum[0]){ // если флаг корткого зумирования установлен
    shortZum[0] = 0;
    tone(PIN_TONE, 500, 50); // Включаем звук на 50 мс
    zummerMs = ms;
    zummer = 11;
  } 
  if(shortZum[1]){ // если флаг корткого зумирования установлен
    shortZum[1] = 0;
    tone(PIN_TONE, 400, 50); // Включаем звук на 50 мс
    zummerMs = ms;
    zummer = 11;
  } 
  if(shortZum[2]){ // если флаг корткого зумирования установлен
    shortZum[2] = 0;
    tone(PIN_TONE, 600, 50); // Включаем звук на 50 мс
    zummerMs = ms;
    zummer = 11;
  } 
  if(shortZum[3]){ // если флаг корткого зумирования установлен
    shortZum[3] = 0;
    tone(PIN_TONE, 500, 50); // Включаем звук на 50 мс
    zummerMs = ms;
    zummer = 11;
  } 
  if(longZum){
    longZum = 0;
    tone(PIN_TONE, 1000, 400); // Включаем звук на 400 мс
    zummer = 22;
    zummerMs = ms;
  }
  break;
  case 11:
  if((ms - zummerMs ) > 50){ // пищим 50 мс
    noTone(PIN_TONE); // Останавливаем звук   
    zummer = 0; // на исходную
  }
  break;
  case 22:
  if((ms - zummerMs ) > 400){ // пищим 400 мс
    noTone(PIN_TONE); // Останавливаем звук   
    zummer = 0; // на исходную
  }
  break;
/*  
  case :
  
  break;
*/
}// switch(zummer)


switch (menu){
  case 0: 
  break;
/*  
  case :
  
  break;
*/
} // switch (menu)

}//loop
