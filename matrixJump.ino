#include "LedControl.h" //  need the library
#include<LiquidCrystal.h>
#include <EEPROM.h>
const int buzzerPin = 13;
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;
const int swPin = 2;
const int rsPin = 9;
const int enablePin = 8;
const int d4 = 7;
const int d5 = 6;
const int d6 = 5;
const int d7 = 4;

const int topScoreAddress = 0;
const int secondScoreAddress = 1;
const int thirdScoreAddress = 2;

const int topScorerAddress = 3;
const int secondScorerAddress = 4;
const int thirdScorerAddress = 5;

const int buzzerTone = 450;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No.DRIVER
LiquidCrystal lcd(rsPin, enablePin, d4, d5, d6, d7);

bool inputUsername = 0;
int tileCount = 0;
int startTime = 0;
int blinkDelay = 50;
byte matrixBrightness = 10;
byte xPos = 6;
byte yPos = 5;
byte xLastPos = 0;
byte yLastPos = 0;
const int minThreshold = 200;
const int maxThreshold = 600;
const byte moveInterval = 300;
unsigned long long lastMoved = 0;
unsigned long long lastRefresh = 0;
const byte refreshInterval = 200;
const byte matrixSize = 9;
bool matrixChanged = true;
bool turnOffLed = 1;
int startGame = 0;
int menuState = 1; //keeps track off the current menu tab
bool joyMoved = false; //continuous button pushing not allowed
bool buttonPressed = false;
const byte debounceInterval = 100;
int score = 0;
String topScorer = "N/A"; 
String secondScorer = "N/A";
String thirdScorer = "N/A"; 
int topScore =  EEPROM.read(topScoreAddress);
int secondScore = EEPROM.read(secondScoreAddress);
int thirdScore = EEPROM.read(thirdScoreAddress);
int settingsState = 1; 
int jumpCount = 0;
int initialJump = 2;
bool scroll = false;
bool jumping = 0;
int cnt = 0;
int c = 0;
int xRelativePos = xPos;
int previousTilePosition = 7;
int currentTilePosition = 7;
int scrollAmount = 0;
int randomTilePosition;
int tileDifference;
int nestingLevel = 1;
int difficultyLevel = 1;
int tileLength = 2; 
int scrollCount = 0;
int increment = 0;
int randomFaultyTile;
int previousFaultyTile = 0;
String username = "AAA";
int activeLetter = 0;
int ranking = 0;
int randomTileLength;


int matrix[matrixSize][matrixSize] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1},
  {0,0,0,0,0,0,0,0}
};

/*
bool loserMatrix[matrixSize][matrixSize] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,0,1,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,1,1,0,0,0},
  {0,1,0,0,0,1,0,0},
  {0,1,0,0,0,1,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
*/
/*
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
};
*/
void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  // inverts behaviour of INPUT mode (high = off, low = on)
  // button presses will be handled with interrupts
  attachInterrupt(digitalPinToInterrupt(swPin), toggle, FALLING);

  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness(0~15 possiblevalues)
  lc.clearDisplay(0); // clear screen
  matrix[xPos][yPos] = 1;

  lcd.begin(16, 2); // Print a message to the LCD.
  Serial.begin(9600);
  randomSeed(analogRead(dinPin));
  randomSeed(analogRead(clockPin));
  randomSeed(analogRead(loadPin));

  /*EEPROM.get(topScorerAddress, topScorer);
  EEPROM.get(secondScorerAddress, secondScorer);
  EEPROM.get(thirdScorerAddress, thirdScorer);
  if(topScorer > "ZZZ" || topScorer < "AAA"){
    topScorer = "N/A"; 
    //Serial.println("yes");
  }
  if(secondScorer > "ZZZ" || secondScorer < "AAA"){
    secondScorer = "N/A";
    Serial.println("yes");
  }
  if(thirdScorer > "ZZZ" || thirdScorer < "AAA"){
    thirdScorer = "N/A";
  }
  EEPROM.put(topScorerAddress, topScorer);
  EEPROM.put(secondScorerAddress, secondScorer);
  EEPROM.put(thirdScorerAddress, thirdScorer);

  Serial.println(topScorer);
  //Serial.println(secondScorer);
  Serial.println(thirdScorer);
  */
}

