
#include <LiquidCrystal.h>
#include <TimeLib.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const char welcomeString [] = "Hello World!";
const byte plusPin = 8; //connected to the plus button
const byte minusPin = 10; //connected to the minus button
const byte functionPin = 9; //connected to the function button
int functionState = 0; //variable to track the state of the device (alarm set,alarm not set,count is done)
char formatedTime[6];
char *currentMenuItem;

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

void setup() {
  pinMode(functionPin, INPUT);
  pinMode(plusPin, INPUT);
  pinMode(minusPin, INPUT);
  lcd.begin(16, 2);
  for (int i = 0; i < sizeof(welcomeString) / sizeof(welcomeString[0]) - 1; i++)
  {
    lcd.print(welcomeString[i]);
    delay(100);
  }
  lcd.createChar(0, arrow);
  lcd.setCursor(0, 1);
  Serial.begin(9600);
  setBaseTime();
}


void loop() {
  displayClock();
  delay(500);
  //functionState = 0;
  dsiplayMenu();
}

void displayClock() {
  functionState = 2;
  while (digitalRead(functionPin))
  {
    lcd.setCursor(0, 0);
    writeTimeOfDay();
    lcd.setCursor(0, 1);
    writeTime(hour(), minute());

  }
}

//function to prompot user to set counting time
void readTime(int *h , int *m)
{
  //variable to calc seconds according to how many time the plus and minus buttons have been pushed
  int secondsTotalCount = 0;
  //variable to calc minutes according to how many time the plus and minus buttons have been pushed
  int minsTotalCount = 0;
  unsigned long lastButtonPush = millis();   //last time either plus or mins button has been pushed
  int buttonsState = 0; //to choose between setting minutes or seconds (defaultt 0 seconds)
  //exist only when either plus or mins button have been pushed AND the user is inactive for 5 seconds
  while (!functionState || ( millis() - lastButtonPush < 5000) )
  {
    lcd.setCursor(0, 1);
    writeTime(*h, *m);
    //toggle between setting mins or seconds when function button is pushed
    if (digitalRead(functionPin) == LOW)
    {
      buttonsState = !buttonsState; //toggle between mins and seconds
      formatTime(formatedTime, *h, *m);
      customDelay(formatedTime, 0, 1);
    }
    if (!buttonsState)
    {
      if (calculateTotalPushes(&secondsTotalCount))
      {
        functionState = 1;
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
        functionState = 1;
        lastButtonPush = millis();
      }
      //enable the user to start setting the minutes at 23
      minsTotalCount = setCountLimit(minsTotalCount , 0, 23);
      *h = minsTotalCount;
    }
  }
}

void writeTime(int h, int m) {
  formatTime(formatedTime, h, m);
  lcd.print(formatedTime);
}

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

void writeTimeOfDay()
{
  if (minute() % 2)
  {
    lcd.print("Fun Time!      ");
  }
  else
  {
    if (hour() <= 10 )
    {
      lcd.print("Good Morning!");
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

void dsiplayMenu() {
  const char *menuItems[3];
  menuItems[0] = "Set Time ";
  menuItems[1] = "Set Alarm";
  menuItems[2] = "Exit      ";
  currentMenuItem = (char *) menuItems[0];
  int totalButtonPushes = 0;
  lcd.clear();
  while (digitalRead(functionPin)) {
    calculateTotalPushes(&totalButtonPushes);
    totalButtonPushes = setCountLimit(totalButtonPushes , 0, 2);
    currentMenuItem = (char *) menuItems[totalButtonPushes];
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.print(menuItems[totalButtonPushes]);
  }
  delay(500);
  switch (totalButtonPushes) {
    case 0:
      lcd.setCursor(0, 1);
      setBaseTime();
      break;
    case 1:
      break;
    case 2:
      break;
  }
}


bool calculateTotalPushes(int *totalButtonPushes) {
  bool isPushed = 0;
  if (digitalRead(plusPin) == LOW)
  {
    //delay(500);
    if (functionState == 1 || functionState == 0)
    {
      formatTime(formatedTime, hour(), minute());
      customDelay(formatedTime, 0, 1);
    }
    else
    {
      customDelay(formatedTime, 0, 0);
    }
    ++*totalButtonPushes;
    isPushed = 1;
  }
  if (digitalRead(minusPin) == LOW)
  {
    if (functionState == 1)
    {
      formatTime(formatedTime, hour(), minute());
      customDelay(formatedTime, 0, 1);
    }
    else
    {
      customDelay(formatedTime, 0, 0);
    }
    --*totalButtonPushes;
    isPushed = 1;
  }
  return isPushed;
}

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

void setBaseTime() {
  int hours = 0;
  int minutes = 0;
  functionState = 0;   // functionState = 0 while not counting means that user set time
  lcd.setCursor(0, 1);
  writeTime(hours, minutes);
  readTime(&hours , &minutes);
  setTime(hours, minutes, 0, 28, 9, 2022);
  return;
}
