# Multifunctional-Clock
Multifunctional Clock using Arduino.

  - Multifunctional Clock (Clock + Alarm)
  - A sketch to work with a LCD display (16 * 2 ),active buzzer and 3 push buttons.
  - Using the 3 buttons (function,plus,minus) , the user can set a clock/alarm.
  - The display shows different texts according to the day and time.
  - The user can enter the menu and choose between different operations.
  - The device has 3 states:
    $ state '0' -- Only Clock (every time the user set initial clock)
    $ state '1' -- When user enter setting Menu which has 3 options (Set Clock- Set Alarm - Exit)
    $ state '2' -- Alarm is setted
