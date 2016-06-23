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

//	Define config variable
int movingLength = 1000; 			//longueur du déplacement en mm init=longueur max
unsigned int movingDuration = 0;	//durée du cycle en s max = 65535s soit un peu plus de 18h
int shutterTime = 0;				//temps de pose en 1/10 de s
unsigned int stepsNumber = 2;		//nombre de poses max = 65535 poses

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
int readLcdButtons() {
	delay(250); //debounce delay, tuned experimentally. delay is fine as program shouldn't be doing anything else at this point anyway
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

void config() {
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
	    }
	    while (btnVal==5 || btnVal==0 || btnVal==1);	//on ne sort que sur btnR, btnL ou btnSel
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
	    			lcd.clear();
	    			lcd.setCursor(0,1);
	    			lcd.print("Configuration");
	    			delay(2000);
				}
	    	    break;
	    	default:
	    	      // do something
	    	    break;
	    }
	}
}

void setup() {
	lcd.begin(16, 2);               // initialise LCD lib full-screen
	lcd.setCursor(0,0);             // set cursor position
	lcd.print("Bienvenue dans");	// welcome screen
	lcd.setCursor(0,1);
	lcd.print("OZ-Slider v0.1!");
	delay(2000);
	config();

				lcd.clear();
	    		lcd.setCursor(0,1);
	    		lcd.print("Start cycle ?");
}

void loop() {

}

