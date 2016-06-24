/* OZSlider.ino
   Application de déplacement d'un appareil photo sur un slider
   afin de réaliser des Time-Lapse
   Déplacement du chariot via un moteur pas à pas et déclenchement de l'appariel photo
*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);    //set LCD output pins

//define analog read pin
const int analogReadPin = 0;			//Pin A0 is only for read button value
//define backlightControl pin
const int backlightControlPin = 10;		//Pin 10 is only backlight control, if it goes low, backlight turns off!
//define stepper driver pin
const int stepperDrivePin = 11;
//define chariot direction pin
const int chariotDirectionPin = 12;
//define shutter trigger pin
const int shutterTriggerPin = 13;
//define moving Length maximum
const int movingLengthMax = 1000;	//longueur maximum du déplacement à adapter en fonction du slider

//	Define config variable
int movingLength = 0; 				//longueur du déplacement en mm
unsigned int movingDuration = 0;	//durée du cycle en s max = 65535s soit un peu plus de 18h
int shutterTime = 0;				//temps de pose en 1/10 de s
unsigned int stepsNumber = 2;		//nombre de poses max = 65535 poses
boolean directionMotor = 0;			//Vers le moteur - Depuis le moteur

void blink() {
	lcd.noDisplay();
	delay(250);
	lcd.display();
}

//BUTTONS
//define button values
const int btnUp = 0;
const int btnDn = 1;
const int btnL = 2;
const int btnR = 3;
const int btnSel = 4;
const int btnNone = 5;

//define button-reading variables
int btnVal = 5;
int adcIn = 0;

//declare button poll function
int readLcdButtons() {			// return value of pressed button or none
	delay(200); //debounce delay, tuned experimentally. delay is fine as program shouldn't be doing anything else at this point anyway
	adcIn = analogRead(analogReadPin); //read value
	// threshold values confirmed by experimentation with button calibration sketch returning the following ADC read values:
	// right: 0, up: 141, down: 327 left: 504 select: 741

	if (adcIn > 1000) return btnNone;
	if (adcIn < 50)   return btnR;
	if (adcIn < 250)  return btnUp;
	if (adcIn < 450)  return btnDn;
	if (adcIn < 650)  return btnL;
	if (adcIn < 850)  return btnSel;

	return btnNone; //if it can't detect anything, return no button pressed
}

void config() {					// print menu and and call readConfig
	//	Define menu config
	char* menuConfigItems[] = {
		"Long. du mvt.  >", "< Long. cycle >", "< Long. pause  >", "< Nb de pauses >", "<  Direction   >", "< Terminer conf."};
	int iMenu=0;
	while(iMenu<6){				//Modifier la limite si plus ou moins d'item au menu
		lcd.clear();
	    lcd.setCursor(0,0);
	    lcd.print(menuConfigItems[iMenu]);
	    do {
	    	btnVal = readLcdButtons();      //continually read the buttons...
	    } while (btnVal==5 || btnVal==0 || btnVal==1);	//on ne sort que sur btnR, btnL ou btnSel
	    switch (btnVal) {
	    	case 2:							//Left buton
	    	   	iMenu--;
	    	   	if(iMenu<0){iMenu=0;}
	    	   	break;
	    	case 3:							//Right buton
	    		iMenu++;
	    		if(iMenu>5){iMenu=5;}
	    	    break;
	    	case 4:							//Select buton
	    		if(iMenu==5){				//End conf Menu
	    			iMenu=6;	    		    
	    		}
	    		else{
	    			readConfig (iMenu);
				}
	    	    break;
	    	default:
	    	    break;
	    	delay(500);
	    }
	}
}

void readConfig(int iConfig) {	// for each parameters readConfig
	int currentMovingLength[4] = {0,0,0,0};
	int currentMovingDuration[5] = {0,0,0,0,0};
	int currentShutterTime[4] = {0,0,0,0};
	int currentStepsNumber[5] = {0,0,0,0,2};
	switch (iConfig) {
	    case 0:							//Config de la gestion du mvt movingLength
	    	for (int i = 0; i < 4; i++) {
	    		lcd.setCursor(i, 1);
	    		lcd.print(currentMovingLength[i]);
	    	}
	    	lcd.setCursor(4,1);
	    	lcd.print(" mm ");
	    	lcd.setCursor(8,1);
	    	lcd.print(movingLengthMax);
	    	lcd.setCursor(12,1);
	    	lcd.print(" max");
	    	break;
	    case 1:							//Config de la duree du mvt movingDuration
	    	for (int i = 0; i < 5; i++) {
	    		lcd.setCursor(i, 1);
	    		lcd.print(currentMovingDuration[i]);
	    	}
	    	lcd.setCursor(5,1);
	    	lcd.print("s 65535 max");
	    	break;
	    case 2:							//Config de la duree des pause shutterTime
	    	for (int i = 0; i < 4; i++) {
	    		lcd.setCursor(i, 1);
	    		lcd.print(currentShutterTime[i]);
	    	}
	    	lcd.setCursor(5,1);
	    	lcd.print(" 1/10 s");
	    	break;
	    case 3:							//Config du nombre de pause stepsNumber
	    	for (int i = 0; i < 5; i++) {
	    		lcd.setCursor(i, 1);
	    		lcd.print(currentStepsNumber[i]);
	    	}
	    	lcd.setCursor(5,1);
	    	lcd.print("- 65535 max");
	    	break;
	    case 4:							//Config de la direction du mvt, Vers le moteur ou Depuis le moteur
	    	do{
	    		lcd.setCursor(0, 1);
	    		if(directionMotor){
	    			lcd.print("Vers le moteur  ");
	    		} else {
	    			lcd.print("Depuis le moteur");
	    		}
				btnVal = readLcdButtons();
				if(btnVal==0 || btnVal ==1){ directionMotor = directionMotor ^ 1; }	//bascule de directionMotor
	    	} while (btnVal!=4);		//Sortie sur bouton select
	    	blink();
	    	break;
	    default:
	    	break;
	}
	    delay(3000);
}

void backlightControl() {		//Modification de la luminosité du LCD
	int valBacllight = 4;
	do {
		analogWrite(backlightControlPin, map(valBacllight, 0, 25, 0, 255));
		btnVal = readLcdButtons();
		switch (btnVal) {
		    case 0:
		    	valBacllight++;
		    	if(valBacllight>10){ valBacllight=10; }
		      	break;
		    case 1:
		    	valBacllight--;
		    	if(valBacllight<0){ valBacllight=0; }
		      	break;
		    default:
		    	break;
		}
		delay(100);
	} while (btnVal!=4);
}



void setup() {
	pinMode(backlightControlPin, OUTPUT);	// set backlight pin output
	pinMode(stepperDrivePin, OUTPUT);		// set motor pin output
	pinMode(chariotDirectionPin, OUTPUT);	// set direction pin output
	pinMode(shutterTriggerPin, OUTPUT);		// set shutter pin output

	lcd.begin(16, 2);               // initialise LCD lib full-screen
	lcd.setCursor(0,0);             // set cursor position
	lcd.print("Bienvenue dans");	// welcome screen
	lcd.setCursor(0,1);
	lcd.print("OZ-Slider v0.1!");
	backlightControl();				// gestion manuelle du backlight Up, Dn, Sel
	config();						// enregistrement de la configuration du set


				lcd.clear();
	    		lcd.setCursor(0,1);
	    		lcd.print("Start cycle ?");
}

void loop() {

}

