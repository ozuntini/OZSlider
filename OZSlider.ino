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
//define sleepDrivePin
const int sleepDrivePin = 2;
//define stepper driver pin
const int stepperDrivePin = 11;
//define chariot direction pin
const int chariotDirectionPin = 12;
//define shutter trigger pin
const int shutterTriggerPin = 13;
//define delay for shutter command
const int shutterDelay = 130;
//define moving Length maximum
const int movingLengthMax = 1000;	//longueur maximum du déplacement à adapter en fonction du slider
//step to mm conversion factor
const int stepTommConvFactor = 5;	//x steps for 1mm
//time per steps in ms
const int timePerSteps = 15;		//time in ms for 1 step

//	Define config variable
int movingLength = movingLengthMax; //longueur du déplacement en mm
unsigned int movingDuration = 15;	//durée du cycle en mn max = 1020mn soit 17h
int shutterTime = 1;				//intervalle entre deux poses en s min = 1s
unsigned int shootNumber = 0;		//nombre de poses max = 65535 poses
boolean directionMotor = 0;			//0 Vers le moteur - 1 Depuis le moteur

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

//Print Modify Read
//Print value $2 on $1 car. lengh
//Modify value by push button
//Return modified value
unsigned int printModifyReadValue(int pLastCar, unsigned int currentValue){		// In=Position of last car in array, value. Out=value
	int currentValueArray[5] = {0,0,0,0,0};		// 5 digit limited
	int xCar = pLastCar-1;
	unsigned int valueInter = currentValue;
	unsigned int calc=0;
	//Make array for print and modify currentValue. Warning left alignment 
	for (int i=0; i < pLastCar; i++) {
		int fact=1;
		for (int j=1; j < (pLastCar-i); j++) {
			fact=fact*10;
		}
		valueInter=valueInter-calc;
		calc=(valueInter/fact)*fact;
		currentValueArray[i]=calc/fact;
	}
	for (int i = 0; i <= xCar; i++) {
		lcd.setCursor(i, 1);
	    lcd.print(currentValueArray[i]);
	}
	lcd.setCursor(xCar, 1);
	lcd.blink();
	do{											// set value for all digit between 0 and pLastCar
		btnVal = readLcdButtons();
		switch (btnVal) {
		    case 0:			//btnUp value digit +1
		    	currentValueArray[xCar]++;
		    	if(currentValueArray[xCar]>=10){ currentValueArray[xCar]=9; }
		    	break;
		    case 1:			//btnDn value digit -1
		    	currentValueArray[xCar]--;
		    	if(currentValueArray[xCar]<0){ currentValueArray[xCar]=0; }
		    	break;
		    case 2:			//btnL position digit -1
		    	xCar--;
		    	if(xCar<0){ xCar=0; }
		    	break;
		    case 3:			//btnR position digit +1
		    	xCar++;
		    	if(xCar>=pLastCar){ xCar=pLastCar-1; }
		    	break;
		    default:
		      // do something
		      break;
		}
		lcd.setCursor(xCar, 1);
		lcd.print(currentValueArray[xCar]);
		lcd.setCursor(xCar, 1);
	} while (btnVal!=4);
	lcd.noBlink();
	// calcul a final value
	currentValue = 0;
	for (int i = 0; i < pLastCar; i++) {
		currentValue *= 10;
		currentValue += currentValueArray[i];
	}
	return currentValue;
}

