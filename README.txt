Jun 26 2016
Projet de création d'un slider pour timelapse.
Création d'une application de déplacement du chariot via un moteur pas à pas et déclenchement de l'appariel photo.
Arduino Uno, LCD KeypadShield SainSmart.

LCD Keypad
Commande du LCD : LiquidCrystal lcd(8, 9, 4, 5, 6, 7) - 10 = backlight control
Pin : 13, 12, 11, 3, 2, 1, 0 (disponibles)
Lecture des boutons : analogRead(A0) - right= 0, up = 143, down = 328, left = 504, select = 741
Pin : Reset, 3.3v, 5v, GND, GND, Vin, A1, A2, A3, A4, A5