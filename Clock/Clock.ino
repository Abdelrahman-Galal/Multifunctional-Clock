
/*
  - Multifunctional Clock (Clock + Alarm)
  - A sketch to work with a LCD display,active buzzer and 3 push buttons.
  - Using the 3 buttons (function,plus,minus) buttons the user can set a clock/alarm.
  - The device has 3 states:
    $ state '0' -- Only Clock (every time the user set initial clock)
    $ state '1' -- When user enter setting Menu which has 3 options (Set Clock- Set Alarm - Exit)
    $ state '2' -- Alarm is setted
  - Author : Abdelrahman Galal
*/

#include <LiquidCrystal.h>
#include <TimeLib.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const char welcomeString [] = "Hello World!";
const byte plusPin = 8; //connected to the plus button
const byte minusPin = 10; //connected to the minus button
const byte functionPin = 9; //connected to the function button
const byte alarmPin = 11 ; //connected to the active buzzer positive leg
unsigned int functionState; //variable to track the state of the device (alarm set,alarm not set,count is done)
char formatedTime[6];
char *currentMenuItem;
unsigned int alarmMinutes = 0;
unsigned int alarmHours = 0;
unsigned int clockHours = 0 ;
unsigned int clockMinutes = 0;

byte arrow[] = {
  B10000,
  B11000,
  B11100,
  B11110,
  B11110,
  B11100,
  B11000,
  B10000
};

byte alarm[] = {
  B00000,
  B00000,
  B00000,
  B00100,
  B01010,
  B11111,
  B10001,
  B00000
};

void setup() {
  pinMode(functionPin, INPUT);
  pinMode(plusPin, INPUT);
  pinMode(minusPin, INPUT);
  pinMode(alarmPin, OUTPUT);
  lcd.begin(16, 2);
  for (int i = 0; i < sizeof(welcomeString) / sizeof(welcomeString[0]) - 1; i++)
  {
    lcd.print(welcomeString[i]);
    delay(100);
  }
  lcd.createChar(0, arrow);
  lcd.createChar(1, alarm);
  lcd.setCursor(0, 1);
  setBaseTime(); //set Initial time
}


void loop() {

  displayClock();
  formatTime(formatedTime, hour(), minute());
  customDelay(formatedTime, 0, 1);
  displayMenu();
}

