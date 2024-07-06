/*
Somewhat more stable version of tuner for LMNC oscillator: https://www.lookmumnocomputer.com/projects#/1222-performance-vco

This code is somewhat uses sine wave freq detection by Amanda Ghassaei july 2012
https://www.instructables.com/member/amandaghassaei/
and of course the forementioned LMNC code.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
*/

//LED OUTPUT PINS

int LED3 = 18;
int LED4 = 19;
int LED5 = 17;


//7 segment display output pins;

int LEDE = 2;
int LEDD = 3;
int LEDC = 4;
int LEDDP = 5;
int LEDB = 9;
int LEDA = 8;
int LEDF = 7;
int LEDG = 6;


//clipping indicator variables
boolean clipping = 0;

//data storage variables
byte newData = 0;
byte prevData = 0;

//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
long frequency;

long frequencyArray[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
long frequencyArrayCounter = 0;

struct Note{
    int freq;
    int lowerLimit;
    int upperLimit;
    int tuneLow;
    int tuneHigh;
};


Note notesC[] = { { 164, 150, 168, 163, 165 },{ 327, 318, 337, 326, 328 },{ 654, 635, 673, 652, 656 },{ 1308, 1271, 1346, 1304, 1312 },{ 2616, 2542, 2693, 2609, 2624 },{ 5233, 5084, 5386, 5217, 5248 },{ 10465, 10167, 10772, 10435, 10495 },{ 20930, 20334, 21543, 20870, 20991 }, };
Note notesCs[] = { { 173, 168, 178, 172, 174 },{ 346, 337, 357, 345, 347 },{ 693, 673, 713, 691, 695 },{ 1386, 1346, 1427, 1382, 1390 },{ 2772, 2693, 2853, 2764, 2780 },{ 5544, 5386, 5706, 5528, 5560 },{ 11087, 10772, 11412, 11055, 11119 },{ 22175, 21543, 22824, 22111, 22239 }, };
Note notesD[] = { { 184, 178, 189, 183, 185 },{ 367, 357, 378, 366, 368 },{ 734, 713, 756, 732, 736 },{ 1468, 1427, 1511, 1464, 1473 },{ 2937, 2853, 3023, 2928, 2945 },{ 5873, 5706, 6045, 5856, 5890 },{ 11747, 11412, 12091, 11713, 11781 },{ 23493, 22824, 24182, 23425, 23561 }, };
Note notesDs[] = { { 194, 189, 200, 193, 195 },{ 389, 378, 400, 388, 390 },{ 778, 756, 801, 776, 780 },{ 1556, 1511, 1601, 1551, 1560 },{ 3111, 3023, 3202, 3102, 3120 },{ 6223, 6045, 6405, 6205, 6241 },{ 12445, 12091, 12810, 12409, 12481 },{ 24890, 24182, 25619, 24818, 24962 }, };
Note notesE[] = { { 206, 200, 212, 205, 207 },{ 412, 400, 424, 411, 413 },{ 824, 801, 848, 822, 826 },{ 1648, 1601, 1696, 1643, 1653 },{ 3296, 3202, 3393, 3287, 3306 },{ 6593, 6405, 6786, 6574, 6612 },{ 13185, 12810, 13571, 13147, 13223 },{ 26370, 25619, 27143, 26294, 26446 }, };
Note notesF[] = { { 218, 212, 225, 217, 219 },{ 437, 424, 449, 435, 438 },{ 873, 848, 899, 871, 876 },{ 1746, 1696, 1797, 1741, 1751 },{ 3492, 3393, 3595, 3482, 3502 },{ 6985, 6786, 7189, 6964, 7005 },{ 13969, 13571, 14378, 13929, 14010 },{ 27938, 27143, 28757, 27858, 28019 }, };
Note notesFs[] = { { 231, 225, 238, 230, 232 },{ 462, 449, 476, 461, 464 },{ 925, 899, 952, 922, 928 },{ 1850, 1797, 1904, 1845, 1855 },{ 3700, 3595, 3808, 3689, 3711 },{ 7400, 7189, 7617, 7379, 7421 },{ 14800, 14378, 15233, 14757, 14843 },{ 29600, 28757, 30467, 29514, 29685 }, };
Note notesG[] = { { 245, 238, 252, 244, 246 },{ 490, 476, 504, 489, 491 },{ 980, 952, 1009, 977, 983 },{ 1960, 1904, 2017, 1954, 1966 },{ 3920, 3808, 4035, 3909, 3931 },{ 7840, 7617, 8070, 7817, 7863 },{ 15680, 15233, 16139, 15635, 15725 },{ 31360, 30467, 32279, 31269, 31450 }, };
Note notesGs[] = { { 260, 252, 267, 259, 261 },{ 519, 504, 534, 518, 521 },{ 1038, 1009, 1069, 1035, 1041 },{ 2077, 2017, 2137, 2071, 2083 },{ 4153, 4035, 4275, 4141, 4165 },{ 8306, 8070, 8549, 8282, 8330 },{ 16612, 16139, 17099, 16564, 16660 },{ 33224, 32279, 34198, 33129, 33320 }, };
Note notesA[] = { { 275, 267, 283, 274, 276 },{ 550, 534, 566, 548, 552 },{ 1100, 1069, 1132, 1097, 1103 },{ 2200, 2137, 2264, 2194, 2206 },{ 4400, 4275, 4529, 4387, 4413 },{ 8800, 8549, 9058, 8775, 8825 },{ 17600, 17099, 18116, 17549, 17651 },{ 35200, 34198, 36231, 35098, 35302 }, };
Note notesAs[] = { { 291, 283, 300, 290, 292 },{ 583, 566, 600, 581, 584 },{ 1165, 1132, 1200, 1162, 1169 },{ 2331, 2264, 2399, 2324, 2338 },{ 4662, 4529, 4798, 4648, 4675 },{ 9323, 9058, 9596, 9296, 9350 },{ 18647, 18116, 19193, 18593, 18700 },{ 37293, 36231, 38386, 37186, 37401 }, };
Note notesB[] = { { 309, 300, 318, 308, 310 },{ 617, 600, 635, 616, 619 },{ 1235, 1200, 1271, 1231, 1238 },{ 2469, 2399, 2542, 2462, 2477 },{ 4939, 4798, 5084, 4925, 4953 },{ 9878, 9596, 10167, 9849, 9906 },{ 19755, 19193, 20334, 19698, 19812 },{ 39511, 38386, 40668, 39397, 39625 }, };


void setup(){
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(LEDE, OUTPUT);
  pinMode(LEDD, OUTPUT);
  pinMode(LEDC, OUTPUT);
  pinMode(LEDDP, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDF, OUTPUT);
  pinMode(LEDG, OUTPUT);
  
  
 //to see frequency coming up on the serial uncomment the line beloe and also the serial code right at the bottom of this code page 

Serial.begin(9600);

  
  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei(); //enable interrupts
}

ISR(ADC_vect) { //when new ADC value ready

  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    period = timer;//get period
    timer = 0;//reset timer
  }
  
  timer++; //increment timer at rate of 38.5kHz
}

