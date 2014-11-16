int dataPin = 9; //PCINT8 //DS
int clockPin = 7; //PCINT9 //SHCP
int latchPin = 8; //PCINT10 //STCP
int resetPin = 5;

int countIter = 0;
int powerPinArray[5] = {2, 3, 4, 6, 10};

//Gif test
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
    {0, 1, 0, 0, 0}
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

void loop(){
  //Count from 0 to 255 and display the number
  //on the LEDS
  if (countIter >= 25){
    lifeStep();
    countIter = 0;  
  }
  lightLeds();
  countIter++;
  //lifeStep();
  //delay(1000);
}

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
      //Serial.print(ledArray[i][j]);
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
    /*for (int k =0; k < 5; k++){
      digitalWrite(powerPinArray[k], LOW);
    }*/
    
    //Shift left by 4 first to have bit 4 for pin 6
    //starting          B0005 4321 
    //we want   portD = Bx4x3 21xx
    //we want thidByte(pins) = Bx6x4 32xx
    int topBit = (thirdByte & (1 <<  3)) << 3;
    //Remove top bit and left shift by 2
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
    
    //Serial.print(firstByte);
    //Serial.print(" Second: ");
    //Serial.println(secondByte);
   /*for(int i = 7; i < 14; i++){
     if (ledArray[i][j] == true){
       
     }
   }*/
}