void loop() {
  
  //the menu routine
  if (startGame == 0) {

    if (millis() - lastMoved > moveInterval) {
      int xValue = analogRead(xPin); // moving through the options
      int yValue = analogRead(yPin); //selecting option in the subsections of the menu
      lastMoved = millis();

      switch (menuState) {
        //menu tab (can only go left and right)
      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("->PLAY GAME");
        lcd.setCursor(0, 1);
        lcd.print("HIGHSCORE");
        //enter the game state

        
        if (buttonPressed == true) {

          
          //refresh previous data
          for (int row = 0; row < matrixSize; row++) {
            for (int col = 0; col < matrixSize; col++) {
              if((row == 1 && col >=5 && col <= 7) || (row == 4 && col >=1 && col <= 3) || (row == 7 && col >= 5 && col <=7 )){
                matrix[row][col] = 1;
          }
              else{
                matrix[row][col] = 0;
          }
        }
      }
        //refresh score
          score = 0;

        //refresh dot positioning
          xPos = 3;
          yPos = 6;

        //refresh variables
        xLastPos = 0;
        yLastPos = 0;
        jumpCount = 0;
        jumping = 0;
        scroll = false;
        scrollAmount = 0;
        currentTilePosition = 7;
        previousTilePosition = 7;
        increment = 0;
        previousFaultyTile = 0;
        lastMoved = 0;
        ranking = 0;
        tileCount = 0;
          
          startGame = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Lvl  Score ");
          lcd.setCursor(0, 1);
          lcd.print(difficultyLevel);
          lcd.setCursor(5, 1);
          lcd.print(score);
        }
        if (xValue > maxThreshold && joyMoved == false && buttonPressed == false) {
          menuState += 1;
          lcd.clear();
          joyMoved = true;
        }

        if (xValue >= minThreshold && xValue <= maxThreshold) {
          joyMoved = false; //joystick is back in neutral position
        }
        break;

      case 2:
        if (buttonPressed == false) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("->HIGHSCORE");
          lcd.setCursor(0, 1);
          lcd.print("SETTINGS");
        }
        //display top 3 scores with usernames
        else {
          lcd.clear();
          // highest scorer
          lcd.setCursor(0, 0);
          lcd.print("1");
          lcd.setCursor(1, 0);
          lcd.print(topScorer);
          lcd.setCursor(0, 1);
          lcd.print(EEPROM.read(topScoreAddress));

          // 2nd highest scorer
          lcd.setCursor(6, 0);
          lcd.print("2");
          lcd.setCursor(7, 0);
          lcd.print(secondScorer);
          lcd.setCursor(6, 1);
          lcd.print(EEPROM.read(secondScoreAddress));

          // 3rd highest scorer
          lcd.setCursor(12, 0);
          lcd.print("3");
          lcd.setCursor(13, 0);
          lcd.print(thirdScorer);
          lcd.setCursor(12, 1);
          lcd.print(EEPROM.read(thirdScoreAddress));
        }

        if (xValue < minThreshold && joyMoved == false && buttonPressed == false) {
          menuState -= 1;
          lcd.clear();
          joyMoved = true;
        } else if (xValue > maxThreshold && joyMoved == false && buttonPressed == false) {
          menuState += 1;
          lcd.clear();
          joyMoved = true;
        }

        if (xValue >= minThreshold && xValue <= maxThreshold) {
          joyMoved = false; //joystick is back in neutral position
        }
        break;

        // settings tab
      case 3:
        if (buttonPressed == false && nestingLevel == 1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("->SETTINGS");
          lcd.setCursor(0, 1);
          lcd.print("ABOUT");

          if (xValue < minThreshold && joyMoved == false && buttonPressed == false) {
            menuState -= 1;
            lcd.clear();
            joyMoved = true;
          } else if (xValue > maxThreshold && joyMoved == false && buttonPressed == false) {
            menuState = 4;
            lcd.clear();
            joyMoved = true;
          }
        } else {
          lcd.clear();
          nestingLevel = 2;
          //difficulty  lcd contrast  lcd brightness  matrix brightness

          switch (settingsState) {

            //choose difficulty menu
          case 1:
            if (buttonPressed == true) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("->DIFFICULTY");
              lcd.setCursor(0, 1);
              lcd.print("GAME BRIGHTNESS");

              //go back to main menu
              if (yValue < minThreshold && joyMoved == false) {
                nestingLevel = 1;
                buttonPressed = false;
                joyMoved = true;
              }
            }

            //button was pressed again to enter the "difficulty" tab
            else {

              if (difficultyLevel == 1) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("->EASY (DEFAULT)");
                lcd.setCursor(0, 1);
                lcd.print("OVERKILL");

                //choose overkill difficulty
                if (xValue > maxThreshold && joyMoved == false) {
                  difficultyLevel = 2;
                  tileLength = 1;
                  joyMoved = true;
                }
              } else if (difficultyLevel == 2) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("->OVERKILL");
                lcd.setCursor(0, 1);
                lcd.print("SPEEDRUN (ALPHA)");
                //choose the hard difficulty
                if (xValue > maxThreshold && joyMoved == false) {
                  difficultyLevel = 3;
                  tileLength = 1;
                  joyMoved = true;
                }
              }
              else if (difficultyLevel == 3){
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("->SPEEDRUN (ALPHA)");
                lcd.setCursor(0, 1);
                lcd.print("EASY (DEFAULT)");
                //choose the easy difficulty
                if (xValue > maxThreshold && joyMoved == false) {
                  difficultyLevel = 1;
                  tileLength = 2;
                  joyMoved = true;
                }
              }
            }
          }

        }

        if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
          joyMoved = false; //joystick is back in neutral position
        }
        break;

        // for some reason this messes up the menu behaviour (nvm, it works now lol)
      case 4:
        if (buttonPressed == false) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("->ABOUT");
          lcd.setCursor(0, 1);
          lcd.print("PLAY GAME");
        } else {
          lcd.clear();
          lcd.setCursor(8, 0);
          lcd.print("Matrix");
          lcd.setCursor(8, 1);
          lcd.print("Jump");
          lcd.setCursor(16, 0);
          lcd.print("Creator:");
          lcd.setCursor(16, 1);
          lcd.print("Alex Oana");

          lcd.setCursor(27, 0);
          lcd.print("Github:");
          lcd.setCursor(27, 1);
          lcd.print("Alexandru-97");
          for (int i = 0; i < 40; i++) {
            lcd.scrollDisplayLeft();
            if (buttonPressed == false) {
              break; //i want to be able to go back to main menu even if the text is scrolling
            }
            delay(800);
          }

        }

        if (xValue < minThreshold && joyMoved == false && buttonPressed == false) {
          menuState -= 1;
          lcd.clear();
          joyMoved = true;
        } else if (xValue > maxThreshold && joyMoved == false && buttonPressed == false) {
          menuState = 1;
          lcd.clear();
          joyMoved = true;
        }

        if (xValue >= minThreshold && xValue <= maxThreshold) {
          joyMoved = false; //joystick is back in neutral position
        }
        break;

      }
    }
  } else {

    lcd.setCursor(5, 1);
    lcd.print(score);
    if (millis() - lastMoved > moveInterval) {
      // game logic
      updatePositions();
      lastMoved = millis();
    }

    // functia de afisare a matricei se apeleaza doar daca s-a schimbat ceva 
    // fata de configuratia precedenta

    // matrix display logic
    updateMatrix();
    //matrixChanged = false;

  }

}