void readConfig(int iConfig) {	// for each parameters readConfig
	switch (iConfig) {
	    case 0:							//Config de la gestion du mvt movingLength
	    	// display value
			lcd.setCursor(0,1);
	    	lcd.print(movingLength);
	    	// display info
	    	lcd.setCursor(4,1);
	    	lcd.print(" mm ");
	    	lcd.setCursor(8,1);
	    	lcd.print(movingLengthMax);
	    	lcd.setCursor(12,1);
	    	lcd.print(" max");
	    	// set value
	    	movingLength = int(printModifyReadValue(4,movingLength));
	    	if(movingLength > movingLengthMax){movingLength = movingLengthMax;}
			lcd.setCursor(0,1);
			lcd.print("    ");
			lcd.setCursor(0,1);
	    	lcd.print(movingLength);
	    	lcd.print(" mm");
	    	break;
	    case 1:							//Config de la duree du mvt movingDuration
	    	// display value
			lcd.setCursor(0,1);
	    	lcd.print(movingDuration);
	    	// display info
	    	lcd.setCursor(5,1);
	    	lcd.print("mn 1020 max");
	    	// set value
	    	movingDuration = printModifyReadValue(4,movingDuration);
	    	if(movingDuration > 1020){movingDuration = 1020;}
			lcd.setCursor(0,1);
			lcd.print("     ");
			lcd.setCursor(0,1);
	    	lcd.print(movingDuration);
	    	break;
	    case 2:							//Config de la duree des pause shutterTime
	    	// display value
			lcd.setCursor(0,1);
	    	lcd.print(shutterTime);
	    	// display info
	    	lcd.setCursor(5,1);
	    	lcd.print(" s     ");
	    	// set value
	    	shutterTime = int(printModifyReadValue(4,shutterTime));
			lcd.setCursor(0,1);
			lcd.print("    ");
			lcd.setCursor(0,1);
	    	lcd.print(shutterTime);
	    	break;
	    case 3:							//Config du nombre de pause shootNumber
	    	// calcul du shootNumber
	    	shootNumber = (movingDuration * 60) / shutterTime;
	    	// display value
			lcd.setCursor(0,1);
	    	lcd.print(shootNumber);
	    	// display info
	    	lcd.setCursor(5,1);
	    	lcd.print("Non modif.");
	    	// set value
	    	// shootNumber = printModifyReadValue(5,shootNumber);
			lcd.setCursor(0,1);
			lcd.print("     ");
			lcd.setCursor(0,1);
	    	lcd.print(shootNumber);
	    	break;
	    case 4:							//Config de la direction du mvt, Vers le moteur ou Depuis le moteur
	    	do{
	    		lcd.setCursor(0, 1);
	    		lcd.blink();
	    		if(directionMotor){
	    			lcd.print("Vers le moteur  ");
	    		} else {
	    			lcd.print("Depuis le moteur");
	    		}
	    		lcd.setCursor(0,1);
				btnVal = readLcdButtons();
				if(btnVal==0 || btnVal ==1){ directionMotor = directionMotor ^ 1; }	//bascule de directionMotor
	    	} while (btnVal!=4);		//Sortie sur bouton select
	    	lcd.noBlink();
	    	break;
	    default:
	    	break;
	}
	delay(1000);
}

void config() {					// print menu and and call readConfig
	//	Define menu config
	char* menuConfigItems[] = {
		"Long. du mvt.  >", "< Long. cycle >", "< Intervalle   >", "< Nb de pauses >", "<  Direction   >", "< Terminer conf."};
	// calcul du shootNumber
	shootNumber = (movingDuration * 60) / shutterTime;
	unsigned int valueItems[5] = {movingLength,movingDuration,shutterTime,shootNumber,directionMotor};
	int iMenu=0;
	while(iMenu<6){				//Modifier la limite si plus ou moins d'item au menu
		lcd.clear();
	    lcd.setCursor(0,0);
	    lcd.print(menuConfigItems[iMenu]);
	    lcd.setCursor(0,1);
	    if(iMenu<4){
	    	lcd.print(valueItems[iMenu]);
	    } else {
	    	if(iMenu==4){
	    		if(directionMotor){
	    			lcd.print("Vers le moteur  ");
		    	} else {
		    		lcd.print("Depuis le moteur"); }
	    	} else {
	    		lcd.print("                "); }
	    }
	    do {
	    	btnVal = readLcdButtons();      //continually read the buttons...
	    } while (btnVal==5 || btnVal==0 || btnVal==1);	//on ne sort que sur btnR, btnL ou btnSel
	    switch (btnVal) {
	    	case 2:							//Left button
	    	   	iMenu--;
	    	   	if(iMenu<0){iMenu=0;}
	    	   	break;
	    	case 3:							//Right button
	    		iMenu++;
	    		if(iMenu>5){iMenu=5;}
	    	    break;
	    	case 4:							//Select button
	    		if(iMenu==5){				//End conf Menu
	    			iMenu=6;	    		    
	    		}
	    		else{
	    			readConfig (iMenu);
	    			valueItems[0] = movingLength;
	    			valueItems[1] = movingDuration;
	    			valueItems[2] = shutterTime;
					// calcul du shootNumber
					shootNumber = (movingDuration * 60) / shutterTime;
	    			valueItems[3] = shootNumber;
				}
	    	    break;
	    	default:
	    	    break;
	    	delay(500);
	    }
	}
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
	Serial.begin(9600);
	Serial.println("OZ-Slider v0.1!");
	// set pin mode
	pinMode(backlightControlPin, OUTPUT);	// set backlight pin output
	pinMode(sleepDrivePin, OUTPUT);			// set sleep motor pin output
	pinMode(stepperDrivePin, OUTPUT);		// set motor pin output
	pinMode(chariotDirectionPin, OUTPUT);	// set direction pin output
	pinMode(shutterTriggerPin, OUTPUT);		// set shutter pin output
	// set pin state
	digitalWrite(sleepDrivePin, LOW);
	digitalWrite(stepperDrivePin, LOW);
	digitalWrite(chariotDirectionPin, LOW);
	digitalWrite(shutterTriggerPin, LOW);

	lcd.begin(16, 2);               // initialise LCD lib full-screen
	lcd.setCursor(0,0);             // set cursor position
	lcd.print("Bienvenue dans");	// welcome screen
	lcd.setCursor(0,1);
	lcd.print("OZ-Slider v0.1!");
	backlightControl();				// gestion manuelle du backlight Up, Dn, Sel
}

