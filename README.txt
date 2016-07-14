Jun 26 2016
Projet de création d'un slider pour timelapse.
Create your own timelapse slider.

Création d'une application de déplacement du chariot via un moteur pas à pas et déclenchement de l'appareil photo.
Move a camera with stepper motor and make picture.
Arduino Uno, LCD KeypadShield SainSmart.

Thanks to Rob Taylor http://tutsplus.com/authors/rob-taylor
http://computers.tutsplus.com/tutorials/motion-control-with-arduino-motorising-a-camera-slider--cms-21539

LCD Keypad
Commande du LCD : LiquidCrystal lcd(8, 9, 4, 5, 6, 7) - 10 = backlight control
Pin : 13, 12, 11, 3, 2, 1, 0 (disponibles)
Lecture des boutons : analogRead(A0) - right= 0, up = 143, down = 328, left = 504, select = 741
Pin : Reset, 3.3v, 5v, GND, GND, Vin, A1, A2, A3, A4, A5

Utilisation : 13 = Shutter, 12 = Direction, 11 = Step, 2 = Sleep

Pololu 8825
https://www.pololu.com/product/2133

Tools
Genreral Wiring.fzz : use http://fritzing.org/home/
Modules.graphml	: use https://www.yworks.com/products/yed
Slider.xmind : use http://www.xmind.net/
Slider-Stepper-Shutter.tci : use http://b.urbani.free.fr/pagetci/tci.htm "only Windows sorry !"

Documentations
Guide de l'utilisateur.md