void setDisplayC() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,LOW );
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,LOW );
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,LOW );
}

void setDisplayCSharp() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,LOW );
  digitalWrite(LEDDP,HIGH);
  digitalWrite(LEDB,LOW );
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,LOW ); 
}

void setDisplayD() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,LOW );
  digitalWrite(LEDF,LOW );
  digitalWrite(LEDG,HIGH);
}

void setDisplayDSharp() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,HIGH);
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,LOW );
  digitalWrite(LEDF,LOW );
  digitalWrite(LEDG,HIGH);
}

void setDisplayE() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,LOW );
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,LOW );
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);
}

void setDisplayF() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,LOW );
  digitalWrite(LEDC,LOW );
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,LOW );
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);
}

void setDisplayFSharp() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,LOW );
  digitalWrite(LEDC,LOW );
  digitalWrite(LEDDP,HIGH);
  digitalWrite(LEDB,LOW );
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);  
}

void setDisplayG() {
  digitalWrite(LEDE,LOW );
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);  
}

void setDisplayGSharp() {
  digitalWrite(LEDE,LOW );
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,HIGH);
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH); 
}

void setDisplayA() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,LOW );
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);  
}

void setDisplayASharp() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,LOW );
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,HIGH);
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);
}

void setDisplayB() {
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,HIGH);
}

void setDisplayTooLow() {
  digitalWrite(LED3,HIGH);
  digitalWrite(LED4,LOW );
  digitalWrite(LED5,LOW );
}

void setDisplayTooHigh() {
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW );
  digitalWrite(LED5,HIGH );
}

void setDisplayJustRight() {
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,HIGH );
  digitalWrite(LED5,LOW );
}