void scrollDownMatrix() {
  // incepand de jos, fiecare rand ia valoarea randului de deasupra(pana 
  // ajungem la cel mai de sus rand, care va fi generat

  xPos++;
  for (int row = matrixSize - 1; row > 0; row--) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = matrix[row - 1][col];
    }
  }
  randomTilePosition = random(0, 6);
  randomFaultyTile = random(0,6); //decid daca randul de sus va avea un faulty tile sau nu
  if(score > 50){
    if(difficultyLevel == 1){
      tileLength = random(1,3);
    }
    else if(difficultyLevel == 2 || difficultyLevel == 3){
      randomTileLength = random(0,7);
      if(randomTileLength > 1){
        tileLength = 1;
      }
      else{
        tileLength = 0;
      }
      //tileLength = random(0,2);
    }
  }
  //Serial.println(randomFaultyTile);
  for (int col = 0; col < matrixSize; col++) {

    if (cnt % 2 == 0) {
      //rand pe care pun tile
      
      if(randomFaultyTile == 1 && previousFaultyTile ==0 && (difficultyLevel == 2 || difficultyLevel == 3)) {
       
       if (col >= randomTilePosition && col <= randomTilePosition + tileLength){
         matrix[0][col] = 2;  //marchez tile urile faulty
         if(col == randomTilePosition + tileLength){
          previousFaultyTile = 1;
         }
         Serial.println("yes");
       }
        else {
        matrix[0][col] = 0;
      }
    }
      
      else if (col >= randomTilePosition && col <= randomTilePosition + tileLength) {
        matrix[0][col] = 1; // randul de sus(este generat mereu aleatoriu)
         if(col == randomTilePosition + tileLength){
          previousFaultyTile = 0;
         }
      } else {
        matrix[0][col] = 0;
      }

    } 
    //daca nu pun un tile, ramane gol
    else {
      matrix[0][col] = 0; //no tiles on this row
    }
  }
  cnt += 1;
  // Serial.println(cnt);

}

