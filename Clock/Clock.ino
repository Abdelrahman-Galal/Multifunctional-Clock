#include <LiquidCrystal.h>
#include <TimeLib.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const char welcomeString [] = "Hello World!";
const byte plusPin = 8; //connected to the plus button
const byte minusPin = 10; //connected to the minus button
const byte functionPin = 9; //connected to the function button
int functionState = 0; //variable to track the state of the device (alarm set,alarm not set,count is done)
int hours = 0;
int minutes = 0;


void setup() {
  pinMode(functionPin, INPUT);
  pinMode(plusPin, INPUT);
  pinMode(minusPin, INPUT);
  lcd.begin(16, 2);
  for (int i = 0; i < sizeof(welcomeString) / sizeof(welcomeString[0]) - 1; i++)
  {
    lcd.print(welcomeString[i]);
    delay(300);
  }
  lcd.setCursor(0, 1);
  writeTime(hours, minutes);
  //setTime(5, 3, 2, 28, 9, 2022);
}


void loop() {
  readTime(&hours , &minutes);
  setTime(hours, minutes, 0, 28, 9, 2022);
  while (1)
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
  //three variables to calc seconds according to how many time the plus and minus buttons have been pushed
  int secondsPlusCount = 0;
  int secondsMinusCount = 0;
  int secondsTotalCount = 0;
  //three variables to calc minutes according to how many time the plus and minus buttons have been pushed
  int minsPlusCount = 0;
  int minsMinusCount = 0;
  int minsTotalCount = 0;
  unsigned long lastButtonPush = millis();   //last time either plus or mins button has been pushed
  int buttonsState = 0; //to choose between setting minutes or seconds (defaultt 0 seconds)
  functionState = 0;   // functionState = 0 while not counting means that user set time
  //exist only when either plus or mins button have been pushed AND the user is inactive for 5 seconds
  while (!functionState || ( millis() - lastButtonPush < 5000) )
  {
    //toggle between setting mins or seconds when function button is pushed
    if (digitalRead(functionPin) == LOW)
    {
      buttonsState = !buttonsState; //toggle between mins and seconds
      customDelay(*h, *m);
    }

    if (!buttonsState)
    {
      lcd.setCursor(0, 1);
      writeTime(*h, *m);
      //check plus button
      if (digitalRead(plusPin) == LOW)
      {
        customDelay(*h, *m);
        secondsPlusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //check minus button
      if (digitalRead(minusPin) == LOW)
      {
        customDelay(*h, *m);
        secondsMinusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //to enable the user to start setting the seconds at 59
      if (secondsTotalCount + secondsPlusCount - secondsMinusCount < 0 )
      {
        secondsTotalCount = 60 - secondsMinusCount;
      }
      else
      {
        secondsTotalCount = secondsTotalCount + secondsPlusCount - secondsMinusCount;
      }
      secondsPlusCount = 0;
      secondsMinusCount = 0;
      if (secondsTotalCount > 59)
      {
        *m = 0;
        secondsTotalCount = 0;
      }
      else
      {
        *m = secondsTotalCount;
      }
    }
    else {
      //check plus button
      lcd.setCursor(0, 1);
      writeTime(*h, *m);
      if (digitalRead(plusPin) == LOW)
      {
        customDelay(*h, *m);
        minsPlusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //check minus button
      if (digitalRead(minusPin) == LOW)
      {
        customDelay(*h, *m);
        minsMinusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //to enable the user to start setting the minutes at 23
      if (minsTotalCount + minsPlusCount - minsMinusCount < 0 )
      {
        minsTotalCount = 24 - minsMinusCount;
      }
      else
      {
        minsTotalCount = minsTotalCount + minsPlusCount - minsMinusCount;
      }
      minsPlusCount = 0;
      minsMinusCount = 0;
      if (minsTotalCount > 23)
      {
        *h = 0;
        minsTotalCount = 0;
      }
      else
      {
        *h = minsTotalCount;
      }
    }
  }
  functionState = 0;
}

void writeTime(int h, int m) {
  char formatedTime[6];
  if (second() % 2)
  {
    sprintf(formatedTime, "%02d %02d", h, m);
  }
  else
  {
    sprintf(formatedTime, "%02d:%02d", h, m);
  }
  lcd.print(formatedTime);
}

//function to encounter for button bouncing by introducing a delay
void customDelay(int h, int m)
{
  for (int i = 0 ; i < 120; i++)
  {
    lcd.setCursor(0, 1);
    writeTime(h, m);
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
