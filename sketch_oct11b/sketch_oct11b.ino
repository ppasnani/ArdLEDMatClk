int dataPin = 9; //PCINT8 //DS
int clockPin = 7; //PCINT9 //SHCP
int latchPin = 8; //PCINT10 //STCP
int resetPin = 5;

int countIter = 0;
int powerPinArray[5] = {2, 3, 4, 6, 10};

const int LEDNumbers[10][3][5]={
  {{1, 1, 1, 1, 1},
   {1, 0, 0, 0, 1},
   {1, 1, 1, 1, 1}},
  
  {{0, 0, 0, 0, 0},
   {1, 1, 1, 1, 1},
   {0, 0, 0, 0, 0}},
   
  {{1, 1, 1, 0, 1},
   {1, 0, 1, 0, 1},
   {1, 0, 1, 1, 1}},
  
  {{1, 1, 1, 1, 1},
   {1, 0, 1, 0, 1},
   {1, 0, 1, 0, 1}},
  
  {{1, 1, 1, 1, 1},
   {0, 0, 1, 0, 0},
   {1, 1, 1, 0, 0}},
  
  {{1, 0, 1, 1, 1},
   {1, 0, 1, 0, 1},
   {1, 1, 1, 0, 1}},
  
  {{0, 0, 1, 1, 1},
   {1, 0, 1, 0, 1},
   {1, 1, 1, 1, 1}},
  
  {{1, 1, 1, 1, 1},
   {1, 0, 0, 0, 0},
   {1, 0, 0, 0, 0}},
  
  {{1, 1, 1, 1, 1},
   {1, 0, 1, 0, 1},
   {1, 1, 1, 1, 1}},
  
  {{1, 1, 1, 1, 1},
   {1, 0, 1, 0, 0},
   {1, 1, 1, 0, 0}} 
};

