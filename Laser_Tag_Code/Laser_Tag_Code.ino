// MIE438 Group 12 Laser Tag Program

// Initialize LCD display
#include <LiquidCrystal.h>

//initialize LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//initialize pins
int sensorPin = A0;  // IR reciever on pin 7
int senderPin = 6;   // IR emitter on pin 6
int triggerPin = 8;  // pushbutton to fire shot on pin 8
int speakerPin = 9;  // speaker on pin 9
int blinkPin = 10;   // blinking LED used for visual effects on pin 10

//initialize boolean variables
bool hit = false;    //used to determine if player got hit
bool begin = true;   //used to initialize game play
bool fired = false;  //used to determine if player fired a shot
bool trigger;        //used to determine if player hit push button

//initialize string variables
String name;  //stores name of player

//initialize integer variables
int name_len;         //used to keep track of length of player's name
int maxShots = 6;     // maximum number of shots a player can fire;
int maxHits = 6;      // maximum number of hits a player can get;
int myShots = 0;      // keeps track of current shots fired;
int myHits = 0;       // keeps track of current hits recieved;
int totalHits = 0;    // keeps track of total hits during game play
int totalShots = 0;   // leeps track of total shots fired during game play
int numLives = 3;     // player has a maximum of 3 lives



void setup() {
  //regular setup
  lcd.begin(16, 2);
  pinMode(blinkPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(senderPin, OUTPUT);
  pinMode(triggerPin, INPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);

  //play sound at beginning
  startUpMode();
}


void loop() {
  while (millis() < 600000) {
    //check to see if the player has fired a shot
    senseFire();

    //check to see if the player has been hit
    detectHit();

    //get player name
    if (begin == true) {
      getPlayerName();
      //give the player time to hide
      hiding();
      begin = false;
    }

    //display current shots and hits
    displayShotsHits();

    //reset ammo if max shots reached
    if (myShots == maxShots) {
      ammoReset();
    }

    //decrement lives if max hits reached
    if (myHits == maxHits) {
      playerDied();
    }
  }
  //if time runs out
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("Times Up!");
  delay(3000);
  gameOver();
}

//FUNCTIONS:

//function that plays music on the speaker
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

//function that determines if a shot has been fired
void senseFire() {
  //check the push button state
  trigger = digitalRead(triggerPin);

  if (trigger == HIGH && fired == false) {
    fired = true;
    myShots++;
    if (myHits <= maxHits) {
      Serial.print("Firing Shot : ");
      Serial.println(myShots);
      fireShot();
    }
  } else if (trigger == LOW) {
    if (fired == true) {
    }
    // reset the fired variable
    fired = false;
  }
}

//function that sets the senderPin high to fire a shot
void fireShot() {
  //set blinker pin high to indicate a shot is being fired
  digitalWrite(blinkPin, HIGH);

  // play firing sond
  playTone(100, 5);
  playTone(200, 5);
  playTone(300, 5);
  playTone(400, 5);
  playTone(500, 5);
  playTone(600, 5);

  //set senderPin high, this fires a beam of IR
  unsigned long senderStartTime = millis();
  while (millis() - senderStartTime < 500) {
    digitalWrite(senderPin, HIGH);
    if (detectHit()) {
      break;
    }
  }

  //reset the blinking pin to low
  digitalWrite(blinkPin, LOW);
  //reset sender pin to low
  digitalWrite(senderPin, LOW);
}



bool detectHit() {
  //sensorPin returns a value with 0-1024 based on the IR signal it receives
  if (analogRead(sensorPin) > 100) {
    hit = true;
    myHits = myHits + 1;
    Serial.println("IR receiver triggered - Hit!");
    clearRow(2);
    lcd.setCursor(0, 1);
    lcd.print("HIT!");
    playTone(900, 150);
    playTone(500, 150);
    digitalWrite(blinkPin, HIGH);
    delay(500);
    digitalWrite(blinkPin, LOW);
    clearRow(2);
  } else {
    hit = false;
  }
  return hit;
}

//function to clear the LCD
void clearRow(byte rowToClear) {
  lcd.setCursor(0, rowToClear);
  lcd.print("                ");
}

//function to play sounds and light up LED at the beginning
void startUpMode() {
  for (int i = 1; i < 4; i++) {
    digitalWrite(blinkPin, HIGH);
    playTone(900 * i, 200);
    digitalWrite(blinkPin, LOW);
    delay(200);
  }
}

//function to get the player's name using the serial monitor
//player's name is then displayed on the LCD
void getPlayerName() {
  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.println("Please enter your player name.");
  lcd.print("Enter name");

  while (Serial.available() == 0) {};

  name = Serial.readString();
  name_len = name.length();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(name_len - 1, 0);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print("Lives: ");
  lcd.print(numLives);
  delay(5000);
}

//function to give the player 5 seconds to hide
void hiding() {
  //give the player 5 seconds to hide
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("5s to hide!");
  unsigned long startTime = millis();    // start time of the loop
  while (millis() - startTime < 5000) {  // repeat for 5 seconds
    playTone(1000, 50);
    playTone(750, 50);
    playTone(500, 50);
    playTone(250, 50);
    playTone(500, 50);
    playTone(750, 50);
    playTone(1000, 50);
  }
}

//function to display hits and shots
void displayShotsHits() {
  //display the number of shots and hits
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("Shots:");
  lcd.print(myShots);
  lcd.print(" Hits:");
  lcd.print(myHits);
}

//function to similar an ammor reset after 6 shots
void ammoReset() {
  //store the total number of shots
  int melody = 0;
  totalShots = totalShots + myShots;

  //clear LCD screen
  clearRow(2);
  lcd.setCursor(0, 1);

  //let player know ammo is resetting
  lcd.print("AMMO RESETTING!");

  //play resetting song
  while (melody < 10) {
    playTone(900, 150);
    playTone(450, 150);
    melody = melody + 1;
  }

  //clear the LCD again
  clearRow(2);

  //reset the number of shots back to 0 (ammo reset)
  myShots = 0;
}

//function that contains instructions for when a player dies
void playerDied() {
  int melody = 0;
  numLives = numLives - 1;

  //clear the LCD
  //clearRow(1);
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("Life Lost");
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("Num Lives:");
  lcd.print(numLives);
  delay(3000);

  //if no lives are left, the game is over
  if (numLives == 0) {
    gameOver();
  }

  //else, the player's life is decremented by 1 and they are revived.
  else {
    //clearRow(1);
    clearRow(2);
    lcd.setCursor(0, 1);
    lcd.print("Reviving...");
    totalHits = totalHits + myHits;
    totalShots = totalShots + myShots;
    myShots = 0;
    myHits = 0;
    while (melody < 10) {
      playTone(900, 50);
      playTone(1800, 50);
      playTone(900, 50);
      melody = melody + 1;
    }
    clearRow(2);
    lcd.setCursor(0, 1);
    lcd.print("Revived!");
    delay(1000);
    //clearRow(1);
  }
}

//function that contains instructions for when the game is over
void gameOver() {
  totalShots = totalShots + myShots;
  totalHits = totalHits + myHits;
  myShots = 0;
  myHits = 0;

  //clearRow(1);
  clearRow(2);
  playTone(9000, 100);
  playTone(8000, 100);
  playTone(7000, 100);
  playTone(6000, 100);
  playTone(5000, 100);
  playTone(4000, 100);
  playTone(3000, 100);
  playTone(2000, 100);
  playTone(1000, 100);
  lcd.setCursor(0, 1);
  lcd.print("GAME OVER");
  delay(2000);
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("Shots:");
  lcd.print(totalShots);
  lcd.print(" Hits:");
  lcd.print(totalHits);
  delay(2000);
  clearRow(2);
  lcd.setCursor(0, 1);
  lcd.print("YOU LOSE!!!");
  delay(200000);
}