void loop() {
	config();						// enregistrement de la configuration du set

	unsigned int stepsMovingLength = movingLength * stepTommConvFactor; // déplacement total en pas
	int stepsInterval = (10 * stepsMovingLength) / shootNumber;			// nb de pas *10 entre deux pause
	//int stepsIntervalTime = (stepsInterval * timePerSteps / 10);		// temps de déplacement de stepsInterval en ms

			/*/ put config on serial port
			Serial.println("Config");
			Serial.print("Long. Mvt : ");
			Serial.println(movingLength);
			Serial.print("Long. Cycle : ");
			Serial.println(movingDuration);
			Serial.print("Temps de pose : ");
			Serial.println(shutterTime);
			Serial.print("Nb. de pause : ");
			Serial.println(shootNumber);
			Serial.print("Direction : ");
			Serial.println(directionMotor);
			//
			Serial.print("Long. Mvt en step : ");
			Serial.println(stepsMovingLength);
			Serial.print("Intervalle en pas : ");
			Serial.println((stepsInterval + 5)/10);

			Serial.print("Temps de déplacement : ");
			Serial.println(stepsIntervalTime);
			*/
	// Lancement du cycle ?
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Lancement du");
	lcd.setCursor(0,1);
	lcd.print("cycle ?");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Positionnement du chariot
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Placer chariot");
	lcd.setCursor(0,1);
	if(directionMotor){
	    lcd.print("Oppose au moteur");
	} else {
		lcd.print("Cote moteur     ");
	}
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Cycle de fonctionnement
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Cycle en cours");
	// Sens du mouvement
	digitalWrite(chariotDirectionPin, directionMotor);
	// Motor sleep mode off
	digitalWrite(sleepDrivePin, HIGH);
	// Boucle de Cycle
	unsigned long timeStart;
	unsigned int stepsNumber=0;
	for(int seqCycle=1; seqCycle<=shootNumber; seqCycle++){
		timeStart = millis();
			Serial.print(seqCycle);
		// déclenchement de la photo
			Serial.print(" - Shutter ");
		digitalWrite(shutterTriggerPin, LOW);
		digitalWrite(shutterTriggerPin, HIGH);		// Shutter On
		delay(shutterDelay);
		digitalWrite(shutterTriggerPin, LOW);
		do{
		    delay(50);
		} while ((millis() - timeStart) < ((shutterTime * 1000) - shutterDelay));
		// déplacement du chariot
			Serial.println("- Move.");
		for(int steps=0; steps<((stepsInterval + 5)/10); steps++){
			digitalWrite(stepperDrivePin, HIGH);
			delay(1);
			digitalWrite(stepperDrivePin, LOW);
		    delay(10);
		    stepsNumber++;
		}
		// Print conditions
		lcd.setCursor(0,1);
		lcd.print(seqCycle);
		lcd.setCursor(5,1);
		lcd.print(stepsNumber);
		delay(100);
		// Max steps limit
		if(stepsNumber > stepsMovingLength){ break;}
	}
	// Motor sleep mode on
	digitalWrite(sleepDrivePin, LOW);
	lcd.setCursor(0,0);
	lcd.print("Fin du cycle !");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
}