void toggle() {

  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceInterval) {

    buttonPressed = !buttonPressed;

  }

  lastInterruptTime = interruptTime;
}

int detectCounter = 0;
void detectTileCollision() {

  //if there is a tile underneath our dot, we jump
  if (matrix[xPos + 1][yPos] == 1 && matrix[xPos][yPos] == 1) {
    //we are on a tile, jump 3 units
    tone(buzzerPin, buzzerTone, 20);
    if(difficultyLevel == 2 || difficultyLevel == 3){
      jumpCount = 4; //how far we jump
    }
    else{
      jumpCount = 3; //for the 1st level
    }
    previousTilePosition = currentTilePosition + increment;
    currentTilePosition = xPos + 1;

  //  Serial.println(currentTilePosition);

    //we only move forward if we find a tile that s higher than the tile we came from
    if (previousTilePosition > currentTilePosition) {
      score += 1;
      tileDifference = previousTilePosition - currentTilePosition;
      scrollAmount = tileDifference;
      increment = scrollAmount;
      //Serial.println(tileDifference);
    }
    else if(difficultyLevel == 3 && previousTilePosition == currentTilePosition){
      tileCount ++;
      if(tileCount > 1){
         increment = 0;
         xPos = 20;
         for (int i = 0; i < matrixSize; i++)
         for (int j = 0; j <matrixSize; j++){
           if (i == xPos + 1){
             matrix [i][j] = 0;
           }
         }
      }
     
    }
    else{
      increment = 0;
    }
    
    //Serial.println(scrollAmount);
  }
  //else, start going down
  else if((matrix[xPos + 1][yPos] == 2 && matrix[xPos][yPos] == 1) || (matrix[xPos - 1][yPos] == 2 && matrix[xPos][yPos] == 1 )){
    tone(buzzerPin, 120, 120);
    if(jumping == 0){
      xPos++;
    }
  }
  else {
    if (jumping == 0) {
      xPos++;
    }
  }
}

void updateMatrix() {
int timeNow = millis();
bool value = 0;
for (int row = 0; row < matrixSize; row++) {
for (int col = 0; col < matrixSize; col++) {

   if(timeNow - startTime > blinkDelay){
    if(value == 0){
      value = 1;
    }
    else{
      value = 0;
    }
    startTime = timeNow;
    //Serial.println("value");
   }
    if(matrix[row][col] == 2){
    Serial.println("faultyTile");
    lc.setLed(0, row, col, value);
}
  
    else{
      lc.setLed(0,row, col, matrix[row][col]);
    }
}
}
}

