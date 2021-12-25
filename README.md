# MatrixJump
Arduino project which recreates the classic game, Doodle Jump, on a 8x8 led matrix.

### Overview 
The character is represented by a dot that jumps from tile to tile to reach the highest possible scores. Throughout the journey, the dot climbs tiles ranging from small to big, from solid ones to faulty ones. 

### Game controls and rules
The dot is controlled by moving left and right on the joystick to change its direction. If the dot lands on a solid tile (powered on LEDs), the dot jumps by itself. 
Do not be fooled by blinking tiles! The dot falls right through them until it finds another solid tile to jump back on. 

### Levels 
The game has 3 levels:
- easy (1st level) -> the gamespeed is slower and the tiles are bigger, only 3 dots and 2 dots tiles occasionally. Also, there are no trappy tiles.
- overkill (2nd level) -> increased gamespeed and smaller tiles (1 dot tile appears rarely), plus faulty tiles. 
- speedrun (3rd level) -> all the same elements as in the 2nd level, but no tile stalling allowed a.k.a if you touch the same tile twice, you've lost.

### Game flow
The level played can be chosen at any time through the difficulty setting. Achieving a minimum of 50 score in the 1st level makes it so that the level is considered "passed" and the progression leads to the 2nd level. The last level is made for those who really want to challenge themselves to the max, as always finding the optimum path forward requires a lot of attention and very fast reactions. Each individiual level rewards the player with more points than the easier ones. A single jump is rewarded with 3 points on speedrun level, 2 points in overkill and only 1 point in the easy level. The player's purpose is to reach the leaderboard, as only the top 3 scores make it up there.

### Implementation details
The main functions used are: updatePositions(), scrollDownMatrix(), detectTileCollision(), generateTopRow(). Besides these, there are the menu function() and gameOver(), that just perform standard checks.

- updatePositions() : control the joystick movement, changes the dot position when jumping, changes the previous dot on and off value, checks if the game is over, checks how much scrolling/jumping occurs

- scrollDownMatrix() : makes every row get the value of the row above it, calls generateTopRow, fixes the random parameters (calls getRandomVariables()), decrements the x value of the dot to keep it at a constant height after scrolling

- detectTileCollision() : checks if the dot interacts with a higher tile, a solid tile, a trappy tile or with no tile at all, updates score and scrolling parameters accordingly

- generateTopRow() : generates a new row using the random variables (empty row, solid tile, trappy tile, small tile, big tile, medium tile)


### Setup
- 8x8 led matrix
- lcd display
- potentiometer (controls lcd brightness)
- joystick
- buzzer  
- max 7219 driver

### Picture of the setup
![WhatsApp Image 2021-12-25 at 02 07 24](https://user-images.githubusercontent.com/62179598/147374714-547eb8e8-7938-4d2f-8c31-86733b8bdceb.jpeg)


### Video demonstration
https://www.youtube.com/watch?v=P6bDHQpo6YQ&ab_channel=AlexandruOana