void setDisplayCannot() {
  digitalWrite(LED3,LOW );
  digitalWrite(LED4,LOW );
  digitalWrite(LED5,LOW );
  digitalWrite(LEDE,HIGH);
  digitalWrite(LEDD,HIGH);
  digitalWrite(LEDC,HIGH);
  digitalWrite(LEDDP,LOW );
  digitalWrite(LEDB,HIGH);
  digitalWrite(LEDA,HIGH);
  digitalWrite(LEDF,HIGH);
  digitalWrite(LEDG,LOW );
}

void setDisplayCannotHigh() {
      digitalWrite(LED3,HIGH );
      digitalWrite(LED4,HIGH );
      digitalWrite(LED5,HIGH );
      digitalWrite(LEDE,LOW);
      digitalWrite(LEDD,LOW);
      digitalWrite(LEDC,LOW);
      digitalWrite(LEDDP,HIGH );
      digitalWrite(LEDB,LOW);
      digitalWrite(LEDA,LOW);
      digitalWrite(LEDF,LOW);
      digitalWrite(LEDG,LOW);
}

void setLowHighRight(int frequency, int low, int high) {
  if (frequency < low) {
    setDisplayTooLow();
  } else if (frequency > high) {
    setDisplayTooHigh(); 
  } else {
    setDisplayJustRight();
  }
}