//function to display the clock on LCD
void displayClock() {
  if (functionState == 2)
  { lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
  while (digitalRead(functionPin))
  {
    if (functionState == 2 && hour() == alarmHours && minute() == alarmMinutes)
    {
      fireAlarm();
    }
    lcd.setCursor(0, 0);
    writeTimeOfDay();
    lcd.setCursor(0, 1);
    writeTime(hour(), minute());

  }
}

//function to prompot user to set clock/alarm
void readTime(unsigned int *h , unsigned int *m)
{
  //variable to calc seconds according to how many time the plus and minus buttons have been pushed
  int secondsTotalCount = *m;
  //variable to calc minutes according to how many time the plus and minus buttons have been pushed
  int minsTotalCount = *h;
  unsigned long lastButtonPush = millis();   //last time either plus or mins button has been pushed
  int buttonsState = 0; //to choose between setting minutes or seconds (defaultt 0 seconds)
  //exist only when either plus or mins button have been pushed AND the user is inactive for 5 seconds
  //bool isStarted = 0;
  //while (!isStarted || ( millis() - lastButtonPush < 5000) )
  while ( millis() - lastButtonPush < 5000 )
  {
    lcd.setCursor(0, 1);
    writeTime(*h, *m);
    //toggle between setting mins or seconds when function button is pushed
    if (digitalRead(functionPin) == LOW)
    {
      buttonsState = !buttonsState; //toggle between mins and seconds
      lastButtonPush = millis();
      formatTime(formatedTime, *h, *m);
      customDelay(formatedTime, 0, 1);
    }
    if (!buttonsState)
    {
      if (calculateTotalPushes(&secondsTotalCount))
      {
        //isStarted = 1;
        lastButtonPush = millis();
      }
      //enable the user to start setting the seconds at 59
      secondsTotalCount = setCountLimit(secondsTotalCount , 0, 59);
      *m = secondsTotalCount;
    }
    else {
      //check plus button
      if (calculateTotalPushes(&minsTotalCount))
      {
        //isStarted = 1;
        lastButtonPush = millis();
      }
      //enable the user to start setting the minutes at 23
      minsTotalCount = setCountLimit(minsTotalCount , 0, 23);
      *h = minsTotalCount;
    }
  }
}

//function to print formated time on LCD
void writeTime(int h, int m) {
  formatTime(formatedTime, h, m);
  lcd.print(formatedTime);
}

//function to format the time in hh:mm format with blinking ":" based on seconds value
void formatTime(char *string, int h, int m)
{
  if (second() % 2)
  {
    sprintf(string, "%02d %02d", h, m);
  }
  else
  {
    sprintf(string, "%02d:%02d", h, m);
  }
}

//function to encounter for button bouncing by introducing a delay
void customDelay(char *string, int column, int row)
{
  for (int i = 0 ; i < 120; i++)
  {
    lcd.setCursor(column, row);
    lcd.print(string);
  }
}

//function to show time of th day according to th clock
void writeTimeOfDay()
{
  if (minute() % 2)
  {
    lcd.print("Fun Time!      ");
  }
  else
  {
    if (hour() <= 5 )
    {
      lcd.print("Good Night! ");
    }
    else if (hour() <= 11 )
    {
      lcd.print("Good Morning! ");
    }
    else if (hour() <= 18)
    {
      lcd.print("Good Afternoon!");
    }
    else
    {
      lcd.print("Good Evening!");
    }
  }
}

//function to display the user menu
void displayMenu() {
  const char *menuItems[3];
  menuItems[0] = "Set Clock ";
  menuItems[1] = "Set Alarm ";
  menuItems[2] = "Exit      ";
  currentMenuItem = (char *) menuItems[0];
  int totalButtonPushes = 0;
  lcd.clear();
  unsigned int previous_functionState = functionState;
  functionState = 1;
  while (digitalRead(functionPin)) {
    currentMenuItem = (char *) menuItems[totalButtonPushes];
    calculateTotalPushes(&totalButtonPushes);
    totalButtonPushes = setCountLimit(totalButtonPushes , 0, 2);
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.print(menuItems[totalButtonPushes]);
  }
  customDelay(currentMenuItem, 0, 0);
  functionState = previous_functionState;
  switch (totalButtonPushes) {
    case 0:
      setBaseTime();
      break;
    case 1:
      setAlarm();
      break;
    case 2:
      break;
  }
}


//function to calculate total number of button (plus/minus) pushes
bool calculateTotalPushes(int *totalButtonPushes) {
  bool isPushed = 0;
  if (digitalRead(plusPin) == LOW)
  {
    if (functionState == 0 )
    {
      formatTime(formatedTime, clockHours, clockMinutes);
      customDelay(formatedTime, 0, 1);
    }
    else if (functionState == 2)
    {
      formatTime(formatedTime, alarmHours, alarmMinutes);
      customDelay(formatedTime, 0, 1);
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      customDelay(currentMenuItem, 1, 0);
    }
    ++*totalButtonPushes;
    isPushed = 1;
  }
  if (digitalRead(minusPin) == LOW)
  {
    if (functionState == 0)
    {
      formatTime(formatedTime, clockHours, clockMinutes);
      customDelay(formatedTime, 0, 1);
    }
    else if (functionState == 2)
    {
      formatTime(formatedTime, alarmHours, alarmMinutes);
      customDelay(formatedTime, 0, 1);
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      customDelay(currentMenuItem, 1, 0);
    }
    --*totalButtonPushes;
    isPushed = 1;
  }
  return isPushed;
}

//function to set upper and lower boundries of any counter
int setCountLimit(int count , int lowerLimit, int upperLimit)
{
  if (count < lowerLimit)
  {
    count = count + upperLimit + 1;
  }
  else if (count > upperLimit)
  {
    count = 0;
  }
  return count;
}

//function to set clock
void setBaseTime() {

  unsigned int previous_functionState = functionState;
  functionState = 0;
  lcd.setCursor(0, 1);
  clockHours = hour();
  clockMinutes = minute();
  writeTime(clockHours, clockMinutes);
  readTime(&clockHours , &clockMinutes);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.print("Save?       ");
  unsigned long lastButtonPush = millis();
  //save if the user confirmed the input within 3 mins
  while ( millis() - lastButtonPush < 3000  )
  {
    functionState = previous_functionState;
    if ((digitalRead(functionPin) == LOW))
    {
      functionState = 0;   // functionState = 0 while not counting means that user set time
      setTime(clockHours, clockMinutes, 0, 28, 9, 2022);
      formatTime(formatedTime, hour(), minute());
      customDelay(formatedTime, 0, 1);
      break;
    }
  }
  return;
}

//function to set alarm
void setAlarm() {
  unsigned int previous_functionState = functionState;
  functionState = 2;
  lcd.setCursor(0, 1);
  writeTime(alarmHours, alarmMinutes);
  unsigned int hours = alarmHours;
  unsigned int minutes = alarmMinutes;
  readTime(&alarmHours , &alarmMinutes);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.print("Save?       ");
  unsigned long lastButtonPush = millis();
  //save if the user confirmed the input within 3 mins
  while ( millis() - lastButtonPush < 3000  )
  {
    if ((digitalRead(functionPin) == LOW))
    {
      functionState = 2;
      formatTime(formatedTime, alarmHours, alarmMinutes);
      customDelay(formatedTime, 0, 1);
      return;
    }
  }
  // in case user does not confrm new alaram , keep old values
  functionState = previous_functionState;
  alarmHours = hours;
  alarmMinutes = minutes;
  return;
}

//function to activate/deactivate the buzzer
void fireAlarm()
{
  lcd.clear();
  lcd.print("Alarm is ON!");
  while (digitalRead(functionPin) == HIGH)
  {
    digitalWrite(alarmPin, HIGH);
    lcd.setCursor(0, 1);
    writeTime(hour(), minute());
  }
  digitalWrite(alarmPin, LOW);
  functionState = 0;
  formatTime(formatedTime, hour(), minute());
  customDelay(formatedTime, 0, 1);
  return;
}
