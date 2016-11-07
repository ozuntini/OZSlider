/* OZSlider.ino
   Application de déplacement d'un appareil photo sur un slider
   afin de réaliser des Time-Lapse
   Déplacement du chariot via un moteur pas à pas et déclenchement de l'appariel photo
*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);    //set LCD output pins

// ------------------------------------- Define constants of connection
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
// ------------------------------------- Define constants of motion
//define delay for damping in ms
const int dampingDelay = 500;
//define delay for shutter command in ms
const int shutterDelay = 130;
//define moving Length maximum in mm
const int movingLengthMax = 680;	//longueur maximum du déplacement à adapter en fonction du slider
//step to mm conversion factor
const int stepTommConvFactor = 40;	//x steps for 1mm
//time per steps in ms
const int timePerSteps = 7;		//time in ms for 1 step
// ------------------------------------- Define seting variables
int movingLength = movingLengthMax; //longueur du déplacement en mm
unsigned int movingDuration = 60;	//durée du cycle en min max = 1020min soit 17h
int cycleDuration = 3;				//intervalle entre deux pauses en s min = 1s max 65s
boolean directionMotor = 0;			//0 Depuis le moteur - 1 Vers le moteur
// ------------------------------------- Define calculated variables
//Nombre de pauses calculé
unsigned int shootNumber = ((movingDuration*60)/cycleDuration);
//Temps de pause max à configurer sur le boitier photo
unsigned int shutterTimeMax = ((cycleDuration * 1000) - shutterDelay - dampingDelay - ((movingLength * stepTommConvFactor * timePerSteps) / shootNumber));

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
	if (adcIn < 200)  return btnUp;
	if (adcIn < 400)  return btnDn;
	if (adcIn < 600)  return btnL;
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
	    	lcd.setCursor(4,1);
	    	lcd.print(" min 1020max");
	    	// set value
	    	movingDuration = printModifyReadValue(4,movingDuration);
	    	if(movingDuration > 1020){movingDuration = 1020;}
			lcd.setCursor(0,1);
			lcd.print("     ");
			lcd.setCursor(0,1);
	    	lcd.print(movingDuration);
	    	break;
	    case 2:							//Config de la duree d'un cycle cycleDuration
	    	// display value
			lcd.setCursor(0,1);
	    	lcd.print(cycleDuration);
	    	// display info
	    	lcd.setCursor(5,1);
	    	lcd.print(" s     ");
	    	// set value
	    	cycleDuration = int(printModifyReadValue(4,cycleDuration));
			lcd.setCursor(0,1);
			lcd.print("    ");
			lcd.setCursor(0,1);
	    	lcd.print(cycleDuration);
	    	break;
	    case 3:							//Config de la direction du mvt, Vers le moteur ou Depuis le moteur
	    	do{
	    		lcd.setCursor(0, 1);
	    		lcd.blink();
	    		if(!directionMotor){
	    			lcd.print("Depuis le moteur");  
	    		} else {
	    			lcd.print("Vers le moteur  ");
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
		"Long. du mvt.  >", "< Long. cycle >", "< Intervalle   >", "<  Direction   >", "< Terminer conf?"};
	unsigned int valueItems[4] = {movingLength,movingDuration,cycleDuration,directionMotor};
	char* unitItems[] = {" mm", " min", " s", "", ""};
	int iMenu=0;
	while(iMenu<5){				//Modifier la limite si plus ou moins d'item au menu
		lcd.clear();
	    lcd.setCursor(0,0);
	    lcd.print(menuConfigItems[iMenu]);
	    lcd.setCursor(0,1);
	    if(iMenu<3){
	    	lcd.print(valueItems[iMenu]);
	    	lcd.print(unitItems[iMenu]);
	    } else {
	    	if(iMenu==3){
	    		if(!directionMotor){
	    			lcd.print("Depuis le moteur");
		    	} else {
		    		lcd.print("Vers le moteur  "); }
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
	    		if(iMenu>4){iMenu=4;}
	    	    break;
	    	case 4:							//Select button
	    		if(iMenu==4){				//End conf Menu
	    			iMenu=5;	    		    
	    		}
	    		else{
	    			readConfig (iMenu);
	    			valueItems[0] = movingLength;
	    			valueItems[1] = movingDuration;
	    			valueItems[2] = cycleDuration;
				}
	    	    break;
	    	default:
	    	    break;
	    	delay(500);
	    }
	}
}

// Display movingLength, movingDuration, cycleDuration during 10 seconds
// Display calculated shootNumber and shutterTimeMax
void page1(){
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Mvt. : ");
	lcd.print(movingLength);
	lcd.print(" mm     ");
	lcd.setCursor(0,1);
	lcd.print("Cycle : ");
	lcd.print(movingDuration);
	lcd.print(" min   ");
}
void page2(){
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Inter.: ");
	lcd.print(cycleDuration);
	lcd.print(" s     ");
	lcd.setCursor(0,1);
	if(!directionMotor){
		lcd.print("Depuis le moteur");
	} else {
		lcd.print("Vers le moteur  ");
	}
}
void displayConfig(){
	page1();
	do {
		btnVal = readLcdButtons();
		switch (btnVal) {
		    case 0:
		    	page1();
		      	break;
		    case 1:
		    	page2();
		      	break;
		    default:
		    	break;
		}
		delay(100);
	} while (btnVal!=4);
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
	Serial.println("OZ-Slider v1.1!");
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
	lcd.print("OZ-Slider v1.1!");
	backlightControl();				// gestion manuelle du backlight Up, Dn, Sel
}

void loop() {
	// ------------------------------------------------------ Gestion de la config
	config();						// enregistrement de la configuration du set
	// ------------------------------------------------------ Calcul des valeurs de travail et affichage 
	// Nombre de pauses calculé
	shootNumber = ((movingDuration*60)/cycleDuration);
	// Temps de pause max en ms à configurer sur le boitier photo
	shutterTimeMax = ((cycleDuration * 1000) - shutterDelay - dampingDelay - ((movingLength * stepTommConvFactor * timePerSteps) / shootNumber));
	displayConfig();					// print config and shootNumber and cycleDuration
	// Display shutterTimeMax
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Temps de pause  ");
	lcd.setCursor(0,1);
	lcd.print(shutterTimeMax);
	lcd.setCursor(5,1);
	lcd.print(" ms maximum     ");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Display shutterTimeMax
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Nb. de pauses   ");
	lcd.setCursor(0,1);
	lcd.print(shootNumber);
	lcd.setCursor(5,1);
	lcd.print(" pauses     ");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Connexion de l'appareil photo
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Connecter ");
	lcd.setCursor(0,1);
	lcd.print("l'appareil photo");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Positionnement du chariot
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Placer chariot");
	lcd.setCursor(0,1);
	if(!directionMotor){
	    lcd.print("Cote moteur     ");     
	} else {
		lcd.print("Oppose au moteur");
	}
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);


	long stepsMovingLength = movingLength * stepTommConvFactor; 	// déplacement total en pas
	int stepsInterval = stepsMovingLength / shootNumber;			// nb de pas à déplacer entre deux pause

			// put config on serial port
			Serial.println("Config");
			Serial.print("Long. Mvt : ");
			Serial.println(movingLength);
			Serial.print("Long. du set : ");
			Serial.println(movingDuration);
			Serial.print("Temps d'un Cycle : ");
			Serial.println(cycleDuration);
			Serial.print("Direction : ");
			Serial.println(directionMotor);
			//
			Serial.print("Nb. de pause : ");
			Serial.println(shootNumber);
			Serial.print("Temps de pause max : ");
			Serial.println(shutterTimeMax);
			Serial.print("Long. Mvt en step : ");
			Serial.println(stepsMovingLength);
			Serial.print("Intervalle en pas : ");
			Serial.println(stepsInterval);
	int stepsIntervalTime = (stepsInterval * timePerSteps);				// temps de déplacement de stepsInterval en ms
			Serial.print("Temps de déplacement : ");
			Serial.println(stepsIntervalTime);
			//

	// ------------------------------------------------------ Lancement du cycle ?
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Lancement du");
	lcd.setCursor(0,1);
	lcd.print("cycle ?");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// ------------------------------------------------------ Cycle de fonctionnement
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Cycle en cours");
	// Sens du mouvement
	digitalWrite(chariotDirectionPin, directionMotor);
	// Motor sleep mode off
	digitalWrite(sleepDrivePin, HIGH);
	// ------------------------------------------------------ Départ du Cycle
	unsigned long timeStart;
	unsigned int stepsNumber=0;
	timeStart = millis();
	// déclenchement de la première photo
			//Serial.print(" - Shutter ");
	digitalWrite(shutterTriggerPin, LOW);
	digitalWrite(shutterTriggerPin, HIGH);		// Shutter On
	delay(shutterDelay);
	digitalWrite(shutterTriggerPin, LOW);
	do{											// Attente de la fin de pause
		delay(50);
	} while ((millis() - timeStart) < (cycleDuration * 1000));
	// ------------------------------------------------------ Boucle de Cycle
	int seqCycle=0;
	do{
	    // Incrementation du cycle
	    seqCycle++;
	    // Mesure de la durée du cycle
		timeStart = millis();
			//Serial.print(seqCycle);
		// déplacement du chariot
			//Serial.println("- Move.");
		for(int steps=0; steps < stepsInterval; steps++){
			digitalWrite(stepperDrivePin, HIGH);
			delay(2);
			digitalWrite(stepperDrivePin, LOW);
		    delay(2);
		    stepsNumber++;
		}
		// amortissement
		delay(dampingDelay);
		// déclenchement de la photo
			//Serial.print(" - Shutter ");
		digitalWrite(shutterTriggerPin, LOW);
		digitalWrite(shutterTriggerPin, HIGH);		// Shutter On
		delay(shutterDelay);
		digitalWrite(shutterTriggerPin, LOW);
		// Print conditions
		lcd.setCursor(0,0);
		lcd.print(seqCycle);
		lcd.print("/");
		lcd.print(shootNumber);
		lcd.print(" Photos      ");
		lcd.setCursor(0,1);
		lcd.print(stepsNumber/stepTommConvFactor);
		lcd.print(" mm parcourus");
		do{
		    delay(50);
		} while ((millis() - timeStart) < (cycleDuration * 1000));
		// Max steps limit
	} while (stepsNumber <= stepsMovingLength - stepsInterval);
	// ------------------------------------------------------ Fin de boucle de Cycle
		Serial.println("Fin de Cycle !");
		Serial.print("Nb de Cycle : ");
		Serial.println(seqCycle);
		Serial.print("Nb de pas : ");
		Serial.println(stepsNumber);
	lcd.setCursor(0,0);
	lcd.print("Fin du cycle !  ");
	lcd.setCursor(0,1);
	lcd.print(seqCycle);
	lcd.print(" Pho. ");
	lcd.print(stepsNumber/stepTommConvFactor);
	lcd.print(" mm");
	do {
		btnVal = readLcdButtons();
		delay(100);
	} while (btnVal!=4);
	// Motor sleep mode on
	digitalWrite(sleepDrivePin, LOW);
}