void loop(){

  frequencyArray[frequencyArrayCounter] = 384620 / period;
  frequencyArrayCounter++;
  if (frequencyArrayCounter > 9) {
    frequencyArrayCounter = 0;
  }

  frequency = (frequencyArray[0] + frequencyArray[1] + frequencyArray[2] + frequencyArray[3] + frequencyArray[4] + frequencyArray[5] + frequencyArray[6] + frequencyArray[7] + frequencyArray[8] + frequencyArray[9]) / 10; //timer rate with an extra zero/period

  if (frequency < 158) { setDisplayCannot(); }
  else if (frequency < notesC[0].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[0].tuneLow,  notesC[0].tuneHigh);  } //C0
  else if (frequency < notesCs[0].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[0].tuneLow, notesCs[0].tuneHigh); } //C#0
  else if (frequency < notesD[0].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[0].tuneLow,  notesD[0].tuneHigh);  } //D0
  else if (frequency < notesDs[0].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[0].tuneLow, notesDs[0].tuneHigh); } //D#0
  else if (frequency < notesE[0].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[0].tuneLow,  notesE[0].tuneHigh);  } //E0
  else if (frequency < notesF[0].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[0].tuneLow,  notesF[0].tuneHigh);  } //F0
  else if (frequency < notesFs[0].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[0].tuneLow, notesFs[0].tuneHigh); } //F#0
  else if (frequency < notesG[0].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[0].tuneLow,  notesG[0].tuneHigh);  } //G0
  else if (frequency < notesGs[0].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[0].tuneLow, notesGs[0].tuneHigh); } //G#0
  else if (frequency < notesA[0].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[0].tuneLow,  notesA[0].tuneHigh);  } //A0
  else if (frequency < notesAs[0].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[0].tuneLow, notesAs[0].tuneHigh); } //A#0
  else if (frequency < notesB[0].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[0].tuneLow,  notesB[0].tuneHigh);  } //B0
  else if (frequency < notesC[1].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[1].tuneLow,  notesC[1].tuneHigh);  } //C1
  else if (frequency < notesCs[1].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[1].tuneLow, notesCs[1].tuneHigh); } //C#1
  else if (frequency < notesD[1].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[1].tuneLow,  notesD[1].tuneHigh);  } //D1
  else if (frequency < notesDs[1].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[1].tuneLow, notesDs[1].tuneHigh); } //D#1
  else if (frequency < notesE[1].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[1].tuneLow,  notesE[1].tuneHigh);  } //E1
  else if (frequency < notesF[1].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[1].tuneLow,  notesF[1].tuneHigh);  } //F1
  else if (frequency < notesFs[1].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[1].tuneLow, notesFs[1].tuneHigh); } //F#1
  else if (frequency < notesG[1].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[1].tuneLow,  notesG[1].tuneHigh);  } //G1
  else if (frequency < notesGs[1].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[1].tuneLow, notesGs[1].tuneHigh); } //G#1
  else if (frequency < notesA[1].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[1].tuneLow,  notesA[1].tuneHigh);  } //A1
  else if (frequency < notesAs[1].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[1].tuneLow, notesAs[1].tuneHigh); } //A#1
  else if (frequency < notesB[1].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[1].tuneLow,  notesB[1].tuneHigh);  } //B1
  else if (frequency < notesC[2].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[2].tuneLow,  notesC[2].tuneHigh);  } //C2
  else if (frequency < notesCs[2].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[2].tuneLow, notesCs[2].tuneHigh); } //C#2
  else if (frequency < notesD[2].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[2].tuneLow,  notesD[2].tuneHigh);  } //D2
  else if (frequency < notesDs[2].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[2].tuneLow, notesDs[2].tuneHigh); } //D#2
  else if (frequency < notesE[2].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[2].tuneLow,  notesE[2].tuneHigh);  } //E2
  else if (frequency < notesF[2].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[2].tuneLow,  notesF[2].tuneHigh);  } //F2
  else if (frequency < notesFs[2].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[2].tuneLow, notesFs[2].tuneHigh); } //F#2
  else if (frequency < notesG[2].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[2].tuneLow,  notesG[2].tuneHigh);  } //G2
  else if (frequency < notesGs[2].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[2].tuneLow, notesGs[2].tuneHigh); } //G#2
  else if (frequency < notesA[2].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[2].tuneLow,  notesA[2].tuneHigh);  } //A2
  else if (frequency < notesAs[2].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[2].tuneLow, notesAs[2].tuneHigh); } //A#2
  else if (frequency < notesB[2].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[2].tuneLow,  notesB[2].tuneHigh);  } //B2
  else if (frequency < notesC[3].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[3].tuneLow,  notesC[3].tuneHigh);  } //C3
  else if (frequency < notesCs[3].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[3].tuneLow, notesCs[3].tuneHigh); } //C#3
  else if (frequency < notesD[3].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[3].tuneLow,  notesD[3].tuneHigh);  } //D3
  else if (frequency < notesDs[3].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[3].tuneLow, notesDs[3].tuneHigh); } //D#3
  else if (frequency < notesE[3].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[3].tuneLow,  notesE[3].tuneHigh);  } //E3
  else if (frequency < notesF[3].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[3].tuneLow,  notesF[3].tuneHigh);  } //F3
  else if (frequency < notesFs[3].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[3].tuneLow, notesFs[3].tuneHigh); } //F#3
  else if (frequency < notesG[3].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[3].tuneLow,  notesG[3].tuneHigh);  } //G3
  else if (frequency < notesGs[3].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[3].tuneLow, notesGs[3].tuneHigh); } //G#3
  else if (frequency < notesA[3].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[3].tuneLow,  notesA[3].tuneHigh);  } //A3
  else if (frequency < notesAs[3].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[3].tuneLow, notesAs[3].tuneHigh); } //A#3
  else if (frequency < notesB[3].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[3].tuneLow,  notesB[3].tuneHigh);  } //B3
  else if (frequency < notesC[4].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[4].tuneLow,  notesC[4].tuneHigh);  } //C4
  else if (frequency < notesCs[4].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[4].tuneLow, notesCs[4].tuneHigh); } //C#4
  else if (frequency < notesD[4].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[4].tuneLow,  notesD[4].tuneHigh);  } //D4
  else if (frequency < notesDs[4].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[4].tuneLow, notesDs[4].tuneHigh); } //D#4
  else if (frequency < notesE[4].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[4].tuneLow,  notesE[4].tuneHigh);  } //E4
  else if (frequency < notesF[4].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[4].tuneLow,  notesF[4].tuneHigh);  } //F4
  else if (frequency < notesFs[4].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[4].tuneLow, notesFs[4].tuneHigh); } //F#4
  else if (frequency < notesG[4].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[4].tuneLow,  notesG[4].tuneHigh);  } //G4
  else if (frequency < notesGs[4].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[4].tuneLow, notesGs[4].tuneHigh); } //G#4
  else if (frequency < notesA[4].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[4].tuneLow,  notesA[4].tuneHigh);  } //A4
  else if (frequency < notesAs[4].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[4].tuneLow, notesAs[4].tuneHigh); } //A#4
  else if (frequency < notesB[4].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[4].tuneLow,  notesB[4].tuneHigh);  } //B4
  else if (frequency < notesC[5].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[5].tuneLow,  notesC[5].tuneHigh);  } //C5
  else if (frequency < notesCs[5].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[5].tuneLow, notesCs[5].tuneHigh); } //C#5
  else if (frequency < notesD[5].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[5].tuneLow,  notesD[5].tuneHigh);  } //D5
  else if (frequency < notesDs[5].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[5].tuneLow, notesDs[5].tuneHigh); } //D#5
  else if (frequency < notesE[5].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[5].tuneLow,  notesE[5].tuneHigh);  } //E5
  else if (frequency < notesF[5].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[5].tuneLow,  notesF[5].tuneHigh);  } //F5
  else if (frequency < notesFs[5].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[5].tuneLow, notesFs[5].tuneHigh); } //F#5
  else if (frequency < notesG[5].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[5].tuneLow,  notesG[5].tuneHigh);  } //G5
  else if (frequency < notesGs[5].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[5].tuneLow, notesGs[5].tuneHigh); } //G#5
  else if (frequency < notesA[5].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[5].tuneLow,  notesA[5].tuneHigh);  } //A5
  else if (frequency < notesAs[5].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[5].tuneLow, notesAs[5].tuneHigh); } //A#5
  else if (frequency < notesB[5].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[5].tuneLow,  notesB[5].tuneHigh);  } //B5
  else if (frequency < notesC[6].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[6].tuneLow,  notesC[6].tuneHigh);  } //C6
  else if (frequency < notesCs[6].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[6].tuneLow, notesCs[6].tuneHigh); } //C#6
  else if (frequency < notesD[6].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[6].tuneLow,  notesD[6].tuneHigh);  } //D6
  else if (frequency < notesDs[6].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[6].tuneLow, notesDs[6].tuneHigh); } //D#6
  else if (frequency < notesE[6].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[6].tuneLow,  notesE[6].tuneHigh);  } //E6
  else if (frequency < notesF[6].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[6].tuneLow,  notesF[6].tuneHigh);  } //F6
  else if (frequency < notesFs[6].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[6].tuneLow, notesFs[6].tuneHigh); } //F#6
  else if (frequency < notesG[6].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[6].tuneLow,  notesG[6].tuneHigh);  } //G6
  else if (frequency < notesGs[6].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[6].tuneLow, notesGs[6].tuneHigh); } //G#6
  else if (frequency < notesA[6].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[6].tuneLow,  notesA[6].tuneHigh);  } //A6
  else if (frequency < notesAs[6].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[6].tuneLow, notesAs[6].tuneHigh); } //A#6
  else if (frequency < notesB[6].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[6].tuneLow,  notesB[6].tuneHigh);  } //B6
  else if (frequency < notesC[7].upperLimit ) { setDisplayC();      setLowHighRight(frequency, notesC[7].tuneLow,  notesC[7].tuneHigh);  } //C7
  else if (frequency < notesCs[7].upperLimit) { setDisplayCSharp(); setLowHighRight(frequency, notesCs[7].tuneLow, notesCs[7].tuneHigh); } //C#7
  else if (frequency < notesD[7].upperLimit ) { setDisplayD();      setLowHighRight(frequency, notesD[7].tuneLow,  notesD[7].tuneHigh);  } //D7
  else if (frequency < notesDs[7].upperLimit) { setDisplayDSharp(); setLowHighRight(frequency, notesDs[7].tuneLow, notesDs[7].tuneHigh); } //D#7
  else if (frequency < notesE[7].upperLimit)  { setDisplayE();      setLowHighRight(frequency, notesE[7].tuneLow,  notesE[7].tuneHigh);  } //E7
  else if (frequency < notesF[7].upperLimit)  { setDisplayF();      setLowHighRight(frequency, notesF[7].tuneLow,  notesF[7].tuneHigh);  } //F7
  else if (frequency < notesFs[7].upperLimit) { setDisplayFSharp(); setLowHighRight(frequency, notesFs[7].tuneLow, notesFs[7].tuneHigh); } //F#7
  else if (frequency < notesG[7].upperLimit)  { setDisplayG();      setLowHighRight(frequency, notesG[7].tuneLow,  notesG[7].tuneHigh);  } //G7
  else if (frequency < notesGs[7].upperLimit) { setDisplayGSharp(); setLowHighRight(frequency, notesGs[7].tuneLow, notesGs[7].tuneHigh); } //G#7
  else if (frequency < notesA[7].upperLimit)  { setDisplayA();      setLowHighRight(frequency, notesA[7].tuneLow,  notesA[7].tuneHigh);  } //A7
  else if (frequency < notesAs[7].upperLimit) { setDisplayASharp(); setLowHighRight(frequency, notesAs[7].tuneLow, notesAs[7].tuneHigh); } //A#7
  else if (frequency < notesB[7].upperLimit)  { setDisplayB();      setLowHighRight(frequency, notesB[7].tuneLow,  notesB[7].tuneHigh);  } //B7
  else {
    setDisplayCannotHigh();
  }

delay(70);

}