int ledArray[14][5]={
  //Top ------- Bottom
    {1, 1, 1, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    
    {1, 1, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0}
  };

void setup(){
  Serial.begin(9600);
  pinMode(dataPin, OUTPUT); 
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  
  pinMode(powerPinArray[0], OUTPUT);
  pinMode(powerPinArray[1], OUTPUT);
  pinMode(powerPinArray[2], OUTPUT);
  pinMode(powerPinArray[3], OUTPUT);
  pinMode(powerPinArray[4], OUTPUT);
  Serial.println("Ready");
}

int toDisplay = 0;

void loop(){
  //Count up to a certain number of times to display each
  //frame

  if (countIter >= 250){
    //lifeStep();
    displayNum(toDisplay, 0);
    displayNum(toDisplay, 4);
    displayNum(toDisplay, 7);
    displayNum(toDisplay, 11);
    Serial.println(toDisplay);
    toDisplay++;
    if (toDisplay >= 10){
      toDisplay = 0;
    }
    countIter = 0;  
  }
  lightLeds();
  countIter++;
}

//Requires: An integer 0 <= num <= 9 and a position 0 <= pos <= 11
//          to insert anywhere on the matrix with width of 3
//Ensures: The ledArray variable is changed to display
//          the num at the appropriate position
void displayNum(int num, int pos){
  for (int i = pos; i < pos + 3; i++){
    for (int k = 0; k < 5; k++){
      ledArray[i][k] = LEDNumbers[num][i-pos][k];  
    }  
  }
}

//Requires: An existing array of size 14x5 containing
//          1 for live cell and 0 for dead cell
//Ensures: Determines and sets the array for the next
//          generation based on the rules of the game
//          of life

void lifeStep(){
  int nextGen[14][5];
  for (int i = 0; i < 14; i++){
    for (int k = 0; k < 5; k++){
      //Go through all the cells
      int sumAround = sumAdjacents(i, k);
      //Serial.print("x: ");
      //Serial.print(i);
      //Serial.print(" y: ");
      //Serial.print(k);
      //Serial.print(" sum: ");
      //Serial.println(sumAround);
      //Live cell
      if (ledArray[i][k] ==1){
       if (sumAround < 2){
         nextGen[i][k] = 0;  
       }else if (sumAround == 2 || sumAround == 3){
         nextGen[i][k] = 1; 
       }else if(sumAround > 3){
         nextGen[i][k] = 0; 
       }
       //Dead cell 
      }else{
        if (sumAround == 3){
          nextGen[i][k] = 1;  
        }
      }
    }
  }
  //After finished generating nextGen, copy the matrix
  //ledArray = nextGen;  
  for (int i = 0; i < 14; i++){
    for (int k = 0; k < 5; k++){
      ledArray[i][k] = nextGen[i][k];
    }  
  }
}

//Requires: A horizontal coordinate 0 < x < 5
//          A vertical coordinate 0 < y < 14
//Ensures: The result is the number of adjacent cells that
//          are alive ("1")
int sumAdjacents(int x, int y){
  int total = 0;
  //Not first, last row and first and last columns
  if (x > 0 && x < 13 && y > 0 && y < 4){
    //Add around
    total += ledArray[x-1][y];
    total += ledArray[x-1][y-1];
    total += ledArray[x-1][y+1];
    total += ledArray[x][y-1];
    total += ledArray[x][y+1];
    total += ledArray[x+1][y-1];
    total += ledArray[x+1][y];
    total += ledArray[x+1][y+1];
  }else{ 
    if(x == 0){
      total += ledArray[x+1][y];
      if (y > 0){
        total += ledArray[x+1][y-1];
        total += ledArray[x][y-1]; 
      }
      if (y < 13){
        total += ledArray[x][y+1];
        total += ledArray[x+1][y+1];
      } 
    }else if(x == 13){
      total += ledArray[x-1][y];
      if (y > 0){
        total += ledArray[x-1][y-1];
        total += ledArray[x][y-1];        
      }
      if (y < 13){
        total += ledArray[x][y+1];
        total += ledArray[x-1][y+1];
      }  
      //Only top and bottom row left
    }else{
      total += ledArray[x+1][y];
      total += ledArray[x-1][y];
      if (y == 4){
        total += ledArray[x-1][y-1];
        total += ledArray[x][y-1];
        total += ledArray[x+1][y-1];
      }
      if (y == 0){
        total += ledArray[x-1][y+1];
        total += ledArray[x][y+1];
        total += ledArray[x+1][y+1];
      }  
    }
  }
  return total;
}
//Requires: An array of booleans of size NxM where N<14 and M<5
//        with True indicating the LED at that position should be on. 
void lightLeds(){
  //Byte array format
  //Q7, Q6, Q5, Q4, Q3, Q2, Q1, Q0              
  int firstByte = 255; 
  int secondByte = 255; 
  int thirdByte = 0; 
  
  //Depending on the column that we are on,
  //set the appropriate pin low
  for (int i = 0; i < 14; i++){ 
    if (i < 7){ 
      firstByte = (~(1 << i)); //Sets ith bit 
    }else{
      secondByte = (~(1 << (i-7)));
    }
    
    for (int j = 0; j < 5; j++){
      if (ledArray [i][4-j] == 1){
        thirdByte |= (1 << j);  
        //digitalWrite(powerPinArray[j], HIGH);
      }
    }
    //thirdByte = firstByte;
    
    digitalWrite(resetPin, HIGH);
    
    digitalWrite(latchPin, LOW);
    //shiftOut(dataPin, clockPin, MSBFIRST, thirdByte);
    shiftOut(dataPin, clockPin, MSBFIRST, secondByte);
    shiftOut(dataPin, clockPin, MSBFIRST, firstByte);
    
    digitalWrite(latchPin, HIGH);
    digitalWrite(resetPin, LOW);
    
    //starting          B0005 4321 
    //we want   portD = Bx4x3 21xx
    //we want thidByte(pins) = Bx6x4 32xx
    int topBit = (thirdByte & (1 <<  3)) << 3;
    //Remove top bits and left shift by 2
    PORTD |= thirdByte << 2;
    PORTD &= ~(3 << 5);
    PORTD |= topBit;
    
    //we want portB = Bxxx6xx
    //Shift right by 2 to have 5th bit at 3rd position
    PORTB = thirdByte >> 2;
    PORTB &= 1 << 2; //0 all the other bits
    
    delay(1);
    
    //Resetting all the bytes for the next line
    firstByte = 255;
    secondByte = 255;
   
    thirdByte = 0;
    PORTD = 0;
    PORTB = 0;
  }
}