void updatePositions() {
  //  int xValue = analogRead(xPin);
  //Serial.println("pozitii");
  int yValue = analogRead(yPin);

  xLastPos = xPos;
  yLastPos = yPos;

  //we can only jump again if we are not already in the process of jumping
  /*if(jumping == 0){
    detectTileCollision();
  
  
  */
  detectTileCollision();
  // s-a terminat saritura(x a fost decrementat de 3 ori)
  if (jumpCount == 0) {
    // Serial.println("aici");
    //scroll = false;
    jumping = 0;
    jumpCount--; // nu neaparat necesara linia asta
  } 
  else if (jumpCount > 0) {
    jumping = 1;
    xPos--;
    jumpCount--;
  }

  //Serial.println(scrollAmount);
  if (scrollAmount > 0) {
    scroll = true;
    scrollAmount--;
  } else {
    scroll = false;
  }

  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } else {
      yPos = 0;
    }
  }
  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    } else {
      yPos = matrixSize - 1;
    }
  }
  
  // daca macar una dintre coordonate e diferita 
  // fata de configuratia precedenta, inseamna
  // ca s a produs o modificare, deci trebuie
  // reafisata matricea

  if (xPos != xLastPos || yPos != yLastPos) {
    if (difficultyLevel == 1) {
      delay(150);
    } else if (difficultyLevel == 2 || difficultyLevel == 3) {
      delay(120);
    }
    //game over
    if (xPos > 6) {
      tone(buzzerPin, 50, 1000);
      
      xPos = 100;
      if (inputUsername == 0){

        activeLetter = 0;
        
        if (score >= thirdScore){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Congratulations!");
          
        
          if (score >= topScore){
            thirdScore = secondScore;
            secondScore = topScore;
            topScore = score;
            ranking = 1;
    
            lcd.setCursor(0,1);
            lcd.print("You are 1st!");
  
            EEPROM.update(topScoreAddress, topScore);
            EEPROM.update(secondScoreAddress, secondScore);
            EEPROM.update(thirdScoreAddress, thirdScore);
            
          }
    
           
          else if (score >= secondScore){
            thirdScore = secondScore;
            secondScore = score;
            ranking = 2;
            lcd.setCursor(0,1);
            lcd.print("You are 2nd!");
  
            EEPROM.update(secondScoreAddress, secondScore);
            EEPROM.update(thirdScoreAddress, thirdScore);
          }
          else {
            thirdScore = score;
            ranking = 3;
            lcd.setCursor(0,1);
            lcd.print("You are 3rd!");
  
            EEPROM.update(thirdScoreAddress, thirdScore);
          }
          delay(4000);
           //ask for name input 
  
          
        
        }

         if(difficultyLevel == 1 && score >= 50){
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("You passed");
           lcd.setCursor(0,1);
           lcd.print("level 1!");
           difficultyLevel = 2;
           delay(4000);
         }
        
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your score:");
      lcd.setCursor(0,1);
      lcd.print(score);
      delay(3000);
      if(ranking == 1 || ranking == 2 || ranking == 3){
        inputUsername = 1;
      }
      else{
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Better luck");
        lcd.setCursor(3,1);
        lcd.print("next time!");
        delay(3000);
      }
      }
      
      buttonPressed = true;
      //prompt user to input their username
      while(inputUsername == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Input username:");
        lcd.setCursor(0,1);
        lcd.print(username);


          int yValue = analogRead(yPin);
          int xValue = analogRead(xPin);

          if(yValue <= minThreshold){
            if(activeLetter > 0){
            activeLetter--;
            
          }
          else if (activeLetter == 0){
            activeLetter = 2;
          }
        }

          else if (yValue >= maxThreshold){
           if(activeLetter < 2){
            activeLetter ++;
            
          }
          else if (activeLetter == 2){
            activeLetter = 0; 
          }
         }
          
          
          
          if(xValue <= minThreshold){
            if(username[activeLetter] <'Z'){
            username[activeLetter] ++;
            
          }
          else if (username[activeLetter] == 'Z'){
            username[activeLetter] = 'A'; 
          }
        }
         else if (xValue >= maxThreshold){
           if(username[activeLetter] >'A'){
            username[activeLetter] --;
            
          }
          else if (username[activeLetter] == 'A'){
            username[activeLetter] = 'Z'; 
          }
         }

         if(buttonPressed == false){
          inputUsername = 0;
         }
          delay(200);
      }
     
      
      
      

      if(ranking == 1){
        topScorer = username;
        
      }
      else if(ranking == 2){
        secondScorer = username;
        
      }
      else if (ranking == 3){
        thirdScorer = username;
  
      }
      
      if (inputUsername == 0){
        startGame = 0;
        buttonPressed = false;
        username = "AAA";
      }
       EEPROM.put(topScorerAddress, topScorer);
       EEPROM.put(secondScorerAddress, secondScorer);
       EEPROM.put(thirdScorerAddress, thirdScorer);
       
      /* Serial.println(topScorer);
       Serial.println(secondScorer);
       Serial.println(thirdScorer);
      */
      for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
          matrix[row][col] = 0;
        }
      }
     
      
    } 
    
    else {
      matrixChanged = true;
      if (turnOffLed == 1) {
        matrix[xLastPos][yLastPos] = 0; //inchid ledul de la coordonatele anterioare
      }

      if (matrix[xPos][yPos] == 1) {
        turnOffLed = 0;
        //daca ledul de la pozitia bilei era deja pornit(face parte din mapa, atunci nu vreau sa il mai sting dupa ce 
        // se schimba coordonatele
      } else {
        turnOffLed = 1;
      }
      //Serial.println(xPos);

      //Serial.println(scrollAmount);
      matrix[xPos][yPos] = 1; //il pornesc pe cel curent

      //vreau sa scrollez doar daca am sarit peste un tile
      if (scroll == true) {
        scrollCount++;
        scrollDownMatrix();
      }
    }
  }
}
