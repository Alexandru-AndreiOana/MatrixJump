#include "LedControl.h" //  need the library
#include<LiquidCrystal.h>
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

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No.DRIVER
LiquidCrystal lcd(rsPin, enablePin, d4, d5, d6, d7);

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
String topScorer = "ALX";
String topScorer2 = "MEH";
String topScorer3 = "BAD";
int highestScore = 0;
int highestScore2 = 0;
int highestScore3 = 0;
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


bool matrix[matrixSize][matrixSize] = {
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
        lcd.setCursor(0, 0);
        lcd.print("->PLAY GAME");
        lcd.setCursor(0, 1);
        lcd.print("HIGHSCORE");
        //enter the game state
        if (buttonPressed == true) {
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
          lcd.print(highestScore);

          // 2nd highest scorer
          lcd.setCursor(6, 0);
          lcd.print("2");
          lcd.setCursor(7, 0);
          lcd.print(topScorer2);
          lcd.setCursor(6, 1);
          lcd.print(highestScore2);

          // 3rd highest scorer
          lcd.setCursor(12, 0);
          lcd.print("3");
          lcd.setCursor(13, 0);
          lcd.print(topScorer3);
          lcd.setCursor(12, 1);
          lcd.print(highestScore3);
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
    matrixChanged = false;

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
  randomTilePosition = random(0, 5);
  for (int col = 0; col < matrixSize; col++) {

    if (cnt % 3 == 1) {
      //rand pe care pun tile
      if (col >= randomTilePosition && col <= randomTilePosition + tileLength) {
        matrix[0][col] = 1; // randul de sus(este generat mereu aleatoriu)
      } else {
        matrix[0][col] = 0;
      }

    } else {
      matrix[0][col] = 0;
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
    jumpCount = 3; //how far we jump
    previousTilePosition = currentTilePosition + increment;
    currentTilePosition = xPos + 1;

    Serial.println(currentTilePosition);

    //we only move forward if we find a tile that s higher than the tile we came from
    //nu intra pe urmatoru if cand sunt platformele prea apropiate/una peste alta
    if (previousTilePosition > currentTilePosition) {
      score += 1;
      tileDifference = previousTilePosition - currentTilePosition;
      scrollAmount = tileDifference;
      increment = scrollAmount;
      //Serial.println(tileDifference);
    }
    else{
      increment = 0;
    }
    
    //Serial.println(scrollAmount);
  }
  //else, start going down
  else {
    if (jumping == 0) {
      xPos++;
    }
  }
}

void updateMatrix() {

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
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
    } else if (difficultyLevel == 2) {
      delay(120);
    }
    //game over
    if (xPos > 6) {
      xPos = 100;
      
      //function to print a death Matrix
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your score:");
      lcd.setCursor(0,1);
      //lcd.print(score);
      

      for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
          matrix[row][col] = loserMatrix[row][col];
        }
      }
      
      //  lcd.print(score);
    } else {
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
